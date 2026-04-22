# CGAssistant JavaScript API 完整参考

本文档基于 CGANode 模块源代码编写，包含所有可用的 JavaScript API 接口。

---

## 目录

1. [连接与状态 API](#连接与状态-api)
2. [玩家信息 API](#玩家信息-api)
3. [宠物信息 API](#宠物信息-api)
4. [物品管理 API](#物品管理-api)
5. [技能信息 API](#技能信息-api)
6. [地图导航 API](#地图导航-api)
7. [战斗系统 API](#战斗系统-api)
8. [NPC 交互 API](#npc 交互-api)
9. [工作制造 API](#工作制造-api)
10. [聊天系统 API](#聊天系统-api)
11. [交易系统 API](#交易系统-api)
12. [卡片邮件 API](#卡片邮件-api)
13. [工具函数 API](#工具函数-api)
14. [异步回调 API](#异步回调-api)
15. [常量定义](#常量定义)
16. [完整示例](#完整示例)

---

## 连接与状态 API

### cga.IsConnected()

检查是否已连接到游戏。

**返回**: `boolean`

**示例**:
```javascript
if (cga.IsConnected()) {
    cga.log("已连接到游戏");
}
```

---

### cga.IsInGame()

检查是否在游戏中。

**返回**: `boolean`

**示例**:
```javascript
if (cga.IsInGame()) {
    cga.log("已在游戏中");
}
```

---

### cga.GetWorldStatus()

获取世界状态。

**返回**: `number` - 世界状态值

**状态值**:
| 值 | 说明 |
|----|------|
| 1 | 登录界面 |
| 2 | 选择服务器 |
| 6 | 选择角色 |
| 9 | 游戏中 |
| 10 | 战斗中 |
| 11 | 未连接 |

**示例**:
```javascript
var status = cga.GetWorldStatus();
cga.log("世界状态：" + status);
```

---

### cga.GetGameStatus()

获取游戏状态。

**返回**: `number` - 游戏状态值

**示例**:
```javascript
var status = cga.GetGameStatus();
cga.log("游戏状态：" + status);
```

---

### cga.GetBGMIndex()

获取背景音乐索引。

**返回**: `number` - BGM 索引

**示例**:
```javascript
var bgm = cga.GetBGMIndex();
cga.log("当前 BGM: " + bgm);
```

---

### cga.GetSysTime()

获取系统时间。

**返回**: `Object` - 时间对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| years | number | 年 |
| month | number | 月 |
| days | number | 日 |
| hours | number | 时 |
| mins | number | 分 |
| secs | number | 秒 |
| local_time | number | 本地时间戳 |
| server_time | number | 服务器时间戳 |

**示例**:
```javascript
var time = cga.GetSysTime();
cga.log("时间：" + time.years + "-" + time.month + "-" + time.days);
cga.log(" " + time.hours + ":" + time.mins + ":" + time.secs);
```

---

### cga.LogBack()

返回登录界面/回城。

**返回**: `boolean`

**说明**: 
- 游戏中使用可返回登录界面
- 在城内使用可回到主城登录点

**示例**:
```javascript
// 返回登录界面
cga.LogBack();

// 回城 (在城内使用)
if (cga.IsInGame()) {
    cga.LogBack();
}
```

---

### cga.LogOut()

退出游戏。

**返回**: `boolean`

**示例**:
```javascript
cga.LogOut();
```

---

## 玩家信息 API

### cga.GetPlayerInfo()

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
| petid | number | 出战宠物 ID |
| direction | number | 面向方向 |
| battle_position | number | 战斗位置 |
| punchclock | number | 打卡时间 |
| usingpunchclock | boolean | 是否打卡中 |
| health | number | 健康值 |
| souls | number | 灵魂数 |
| score | number | 声望 |
| skillslots | number | 技能栏位 |
| use_title | number | 使用称号 |
| avatar_id | number | 头像 ID |
| image_id | number | 形象 ID |
| titles | Array | 称号列表 |
| nick | string | 昵称 |
| manu_endurance | number | 手制耐久 |
| manu_skillful | number | 手制熟练 |
| manu_intelligence | number | 手制智力 |
| value_charisma | number | 魅力值 |
| detail.points_remain | number | 剩余点数 |
| detail.points_endurance | number | 耐力 |
| detail.points_strength | number | 力量 |
| detail.points_defense | number | 防御 |
| detail.points_agility | number | 敏捷 |
| detail.points_magical | number | 魔法 |

**示例**:
```javascript
var player = cga.GetPlayerInfo();
cga.log("玩家：" + player.name);
cga.log("职业：" + player.job);
cga.log("等级：" + player.level);
cga.log("HP: " + player.hp + "/" + player.maxhp);
cga.log("MP: " + player.mp + "/" + player.maxmp);
cga.log("金币：" + player.gold);
```

---

### cga.SetPlayerFlagEnabled(index, enable)

设置玩家标志启用状态。

**参数**:
- `index` (number): 标志索引
- `enable` (boolean): 是否启用

**标志索引**:
| 索引 | 说明 |
|------|------|
| 0 | PK 开关 |
| 1 | 组队聊天 |
| 2 | 允许组队 |
| 3 | 允许名片 |
| 4 | 允许交易 |
| 5 | 允许家族 |

**示例**:
```javascript
// 允许组队
cga.SetPlayerFlagEnabled(2, true);

// 允许交易
cga.SetPlayerFlagEnabled(4, true);
```

---

### cga.IsPlayerFlagEnabled(index)

检查玩家标志是否启用。

**参数**:
- `index` (number): 标志索引

**返回**: `boolean`

**示例**:
```javascript
if (cga.IsPlayerFlagEnabled(2)) {
    cga.log("已允许组队");
}
```

---

### cga.ChangeNickName(name)

修改昵称。

**参数**:
- `name` (string): 新昵称

**返回**: `boolean`

**示例**:
```javascript
cga.ChangeNickName("新昵称");
```

---

### cga.ChangeTitleName(titleId)

修改称号。

**参数**:
- `titleId` (number): 称号 ID

**返回**: `boolean`

**示例**:
```javascript
cga.ChangeTitleName(1);
```

---

### cga.ChangePetName(petId, name)

修改宠物名字。

**参数**:
- `petId` (number): 宠物 ID (0-4)
- `name` (string): 新名字

**返回**: `boolean`

**示例**:
```javascript
cga.ChangePetName(0, "新名字");
```

---

## 宠物信息 API

### cga.IsPetValid(index)

检查宠物是否有效。

**参数**:
- `index` (number): 宠物索引 (0-4, 100-104 为银行)

**返回**: `boolean`

**示例**:
```javascript
if (cga.IsPetValid(0)) {
    cga.log("第 1 个宠物有效");
}
```

---

### cga.GetPetInfo(index)

获取宠物信息。

**参数**:
- `index` (number): 宠物索引 (0-4)

**返回**: `Object` - 宠物信息对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| index | number | 宠物索引 |
| name | string | 昵称 |
| realname | string | 真名 |
| level | number | 等级 |
| hp | number | 当前 HP |
| maxhp | number | 最大 HP |
| mp | number | 当前 MP |
| maxmp | number | 最大 MP |
| xp | number | 当前经验 |
| maxxp | number | 最大经验 |
| flags | number | 标志 |
| battle_flags | number | 战斗标志 |
| loyality | number | 忠诚度 |
| state | number | 状态 |
| race | number | 种族 |
| skillslots | number | 技能栏位 |
| detail.points_remain | number | 剩余点数 |
| detail.points_endurance | number | 耐力 |
| detail.points_strength | number | 力量 |
| detail.points_defense | number | 防御 |
| detail.points_agility | number | 敏捷 |
| detail.points_magical | number | 魔法 |
| detail.value_attack | number | 攻击 |
| detail.value_defensive | number | 防御 |
| detail.value_agility | number | 敏捷 |
| detail.value_spirit | number | 精神 |
| detail.value_recovery | number | 恢复 |
| detail.element_earth | number | 地克 |
| detail.element_water | number | 水克 |
| detail.element_fire | number | 火克 |
| detail.element_wind | number | 风克 |

**示例**:
```javascript
var pet = cga.GetPetInfo(0);
if (pet && pet.name) {
    cga.log("宠物：" + pet.name);
    cga.log("等级：" + pet.level);
    cga.log("HP: " + pet.hp + "/" + pet.maxhp);
    cga.log("忠诚度：" + pet.loyality);
}
```

---

### cga.GetPetsInfo()

获取所有宠物信息。

**返回**: `Array` - 宠物信息数组

**示例**:
```javascript
var pets = cga.GetPetsInfo();
for (var i = 0; i < pets.length; i++) {
    if (pets[i].name) {
        cga.log("宠物 " + i + ": " + pets[i].name);
    }
}
```

---

### cga.GetPetSkillsInfo(petId)

获取宠物技能信息。

**参数**:
- `petId` (number): 宠物 ID

**返回**: `Array` - 宠物技能数组

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| name | string | 技能名称 |
| info | string | 技能信息 |
| cost | number | 消耗 MP |
| flags | number | 技能标志 |
| index | number | 技能索引 |

**示例**:
```javascript
var skills = cga.GetPetSkillsInfo(0);
for (var i = 0; i < skills.length; i++) {
    cga.log("技能：" + skills[i].name);
    cga.log("消耗：" + skills[i].cost + " MP");
}
```

---

### cga.GetBankPetsInfo()

获取银行宠物信息。

**返回**: `Array` - 银行宠物信息数组

**示例**:
```javascript
var bankPets = cga.GetBankPetsInfo();
for (var i = 0; i < bankPets.length; i++) {
    if (bankPets[i].name) {
        cga.log("银行宠物：" + bankPets[i].name);
    }
}
```

---

### cga.ChangePetState(petId, state)

改变宠物状态。

**参数**:
- `petId` (number): 宠物 ID (0-4)
- `state` (number): 状态值

**状态值**:
| 值 | 说明 |
|----|------|
| 1 | 准备 |
| 2 | 战斗 |
| 3 | 休息 |

**示例**:
```javascript
// 设置为战斗状态
cga.ChangePetState(0, 2);
```

---

### cga.DropPet(petPos)

丢弃宠物。

**参数**:
- `petPos` (number): 宠物位置

**返回**: `boolean`

**示例**:
```javascript
cga.DropPet(0);  // 丢弃第 1 个宠物
```

---

## 物品管理 API

### cga.IsItemValid(pos)

检查物品是否有效。

**参数**:
- `pos` (number): 物品位置

**返回**: `boolean`

**示例**:
```javascript
if (cga.IsItemValid(10)) {
    cga.log("位置 10 的物品有效");
}
```

---

### cga.GetItemInfo(pos)

获取物品信息。

**参数**:
- `pos` (number): 物品位置

**返回**: `Object` - 物品信息对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| name | string | 物品名称 |
| attr | string | 属性描述 |
| info | string | 详细信息 |
| itemid | number | 物品 ID |
| count | number | 数量 |
| pos | number | 位置 |
| level | number | 等级 |
| type | number | 类型 |
| assessed | boolean | 已鉴定 |

**示例**:
```javascript
var item = cga.GetItemInfo(10);
if (item && item.name) {
    cga.log("物品：" + item.name);
    cga.log("数量：" + item.count);
    cga.log("属性：" + item.attr);
}
```

---

### cga.GetItemsInfo()

获取所有物品信息。

**返回**: `Array` - 物品信息数组

**示例**:
```javascript
var items = cga.GetItemsInfo();
for (var i = 0; i < items.length; i++) {
    cga.log("物品：" + items[i].name);
    cga.log("数量：" + items[i].count);
}
```

---

### cga.GetBankItemsInfo()

获取银行物品信息。

**返回**: `Array` - 银行物品信息数组

**示例**:
```javascript
var bankItems = cga.GetBankItemsInfo();
cga.log("银行物品数量：" + bankItems.length);
```

---

### cga.GetBankGold()

获取银行存款。

**返回**: `number` - 金币数量

**示例**:
```javascript
var gold = cga.GetBankGold();
cga.log("银行存款：" + gold);
```

---

### cga.UseItem(pos)

使用物品。

**参数**:
- `pos` (number): 物品位置

**返回**: `boolean`

**示例**:
```javascript
// 使用位置 10 的物品
cga.UseItem(10);

// 查找并使用血瓶
var items = cga.GetItemsInfo();
for (var i = 0; i < items.length; i++) {
    if (items[i].name.contains("血瓶")) {
        cga.UseItem(items[i].pos);
        break;
    }
}
```

---

### cga.DropItem(pos)

丢弃物品。

**参数**:
- `pos` (number): 物品位置

**返回**: `boolean`

**示例**:
```javascript
cga.DropItem(10);
```

---

### cga.MoveItem(srcPos, dstPos, count)

移动物品。

**参数**:
- `srcPos` (number): 源位置
- `dstPos` (number): 目标位置
- `count` (number): 数量

**返回**: `boolean`

**示例**:
```javascript
cga.MoveItem(10, 20, 5);  // 从位置 10 移动 5 个到位置 20
```

---

### cga.MovePet(srcPos, dstPos)

移动宠物。

**参数**:
- `srcPos` (number): 源位置
- `dstPos` (number): 目标位置

**返回**: `boolean`

**示例**:
```javascript
cga.MovePet(0, 1);  // 从位置 0 移动到位置 1
```

---

### cga.MoveGold(gold, operation)

移动金币。

**参数**:
- `gold` (number): 金币数量
- `operation` (number): 操作类型

**操作类型**:
| 值 | 说明 |
|----|------|
| 0 | 存钱 |
| 1 | 取钱 |
| 2 | 丢弃 |

**返回**: `boolean`

**示例**:
```javascript
// 存 1000 金币
cga.MoveGold(1000, 0);

// 取 500 金币
cga.MoveGold(500, 1);
```

---

## 技能信息 API

### cga.IsSkillValid(skillId)

检查技能是否有效。

**参数**:
- `skillId` (number): 技能 ID

**返回**: `boolean`

**示例**:
```javascript
if (cga.IsSkillValid(1)) {
    cga.log("技能 1 有效");
}
```

---

### cga.GetSkillInfo(skillId)

获取技能信息。

**参数**:
- `skillId` (number): 技能 ID

**返回**: `Object` - 技能信息对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| name | string | 技能名称 |
| lv | number | 当前等级 |
| maxlv | number | 最大等级 |
| xp | number | 当前经验 |
| maxxp | number | 最大经验 |
| type | number | 技能类型 |
| skill_id | number | 技能 ID |
| pos | number | 位置 |
| index | number | 索引 |
| slotsize | number | 子技能数量 |

**示例**:
```javascript
var skill = cga.GetSkillInfo(1);
if (skill && skill.name) {
    cga.log("技能：" + skill.name);
    cga.log("等级：" + skill.lv + "/" + skill.maxlv);
}
```

---

### cga.GetSkillsInfo()

获取所有技能信息。

**返回**: `Array` - 技能信息数组

**示例**:
```javascript
var skills = cga.GetSkillsInfo();
for (var i = 0; i < skills.length; i++) {
    cga.log("技能：" + skills[i].name);
    cga.log("等级：" + skills[i].lv + "/" + skills[i].maxlv);
}
```

---

### cga.GetSubSkillInfo(skillId, stage)

获取子技能信息。

**参数**:
- `skillId` (number): 技能 ID
- `stage` (number): 子技能阶段

**返回**: `Object` - 子技能信息对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| name | string | 子技能名称 |
| info | string | 子技能信息 |
| cost | number | 消耗 MP |
| flags | number | 技能标志 |
| level | number | 需要等级 |
| available | boolean | 是否可用 |

**示例**:
```javascript
var subSkill = cga.GetSubSkillInfo(1, 0);
if (subSkill && subSkill.name) {
    cga.log("子技能：" + subSkill.name);
    cga.log("消耗：" + subSkill.cost + " MP");
}
```

---

### cga.GetSubSkillsInfo(skillId)

获取所有子技能信息。

**参数**:
- `skillId` (number): 技能 ID

**返回**: `Array` - 子技能信息数组

**示例**:
```javascript
var subSkills = cga.GetSubSkillsInfo(1);
for (var i = 0; i < subSkills.length; i++) {
    cga.log("子技能 " + i + ": " + subSkills[i].name);
}
```

---

### cga.GetCraftInfo(skillIndex, subSkillIndex)

获取制造信息。

**参数**:
- `skillIndex` (number): 技能索引
- `subSkillIndex` (number): 子技能索引

**返回**: `Object` - 制造信息对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| id | number | 制造 ID |
| cost | number | 消耗 |
| level | number | 需要等级 |
| itemid | number | 产物 ID |
| index | number | 索引 |
| name | string | 制造名称 |
| info | string | 制造信息 |
| available | boolean | 是否可用 |
| materials | Array | 材料列表 |

**示例**:
```javascript
var craft = cga.GetCraftInfo(2, 0);
if (craft && craft.name) {
    cga.log("制造：" + craft.name);
    cga.log("需要等级：" + craft.level);
}
```

---

### cga.GetCraftsInfo(skillIndex)

获取所有制造信息。

**参数**:
- `skillIndex` (number): 技能索引

**返回**: `Array` - 制造信息数组

**示例**:
```javascript
var crafts = cga.GetCraftsInfo(2);
for (var i = 0; i < crafts.length; i++) {
    cga.log("制造：" + crafts[i].name);
}
```

---

### cga.GetCraftStatus()

获取制造状态。

**返回**: `number` - 制造状态

**示例**:
```javascript
var status = cga.GetCraftStatus();
cga.log("制造状态：" + status);
```

---

### cga.StartWork(skillId, subSkillId)

开始工作 (采集/制造)。

**参数**:
- `skillId` (number): 技能 ID
- `subSkillId` (number): 子技能 ID

**返回**: `boolean`

**示例**:
```javascript
// 开始采集
cga.StartWork(1, 0);

// 开始制造
cga.StartWork(2, 1);
```

---

### cga.CraftItem(craft)

制造物品。

**参数**:
- `craft` (Object): 制造配置对象

**配置对象属性**:
```javascript
{
    skill_index: number,    // 技能索引
    subskill_index: number, // 子技能索引
    itempos: [number]       // 物品位置数组 (最多 6 个)
}
```

**返回**: `boolean`

**示例**:
```javascript
cga.CraftItem({
    skill_index: 2,
    subskill_index: 0,
    itempos: [10, 11, 12, 13, 14, 15]
});
```

---

### cga.AssessItem(skillIndex, itemPos)

鉴定物品。

**参数**:
- `skillIndex` (number): 技能索引
- `itemPos` (number): 物品位置

**返回**: `boolean`

**示例**:
```javascript
cga.AssessItem(3, 10);  // 使用技能 3 鉴定位置 10 的物品
```

---

### cga.SetImmediateDoneWork(enable)

设置立即完成工作。

**参数**:
- `enable` (boolean): 是否启用

**示例**:
```javascript
cga.SetImmediateDoneWork(true);
```

---

### cga.GetImmediateDoneWorkState()

获取立即完成工作状态。

**返回**: `number` - 状态值

**示例**:
```javascript
var state = cga.GetImmediateDoneWorkState();
cga.log("立即完成工作状态：" + state);
```

---

## 地图导航 API

### cga.GetMapIndex()

获取地图索引。

**返回**: `Array` - [index1, index2, index3]

**示例**:
```javascript
var index = cga.GetMapIndex();
cga.log("地图索引：" + index[0] + ", " + index[1] + ", " + index[2]);
```

---

### cga.GetMapXY()

获取当前坐标 (整数)。

**返回**: `Array` - [x, y]

**示例**:
```javascript
var pos = cga.GetMapXY();
cga.log("坐标：" + pos[0] + ", " + pos[1]);
```

---

### cga.GetMapXYFloat()

获取当前坐标 (浮点数)。

**返回**: `Array` - [x, y]

**示例**:
```javascript
var pos = cga.GetMapXYFloat();
cga.log("精确坐标：" + pos[0].toFixed(2) + ", " + pos[1].toFixed(2));
```

---

### cga.GetMoveSpeed()

获取移动速度。

**返回**: `Array` - [xSpeed, ySpeed]

**示例**:
```javascript
var speed = cga.GetMoveSpeed();
cga.log("移动速度：" + speed[0] + ", " + speed[1]);
```

---

### cga.GetMapName()

获取地图名称。

**返回**: `string`

**示例**:
```javascript
var mapName = cga.GetMapName();
cga.log("当前地图：" + mapName);
```

---

### cga.GetMapUnits()

获取地图单位。

**返回**: `Array` - 地图单位数组

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| valid | number | 有效性 |
| type | number | 类型 |
| unit_id | number | 单位 ID |
| model_id | number | 模型 ID |
| xpos | number | X 坐标 |
| ypos | number | Y 坐标 |
| item_count | number | 物品数量 |
| injury | number | 伤害 |
| level | number | 等级 |
| flags | number | 标志 |
| unit_name | string | 单位名称 |
| nick_name | string | 昵称 |
| title_name | string | 称号 |
| item_name | string | 物品名称 |

**示例**:
```javascript
var units = cga.GetMapUnits();
for (var i = 0; i < units.length; i++) {
    if (units[i].unit_name) {
        cga.log("单位：" + units[i].unit_name);
        cga.log("坐标：" + units[i].xpos + ", " + units[i].ypos);
    }
}
```

---

### cga.GetMapCollisionTableRaw(loadAll)

获取原始碰撞表。

**参数**:
- `loadAll` (boolean): 是否加载全部

**返回**: `Object` - 碰撞表对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| x_bottom | number | 起始 X |
| y_bottom | number | 起始 Y |
| x_size | number | X 大小 |
| y_size | number | Y 大小 |
| cell | Array | 碰撞数据 |

**示例**:
```javascript
var table = cga.GetMapCollisionTableRaw(false);
cga.log("碰撞表大小：" + table.x_size + "x" + table.y_size);
```

---

### cga.GetMapCollisionTable(loadAll)

获取碰撞表。

**参数**:
- `loadAll` (boolean): 是否加载全部

**返回**: `Object` - 碰撞表对象

**示例**:
```javascript
var table = cga.GetMapCollisionTable(false);
```

---

### cga.GetMapObjectTable(loadAll)

获取物体表。

**参数**:
- `loadAll` (boolean): 是否加载全部

**返回**: `Object` - 物体表对象

**示例**:
```javascript
var table = cga.GetMapObjectTable(false);
```

---

### cga.GetMapTileTable(loadAll)

获取地砖表。

**参数**:
- `loadAll` (boolean): 是否加载全部

**返回**: `Object` - 地砖表对象

**示例**:
```javascript
var table = cga.GetMapTileTable(false);
```

---

### cga.WalkTo(x, y)

移动到坐标。

**参数**:
- `x` (number): 目标 X 坐标
- `y` (number): 目标 Y 坐标

**返回**: `void`

**示例**:
```javascript
cga.WalkTo(50, 50);
```

---

### cga.TurnTo(x, y)

转向坐标。

**参数**:
- `x` (number): 目标 X 坐标
- `y` (number): 目标 Y 坐标

**返回**: `void`

**示例**:
```javascript
cga.TurnTo(51, 50);  // 转向右边
```

---

### cga.IsMapCellPassable(x, y)

检查坐标是否可通过。

**参数**:
- `x` (number): X 坐标
- `y` (number): Y 坐标

**返回**: `boolean`

**示例**:
```javascript
if (cga.IsMapCellPassable(51, 50)) {
    cga.WalkTo(51, 50);
}
```

---

### cga.ForceMove(dir, show)

强制移动。

**参数**:
- `dir` (number): 方向 (0-7)
- `show` (boolean): 是否显示

**方向**:
| 值 | 方向 |
|----|------|
| 0 | 东 |
| 1 | 东南 |
| 2 | 南 |
| 3 | 西南 |
| 4 | 西 |
| 5 | 西北 |
| 6 | 北 |
| 7 | 东北 |

**返回**: `boolean`

**示例**:
```javascript
cga.ForceMove(0, true);  // 向东移动
```

---

### cga.ForceMoveTo(x, y, show)

强制移动到坐标。

**参数**:
- `x` (number): X 坐标
- `y` (number): Y 坐标
- `show` (boolean): 是否显示

**返回**: `boolean`

**示例**:
```javascript
cga.ForceMoveTo(51, 50, true);
```

---

### cga.RequestDownloadMap(xbottom, ybottom, xsize, ysize)

请求下载地图。

**参数**:
- `xbottom` (number): 起始 X
- `ybottom` (number): 起始 Y
- `xsize` (number): X 大小
- `ysize` (number): Y 大小

**示例**:
```javascript
cga.RequestDownloadMap(0, 0, 51, 51);
```

---

### cga.FixMapWarpStuck(type)

修复地图卡住问题。

**参数**:
- `type` (number): 修复类型

**类型**:
| 值 | 说明 |
|----|------|
| 0 | 类型 0 |
| 1 | 类型 1 |
| 2 | 禁用检查 |
| 3 | 启用检查 |

**示例**:
```javascript
cga.FixMapWarpStuck(0);
```

---

## 战斗系统 API

### cga.GetBattleUnits()

获取战斗单位。

**返回**: `Array` - 战斗单位数组

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| name | string | 单位名称 |
| modelid | number | 模型 ID |
| level | number | 等级 |
| curhp | number | 当前 HP |
| maxhp | number | 最大 HP |
| curmp | number | 当前 MP |
| maxmp | number | 最大 MP |
| flags | number | 标志 |
| pos | number | 位置 |

**示例**:
```javascript
var units = cga.GetBattleUnits();
for (var i = 0; i < units.length; i++) {
    cga.log("单位：" + units[i].name);
    cga.log("HP: " + units[i].curhp + "/" + units[i].maxhp);
}
```

---

### cga.GetBattleContext()

获取战斗上下文。

**返回**: `Object` - 战斗上下文对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| round_count | number | 回合数 |
| player_pos | number | 玩家位置 |
| player_status | number | 玩家状态 |
| petid | number | 宠物 ID |
| skill_allowbit | number | 技能允许位 |
| petskill_allowbit | number | 宠物技能允许位 |
| weapon_allowbit | number | 武器允许位 |

**示例**:
```javascript
var ctx = cga.GetBattleContext();
cga.log("回合：" + ctx.round_count);
cga.log("玩家位置：" + ctx.player_pos);
cga.log("玩家状态：" + ctx.player_status);
```

---

### cga.BattleNormalAttack(target)

普通攻击。

**参数**:
- `target` (number): 目标位置

**返回**: `boolean`

**示例**:
```javascript
cga.BattleNormalAttack(10);
```

---

### cga.BattleSkillAttack(skillPos, skillLv, target, packetOnly)

技能攻击。

**参数**:
- `skillPos` (number): 技能位置
- `skillLv` (number): 技能等级
- `target` (number): 目标位置
- `packetOnly` (boolean): 仅发包

**返回**: `boolean`

**示例**:
```javascript
cga.BattleSkillAttack(1, 5, 10, false);
```

---

### cga.BattleRebirth()

复活。

**返回**: `boolean`

**示例**:
```javascript
cga.BattleRebirth();
```

---

### cga.BattleGuard()

防御。

**返回**: `boolean`

**示例**:
```javascript
cga.BattleGuard();
```

---

### cga.BattleEscape()

逃跑。

**返回**: `boolean`

**示例**:
```javascript
cga.BattleEscape();
```

---

### cga.BattleDoNothing()

什么都不做。

**返回**: `boolean`

**示例**:
```javascript
cga.BattleDoNothing();
```

---

### cga.BattleExchangePosition()

交换位置。

**返回**: `boolean`

**示例**:
```javascript
cga.BattleExchangePosition();
```

---

### cga.BattleChangePet(petId)

更换宠物。

**参数**:
- `petId` (number): 宠物 ID (0-4, 255=召回)

**返回**: `boolean`

**示例**:
```javascript
cga.BattleChangePet(0);   // 更换为第 1 个宠物
cga.BattleChangePet(255); // 召回宠物
```

---

### cga.BattleUseItem(itemPos, target)

战斗中使用物品。

**参数**:
- `itemPos` (number): 物品位置
- `target` (number): 目标位置

**返回**: `boolean`

**示例**:
```javascript
cga.BattleUseItem(10, 10);
```

---

### cga.BattlePetSkillAttack(skillPos, target, packetOnly)

宠物技能攻击。

**参数**:
- `skillPos` (number): 技能位置
- `target` (number): 目标位置
- `packetOnly` (boolean): 仅发包

**返回**: `boolean`

**示例**:
```javascript
cga.BattlePetSkillAttack(1, 10, false);
```

---

## NPC 交互 API

### cga.ClickNPCDialog(option, index)

点击 NPC 对话选项。

**参数**:
- `option` (number): 选项类型
- `index` (number): 选项索引

**返回**: `boolean`

**示例**:
```javascript
// 选择第 1 个选项
cga.ClickNPCDialog(0, 0);

// 选择第 2 个选项
cga.ClickNPCDialog(0, 1);
```

---

### cga.SellNPCStore(items)

出售物品给 NPC。

**参数**:
- `items` (Array): 物品数组

**物品对象**:
```javascript
{
    itempos: number,  // 物品位置
    count: number     // 数量
}
```

**返回**: `boolean`

**示例**:
```javascript
cga.SellNPCStore([
    {itempos: 10, count: 1},
    {itempos: 11, count: 5}
]);
```

---

### cga.BuyNPCStore(items)

从 NPC 购买物品。

**参数**:
- `items` (Array): 物品数组

**物品对象**:
```javascript
{
    index: number,   // 商店索引
    count: number    // 数量
}
```

**返回**: `boolean`

**示例**:
```javascript
cga.BuyNPCStore([
    {index: 0, count: 10},
    {index: 1, count: 5}
]);
```

---

## 聊天系统 API

### cga.SayWords(message, color, range, size)

发送聊天消息。

**参数**:
- `message` (string): 消息内容
- `color` (number): 颜色 (0=普通)
- `range` (number): 范围 (3=当前地图)
- `size` (number): 大小 (1=普通)

**返回**: `boolean`

**示例**:
```javascript
cga.SayWords("你好", 0, 3, 1);
cga.SayWords("组队来人", 0, 3, 1);
```

---

### cga.GetTeamPlayerInfo()

获取组队玩家信息。

**返回**: `Array` - 组队玩家信息数组

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| unitid | number | 单位 ID |
| hp | number | 当前 HP |
| maxhp | number | 最大 HP |
| mp | number | 当前 MP |
| maxmp | number | 最大 MP |
| xpos | number | X 坐标 |
| ypos | number | Y 坐标 |
| name | string | 名称 |

**示例**:
```javascript
var team = cga.GetTeamPlayerInfo();
for (var i = 0; i < team.length; i++) {
    cga.log("队友：" + team[i].name);
    cga.log("HP: " + team[i].hp + "/" + team[i].maxhp);
}
```

---

## 交易系统 API

### cga.TradeAddStuffs(items, pets, gold)

添加交易物品。

**参数**:
- `items` (Array): 物品数组
- `pets` (Array): 宠物数组
- `gold` (number): 金币数量

**物品对象**:
```javascript
{
    itempos: number,  // 物品位置
    count: number     // 数量
}
```

**示例**:
```javascript
cga.TradeAddStuffs(
    [{itempos: 10, count: 1}],  // 物品
    [0],                         // 宠物
    1000                         // 金币
);
```

---

## 卡片邮件 API

### cga.GetCardsInfo()

获取名片信息。

**返回**: `Array` - 名片信息数组

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| index | number | 索引 |
| level | number | 等级 |
| avatar | number | 头像 |
| server | number | 服务器 |
| name | string | 名称 |
| nick | string | 昵称 |
| family | string | 家族 |

**示例**:
```javascript
var cards = cga.GetCardsInfo();
for (var i = 0; i < cards.length; i++) {
    cga.log("名片：" + cards[i].name);
    cga.log("等级：" + cards[i].level);
}
```

---

### cga.GetPicBooksInfo()

获取图鉴信息。

**返回**: `Array` - 图鉴信息数组

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| can_catch | boolean | 可捕捉 |
| card_type | number | 卡片类型 |
| race | number | 种族 |
| index | number | 索引 |
| image_id | number | 图像 ID |
| name | string | 名称 |

**示例**:
```javascript
var pics = cga.GetPicBooksInfo();
for (var i = 0; i < pics.length; i++) {
    cga.log("图鉴：" + pics[i].name);
}
```

---

### cga.DeleteCard(index, packetOnly)

删除名片。

**参数**:
- `index` (number): 名片索引
- `packetOnly` (boolean): 仅发包

**返回**: `boolean`

**示例**:
```javascript
cga.DeleteCard(0, false);
```

---

### cga.SendMail(index, message)

发送名片邮件。

**参数**:
- `index` (number): 名片索引
- `message` (string): 消息内容

**返回**: `boolean`

**示例**:
```javascript
cga.SendMail(0, "你好！");
```

---

### cga.SendPetMail(index, petId, itemId, message)

发送宠物邮件。

**参数**:
- `index` (number): 名片索引
- `petId` (number): 宠物 ID
- `itemId` (number): 物品 ID
- `message` (string): 消息内容

**返回**: `boolean`

**示例**:
```javascript
cga.SendPetMail(0, 0, 100, "这是我的宠物！");
```

---

## 工具函数 API

### cga.PlayGesture(index)

播放动作。

**参数**:
- `index` (number): 动作索引

**示例**:
```javascript
cga.PlayGesture(1);
```

---

### cga.DoRequest(requestType)

执行请求。

**参数**:
- `requestType` (number): 请求类型

**请求类型**:
| 值 | 说明 |
|----|------|
| 0 | 组队请求 |
| 1 | 离开队伍 |
| 2 | 踢出队伍 |
| 3 | 交易请求 |
| 4 | 交换名片 |
| 5 | PK 请求 |
| 6 | 交易确认 |
| 7 | 交易拒绝 |
| 61 | 开启转生 |
| 62 | 关闭转生 |

**返回**: `boolean`

**示例**:
```javascript
// 组队请求
cga.DoRequest(0);

// 交易请求
cga.DoRequest(3);
```

---

### cga.EnableFlags(type, enable)

启用标志。

**参数**:
- `type` (number): 标志类型
- `enable` (boolean): 是否启用

**标志类型**:
| 值 | 说明 |
|----|------|
| 0 | 允许组队 |
| 1 | 允许交易 |
| 2 | 允许名片 |
| 3 | 允许 PK |
| 4 | 允许组队聊天 |
| 5 | 允许家族 |
| 6 | 显示宠物 |
| 7 | 头像公开 |
| 8 | 战斗位置 |

**返回**: `boolean`

**示例**:
```javascript
// 允许组队
cga.EnableFlags(0, true);

// 允许交易
cga.EnableFlags(1, true);
```

---

### cga.IsUIDialogPresent(dialog)

检查 UI 对话框是否存在。

**参数**:
- `dialog` (number): 对话框类型

**对话框类型**:
| 值 | 说明 |
|----|------|
| 0 | 交易对话框 |
| 1 | 战斗技能对话框 |

**返回**: `boolean`

**示例**:
```javascript
if (cga.IsUIDialogPresent(0)) {
    cga.log("交易对话框存在");
}
```

---

### cga.SetGameTextUIEnabled(enable)

设置游戏文本 UI 启用。

**参数**:
- `enable` (boolean): 是否启用

**示例**:
```javascript
cga.SetGameTextUIEnabled(true);
```

---

### cga.GetGameServerInfo()

获取游戏服务器信息。

**返回**: `Object` - 服务器信息对象

**返回对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| ip | string | 服务器 IP |
| port | number | 服务器端口 |

**示例**:
```javascript
var server = cga.GetGameServerInfo();
cga.log("服务器：" + server.ip + ":" + server.port);
```

---

## 异步回调 API

### cga.AsyncWaitBattleAction(callback)

异步等待战斗动作。

**参数**:
- `callback` (function): 回调函数

**回调参数**:
- `err` (Error): 错误对象
- `flags` (number): 战斗标志

**示例**:
```javascript
cga.AsyncWaitBattleAction(function(err, flags) {
    if (!err) {
        cga.log("战斗动作标志：" + flags);
    }
});
```

---

### cga.AsyncWaitChatMsg(callback)

异步等待聊天消息。

**参数**:
- `callback` (function): 回调函数

**回调参数**:
- `err` (Error): 错误对象
- `msg` (Object): 消息对象

**消息对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| unitid | number | 单位 ID |
| msg | string | 消息内容 |
| size | number | 大小 |
| color | number | 颜色 |

**示例**:
```javascript
cga.AsyncWaitChatMsg(function(err, msg) {
    if (!err) {
        cga.log("收到消息：" + msg.msg);
    }
});
```

---

### cga.AsyncWaitNPCDialog(callback)

异步等待 NPC 对话。

**参数**:
- `callback` (function): 回调函数

**回调参数**:
- `err` (Error): 错误对象
- `dlg` (Object): 对话对象

**对话对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| type | number | 对话类型 |
| options | number | 选项数量 |
| dialog_id | number | 对话 ID |
| npc_id | number | NPC ID |
| message | string | 对话内容 |

**示例**:
```javascript
cga.AsyncWaitNPCDialog(function(err, dlg) {
    if (!err) {
        cga.log("NPC 对话：" + dlg.message);
    }
});
```

---

### cga.AsyncWaitWorkingResult(callback)

异步等待工作结果。

**参数**:
- `callback` (function): 回调函数

**回调参数**:
- `err` (Error): 错误对象
- `result` (Object): 结果对象

**结果对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| type | number | 工作类型 |
| success | boolean | 是否成功 |
| levelup | boolean | 是否升级 |
| xp | number | 获得经验 |
| endurance | number | 耐力增加 |
| skillful | number | 熟练增加 |
| intelligence | number | 智力增加 |
| count | number | 获得数量 |
| name | string | 产物名称 |
| imgid | number | 图像 ID |
| status | number | 状态 |

**示例**:
```javascript
cga.AsyncWaitWorkingResult(function(err, result) {
    if (!err) {
        cga.log("工作结果：" + (result.success ? "成功" : "失败"));
        cga.log("获得经验：" + result.xp);
    }
});
```

---

### cga.AsyncWaitTradeState(callback)

异步等待交易状态。

**参数**:
- `callback` (function): 回调函数

**回调参数**:
- `err` (Error): 错误对象
- `state` (number): 交易状态

**交易状态**:
| 值 | 说明 |
|----|------|
| 0 | 准备交易 |
| 1 | 确认交易 |
| 2 | 交易成功 |

**示例**:
```javascript
cga.AsyncWaitTradeState(function(err, state) {
    if (!err) {
        cga.log("交易状态：" + state);
    }
});
```

---

### cga.AsyncWaitTradeDialog(callback)

异步等待交易对话框。

**参数**:
- `callback` (function): 回调函数

**回调参数**:
- `err` (Error): 错误对象
- `info` (Object): 交易信息对象

**交易信息对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| name | string | 玩家名称 |
| level | number | 玩家等级 |

**示例**:
```javascript
cga.AsyncWaitTradeDialog(function(err, info) {
    if (!err) {
        cga.log("交易请求：" + info.name + " (Lv." + info.level + ")");
    }
});
```

---

### cga.AsyncWaitTradeStuffs(callback)

异步等待交易物品。

**参数**:
- `callback` (function): 回调函数

**回调参数**:
- `err` (Error): 错误对象
- `info` (Object): 交易物品信息对象

**示例**:
```javascript
cga.AsyncWaitTradeStuffs(function(err, info) {
    if (!err) {
        cga.log("交易物品更新");
    }
});
```

---

### cga.AsyncWaitDownloadMap(callback)

异步等待地图下载。

**参数**:
- `callback` (function): 回调函数

**回调参数**:
- `err` (Error): 错误对象
- `msg` (Object): 下载信息对象

**下载信息对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| index1 | number | 地图索引 1 |
| index3 | number | 地图索引 3 |
| xbase | number | 起始 X |
| ybase | number | 起始 Y |
| xtop | number | 结束 X |
| ytop | number | 结束 Y |

**示例**:
```javascript
cga.AsyncWaitDownloadMap(function(err, msg) {
    if (!err) {
        cga.log("地图下载完成");
    }
});
```

---

### cga.AsyncWaitConnectionState(callback)

异步等待连接状态。

**参数**:
- `callback` (function): 回调函数

**回调参数**:
- `err` (Error): 错误对象
- `msg` (Object): 连接状态对象

**连接状态对象属性**:
| 属性 | 类型 | 说明 |
|------|------|------|
| state | number | 状态值 |
| msg | string | 状态消息 |

**状态值**:
| 值 | 说明 |
|----|------|
| 0 | 连接成功 |
| 1 | 登录成功 |
| 2 | 登录失败 |
| 3 | 无角色 |
| 10000 | 连接丢失 |

**示例**:
```javascript
cga.AsyncWaitConnectionState(function(err, msg) {
    if (!err) {
        cga.log("连接状态：" + msg.state + " - " + msg.msg);
    }
});
```

---

### cga.AsyncWaitMessageBox(callback)

异步等待消息框。

**参数**:
- `callback` (function): 回调函数

**回调参数**:
- `err` (Error): 错误对象
- `msg` (string): 消息内容

**示例**:
```javascript
cga.AsyncWaitMessageBox(function(err, msg) {
    if (!err) {
        cga.log("消息框：" + msg);
    }
});
```

---

### cga.AsyncWaitMovement(callback)

异步等待移动完成。

**参数**:
- `callback` (function): 回调函数

**回调参数**:
- `err` (Error): 错误对象

**示例**:
```javascript
cga.WalkTo(50, 50);
cga.AsyncWaitMovement(function(err) {
    if (!err) {
        cga.log("移动完成");
    }
});
```

---

## 常量定义

### 标志常量

```javascript
cga.ENABLE_FLAG_PK = 0;           // PK 开关
cga.ENABLE_FLAG_TEAMCHAT = 1;     // 组队聊天
cga.ENABLE_FLAG_JOINTEAM = 2;     // 允许组队
cga.ENABLE_FLAG_CARD = 3;         // 允许名片
cga.ENABLE_FLAG_TRADE = 4;        // 允许交易
cga.ENABLE_FLAG_FAMILY = 5;       // 允许家族
cga.ENABLE_FLAG_SHOWPETS = 6;     // 显示宠物
cga.ENABLE_FLAG_AVATAR_PUBLIC = 7; // 头像公开
cga.ENABLE_FLAG_BATTLE_POSITION = 8; // 战斗位置
```

### 请求类型常量

```javascript
cga.REQUEST_TYPE_JOINTEAM = 0;    // 组队请求
cga.REQUEST_TYPE_LEAVETEAM = 1;   // 离开队伍
cga.REQUEST_TYPE_KICKTEAM = 2;    // 踢出队伍
cga.REQUEST_TYPE_TRADE = 3;       // 交易请求
cga.REQUEST_TYPE_EXCAHNGECARD = 4; // 交换名片
cga.REQUEST_TYPE_PK = 5;          // PK 请求
cga.REQUEST_TYPE_TRADE_CONFIRM = 6; // 交易确认
cga.REQUEST_TYPE_TRADE_REFUSE = 7;  // 交易拒绝
cga.REQUEST_TYPE_REBIRTH_ON = 61; // 开启转生
cga.REQUEST_TYPE_REBIRTH_OFF = 62; // 关闭转生
```

### 金币操作常量

```javascript
cga.MOVE_GOLD_TOBANK = 0;   // 存钱
cga.MOVE_GOLD_FROMBANK = 1; // 取钱
cga.MOVE_GOLD_DROP = 2;     // 丢弃
```

### 宠物状态常量

```javascript
cga.PET_STATE_READY = 1;    // 准备
cga.PET_STATE_BATTLE = 2;   // 战斗
cga.PET_STATE_REST = 3;     // 休息
```

### 工作类型常量

```javascript
cga.WORK_TYPE_GATHERING = 0; // 采集
cga.WORK_TYPE_HEALING = 1;   // 治疗
cga.WORK_TYPE_ASSESSING = 2; // 鉴定
cga.WORK_TYPE_CRAFTING = 3;  // 制造
cga.WORK_TYPE_PET = 4;       // 宠物
```

### UI 对话框常量

```javascript
cga.UI_DIALOG_TRADE = 0;          // 交易对话框
cga.UI_DIALOG_BATTLE_SKILL = 1;   // 战斗技能对话框
```

---

## 完整示例

### 1. 自动打怪脚本

```javascript
function main() {
    cga.log("开始自动打怪");
    
    // 移动到遇敌点
    cga.WalkTo(50, 50);
    
    while (true) {
        // 等待遇敌
        while (!cga.IsInBattle()) {
            cga.delay(1000);
        }
        
        cga.log("进入战斗");
        
        // 获取战斗单位
        var units = cga.GetBattleUnits();
        for (var i = 0; i < units.length; i++) {
            cga.log("敌人：" + units[i].name + " HP:" + units[i].curhp);
        }
        
        // 自动战斗
        while (cga.IsInBattle()) {
            cga.BattleNormalAttack(10);
            cga.delay(500);
        }
        
        cga.log("战斗结束");
        
        // 检查 HP
        var player = cga.GetPlayerInfo();
        if (player.hp < player.maxhp * 0.3) {
            cga.log("HP 不足，使用血瓶");
            var items = cga.GetItemsInfo();
            for (var i = 0; i < items.length; i++) {
                if (items[i].name.contains("血瓶")) {
                    cga.UseItem(items[i].pos);
                    break;
                }
            }
        }
    }
}

main();
```

---

### 2. 自动采集脚本

```javascript
function main() {
    cga.log("开始自动采集");
    
    var gatherPoints = [
        {x: 50, y: 50},
        {x: 60, y: 60},
        {x: 70, y: 70}
    ];
    
    // 异步等待工作结果
    cga.AsyncWaitWorkingResult(function(err, result) {
        if (!err) {
            if (result.success) {
                cga.log("采集成功：" + result.name + " x" + result.count);
                cga.log("获得经验：" + result.xp);
            } else {
                cga.log("采集失败");
            }
        }
    });
    
    while (true) {
        for (var i = 0; i < gatherPoints.length; i++) {
            var point = gatherPoints[i];
            
            // 移动到采集点
            cga.WalkTo(point.x, point.y);
            
            // 等待移动完成
            cga.AsyncWaitMovement(function(err) {
                if (!err) {
                    cga.delay(2000);
                    
                    // 开始采集
                    cga.StartWork(1, 0);
                    
                    // 等待采集完成
                    while (cga.GetCraftStatus() == 0) {
                        cga.delay(1000);
                    }
                }
            });
        }
    }
}

main();
```

---

### 3. 智能战斗脚本

```javascript
function main() {
    cga.log("开始智能战斗");
    
    // 异步等待战斗动作
    cga.AsyncWaitBattleAction(function(err, flags) {
        if (!err) {
            cga.log("战斗动作标志：" + flags);
            
            // 获取战斗上下文
            var ctx = cga.GetBattleContext();
            cga.log("回合：" + ctx.round_count);
            cga.log("玩家状态：" + ctx.player_status);
            
            // 根据状态执行动作
            if (ctx.player_status == 1) {
                // 玩家回合
                var player = cga.GetPlayerInfo();
                if (player.hp < player.maxhp * 0.3) {
                    // HP 不足，使用物品
                    var items = cga.GetItemsInfo();
                    for (var i = 0; i < items.length; i++) {
                        if (items[i].name.contains("血瓶")) {
                            cga.BattleUseItem(items[i].pos, ctx.player_pos);
                            break;
                        }
                    }
                } else {
                    // 普通攻击
                    cga.BattleNormalAttack(10);
                }
            }
        }
    });
}

main();
```

---

### 4. NPC 对话脚本

```javascript
function main() {
    cga.log("开始 NPC 对话");
    
    // 异步等待 NPC 对话
    cga.AsyncWaitNPCDialog(function(err, dlg) {
        if (!err) {
            cga.log("NPC 对话：" + dlg.message);
            cga.log("选项数量：" + dlg.options);
            
            // 根据对话内容选择选项
            if (dlg.message.contains("治疗")) {
                // 选择治疗选项
                cga.ClickNPCDialog(0, 0);
            } else if (dlg.message.contains("购买")) {
                // 选择购买选项
                cga.ClickNPCDialog(0, 1);
            }
        }
    });
    
    // 移动到 NPC
    cga.WalkTo(100, 100);
    cga.delay(2000);
}

main();
```

---

### 5. 组队脚本

```javascript
function main() {
    cga.log("开始组队");
    
    // 允许组队
    cga.EnableFlags(cga.ENABLE_FLAG_JOINTEAM, true);
    
    // 发起组队请求
    cga.DoRequest(cga.REQUEST_TYPE_JOINTEAM);
    
    // 获取组队信息
    var team = cga.GetTeamPlayerInfo();
    cga.log("队伍成员数量：" + team.length);
    
    for (var i = 0; i < team.length; i++) {
        cga.log("成员 " + i + ": " + team[i].name);
        cga.log("HP: " + team[i].hp + "/" + team[i].maxhp);
        cga.log("位置：" + team[i].xpos + ", " + team[i].ypos);
    }
}

main();
```

---

**最后更新**: 2026-04-22  
**版本**: 2.0 (基于源代码)  
**状态**: 完整 ✅
