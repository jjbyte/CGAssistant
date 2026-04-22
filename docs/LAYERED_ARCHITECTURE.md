# CGAssistant 分层架构设计

本文档详细说明 CGAssistant 的分层架构设计、各层职责和交互方式。

---

## 架构概述

CGAssistant 采用经典的四层分层架构，将系统划分为清晰的职责边界：

```
┌────────────────────────────────────────────────┐
│          Presentation Layer (展示层)            │
│  Qt UI Forms: PlayerForm, BattleForm, ...      │
│  职责：用户交互、数据显示、事件处理             │
└───────────────────┬────────────────────────────┘
                    ↓ 使用服务接口
┌────────────────────────────────────────────────┐
│          Application Layer (应用层)             │
│  Services: PlayerService, BattleService, ...   │
│  职责：业务逻辑、流程控制、数据转换             │
└───────────────────┬────────────────────────────┘
                    ↓ 调用仓库接口
┌────────────────────────────────────────────────┐
│       Infrastructure Layer (基础设施层)         │
│  Repositories: PlayerRepo, BattleRepo, ...     │
│  职责：数据持久化、RPC 通信、外部系统集成         │
└───────────────────┬────────────────────────────┘
                    ↓ 封装底层 API
┌────────────────────────────────────────────────┐
│         Domain Layer (领域层)                   │
│  Entities: Player, Pet, Item, Skill, ...       │
│  职责：核心业务概念、业务规则、不变量           │
└────────────────────────────────────────────────┘
```

---

## 设计原则

### 1. 依赖倒置

- 上层依赖下层的抽象接口
- 下层不依赖上层
- 领域层完全独立

```
展示层 → 应用层 → 基础设施层
   ↓         ↓           ↓
领域层 ←─────┴───────────┘
```

### 2. 单一职责

每层只负责一个方面的职责：

| 层 | 职责 | 变化频率 |
|----|------|----------|
| 展示层 | UI 交互 | 高 |
| 应用层 | 业务流程 | 中 |
| 基础设施层 | 技术实现 | 高 |
| 领域层 | 业务规则 | 低 |

### 3. 接口隔离

- 服务接口细化
- 避免胖接口
- 按需依赖

---

## 各层详解

### 领域层 (Domain Layer)

**位置**: `CGAssistant/domain/`

**职责**:
- 定义核心业务实体
- 封装业务规则
- 维护业务不变量
- **不依赖**任何其他层

**核心实体**:

```cpp
namespace cga { namespace domain {

// 玩家实体
struct Player {
    QString name;
    int level;
    int hp, maxHp;
    int mp, maxMp;
    // ...
    
    // 业务方法
    int hpPercent() const;
    bool needsHealing(int threshold = 30) const;
};

// 宠物实体
struct Pet {
    int id;
    QString name;
    int level;
    // ...
};

// 物品实体
struct Item {
    int itemId;
    QString name;
    int count;
    int pos;
    // ...
    
    bool isValid() const;
};

// 战斗上下文
struct BattleContext {
    int round;
    int playerPos;
    int playerStatus;
    // ...
    
    bool isInBattle() const;
};

}} // namespace cga::domain
```

**服务接口**:

```cpp
// 玩家服务接口
class IPlayerService {
public:
    virtual ~IPlayerService() = default;
    virtual std::shared_ptr<Player> getPlayerInfo() = 0;
    virtual std::vector<Pet> getPets() = 0;
    virtual std::vector<Item> getItems() = 0;
    virtual bool useItem(int pos) = 0;
};

// 战斗服务接口
class IBattleService {
public:
    virtual ~IBattleService() = default;
    virtual std::shared_ptr<BattleContext> getBattleContext() = 0;
    virtual bool normalAttack(int target) = 0;
    virtual bool skillAttack(int skillId, int level, int target) = 0;
    virtual void setAutoBattle(bool enabled) = 0;
};
```

---

### 应用层 (Application Layer)

**位置**: `CGAssistant/application/`

**职责**:
- 实现领域服务接口
- 协调领域对象和仓库
- 处理业务流程
- 依赖领域层和基础设施层

**服务实现**:

```cpp
namespace cga { namespace application {

class PlayerService : public domain::IPlayerService {
public:
    explicit PlayerService(CGA::CGAInterface* cga);
    
    std::shared_ptr<domain::Player> getPlayerInfo() override {
        // 1. 尝试从缓存获取
        if (m_playerCache.get("player", cached)) {
            return cached;
        }
        
        // 2. 调用仓库获取数据
        auto info = m_playerRepo->getInfo();
        
        // 3. 转换为领域对象
        auto player = std::make_shared<domain::Player>();
        player->name = QString::fromStdString(info->name);
        player->level = info->level;
        // ...
        
        // 4. 更新缓存
        m_playerCache.set("player", player);
        
        return player;
    }
    
private:
    CGA::CGAInterface* m_cga;
    RpcCache<domain::Player> m_playerCache;
};

}} // namespace cga::application
```

**服务工厂**:

```cpp
class ServiceFactory {
public:
    static std::shared_ptr<ServiceFactory> Create(CGA::CGAInterface* cga);
    
    domain::IPlayerService& player();
    domain::IBattleService& battle();
    domain::IMapService& map();
    domain::INpcService& npc();
    domain::IChatService& chat();
    
private:
    std::unique_ptr<PlayerService> m_playerService;
    std::unique_ptr<BattleService> m_battleService;
    // ...
};
```

---

### 基础设施层 (Infrastructure Layer)

**位置**: `CGAssistant/infrastructure/`

**职责**:
- 实现数据持久化
- 封装 RPC 调用
- 实现外部系统通信
- 依赖领域层 (使用领域实体)

**仓库实现**:

```cpp
namespace cga { namespace infrastructure {

class PlayerRepository {
public:
    explicit PlayerRepository(CGA::CGAInterface* cga);
    
    std::shared_ptr<CGA::cga_player_info_t> getInfo() {
        auto info = std::make_shared<CGA::cga_player_info_t>();
        if (m_cga && m_cga->GetPlayerInfo(*info)) {
            return info;
        }
        return nullptr;
    }
    
    std::vector<CGA::cga_pet_info_t> getPets() {
        std::vector<CGA::cga_pet_info_t> pets;
        if (m_cga) {
            CGA::cga_pets_info_t petsInfo;
            if (m_cga->GetPetsInfo(petsInfo)) {
                pets.assign(petsInfo.begin(), petsInfo.end());
            }
        }
        return pets;
    }
    
private:
    CGA::CGAInterface* m_cga;
};

}} // namespace cga::infrastructure
```

---

### 展示层 (Presentation Layer)

**位置**: `CGAssistant/` (Qt Forms)

**职责**:
- UI 显示和用户交互
- 调用应用服务
- 依赖领域层和应用层

**Form 实现**:

```cpp
class PlayerForm : public QWidget {
public:
    explicit PlayerForm(QWidget *parent = nullptr);
    
    void InitializeWithServices(
        std::shared_ptr<ServiceFactory> factory
    ) {
        m_serviceFactory = factory;
    }
    
private slots:
    void UpdatePlayerInfo() {
        // 使用服务获取数据
        auto player = m_serviceFactory->player().getPlayerInfo();
        
        // 更新 UI
        ui->label_name->setText(player->name);
        ui->label_level->setText(QString("Lv.%1").arg(player->level));
        ui->label_hp->setText(
            QString("%1/%2").arg(player->hp).arg(player->maxHp)
        );
    }
    
private:
    std::shared_ptr<ServiceFactory> m_serviceFactory;
};
```

---

## 数据流

### 典型数据流

```
用户点击"获取玩家信息"
    ↓
PlayerForm::UpdatePlayerInfo()
    ↓
m_serviceFactory->player().getPlayerInfo()
    ↓
PlayerService::getPlayerInfo()
    ↓
[检查缓存] → 命中 → 返回缓存
    ↓ 未命中
PlayerRepository::getInfo()
    ↓
CGAInterface::GetPlayerInfo()  (RPC 调用)
    ↓
游戏进程
    ↓
返回数据 → 转换为领域对象 → 更新缓存 → 返回 UI
```

### 缓存策略

```cpp
// 2 秒缓存 TTL
RpcCache<Player> cache(std::chrono::seconds(2));

// 获取
Player cached;
if (cache.get("player", cached)) {
    return cached;  // 使用缓存
}

// 设置
auto player = fetchFromRepo();
cache.set("player", player);
```

---

## 性能优化

