# CGAssistant API 参考

完整的 API 文档，包含所有服务、实体和工具类。

---

## 目录

1. [领域层 API](#领域层-api)
2. [应用层 API](#应用层-api)
3. [基础设施层 API](#基础设施层-api)
4. [性能优化 API](#性能优化-api)
5. [使用示例](#使用示例)

---

## 领域层 API

### 实体类

#### Player (玩家)

```cpp
namespace cga { namespace domain {

struct Player {
    QString name;           // 角色名
    QString job;            // 职业
    int level;              // 等级
    int gold;               // 金币
    int hp, maxHp;          // 生命值
    int mp, maxMp;          // 魔法值
    int xp, maxXp;          // 经验值
    int unitId;             // 单位 ID
    int serverIndex;        // 服务器索引
    
    // 计算方法
    int hpPercent() const;           // HP 百分比
    int mpPercent() const;           // MP 百分比
    bool needsHealing(int = 30) const;  // 是否需要治疗
    bool needsMana(int = 30) const;     // 是否需要补魔
};

}} // namespace cga::domain
```

#### Pet (宠物)

```cpp
struct Pet {
    int id;                 // 宠物 ID
    QString name;           // 昵称
    QString realName;       // 真名
    int level;              // 等级
    int hp, maxHp;
    int mp, maxMp;
    int loyalty;            // 忠诚度
    int flags;              // 状态标志
    int battleFlags;        // 战斗标志
    bool isDefault;         // 是否默认出战
    
    int hpPercent() const;
};
```

#### Item (物品)

```cpp
struct Item {
    int itemId;             // 物品 ID
    int type;               // 物品类型
    int count;              // 数量
    int pos;                // 位置
    QString name;           // 名称
    QString attr;           // 属性
    QString info;           // 信息
    bool assessed;          // 已鉴定
    
    bool isValid() const;
};
```

#### BattleContext (战斗上下文)

```cpp
struct BattleContext {
    int round;              // 回合数
    int playerPos;          // 玩家位置
    int playerStatus;       // 玩家状态
    int petId;              // 宠物 ID
    int petPos;             // 宠物位置
    int enemyCount;         // 敌人数量
    int teammateCount;      // 队友数量
    int bgmIndex;           // BGM 索引
    bool isPlayerTurn;      // 是否玩家回合
    bool isDoubleAction;    // 是否双重行动
    
    bool isInBattle() const;
};
```

---

## 应用层 API

### 服务接口

#### IPlayerService (玩家服务)

```cpp
class IPlayerService {
public:
    virtual ~IPlayerService() = default;
    
    // 获取玩家信息
    virtual std::shared_ptr<Player> getPlayerInfo() = 0;
    
    // 获取宠物列表
    virtual std::vector<Pet> getPets() = 0;
    
    // 获取物品列表
    virtual std::vector<Item> getItems() = 0;
    
    // 获取技能列表
    virtual std::vector<Skill> getSkills() = 0;
    
    // 使用物品
    virtual bool useItem(int pos) = 0;
    
    // 丢弃物品
    virtual bool dropItem(int pos) = 0;
};
```

#### IBattleService (战斗服务)

```cpp
class IBattleService {
public:
    virtual ~IBattleService() = default;
    
    // 获取战斗上下文
    virtual std::shared_ptr<BattleContext> getBattleContext() = 0;
    
    // 普通攻击
    virtual bool normalAttack(int target) = 0;
    
    // 技能攻击
    virtual bool skillAttack(int skillId, int level, int target) = 0;
    
    // 防御
    virtual bool guard() = 0;
    
    // 逃跑
    virtual bool escape() = 0;
    
    // 换宠
    virtual bool changePet(int petId) = 0;
    
    // 设置自动战斗
    virtual void setAutoBattle(bool enabled) = 0;
};
```

#### IMapService (地图服务)

```cpp
class IMapService {
public:
    virtual ~IMapService() = default;
    
    // 获取当前位置
    virtual std::pair<int, int> getPosition() = 0;
    
    // 获取地图名称
    virtual QString getMapName() = 0;
    
    // 获取地图单位
    virtual std::vector<MapUnit> getMapUnits() = 0;
    
    // 移动到坐标
    virtual bool walkTo(int x, int y) = 0;
    
    // 检查坐标是否可通过
    virtual bool isPassable(int x, int y) = 0;
};
```

#### INpcService (NPC 服务)

```cpp
class INpcService {
public:
    virtual ~INpcService() = default;
    
    // 点击 NPC 对话选项
    virtual bool clickDialog(int option, int index) = 0;
    
    // 出售物品
    virtual bool sellItems(const std::vector<std::pair<int, int>>& items) = 0;
    
    // 购买物品
    virtual bool buyItems(const std::vector<std::pair<int, int>>& items) = 0;
};
```

#### IChatService (聊天服务)

```cpp
class IChatService {
public:
    virtual ~IChatService() = default;
    
    // 发送消息
    virtual bool sendMessage(const QString& message) = 0;
    
    // 设置消息回调
    virtual void setMessageCallback(
        std::function<void(const ChatMessage&)> callback
    ) = 0;
};
```

---

## 基础设施层 API

### 仓库类

#### PlayerRepository

```cpp
class PlayerRepository {
public:
    explicit PlayerRepository(CGA::CGAInterface* cga);
    
    std::shared_ptr<CGA::cga_player_info_t> getInfo();
    std::vector<CGA::cga_pet_info_t> getPets();
    std::vector<CGA::cga_item_info_t> getItems();
    std::vector<CGA::cga_skill_info_t> getSkills();
    
    bool useItem(int pos);
    bool dropItem(int pos);
};
```

#### BattleRepository

```cpp
class BattleRepository {
public:
    explicit BattleRepository(CGA::CGAInterface* cga);
    
    std::vector<CGA::cga_battle_unit_t> getUnits();
    
    bool normalAttack(int target);
    bool skillAttack(int skillId, int level, int target);
    bool guard();
    bool escape();
    bool changePet(int petId);
};
```

---

## 性能优化 API

### RpcCache (RPC 缓存)

```cpp
namespace cga { namespace optimization {

template<typename T>
class RpcCache {
public:
    // 构造函数 (TTL = 缓存有效期)
    RpcCache(std::chrono::seconds ttl = std::chrono::seconds(5));
    
    // 获取缓存
    bool get(const std::string& key, T& value);
    
    // 设置缓存
    void set(const std::string& key, const T& value);
    
    // 清除过期缓存
    void cleanup();
    
    // 清空缓存
    void clear();
};

}} // namespace cga::optimization
```

**使用示例**:
```cpp
// 创建 5 秒缓存
RpcCache<Player> cache(std::chrono::seconds(5));

// 设置缓存
cache.set("player", player);

// 获取缓存
Player cached;
if (cache.get("player", cached)) {
    // 使用缓存
}
```

### PerformanceMonitor (性能监控)

```cpp
class PerformanceMonitor {
public:
    static PerformanceMonitor& instance();
    
    // 开始计时
    void startTimer(const std::string& name);
    
    // 结束计时 (返回毫秒)
    long long endTimer(const std::string& name);
    
    // 打印统计
    void printStats();
};

// 便捷宏
#define PERF_START(name) /* 开始计时 */
#define PERF_END(name)   /* 结束计时 */
#define PERF_PRINT()     /* 打印统计 */
```

**使用示例**:
```cpp
void ProcessData() {
    PERF_START("ProcessData");
    
    // ... 业务逻辑 ...
    
    PERF_END("ProcessData");
}

// 定期打印统计
QTimer* timer = new QTimer();
connect(timer, &QTimer::timeout, []() {
    PERF_PRINT();
});
timer->start(60000);  // 每分钟
```

### BatchRequest (批量请求)

```cpp
class BatchRequest {
public:
    void addPlayerRequest(int playerId);
    void addPetRequest(int petId);
    void addItemRequest(int itemId);
    
    bool hasRequests() const;
    void clear();
    
    const std::vector<int>& getPlayerIds() const;
    const std::vector<int>& getPetIds() const;
    const std::vector<int>& getItemIds() const;
};
```

---

## 使用示例

### 完整示例

```cpp
#include "application/service_factory.h"
#include "optimization/performance.h"
#include "../CGALib/logger.h"

void Example() {
    // 1. 创建服务工厂
    auto factory = cga::application::ServiceFactory::Create(g_CGAInterface);
    
    // 2. 获取服务
    auto& player = factory->player();
    auto& battle = factory->battle();
    auto& map = factory->map();
    
    // 3. 获取玩家信息 (带缓存)
    PERF_START("GetPlayerInfo");
    auto playerInfo = player.getPlayerInfo();
    PERF_END("GetPlayerInfo");
    
    LOG_INFO("玩家：%s, 等级：%d, HP: %d/%d",
             playerInfo->name.toStdString(),
             playerInfo->level,
             playerInfo->hp,
             playerInfo->maxHp);
    
    // 4. 检查是否需要治疗
    if (playerInfo->needsHealing(30)) {
        LOG_WARN("HP 低于 30%，需要治疗！");
        
        // 使用物品
        auto items = player.getItems();
        for (const auto& item : items) {
            if (item.name.contains("血瓶")) {
                player.useItem(item.pos);
                break;
            }
        }
    }
    
    // 5. 地图导航
    auto [x, y] = map.getPosition();
    LOG_INFO("当前位置：(%d, %d)", x, y);
    
    if (map.isPassable(x + 1, y)) {
        map.walkTo(x + 1, y);
    }
    
    // 6. 自动战斗
    battle.setAutoBattle(true);
    
    // 打印性能统计
    PERF_PRINT();
}
```

### JavaScript 脚本示例

```javascript
// 通过 Node.js 调用 C++ 服务
const cga = require('cga');

// 获取玩家信息
cga.GetPlayerInfo((info) => {
    console.log(`玩家：${info.name}, 等级：${info.level}`);
    
    // 检查 HP
    if (info.hp < info.maxhp * 0.3) {
        console.log("HP 不足，需要治疗！");
    }
});

// 自动战斗
cga.EnableFlags(cga.ENABLE_FLAG_JOINTEAM, true);
cga.SetAutoBattle(true);

// 添加战斗条件
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
```

---

## 错误处理

### 异常类型

```cpp
namespace cga {

// RPC 超时异常
class RpcTimeoutException : public std::runtime_error {
public:
    explicit RpcTimeoutException(const std::string& method);
    const std::string& method() const;
};

// RPC 连接异常
class RpcConnectionException : public std::runtime_error {
public:
    explicit RpcConnectionException(const std::string& message);
};

// RPC 调用异常
class RpcCallException : public std::runtime_error {
public:
    RpcCallException(const std::string& method, const std::string& error);
    const std::string& method() const;
    const std::string& error() const;
};

} // namespace cga
```

### 错误处理示例

```cpp
try {
    auto player = playerService->getPlayerInfo();
} catch (const cga::RpcTimeoutException& e) {
    LOG_ERROR("RPC 超时：%s", e.method().c_str());
} catch (const cga::RpcConnectionException& e) {
    LOG_ERROR("RPC 连接失败：%s", e.what());
} catch (const std::exception& e) {
    LOG_ERROR("未知错误：%s", e.what());
}
```

---

**最后更新**: 2026-04-22  
**版本**: 1.0  
**状态**: 完整 ✅
