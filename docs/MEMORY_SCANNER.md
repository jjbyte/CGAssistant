# 内存特征码扫描指南

## 概述

CGAssistant 现在支持内存特征码扫描，可以自动查找游戏内存地址，避免硬编码偏移。游戏小版本更新后无需重新适配。

## 为什么需要特征码扫描

### 传统方式 (硬编码)

```cpp
// 硬编码偏移 - 游戏更新后可能失效
g_playerBase = (playerbase_t**)(0xE12C30);
```

**问题**:
- 游戏更新后地址变化
- 需要手动分析新地址
- 每次更新都要发布新版本

### 特征码扫描 (推荐)

```cpp
// 特征码扫描 - 自动适应地址变化
auto match = cga::MemoryScanner::ScanPattern("8B 0D ?? ?? ?? ??", "xx??????", 2);
if (match.found) {
    g_playerBase = *(playerbase_t**)(match.address + match.offset);
}
```

**优势**:
- 自动适应小版本更新
- 降低维护成本
- 社区友好

## 快速开始

### 1. 包含头文件

```cpp
#include "memscanner.h"
```

### 2. 初始化扫描器

```cpp
// 在游戏进程初始化时调用
HMODULE hGameBase = GetModuleHandleA(NULL);
cga::MemoryScanner::Initialize(hGameBase);
```

### 3. 扫描特征码

```cpp
// 基本用法
auto match = cga::MemoryScanner::ScanPattern(
    "8B 0D ?? ?? ?? ??",  // 特征码
    "xx??????",           // 掩码
    2                     // 偏移
);

if (match.found) {
    uintptr_t address = match.address + match.offset;
    // 使用地址...
}
```

## 特征码格式

### 基本语法

| 符号 | 含义 | 示例 |
|------|------|------|
| `xx` | 精确匹配字节 | `8B 0D` |
| `??` | 通配符 (任意值) | `?? ?? ??` |
| 空格 | 分隔符 (可选) | `8B0D` 或 `8B 0D` |

### 掩码语法

| 字符 | 含义 |
|------|------|
| `x` | 精确匹配对应字节 |
| `?` | 通配符 (跳过检查) |

### 示例

```
汇编指令：8B 0D 12 34 56 78  ->  mov ecx, [0x78563412]
特征码：  8B 0D ?? ?? ?? ??
掩码：    xx??????
偏移：    2 (指向地址 0x12345678)
```

## 常用模式

### 1. 查找全局变量

```cpp
// 指令：8B 0D ?? ?? ?? ??  ->  mov ecx, [g_PlayerBase]
auto match = cga::MemoryScanner::ScanPattern("8B 0D ?? ?? ?? ??", "xx??????", 2);
if (match.found) {
    DWORD* ptr = *(DWORD**)(match.address + match.offset);
}
```

### 2. 查找函数调用

```cpp
// 指令：E8 ?? ?? ?? ??  ->  call SomeFunction
auto match = cga::MemoryScanner::ScanPattern("E8 ?? ?? ?? ??", "x????", 0);
if (match.found) {
    uintptr_t funcAddr = match.address + 5 + *(int32_t*)(match.address + 1);
}
```

### 3. 查找字符串引用

```cpp
// 指令：68 ?? ?? ?? ??  ->  push offset "PlayerName"
auto match = cga::MemoryScanner::ScanPattern("68 ?? ?? ?? ?? 8D", "x????x", 1);
if (match.found) {
    const char* str = *(const char**)(match.address + match.offset);
}
```

### 4. 查找虚函数表

```cpp
// 指令：8D 05 ?? ?? ?? ??  ->  lea eax, [vtable]
auto match = cga::MemoryScanner::ScanPattern("8D 05 ?? ?? ?? ?? 89", "xx????x", 2);
if (match.found) {
    void** vtable = *(void***)(match.address + match.offset);
}
```

## 高级用法

### 多结果扫描

```cpp
// 查找所有匹配
auto matches = cga::MemoryScanner::ScanPatternMulti(
    "8B 0D ?? ?? ?? ??",
    "xx??????",
    10  // 最多 10 个结果
);

for (const auto& match : matches) {
    LOG_INFO("找到匹配 - 地址：0x{:08X}", match.address);
}
```

### 指针链解析

```cpp
// 读取 [ [base + 0x10] + 0x20 ]
std::vector<int> offsets = {0x10, 0x20};
uintptr_t ptr = cga::MemoryScanner::ReadPointerChain(baseAddress, offsets);
```

### 相对地址解析

```cpp
// 指令使用 RIP 相对寻址
uintptr_t addr = cga::MemoryScanner::ReadRelativeOffset(instructionAddress, 2);
```

### AddressResolver 辅助类

```cpp
// 解析指针
uintptr_t ptr = cga::AddressResolver::ResolvePointer(
    moduleBase,
    "8B 0D ?? ?? ?? ??",
    "xx??????",
    2,
    true  // 解引用
);

// 解析 CALL 目标
uintptr_t func = cga::AddressResolver::ResolveCall(
    moduleBase,
    "E8 ?? ?? ?? ??",
    "x????"
);

// 解析数据引用
uintptr_t data = cga::AddressResolver::ResolveDataRef(
    moduleBase,
    "8D 05 ?? ?? ?? ??",
    "xx??????"
);
```

## 实战示例

### 示例 1: 查找玩家基址

