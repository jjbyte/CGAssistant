# CGAssistant 分层架构文档

## 架构概述

CGAssistant 采用经典的分层架构，将系统划分为四个清晰的层次：

```
┌─────────────────────────────────────────────────────────────┐
│                  Presentation Layer (展示层)                 │
│  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐   │
│  │ PlayerForm│ │BattleForm │ │ MapForm   │ │ ChatForm  │   │
│  └───────────┘ └───────────┘ └───────────┘ └───────────┘   │
└─────────────────────────────────────────────────────────────┘
                            ↓ 使用服务接口
┌─────────────────────────────────────────────────────────────┐
│                  Application Layer (应用层)                  │
│  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐   │
│  │PlayerServ │ │BattleServ │ │ MapServ   │ │ ChatServ  │   │
│  └───────────┘ └───────────┘ └───────────┘ └───────────┘   │
└─────────────────────────────────────────────────────────────┘
                            ↓ 调用仓库接口
┌─────────────────────────────────────────────────────────────┐
│               Infrastructure Layer (基础设施层)               │
│  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐   │
│  │PlayerRepo │ │BattleRepo │ │ MapRepo   │ │ ChatRepo  │   │
│  └───────────┘ └───────────┘ └───────────┘ └───────────┘   │
│                     ↓ 使用 RPC 客户端                        │
│  ┌──────────────────────────────────────────────────────┐  │
│  │              CGAInterface (RPC Client)                │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                            ↓ 定义核心概念
┌─────────────────────────────────────────────────────────────┐
│                    Domain Layer (领域层)                     │
│  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐   │
│  │  Player   │ │  Battle   │ │   Map     │ │   Item    │   │
│  │   Pet     │ │  Skill    │ │   Npc     │ │  Chat     │   │
│  └───────────┘ └───────────┘ └───────────┘ └───────────┘   │
└─────────────────────────────────────────────────────────────┘
```

## 各层职责

### 1. Domain Layer (领域层)

**位置**: `CGAssistant/domain/`

**职责**:
- 定义核心业务实体 (Player, Pet, Item, Battle 等)
- 定义领域服务接口
- 包含业务规则和业务逻辑
- **不依赖**任何其他层

**文件**:
- `entities.h` - 业务实体定义
- `services.h` - 领域服务接口

**示例**:
```cpp
// domain/entities.h
struct Player {
    QString name;
    int level;
    int hp, maxHp;
    
    int hpPercent() const {
        return maxHp > 0 ? (hp * 100 / maxHp) : 0;
    }
    
    bool needsHealing(int threshold = 30) const {
        return hpPercent() < threshold;
    }
};
```

### 2. Infrastructure Layer (基础设施层)

**位置**: `CGAssistant/infrastructure/`

**职责**:
- 实现数据持久化 (RPC 调用、文件读写等)
- 实现外部系统通信
- 实现日志、配置等基础设施
- **依赖**领域层 (使用领域实体)

**文件**:
- `repositories.h/.cpp` - 数据仓库实现

**示例**:
```cpp
// infrastructure/repositories.h
class PlayerRepository {
public:
    std::shared_ptr<CGA::cga_player_info_t> getInfo();
    std::vector<CGA::cga_pet_info_t> getPets();
    
private:
    CGA::CGAInterface* m_cga;
};
```

### 3. Application Layer (应用层)

**位置**: `CGAssistant/application/`

**职责**:
- 实现领域服务接口
- 协调领域对象和仓库
- 处理业务流程
- **依赖**领域层和基础设施层

**文件**:
- `services.h/.cpp` - 应用服务实现

**示例**:
```cpp
// application/services.cpp
class PlayerService : public domain::IPlayerService {
public:
    std::shared_ptr<domain::Player> getPlayerInfo() override {
        auto player = std::make_shared<domain::Player>();
        
        CGA::cga_player_info_t info;
        if (m_cga->GetPlayerInfo(info)) {
            player->name = QString::fromStdString(info.name);
            player->level = info.level;
            // ... 转换其他字段
        }
        
        return player;
    }
    
private:
    CGA::CGAInterface* m_cga;
};
```

### 4. Presentation Layer (展示层)

**位置**: `CGAssistant/` (现有的 Form 文件)

**职责**:
- UI 显示和用户交互
- 调用应用服务
- **依赖**领域层和应用层

**示例**:
```cpp
// playerform.cpp
void PlayerForm::OnNotifyGetPlayerInfo(...)
{
    // 使用领域服务
    auto player = m_playerService->getPlayerInfo();
    
    // 显示 UI
    ui->label_name->setText(player->name);
    ui->label_hp->setText(QString("%1/%2").arg(player->hp).arg(player->maxHp));
}
```

## 依赖规则

