# CGAssistant JavaScript API 参考

完整的 JavaScript 脚本 API 文档，包含所有可用的函数、参数和使用示例。

---

## 目录

1. [基础 API](#基础-api)
2. [玩家 API](#玩家-api)
3. [战斗 API](#战斗-api)
4. [物品 API](#物品-api)
5. [地图 API](#地图-api)
6. [NPC API](#npc-api)
7. [技能 API](#技能-api)
8. [聊天 API](#聊天-api)
9. [工具函数](#工具函数)
10. [常量定义](#常量定义)
11. [完整示例](#完整示例)

---

## 基础 API

### cga.log(message)

输出日志信息到控制台。

**参数**:
- `message` (string): 日志消息

**示例**:
```javascript
cga.log("脚本开始执行");
cga.log("玩家等级：" + player.level);
```

---

### cga.delay(ms)

延迟指定毫秒数。

**参数**:
- `ms` (number): 延迟毫秒数

**示例**:
```javascript
cga.delay(1000);  // 延迟 1 秒
cga.delay(500);   // 延迟 0.5 秒
```

---

### cga.isInGame()

检查是否在游戏中。

**返回**: `boolean` - 是否在游戏中

**示例**:
```javascript
if (cga.isInGame()) {
    cga.log("已在游戏中");
} else {
    cga.log("未进入游戏");
}
```

---

### cga.isInBattle()

检查是否在战斗中。

**返回**: `boolean` - 是否在战斗中

**示例**:
```javascript
if (cga.isInBattle()) {
    cga.log("正在战斗中");
}
```

---

### cga.isMoving()

检查是否正在移动。

**返回**: `boolean` - 是否正在移动

**示例**:
```javascript
if (cga.isMoving()) {
    cga.delay(1000);
}
```

---

### cga.isWorking()

检查是否正在工作中 (采集、制造等)。

**返回**: `boolean` - 是否正在工作

**示例**:
```javascript
while (cga.isWorking()) {
    cga.delay(1000);
}
```

---

## 玩家 API

### cga.getPlayerInfo()

获取玩家信息。

**返回**: `Object` - 玩家信息对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| name | string | 角色名 |
| job | string | 职业 |
| level | number | 等级 |
| gold | number | 金币 |
| hp | number | 当前 HP |
| maxhp | number | 最大 HP |
| mp | number | 当前 MP |
| maxmp | number | 最大 MP |
| xp | number | 当前经验 |
| maxxp | number | 最大经验 |
| unitid | number | 单位 ID |

**示例**:
```javascript
var player = cga.getPlayerInfo();
cga.log("玩家：" + player.name);
cga.log("等级：" + player.level);
cga.log("HP: " + player.hp + "/" + player.maxhp);
cga.log("MP: " + player.mp + "/" + player.maxmp);
```

---

### cga.getPetInfo(index)

获取指定索引的宠物信息。

**参数**:
- `index` (number): 宠物索引 (0-4)

**返回**: `Object` - 宠物信息对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| id | number | 宠物 ID |
| name | string | 昵称 |
| realname | string | 真名 |
| level | number | 等级 |
| hp | number | 当前 HP |
| maxhp | number | 最大 HP |
| mp | number | 当前 MP |
| maxmp | number | 最大 MP |
| xp | number | 当前经验 |
| maxxp | number | 最大经验 |
| loyalty | number | 忠诚度 |
| battle_flags | number | 战斗标志 |

**示例**:
```javascript
for (var i = 0; i < 5; i++) {
    var pet = cga.getPetInfo(i);
    if (pet && pet.name) {
        cga.log("宠物 " + i + ": " + pet.name);
        cga.log("等级：" + pet.level);
        cga.log("HP: " + pet.hp + "/" + pet.maxhp);
    }
}
```

---

### cga.getPetsInfo()

获取所有宠物信息。

**返回**: `Array` - 宠物信息数组

**示例**:
```javascript
var pets = cga.getPetsInfo();
for (var i = 0; i < pets.length; i++) {
    cga.log("宠物 " + i + ": " + pets[i].name);
}
```

---

## 战斗 API

### cga.battleAttack()

普通攻击。

**示例**:
```javascript
if (cga.isInBattle()) {
    cga.battleAttack();
}
```

---

### cga.battleSkillAttack(skillId, level)

技能攻击。

**参数**:
- `skillId` (number): 技能 ID
- `level` (number): 技能等级

**示例**:
```javascript
// 使用技能 ID 为 1 的技能，等级 5
cga.battleSkillAttack(1, 5);
```

---

### cga.battleGuard()

防御。

**示例**:
```javascript
cga.battleGuard();
```

---

### cga.battleEscape()

逃跑。

**示例**:
```javascript
cga.battleEscape();
```

---

### cga.battleChangePet(petId)

更换宠物。

**参数**:
- `petId` (number): 宠物 ID (0-4, 255=召回)

**示例**:
```javascript
// 更换为第 1 个宠物
cga.battleChangePet(0);

// 召回宠物
cga.battleChangePet(255);
```

---

### cga.battleUseItem(itemPos)

战斗中使用物品。

**参数**:
- `itemPos` (number): 物品位置

**示例**:
```javascript
cga.battleUseItem(10);  // 使用位置 10 的物品
```

---

### cga.EnableFlags(flag, enable)

启用/禁用标志。

**参数**:
- `flag` (number): 标志 ID
- `enable` (boolean): 是否启用

**示例**:
```javascript
// 允许组队
cga.EnableFlags(cga.ENABLE_FLAG_JOINTEAM, true);

// 允许交易
cga.EnableFlags(cga.ENABLE_FLAG_TRADE, true);
```

---

### cga.SetAutoBattle(enabled)

设置自动战斗。

**参数**:
- `enabled` (boolean): 是否启用自动战斗

**示例**:
```javascript
cga.SetAutoBattle(true);
```

---

### cga.AddBattleAction(config)

添加战斗动作配置。

**参数**:
- `config` (Object): 战斗配置对象

**配置对象属性**:
```javascript
{
    condition: {
        type: number,    // 条件类型
        op: number,      // 操作符
        value: number    // 值
    },
    action: {
        type: number,    // 动作类型
        target: number,  // 目标
        item: string     // 物品名称 (如果使用物品)
    }
}
```

**示例**:
```javascript
// HP < 30% 时使用血瓶
cga.AddBattleAction({
    condition: {
        type: cga.COND_PLAYER_HP,
        op: cga.OP_LT,
        value: 30
    },
    action: {
        type: cga.ACT_USE_ITEM,
        item: "血瓶"
    }
});

// 敌人数量 > 5 时逃跑
cga.AddBattleAction({
    condition: {
        type: cga.COND_ENEMY_COUNT,
        op: cga.OP_GT,
        value: 5
    },
    action: {
        type: cga.ACT_ESCAPE
    }
});
```

---

## 物品 API

### cga.getItems()

获取物品列表。

**返回**: `Array` - 物品信息数组

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| itemid | number | 物品 ID |
| type | number | 物品类型 |
| count | number | 数量 |
| pos | number | 位置 |
| name | string | 名称 |
| attr | string | 属性 |
| info | string | 信息 |
| assessed | boolean | 已鉴定 |

**示例**:
```javascript
var items = cga.getItems();
for (var i = 0; i < items.length; i++) {
    cga.log("物品：" + items[i].name);
    cga.log("数量：" + items[i].count);
    cga.log("位置：" + items[i].pos);
}
```

---

### cga.useItem(pos)

使用物品。

**参数**:
- `pos` (number): 物品位置

**示例**:
```javascript
// 使用位置 10 的物品
cga.useItem(10);

// 查找并使用血瓶
var items = cga.getItems();
for (var i = 0; i < items.length; i++) {
    if (items[i].name.contains("血瓶")) {
        cga.useItem(items[i].pos);
        break;
    }
}
```

---

### cga.dropItem(pos)

丢弃物品。

**参数**:
- `pos` (number): 物品位置

**示例**:
```javascript
cga.dropItem(10);  // 丢弃位置 10 的物品
```

---

### cga.moveItem(srcPos, dstPos, count)

移动物品。

**参数**:
- `srcPos` (number): 源位置
- `dstPos` (number): 目标位置
- `count` (number): 数量

**示例**:
```javascript
cga.moveItem(10, 20, 5);  // 从位置 10 移动 5 个物品到位置 20
```

---

## 地图 API

### cga.getPosition()

获取当前位置。

**返回**: `Object` - 位置对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| x | number | X 坐标 |
| y | number | Y 坐标 |

**示例**:
```javascript
var pos = cga.getPosition();
cga.log("当前位置：(" + pos.x + ", " + pos.y + ")");
```

---

### cga.getMapName()

获取地图名称。

**返回**: `string` - 地图名称

**示例**:
```javascript
var mapName = cga.getMapName();
cga.log("当前地图：" + mapName);
```

---

### cga.walkTo(x, y)

移动到坐标。

**参数**:
- `x` (number): 目标 X 坐标
- `y` (number): 目标 Y 坐标

**示例**:
```javascript
cga.walkTo(50, 50);  // 移动到 (50, 50)
```

---

### cga.turnTo(x, y)

转向坐标。

**参数**:
- `x` (number): 目标 X 坐标
- `y` (number): 目标 Y 坐标

**示例**:
```javascript
cga.turnTo(51, 50);  // 转向右边
```

---

### cga.isPassable(x, y)

检查坐标是否可通过。

**参数**:
- `x` (number): X 坐标
- `y` (number): Y 坐标

**返回**: `boolean` - 是否可通过

**示例**:
```javascript
if (cga.isPassable(51, 50)) {
    cga.walkTo(51, 50);
}
```

---

### cga.getDistance(x1, y1, x2, y2)

计算两点间距离。

**参数**:
- `x1` (number): 起点 X
- `y1` (number): 起点 Y
- `x2` (number): 终点 X
- `y2` (number): 终点 Y

**返回**: `number` - 距离

**示例**:
```javascript
var dist = cga.getDistance(50, 50, 60, 60);
cga.log("距离：" + dist);
```

---

## NPC API

### cga.clickNpcDialog(option, index)

点击 NPC 对话选项。

**参数**:
- `option` (number): 选项类型
- `index` (number): 选项索引

**示例**:
```javascript
// 选择第 1 个选项
cga.clickNpcDialog(0, 0);

// 选择第 2 个选项
cga.clickNpcDialog(0, 1);
```

---

### cga.sellNpcStore(items)

出售物品给 NPC。

**参数**:
- `items` (Array): 物品数组

**物品对象**:
```javascript
{
    pos: number,   // 物品位置
    count: number  // 数量
}
```

**示例**:
```javascript
cga.sellNpcStore([
    {pos: 10, count: 1},
    {pos: 11, count: 5}
]);
```

---

### cga.buyNpcStore(items)

从 NPC 购买物品。

**参数**:
- `items` (Array): 物品数组

**物品对象**:
```javascript
{
    index: number,  // 商店索引
    count: number   // 数量
}
```

**示例**:
```javascript
cga.buyNpcStore([
    {index: 0, count: 10},  // 购买第 1 个物品 10 个
    {index: 1, count: 5}    // 购买第 2 个物品 5 个
]);
```

---

## 技能 API

### cga.getSkillsInfo()

获取技能列表。

**返回**: `Array` - 技能信息数组

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| id | number | 技能 ID |
| name | string | 技能名称 |
| level | number | 当前等级 |
| maxlv | number | 最大等级 |
| pos | number | 位置 |

**示例**:
```javascript
var skills = cga.getSkillsInfo();
for (var i = 0; i < skills.length; i++) {
    cga.log("技能：" + skills[i].name);
    cga.log("等级：" + skills[i].level + "/" + skills[i].maxlv);
}
```

---

### cga.startWork(skillId, subSkillId)

开始工作 (采集、制造等)。

**参数**:
- `skillId` (number): 技能 ID
- `subSkillId` (number): 子技能 ID

**示例**:
```javascript
// 开始采集
cga.startWork(1, 0);

// 开始制造
cga.startWork(2, 1);
```

---

## 聊天 API

### cga.sayWords(message, color, range, size)

发送聊天消息。

**参数**:
- `message` (string): 消息内容
- `color` (number): 颜色 (0=普通)
- `range` (number): 范围 (3=当前地图)
- `size` (number): 大小 (1=普通)

**示例**:
```javascript
cga.sayWords("你好", 0, 3, 1);
cga.sayWords("组队来人", 0, 3, 1);
```

---

## 工具函数

### cga.getRandomInt(min, max)

生成随机整数。

**参数**:
- `min` (number): 最小值
- `max` (number): 最大值

**返回**: `number` - 随机整数

**示例**:
```javascript
var rand = cga.getRandomInt(1, 100);
cga.log("随机数：" + rand);
```

---

### cga.string.contains(str)

检查字符串是否包含子串。

**参数**:
- `str` (string): 子串

**返回**: `boolean` - 是否包含

**示例**:
```javascript
if ("血瓶".contains("血")) {
    cga.log("包含");
}
```

---

## 常量定义

### 标志常量

```javascript
cga.ENABLE_FLAG_JOINTEAM   = 0  // 允许组队
cga.ENABLE_FLAG_TRADE      = 1  // 允许交易
cga.ENABLE_FLAG_CARD       = 2  // 允许名片
cga.ENABLE_FLAG_PK         = 3  // 允许 PK
cga.ENABLE_FLAG_TEAMCHAT   = 4  // 允许组队聊天
cga.ENABLE_FLAG_FAMILY     = 5  // 允许家族
```

### 战斗条件常量

```javascript
cga.COND_IGNORE            = 0   // 忽略
cga.COND_PLAYER_HP         = 1   // 玩家 HP
cga.COND_PLAYER_MP         = 2   // 玩家 MP
cga.COND_PET_HP            = 3   // 宠物 HP
cga.COND_PET_MP            = 4   // 宠物 MP
cga.COND_ENEMY_COUNT       = 10  // 敌人数量
cga.COND_ROUND             = 17  // 回合数
```

### 战斗操作符常量

```javascript
cga.OP_EGT                 = 0   // >=
cga.OP_GT                  = 1   // >
cga.OP_ELT                 = 2   // <=
cga.OP_LT                  = 3   // <
cga.OP_EQ                  = 4   // =
cga.OP_NEQ                 = 5   // !=
```

### 战斗动作常量

```javascript
cga.ACT_IGNORE             = 0   // 忽略
cga.ACT_ATTACK             = 1   // 攻击
cga.ACT_GUARD              = 2   // 防御
cga.ACT_ESCAPE             = 3   // 逃跑
cga.ACT_CHANGE_PET         = 5   // 换宠
cga.ACT_USE_ITEM           = 6   // 使用物品
cga.ACT_SKILL              = 100 // 技能
```

---

## 完整示例

### 1. 自动打怪

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
            var items = cga.getItems();
            for (var i = 0; i < items.length; i++) {
                if (items[i].name.contains("血瓶")) {
                    cga.useItem(items[i].pos);
                    break;
                }
            }
        }
    }
}

main();
```

---

### 2. 自动任务

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

---

### 3. 自动采集

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
            cga.startWork(1, 0);
            
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

### 4. 自动补给

```javascript
function main() {
    cga.log("开始自动补给");
    
    while (true) {
        var player = cga.getPlayerInfo();
        
        // 检查 HP
        if (player.hp < player.maxhp * 0.5) {
            cga.log("HP 不足，前往医院");
            cga.walkTo(医院 X, 医院 Y);
            cga.delay(2000);
            // 治疗逻辑...
        }
        
        // 检查 MP
        if (player.mp < player.maxmp * 0.3) {
            cga.log("MP 不足，使用魔瓶");
            var items = cga.getItems();
            for (var i = 0; i < items.length; i++) {
                if (items[i].name.contains("魔瓶")) {
                    cga.useItem(items[i].pos);
                    break;
                }
            }
        }
        
        cga.delay(5000);
    }
}

main();
```

---

### 5. 智能战斗

```javascript
function main() {
    cga.log("开始智能战斗");
    
    // 启用自动战斗
    cga.SetAutoBattle(true);
    
    // 配置战斗策略
    cga.AddBattleAction({
        condition: {
            type: cga.COND_PLAYER_HP,
            op: cga.OP_LT,
            value: 30
        },
        action: {
            type: cga.ACT_USE_ITEM,
            item: "血瓶"
        }
    });
    
    cga.AddBattleAction({
        condition: {
            type: cga.COND_PET_HP,
            op: cga.OP_LT,
            value: 50
        },
        action: {
            type: cga.ACT_USE_ITEM,
            item: "宠物食品"
        }
    });
    
    cga.AddBattleAction({
        condition: {
            type: cga.COND_ENEMY_COUNT,
            op: cga.OP_GT,
            value: 5
        },
        action: {
            type: cga.ACT_ESCAPE
        }
    });
    
    cga.log("战斗策略已配置");
}

main();
```

---

## 错误处理

### try-catch

```javascript
try {
    var player = cga.getPlayerInfo();
    cga.log("玩家：" + player.name);
} catch (e) {
    cga.log("错误：" + e);
}
```

### 检查返回值

```javascript
var player = cga.getPlayerInfo();
if (player && player.name) {
    cga.log("玩家：" + player.name);
} else {
    cga.log("获取玩家信息失败");
}
```

---

**最后更新**: 2026-04-22  
**版本**: 1.0  
**状态**: 完整 ✅