```cpp
// 在 gameservice.cpp 中
void CGAService::Initialize(game_type type)
{
    // 初始化扫描器
    cga::MemoryScanner::Initialize(hGameBase);
    
    // 扫描玩家基址
    auto playerBaseMatch = cga::MemoryScanner::ScanPattern(
        "8B 0D ?? ?? ?? ??",  // mov ecx, [playerBase]
        "xx??????",
        2
    );
    
    if (playerBaseMatch.found) {
        g_playerBase = *(playerbase_t**)(playerBaseMatch.address + playerBaseMatch.offset);
        LOG_INFO("玩家基址扫描成功：0x{:08X}", (uintptr_t)g_playerBase);
    } else {
        // 回退到硬编码
        g_playerBase = CONVERT_GAMEVAR(playerbase_t **, 0xE12C30);
        LOG_WARN("玩家基址扫描失败，使用硬编码地址");
    }
}
```

### 示例 2: 查找地图名称

```cpp
// 扫描地图名称指针
auto mapNameMatch = cga::MemoryScanner::ScanPattern(
    "8D 05 ?? ?? ?? ??",  // lea eax, [mapName]
    "xx??????",
    2
);

if (mapNameMatch.found) {
    g_map_name = *(char**)(mapNameMatch.address + mapNameMatch.offset);
}
```

### 示例 3: 查找战斗状态

```cpp
// 扫描战斗状态
auto battleMatch = cga::MemoryScanner::ScanPattern(
    "83 3D ?? ?? ?? ?? ??",  // cmp [battleStatus], imm8
    "xx?????x",
    2
);

if (battleMatch.found) {
    g_battle_status = *(int*)(battleMatch.address + battleMatch.offset);
}
```

## 调试技巧

### 1. 验证特征码

使用 IDA Pro 或 x64dbg 验证特征码：

```
1. 在 IDA 中定位目标指令
2. 复制机器码 (如：8B 0D 12 34 56 78)
3. 将具体地址替换为 ?? (如：8B 0D ?? ?? ?? ??)
4. 创建掩码 (如：xx??????)
```

### 2. 测试扫描

```cpp
// 测试特征码是否有效
auto match = cga::MemoryScanner::ScanPattern(pattern, mask, offset);
if (match.found) {
    LOG_INFO("扫描成功 - 地址：0x{:08X} 偏移：{}", match.address, match.offset);
    
    // 验证读取
    if (cga::MemoryScanner::IsMemoryReadable(match.address, 4)) {
        LOG_INFO("内存可读 - 值：0x{:08X}", *(DWORD*)match.address);
    }
} else {
    LOG_ERROR("扫描失败 - 特征码：{} 掩码：{}", pattern, mask);
}
```

### 3. 日志分析

```bash
# 查找所有扫描成功
grep "扫描成功" logs/cga.log

# 查找扫描失败
grep "扫描失败" logs/cga.log

# 统计回退次数
grep "使用硬编码" logs/cga.log | wc -l
```

## 性能优化

### 1. 缓存结果

```cpp
// 全局变量缓存
static uintptr_t g_playerBaseAddr = 0;

uintptr_t GetPlayerBase() {
    if (g_playerBaseAddr == 0) {
        auto match = cga::MemoryScanner::ScanPattern(...);
        if (match.found) {
            g_playerBaseAddr = match.address + match.offset;
        }
    }
    return g_playerBaseAddr;
}
```

### 2. 批量扫描

```cpp
// 一次性扫描多个地址
struct AddressCache {
    uintptr_t playerBase;
    uintptr_t mapName;
    uintptr_t battleStatus;
};

void ScanAllAddresses(AddressCache& cache) {
    cache.playerBase = ScanPattern(...).address;
    cache.mapName = ScanPattern(...).address;
    cache.battleStatus = ScanPattern(...).address;
}
```

### 3. 异步扫描

```cpp
// 在后台线程扫描
std::async(std::launch::async, []() {
    auto match = cga::MemoryScanner::ScanPattern(...);
    // 处理结果...
});
```

## 常见问题

### Q: 扫描失败怎么办？

**A**: 
1. 检查特征码是否正确
2. 确认游戏版本是否匹配
3. 尝试更长的特征码 (提高唯一性)
4. 回退到硬编码地址

### Q: 如何选择特征码长度？

**A**:
- 太短：可能匹配多个位置
- 太长：游戏更新后容易失效
- 推荐：6-10 字节 (平衡唯一性和稳定性)

### Q: 扫描速度慢？

**A**:
- 模块太大时扫描较慢 (100-500ms)
- 建议：缓存结果，避免重复扫描
- 优化：缩小扫描范围

### Q: 如何更新特征码？

**A**:
1. 用 IDA 打开新游戏版本
2. 定位目标指令
3. 复制新机器码
4. 更新特征码字符串

## 最佳实践

### ✅ 推荐

```cpp
// 1. 始终检查扫描结果
auto match = ScanPattern(...);
if (match.found) {
    // 使用扫描结果
} else {
    // 回退到硬编码
}

// 2. 记录日志
LOG_INFO("扫描{} - 地址：0x{:08X}", name, match.address);

// 3. 使用有意义的特征码名
constexpr const char* PLAYER_BASE_PATTERN = "8B 0D ?? ?? ?? ??";
```

### ❌ 避免

```cpp
// 1. 不检查扫描结果
auto match = ScanPattern(...);
uintptr_t ptr = match.address;  // 可能为 0!

// 2. 特征码太短
ScanPattern("8B 0D", "xx");  // 可能匹配多个位置

// 3. 重复扫描
for (int i = 0; i < 100; i++) {
    ScanPattern(...);  // 应该缓存结果!
}
```

---

**最后更新**: 2026-04-22  
**版本**: 1.0
