# CGAssistant 使用教程

新手入门指南，从零开始学习使用 CGAssistant。

---

## 目录

1. [快速开始](#快速开始)
2. [界面介绍](#界面介绍)
3. [基本功能](#基本功能)
4. [脚本编写](#脚本编写)
5. [高级功能](#高级功能)
6. [常见问题](#常见问题)

---

## 快速开始

### 1. 下载和安装

#### 方式一：使用编译好的版本

1. 访问 [Releases](https://github.com/hzqst/CGAssistant/releases)
2. 下载最新版本
3. 解压到任意目录
4. 运行 `CGAssistant.exe`

#### 方式二：自行编译

```bash
# 克隆仓库
git clone https://github.com/hzqst/CGAssistant.git
cd CGAssistant

# 编译
./init-dep.bat
./build-boost.bat
./build-CGAHook.bat
./build-CGAssistant.bat
./build-nodegyp.bat
./windeploy.bat
```

### 2. 连接游戏

1. 启动魔力宝贝游戏
2. 运行 CGAssistant
3. 点击 **Attach** 标签
4. 选择游戏进程
5. 点击 **Attach** 按钮

成功连接后，状态栏会显示游戏信息。

---

## 界面介绍

### 主界面

```
┌─────────────────────────────────────────────┐
│  CGAssistant                                │
├─────────────────────────────────────────────┤
│  [Attach] [Player] [Auto Battle] [Item]     │
│  [Map] [Script] [Account] [Chat]            │
├─────────────────────────────────────────────┤
│                                             │
│              当前标签页内容                  │
│                                             │
└─────────────────────────────────────────────┘
```

### 标签页说明

| 标签 | 功能 | 说明 |
|------|------|------|
| **Attach** | 进程连接 | 选择并连接游戏进程 |
| **Player** | 玩家信息 | 查看玩家状态、宠物、技能 |
| **Auto Battle** | 自动战斗 | 配置战斗策略 |
| **Item** | 物品管理 | 查看物品、配置自动丢弃 |
| **Map** | 地图导航 | 地图显示、自动寻路 |
| **Script** | 脚本执行 | 加载和执行 JavaScript 脚本 |
| **Account** | 账号管理 | 多账号配置、自动登录 |
| **Chat** | 聊天系统 | 游戏聊天、消息过滤 |

---

## 基本功能

### 1. 查看玩家信息

1. 切换到 **Player** 标签
2. 查看角色名、等级、HP、MP 等信息
3. 查看宠物列表和技能

### 2. 自动战斗

1. 切换到 **Auto Battle** 标签
2. 勾选 **Auto Battle** 启用自动战斗
3. 配置战斗策略：
   - 条件：HP < 30%
   - 动作：使用血瓶
4. 点击 **Add** 添加策略

**示例配置**:
```
条件 1: 玩家 HP < 30%  → 动作：使用物品 (血瓶)
条件 2: 宠物 HP < 50%  → 动作：使用物品 (宠物食品)
条件 3: 敌人数量 > 3   → 动作：逃跑
```

### 3. 物品管理

#### 自动丢弃

1. 切换到 **Item** 标签
2. 在 **Auto Drop** 输入框输入物品名称或 ID
3. 按回车添加规则

**示例**:
```
# 按名称
哥布林手环
强盾

# 按 ID
#1234
#5678
```

#### 自动整理

1. 在 **Auto Tweak** 输入框输入规则
2. 格式：`物品名称 | 最大堆叠数`

**示例**:
```
血瓶|10
魔瓶|10
料理|50
```

### 4. 地图导航

1. 切换到 **Map** 标签
2. 查看当前地图和坐标
3. 点击地图上的点进行寻路

**快捷键**:
- `Ctrl + 点击`: 强制移动
- `Alt + 点击`: 显示坐标

---

## 脚本编写

### JavaScript 基础

CGAssistant 支持 JavaScript 脚本自动化。

#### 脚本结构

```javascript
// 主函数
function main() {
    cga.log("脚本开始执行");
    
    // 你的代码...
    
    cga.log("脚本执行完成");
}

// 启动脚本
main();
```

#### 常用 API

##### 移动

```javascript
// 移动到坐标
cga.walkTo(50, 50);

// 检查是否在移动
if (cga.isMoving()) {
    cga.delay(1000);
}
```

##### 战斗

```javascript
// 检查是否在战斗
if (cga.isInBattle()) {
    // 普通攻击
    cga.battleAttack();
    
    // 技能攻击
    cga.battleSkillAttack(1, 5);  // 技能 ID, 等级
    
    // 防御
    cga.battleGuard();
    
    // 逃跑
    cga.battleEscape();
}
```

##### 物品

```javascript
// 获取物品列表
var items = cga.getItems();
for (var i = 0; i < items.length; i++) {
    cga.log("物品：" + items[i].name);
}

// 使用物品
cga.useItem(10);  // 物品位置

// 丢弃物品
cga.dropItem(10);
```

##### NPC

```javascript
// 点击 NPC 对话
cga.clickNpcDialog(0, 0);  // 选项，索引
```

### 实用脚本示例

#### 1. 自动打怪

```javascript
function main() {
    cga.log("开始自动打怪");
    
    // 移动到遇敌点
    cga.walkTo(50, 50);
    
    while (true) {
        // 等待遇敌
        while (!cga.isInBattle()) {
            cga.delay(1000);
        }
        
        cga.log("进入战斗");
        
        // 自动战斗
        while (cga.isInBattle()) {
            cga.battleAttack();
            cga.delay(500);
        }
        
        cga.log("战斗结束");
        
        // 检查 HP
        var player = cga.getPlayerInfo();
        if (player.hp < player.maxhp * 0.3) {
            cga.log("HP 不足，使用血瓶");
            // 使用血瓶逻辑...
        }
    }
}

main();
```

#### 2. 自动任务

```javascript
function main() {
    cga.log("开始自动任务");
    
    // 移动到 NPC
    cga.walkTo(100, 100);
    cga.delay(2000);
    
    // 与 NPC 对话
    cga.clickNpcDialog(0, 0);
    cga.delay(1000);
    
    // 选择选项
    cga.clickNpcDialog(0, 1);
    cga.delay(1000);
    
    cga.log("任务完成");
}

main();
```

#### 3. 自动采集

```javascript
function main() {
    cga.log("开始自动采集");
    
    var gatherPoints = [
        {x: 50, y: 50},
        {x: 60, y: 60},
        {x: 70, y: 70}
    ];
    
    while (true) {
        for (var i = 0; i < gatherPoints.length; i++) {
            var point = gatherPoints[i];
            
            // 移动到采集点
            cga.walkTo(point.x, point.y);
            cga.delay(2000);
            
            // 开始采集
            cga.startWork(1, 0);  // 技能 ID, 子技能 ID
            
            // 等待采集完成
            while (cga.isWorking()) {
                cga.delay(1000);
            }
        }
    }
}

main();
```

---

## 高级功能

### 1. 战斗策略配置

#### 条件类型

| ID | 名称 | 说明 |
|----|------|------|
| 1 | Player HP | 玩家 HP |
| 2 | Player MP | 玩家 MP |
| 3 | Pet HP | 宠物 HP |
| 4 | Pet MP | 宠物 MP |
| 10 | Enemy Count | 敌人数量 |
| 17 | Round | 回合数 |

#### 动作类型

| ID | 名称 | 说明 |
|----|------|------|
| 1 | Attack | 普通攻击 |
| 2 | Guard | 防御 |
| 3 | Escape | 逃跑 |
| 5 | Change Pet | 换宠 |
| 6 | Use Item | 使用物品 |
| 100 | Skill | 技能攻击 |

### 2. 性能监控

```javascript
// 启用性能监控
cga.enablePerfMonitor(true);

// 定期打印统计
setInterval(function() {
    cga.printPerfStats();
}, 60000);  // 每分钟
```

### 3. 多账号管理

1. 切换到 **Account** 标签
2. 配置账号信息
3. 设置自动登录
4. 勾选 **Auto Login**

---

## 常见问题

### Q: 无法连接游戏？

**A**: 
1. 确保游戏已启动
2. 以管理员身份运行 CGAssistant
3. 检查杀毒软件是否拦截
4. 确认游戏版本兼容

### Q: 脚本不执行？

**A**:
1. 检查脚本语法
2. 确认 Node.js 已安装
3. 查看日志输出
4. 检查游戏是否在战斗中

### Q: 自动战斗不工作？

**A**:
1. 确认已勾选 **Auto Battle**
2. 检查战斗策略配置
3. 确认角色状态正常
4. 查看日志是否有错误

### Q: 性能下降？

**A**:
1. 减少日志输出
2. 优化脚本逻辑
3. 启用性能监控
4. 调整缓存设置

### Q: 如何反馈问题？

**A**:
1. 访问 [Issues](https://github.com/hzqst/CGAssistant/issues)
2. 提供详细信息：
   - 问题描述
   - 复现步骤
   - 日志信息
   - 截图/视频

---

## 下一步

- [ ] 阅读 [API 参考](API_REFERENCE.md)
- [ ] 学习 [架构设计](LAYERED_ARCHITECTURE.md)
- [ ] 查看 [性能优化](PERFORMANCE_OPTIMIZATION.md)
- [ ] 加入 QQ 群：300303705

---

**最后更新**: 2026-04-22  
**版本**: 1.0  
**状态**: 完整 ✅
