# CGAssistant 技术文档

**版本**: 1.0  
**创建时间**: 2026-04-22  
**最后更新**: 2026-04-22  
**仓库**: https://github.com/hzqst/CGAssistant

---

## 目录

1. [项目概述](#1-项目概述)
2. [系统架构](#2-系统架构)
3. [模块详解](#3-模块详解)
4. [核心技术](#4-核心技术)
5. [API 参考](#5-api 参考)
6. [编译指南](#6-编译指南)
7. [代码质量报告](#7-代码质量报告)
8. [常见问题](#8-常见问题)

---

## 1. 项目概述

### 1.1 项目简介

CGAssistant 是一个免费开源的自动化程序，专为游戏"CrossGate"（魔力宝贝）设计。项目提供游戏内存读取、自动化战斗、任务脚本执行等功能。

### 1.2 技术栈

| 组件 | 技术 | 说明 |
|------|------|------|
| **主程序** | C++ / Qt 5.12 | Windows 桌面应用 |
| **注入模块** | C++ / Detours | DLL 注入与 Hook |
| **RPC 库** | C++ / rest_rpc | 进程间通信 |
| **Node.js 绑定** | Nan / V8 | JavaScript 脚本支持 |
| **构建系统** | MSBuild / qmake / node-gyp | 多平台编译 |

### 1.3 项目规模

| 模块 | 文件数 | 代码行数 |
|------|--------|----------|
| CGAssistant (Qt UI) | ~40 | ~12,000 |
| cgahook (注入) | ~10 | ~5,000 |
| CGALib (RPC 接口) | ~3 | ~4,000 |
| CGANode (Node.js) | ~12 | ~8,000 |
| **总计** | **~65** | **~29,000** |

### 1.4 支持的游戏版本

```cpp
enum game_type {
    cg_se_3000,    // 魔力宝贝 3.0 版本
    cg_se_6000,    // 魔力宝贝 6.0 版本
    cg_item_6000,  // 魔力宝贝 6.0 正式版
    polcn          // 魔力宝贝 中文版
};
```

---

## 2. 系统架构

### 2.1 整体架构图

```
┌─────────────────────────────────────────────────────────────┐
│                     CGAssistant.exe (Qt UI)                  │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐           │
│  │ PlayerForm  │ │BattleForm   │ │ ScriptForm  │           │
│  └──────┬──────┘ └──────┬──────┘ └──────┬──────┘           │
│         │               │               │                    │
│  ┌──────▼──────────────▼───────────────▼──────┐            │
│  │           CPlayerWorker / CBattleWorker    │            │
│  └───────────────────┬────────────────────────┘            │
└──────────────────────┼─────────────────────────────────────┘
                       │ RPC (rest_rpc / msgpack)
                       │ Port 4396-5395
┌──────────────────────▼─────────────────────────────────────┐
│                    cgahook.dll (注入游戏进程)                │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              CGAService (游戏服务层)                  │   │
│  │  ┌───────────┐ ┌───────────┐ ┌───────────┐         │   │
│  │  │内存读取    │ │Hook 函数   │ │封包解析    │         │   │
│  │  └───────────┘ └───────────┘ └───────────┘         │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                       │
┌──────────────────────▼─────────────────────────────────────┐
│                  魔力宝贝游戏进程 (cg_se_6000.exe)           │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 进程模型

| 进程 | 职责 | 通信方式 |
|------|------|----------|
| **CGAssistant.exe** | UI 渲染、用户交互、脚本执行 | 主进程 |
| **cg_se_6000.exe** | 游戏本体 | 被注入目标 |
| **cgahook.dll** | 游戏内存访问、封包拦截 | 注入到游戏进程 |
| **Node.js** | JavaScript 脚本引擎 (可选) | 通过.node 绑定 |

### 2.3 线程模型

```
CGAssistant.exe:
├── Main Thread (UI)
├── m_playerWorkerThread (玩家数据轮询)
├── m_processWorkerThread (进程监控)
└── m_battleWorkerThread (战斗处理)

cgahook.dll (游戏进程内):
├── Main Thread (游戏主线程)
├── CGAServerThread (RPC 服务器)
└── Hook 回调 (WndProc/NPC 对话框等)
```

---

## 3. 模块详解

### 3.1 CGAssistant (Qt 主程序)

#### 目录结构
```
CGAssistant/
├── mainwindow.cpp/h      # 主窗口
├── player.cpp/h          # 玩家数据轮询 Worker
├── battle.cpp/h          # 自动战斗逻辑
├── psworker.cpp/h        # 进程注入 Worker
├── scriptform.cpp/h      # 脚本编辑器
├── itemform.cpp/h        # 物品管理
├── mapform.cpp/h         # 地图显示
└── chatform.cpp/h        # 聊天监控
```

#### 核心类

**CPlayerWorker** - 玩家数据轮询
```cpp
class CPlayerWorker : public QObject {
    Q_OBJECT
public slots:
    void OnQueueGetPlayerInfo();    // 每秒轮询
    void OnQueueGetItemInfo();      // 每秒 2 次
    void OnQueueGetMapInfo();       // 每秒 10 次
    void OnQueueAntiAFKKick();      // 防踢 (每分钟)
signals:
    void NotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t>);
    void NotifyGetItemsInfo(QSharedPointer<CGA_ItemList_t>);
};
```

**CBattleWorker** - 自动战斗
```cpp
class CBattleWorker : public QObject {
    Q_OBJECT
public:
    bool m_bAutoBattle;           // 自动战斗开关
    bool m_bHighSpeed;            // 高速模式
    CBattleSettingList m_SettingList;  // 战斗配置列表
public slots:
    void OnSetAutoBattle(int state);
    void OnSyncList(CBattleSettingList list);
};
```

### 3.2 cgahook (注入模块)

#### 目录结构
```
cgahook/
├── main.cpp            # DLL 入口 & Hook 逻辑
├── gameservice.cpp/h   # 游戏服务实现 (核心)
├── server.cpp          # RPC 服务器
├── inject.cpp/h        # DLL 注入工具
└── commandline.cpp/h   # 命令行参数解析
```

#### 注入流程

```cpp
// 1. GetMsgProc Hook (WH_GETMESSAGE)
extern "C" __declspec(dllexport) LRESULT WINAPI GetMsgProc(...)
{
    // 检测游戏窗口类名
    if (!_wcsicmp(pModuleName, L"cg_item_6000.exe") && 
        !strcmp(szClass, GAME_WINDOW_CLASS)) {
        InitializeHooks(...);  // 初始化 Hook
    }
}

// 2. 初始化游戏服务
void InitializeHooks(...) {
    g_CGAService.Initialize(type);  // 扫描内存地址
    
    // 3. 子类化窗口过程
    g_OldProc = (WNDPROC)GetWindowLongPtrA(hWnd, GWL_WNDPROC);
    SetWindowLongPtrA(hWnd, GWL_WNDPROC, (LONG_PTR)NewWndProc);
    
    // 4. 启动 RPC 服务器
    g_hServreThread = CreateThread(..., CGAServerThread, ...);
}
```

#### 内存扫描示例

```cpp
// gameservice.cpp - 初始化内存地址
void CGAService::Initialize(game_type type)
{
    HMODULE hGameBase = GetModuleHandleA(NULL);
    m_ImageBase = (ULONG_PTR)hGameBase;
    
    if (type == cg_item_6000) {
        // 硬编码偏移 (基于游戏版本)
        g_playerBase = CONVERT_GAMEVAR(playerbase_t **, 0xE12C30);
        g_pet_base = CONVERT_GAMEVAR(pet_t *, 0xE109F8);
        g_skill_base = CONVERT_GAMEVAR(skill_t *, 0xC1CD64);
        // ... 更多地址
    }
}
```

### 3.3 CGALib (RPC 接口库)

#### 文件结构
```
CGALib/
├── gameinterface.h     # 接口定义 (1700+ 行)
├── gameinterface.cpp   # RPC 客户端实现
└── packdata.h          # msgpack 序列化
```

#### 接口示例

```cpp
// gameinterface.h - 玩家信息结构
typedef struct cga_player_info_t {
    int hp, maxhp;
    int mp, maxmp;
    int xp, maxxp;
    int level;
    int gold;
    std::string name;
    std::string job;
    // ... 30+ 字段
} cga_player_info_t;

// gameinterface.h - RPC 接口
class CGAInterface {
public:
    virtual cga_player_info_t GetPlayerInfo() = 0;
    virtual bool BattleNormalAttack(int target) = 0;
    virtual bool UseItem(int itempos) = 0;
    // ... 100+ 接口
};
```

### 3.4 CGANode (Node.js 绑定)

#### 文件结构
```
CGANode/
├── main.cc             # Node.js 模块入口
├── info.cc/h           # 信息查询接口
├── battle.cc/h         # 战斗接口
├── item.cc/h           # 物品接口
├── map.cc/h            # 地图接口
└── work.cc/h           # 工作接口
```

#### JavaScript API 示例

```javascript
// 通过 Node.js 调用
const cga = require('cga');

// 获取玩家信息
cga.GetPlayerInfo((info) => {
    console.log(`HP: ${info.hp}/${info.maxhp}`);
});

// 自动战斗
cga.BattleNormalAttack(target, (err, result) => {
    if (!err) console.log('攻击成功');
});

// 移动
cga.WalkTo(x, y);
```

---

## 4. 核心技术

### 4.1 DLL 注入技术

#### 方法：消息钩子注入

```cpp
// inject.cpp - 内联 Hook 代码
const UCHAR HookCode[] = {
    0x55,                   // push ebp
    0x8B, 0xEC,             // mov ebp, esp
    // ... 保存原函数
    0xE8, 0, 0, 0, 0,       // call LdrLoadDll
    // ... 恢复执行
    0xE9, 0, 0, 0, 0,       // jmp 原函数
};

// 注入流程
bool CInject::InjectByHookNtdll(HANDLE hProcess, const std::wstring &dllPath)
{
    PINJECT_BUFFER pBuffer = GetInlineHookCode(hProcess, &ustrDllPath);
    
    // 修改 ZwTestAlert 函数前 5 字节为 JMP
    UCHAR trampo[] = { 0xE9, 0, 0, 0, 0 };
    *(DWORD *)(trampo + 1) = (DWORD)((DWORD)pBuffer->code - 
                                      ((DWORD)m_pfnZwTestAlert + 5));
    
    // 写入目标进程
    NtWriteVirtualMemory(hProcess, m_pfnZwTestAlert, trampo, sizeof(trampo));
    return true;
}
```

### 4.2 内存读取技术

#### XOR 加密值解码

```cpp
// gameservice.h - 游戏使用 XOR 加密重要数值
class CXorValue {
public:
    int decode() { return key1 ^ key2; }
    void set(int value) {
        key1 = rand();
        key2 = value ^ key1;
    }
private:
    int key1, key2;
    unsigned char refcount;
};

// 使用示例
info->hp = (*g_playerBase)->hp.decode();  // HP 是加密的
info->mp = (*g_playerBase)->mp.decode();  // MP 是加密的
```

### 4.3 RPC 通信

#### 协议：rest_rpc + msgpack

```cpp
// server.cpp - RPC 服务器注册
server = std::make_shared<server_t>(port, pool_size, 
                                     std::chrono::seconds{15});

// 注册 100+ 个处理函数
server->register_handler("GetPlayerInfo", 
    timax::bind(&CGAService::GetPlayerInfo, &g_CGAService));
server->register_handler("BattleNormalAttack", 
    timax::bind(&CGAService::BattleNormalAttack, &g_CGAService));

// 启动服务器
server->start();
```

#### 序列化示例

```cpp
// packdata.h - msgpack 序列化
template<>
struct pack<cga_player_info_t> {
    template <typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, 
                               cga_player_info_t const& v) const {
        o.pack_array(32);  // 32 个字段
        o.pack(v.hp);
        o.pack(v.maxhp);
        o.pack(v.mp);
        // ... 所有字段
        return o;
    }
};
```

### 4.4 战斗自动化

#### 条件 - 动作系统

```cpp
// battle.h - 战斗条件
#define BattleCond_Type_PlayerHp 1
#define BattleCond_Type_EnemyCount 10
#define BattleCond_NumRel_LT 3  // 小于

// battle.cpp - 条件检查
bool CBattleCondition_PlayerHp::Check(CGA_BattleContext_t &context, 
                                       int &conditionTarget)
{
    int curv = context.m_UnitGroup[context.m_iPlayerPosition].curhp;
    int maxv = context.m_UnitGroup[context.m_iPlayerPosition].maxhp;
    
    // 百分比检查
    if(m_percentage && maxv > 0) {
        if(curv * 100 / maxv < m_value) {  // HP < 30%
            conditionTarget = context.m_iPlayerPosition;
            return true;
        }
    }
    return false;
}

// battle.cpp - 动作执行
bool CBattleAction_PlayerSkillAttack::DoAction(int target, 
                                                int defaultTarget, 
                                                CGA_BattleContext_t &context)
{
    int skill_pos, skill_level;
    if(GetSkillPos(context, skill_pos, skill_level)) {
        g_CGAInterface->BattleSkillAttack(skill_pos, skill_level, 
                                          target, false, result);
    }
    return result;
}
```

---

## 5. API 参考

### 5.1 玩家相关

| 接口 | 参数 | 返回 | 说明 |
|------|------|------|------|
| `GetPlayerInfo()` | - | `cga_player_info_t` | 获取玩家信息 |
| `GetPetsInfo()` | - | `cga_pets_info_t[]` | 获取宠物列表 |
| `GetSkillsInfo()` | - | `cga_skills_info_t[]` | 获取技能列表 |
| `GetItemsInfo()` | - | `cga_items_info_t[]` | 获取物品列表 |
| `UpgradePlayer(attr)` | `attr: int` | - | 升级玩家属性 |
| `UpgradePet(petId, attr)` | `petId: int, attr: int` | - | 升级宠物属性 |

### 5.2 战斗相关

| 接口 | 参数 | 返回 | 说明 |
|------|------|------|------|
| `BattleNormalAttack(target)` | `target: int` | `bool` | 普通攻击 |
| `BattleSkillAttack(pos, lv, target)` | `pos: int, lv: int, target: int` | `bool` | 技能攻击 |
| `BattleGuard()` | - | `bool` | 防御 |
| `BattleEscape()` | - | `bool` | 逃跑 |
| `BattleChangePet(petId)` | `petId: int` | `bool` | 换宠 |
| `BattleUseItem(itemPos, target)` | `itemPos: int, target: int` | `bool` | 使用物品 |

### 5.3 地图相关

| 接口 | 参数 | 返回 | 说明 |
|------|------|------|------|
| `GetMapName()` | - | `string` | 获取地图名 |
| `GetMapXY()` | - | `[x, y]` | 获取坐标 |
| `GetMapUnits()` | - | `cga_map_units_t[]` | 获取地图单位 |
| `WalkTo(x, y)` | `x: int, y: int` | - | 行走 |
| `ForceMove(dir, show)` | `dir: int, show: bool` | `bool` | 强制移动 |
| `IsMapCellPassable(x, y)` | `x: int, y: int` | `bool` | 检查可通过 |

### 5.4 NPC 相关

| 接口 | 参数 | 返回 | 说明 |
|------|------|------|------|
| `ClickNPCDialog(option, index)` | `option: int, index: int` | `bool` | 点击 NPC 选项 |
| `SellNPCStore(items)` | `items: cga_sell_items_t[]` | `bool` | 出售物品 |
| `BuyNPCStore(items)` | `items: cga_buy_items_t[]` | `bool` | 购买物品 |

---

## 6. 编译指南

### 6.1 环境要求

| 组件 | 版本 | 说明 |
|------|------|------|
| **Visual Studio** | 2019+ | 需要 vc142 工具集 |
| **Qt** | 5.12 | 32 位 (x86) |
| **Node.js** | v20.11.1 LTS | 32 位 |
| **Windows SDK** | 10.0+ | - |

### 6.2 编译步骤

```bash
# 1. 初始化依赖
./init-dep.bat

# 2. 编译 Boost (需要 10-30 分钟)
./build-boost.bat

# 3. 编译 cgahook.dll
./build-CGAHook.bat

# 4. 编译 qhttp.dll
./build-qhttp.bat

# 5. 编译 CGAssistant.exe
./build-CGAssistant.bat

# 6. 编译 Node.js 绑定
./build-nodegyp.bat

# 7. 部署 Qt 依赖
./windeploy.bat
```

### 6.3 输出文件

编译完成后，所有二进制文件位于 `build/` 目录：

```
build/
├── CGAssistant.exe      # 主程序
├── cgahook.dll          # 注入 DLL
├── node_cga.node        # Node.js 绑定
├── qhttp.dll            # HTTP 库
└── [Qt 运行时库...]
```

---

## 7. 代码质量报告

### 7.1 代码统计

| 指标 | 数值 |
|------|------|
| 总文件数 | ~65 |
| 总代码行数 | ~29,000 |
| 平均函数复杂度 | 中等 |
| 代码覆盖率 | 未测试 |

### 7.2 已修复问题 (2026-04-22)

| 级别 | 问题 | 状态 |
|------|------|------|
| P0 | 除零崩溃 (2 处) | ✅ 已修复 |
| P1 | HHOOK 资源泄漏 | ✅ 已修复 |
| P2 | RPC 返回值/重试限制等 (5 项) | ✅ 已修复 |

### 7.3 代码规范

- **命名**: 混用驼峰和下划线 (待统一)
- **注释**: 部分中文注释为 GBK 编码 (待转 UTF-8)
- **错误处理**: 部分函数未检查返回值 (已改进)
- **测试**: 无自动化测试 (建议添加)

---

## 8. 常见问题

### 8.1 编译问题

**Q: 编译时找不到 Qt 库**

A: 确保已将 Qt 的 `bin` 目录添加到 PATH 环境变量：
```
C:\Qt\Qt5.12.0\5.12.0\msvc2017\bin
C:\Qt\Qt5.12.0\Tools\QtCreator\bin
C:\Qt\Qt5.12.0\Tools\QtCreator\bin\jom
```

**Q: node-gyp 编译失败**

A: 确保安装了 32 位 Node.js，并运行：
```bash
npm install -g node-gyp
npm config set arch ia32
```

### 8.2 运行时问题

**Q: 无法注入游戏**

A: 检查：
1. 游戏版本是否支持 (3.0/6.0/中文版)
2. 是否以管理员身份运行
3. 杀毒软件是否拦截

**Q: RPC 连接失败**

A: 检查端口 4396-5396 是否被占用：
```bash
netstat -ano | findstr 4396
```

### 8.3 脚本开发

**Q: 如何编写自动战斗脚本？**

```javascript
// JavaScript 示例
cga.EnableFlags(cga.ENABLE_FLAG_JOINTEAM, true);  // 允许组队
cga.SetAutoBattle(true);  // 开启自动战斗

// 配置战斗规则
cga.AddBattleAction({
    condition: { type: cga.COND_PLAYER_HP, op: cga.OP_LT, value: 30 },
    action: { type: cga.ACT_USE_ITEM, item: "血瓶" }
});
```

---

## 附录

### A. 术语表

| 术语 | 说明 |
|------|------|
| **Hook** | 函数拦截技术 |
| **RPC** | 远程过程调用 |
| **MsgPack** | 二进制序列化格式 |
| **Detours** | Microsoft 函数 Hook 库 |

### B. 参考链接

- [GitHub 仓库](https://github.com/hzqst/CGAssistant)
- [脚本仓库](https://github.com/hzqst/CGAssistantJS)
- [下载页面](https://github.com/hzqst/CGAssistant/releases)
- [使用 Wiki](https://github.com/hzqst/CGAssistant/wiki)

### C. 更新日志

**v1.0 (2026-04-22)**
- 初始技术文档发布
- 修复 3 个 P0/P1 级别 BUG
- 修复 5 个 P2 级别问题

---

**文档结束**