```
Presentation → Application → Infrastructure
      ↓              ↓              ↓
   Domain ←────────┴──────────────┘
   
箭头方向表示依赖方向
```

**关键规则**:
1. **单向依赖**: 上层可以依赖下层，下层不能依赖上层
2. **领域层独立**: 领域层不依赖任何其他层
3. **依赖倒置**: 应用层依赖领域层的接口，而非基础设施层的实现

## 迁移指南

### 阶段 1: 创建新架构 (已完成 ✅)

- [x] 创建 domain/ 目录
- [x] 创建 application/ 目录
- [x] 创建 infrastructure/ 目录
- [x] 定义领域实体
- [x] 定义服务接口

### 阶段 2: 逐步迁移 (进行中)

#### 2.1 迁移玩家相关代码

```cpp
// 旧代码 (player.cpp)
class CPlayerWorker : public QObject {
    void OnQueueGetPlayerInfo() {
        CGA::cga_player_info_t info;
        g_CGAInterface->GetPlayerInfo(info);
        // ... 直接操作 UI 数据
    }
};

// 新代码 (application/services.cpp)
class PlayerService : public domain::IPlayerService {
    std::shared_ptr<domain::Player> getPlayerInfo() override {
        // ... 返回领域对象
    }
};
```

#### 2.2 迁移战斗相关代码

```cpp
// 旧代码 (battle.cpp)
class CBattleWorker : public QObject {
    void OnPerformanceBattle() {
        // ... 复杂的战斗逻辑
    }
};

// 新代码
class BattleService : public domain::IBattleService {
    bool normalAttack(int target) override {
        return m_cga->BattleNormalAttack(target);
    }
};
```

### 阶段 3: 重构 UI 层

```cpp
// 旧代码
void PlayerForm::UpdateUI() {
    // 直接访问 Worker 数据
    m_playerWorker->m_player->hp;
}

// 新代码
void PlayerForm::UpdateUI() {
    // 通过服务获取数据
    auto player = m_playerService->getPlayerInfo();
    ui->label_hp->setText(QString::number(player->hp));
}
```

## 优势

### 1. 可测试性

```cpp
// 可以Mock服务进行测试
class MockPlayerService : public domain::IPlayerService {
    std::shared_ptr<domain::Player> getPlayerInfo() override {
        auto player = std::make_shared<domain::Player>();
        player->name = "Test";
        player->level = 150;
        return player;
    }
};

// 单元测试
TEST(PlayerServiceTest, GetPlayerInfo) {
    MockPlayerService service;
    auto player = service.getPlayerInfo();
    EXPECT_EQ(player->name, "Test");
    EXPECT_EQ(player->level, 150);
}
```

### 2. 可替换性

```cpp
// 可以轻松替换基础设施实现
class RpcPlayerRepository : public IPlayerRepository { /* RPC 实现 */ };
class MockPlayerRepository : public IPlayerRepository { /* Mock 实现 */ };
class CachePlayerRepository : public IPlayerRepository { /* 缓存实现 */ };

// 使用时不关心具体实现
class PlayerService {
    PlayerService(IPlayerRepository* repo);  // 依赖抽象
};
```

### 3. 业务逻辑集中

```cpp
// 旧代码：业务逻辑分散在 UI 和 Worker 中
// playerform.cpp
if (hp < 30%) {
    // 使用血瓶
}

// player.cpp
if (mp < 30%) {
    // 使用魔瓶
}

// 新代码：业务逻辑集中在领域层
// domain/entities.h
struct Player {
    bool needsHealing(int threshold = 30) const {
        return hpPercent() < threshold;
    }
};

// application/services.cpp
void PlayerService::autoHeal() {
    auto player = getPlayerInfo();
    if (player->needsHealing()) {
        useItem(HEAL_POTION_ID);
    }
}
```

### 4. 清晰的职责划分

| 层 | 职责 | 变更频率 |
|----|------|----------|
| Domain | 业务规则 | 低 |
| Application | 业务流程 | 中 |
| Infrastructure | 技术实现 | 高 |
| Presentation | UI 展示 | 高 |

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
class BattleService {
    void autoBattle() {
        auto player = m_playerRepo->getPlayer();
        auto enemies = m_battleRepo->getEnemies();
        
        if (player->needsHealing()) {
            m_playerRepo->useItem(HEAL_POTION);
        } else {
            m_battleRepo->attack(findWeakestEnemy(enemies));
        }
    }
};
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

## 迁移检查清单

- [ ] 创建所有领域实体
- [ ] 定义所有服务接口
- [ ] 实现基础设施仓库
- [ ] 实现应用服务
- [ ] 重构 UI 层使用服务
- [ ] 移除 Worker 类
- [ ] 添加单元测试
- [ ] 更新文档

---

**最后更新**: 2026-04-22  
**版本**: 1.0  
**状态**: 实施中