### 1. 缓存优化

- 玩家信息：2 秒缓存
- 宠物信息：5 秒缓存
- 物品信息：3 秒缓存

### 2. 批量请求

```cpp
BatchRequest batch;
batch.addPlayerRequest(1);
batch.addPlayerRequest(2);
batch.addPetRequest(1);

// 一次性处理
processBatch(batch);
```

### 3. 内存优化

```cpp
// 预分配
items.reserve(itemsInfo.size());

// 移动语义
items.push_back(std::move(item));
```

### 4. 性能监控

```cpp
PERF_START("PlayerService::getPlayerInfo");
auto player = getPlayerInfo();
PERF_END("PlayerService::getPlayerInfo");

// 定期打印
PERF_PRINT();
```

---

## 扩展性

### 添加新服务

1. **定义领域接口** (`domain/services.h`):
```cpp
class IInventoryService {
public:
    virtual ~IInventoryService() = default;
    virtual std::vector<Item> getInventory() = 0;
    virtual bool addItem(int itemId, int count) = 0;
};
```

2. **实现应用服务** (`application/services.cpp`):
```cpp
class InventoryService : public domain::IInventoryService {
    // 实现...
};
```

3. **实现基础设施仓库** (`infrastructure/repositories.cpp`):
```cpp
class InventoryRepository {
    // 实现...
};
```

4. **注册到服务工厂**:
```cpp
class ServiceFactory {
    domain::IInventoryService& inventory();
private:
    std::unique_ptr<InventoryService> m_inventoryService;
};
```

### 添加新实体

1. **定义实体** (`domain/entities.h`):
```cpp
struct Inventory {
    int id;
    int capacity;
    std::vector<Item> items;
    
    bool isFull() const;
    int getUsedCapacity() const;
};
```

2. **添加业务方法**:
```cpp
bool Inventory::isFull() const {
    return getUsedCapacity() >= capacity;
}
```

---

## 测试策略

### 单元测试

```cpp
TEST(PlayerServiceTest, GetPlayerInfo) {
    // 1. 准备 Mock 仓库
    MockPlayerRepository repo;
    EXPECT_CALL(repo, getInfo())
        .WillOnce(Return(mockPlayerInfo));
    
    // 2. 创建服务
    PlayerService service(&repo);
    
    // 3. 执行测试
    auto player = service.getPlayerInfo();
    
    // 4. 验证结果
    EXPECT_EQ(player->name, "Test");
    EXPECT_EQ(player->level, 150);
}
```

### 集成测试

```cpp
TEST(IntegrationTest, PlayerWorkflow) {
    auto factory = ServiceFactory::Create(realCgaInterface);
    
    auto player = factory->player().getPlayerInfo();
    EXPECT_TRUE(player != nullptr);
    
    auto pets = factory->player().getPets();
    EXPECT_FALSE(pets.empty());
}
```

---

## 最佳实践

### ✅ 推荐

```cpp
// 1. 依赖抽象
class PlayerForm {
    std::unique_ptr<domain::IPlayerService> m_service;
};

// 2. 领域对象包含业务逻辑
struct Player {
    bool needsHealing() const;
    bool canUseSkill(int skillId) const;
};

// 3. 应用服务协调多个仓库
void autoHeal() {
    auto player = playerRepo->getPlayer();
    if (player->needsHealing()) {
        playerRepo->useItem(HEAL_POTION);
    }
}
```

### ❌ 避免

```cpp
// 1. UI 层直接调用 RPC
void PlayerForm::UpdateUI() {
    g_CGAInterface->GetPlayerInfo(info);  // ❌
}

// 2. 基础设施层依赖 UI
class PlayerRepository {
    void updateUI() {  // ❌
        playerForm->setLabel(...);
    }
};

// 3. 贫血领域模型
struct Player {  // ❌ 只有数据，没有行为
    QString name;
    int level;
};
```

---

## 架构演进

### v1.0 (当前)

- ✅ 四层架构
- ✅ 依赖倒置
- ✅ 服务抽象
- ✅ 缓存优化

### v2.0 (计划)

- [ ] 插件系统
- [ ] Web UI 支持
- [ ] 分布式架构
- [ ] 云同步

---

**最后更新**: 2026-04-22  
**版本**: 1.0  
**状态**: 生产就绪 ✅
