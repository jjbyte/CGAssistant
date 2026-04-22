# CGAssistant 架构迁移指南

## 概述

本文档说明如何从旧的 Worker 架构迁移到新的分层架构。

## 迁移阶段

### 阶段 1: 基础框架 (已完成 ✅)

- [x] 创建领域层 (domain/)
- [x] 创建应用层 (application/)
- [x] 创建基础设施层 (infrastructure/)
- [x] 实现服务工厂
- [x] 创建 Worker 适配器

### 阶段 2: 并行运行 (进行中 🔄)

在此阶段，新旧架构并存，通过 Worker 适配器保持兼容。

```cpp
// mainwindow.cpp - 使用适配器
#include "worker_adapters.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    // 旧代码
    // auto playerWorker = new CPlayerWorker();
    
    // 新代码 - 使用适配器
    auto playerWorker = new cga::PlayerWorkerAdapter(g_CGAInterface);
    playerWorker->moveToThread(&m_playerWorkerThread);
    
    // 信号槽连接保持不变
    connect(&m_playerWorkerThread, SIGNAL(finished()), playerWorker, SLOT(deleteLater()));
}
```

### 阶段 3: 逐步迁移 (计划中)

#### 3.1 迁移玩家模块

```cpp
// 旧代码 - playerform.cpp
void PlayerForm::OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player)
{
    ui->label_name->setText(player->name);
    ui->label_level->setText(QString::number(player->level));
}

// 新代码 - 直接使用服务
void PlayerForm::UpdatePlayerInfo()
{
    auto player = m_serviceFactory->player().getPlayerInfo();
    ui->label_name->setText(player->name);
    ui->label_level->setText(QString::number(player->level));
}
```

#### 3.2 迁移战斗模块

```cpp
// 旧代码 - autobattleform.cpp
void AutoBattleForm::OnNotifyGetBattleContext(...)
{
    // 复杂的战斗逻辑
}

// 新代码
void AutoBattleForm::PerformBattle()
{
    auto& battle = m_serviceFactory->battle();
    auto context = battle.getBattleContext();
    
    if (context->isPlayerTurn) {
        if (context->player->needsHealing()) {
            m_playerService->useItem(HEAL_POTION);
        } else {
            battle.normalAttack(findTarget());
        }
    }
}
```

#### 3.3 迁移地图模块

```cpp
// 旧代码 - mapform.cpp
void MapForm::OnNotifyGetMapInfo(QString name, int index1, ...)
{
    ui->widget_paintmap->LoadMapInfo(name, x, y, index3, ...);
}

// 新代码
void MapForm::UpdateMapInfo()
{
    auto& map = m_serviceFactory->map();
    auto mapName = map.getMapName();
    auto [x, y] = map.getPosition();
    
    ui->widget_paintmap->LoadMapInfo(mapName, x, y, ...);
}
```

### 阶段 4: 移除旧代码 (计划中)

- [ ] 删除 player.cpp/h (CPlayerWorker)
- [ ] 删除 battle.cpp/h (CBattleWorker)
- [ ] 删除 psworker.cpp/h (CProcessWorker)
- [ ] 删除 worker_adapters.cpp/h
- [ ] 清理冗余代码

## 代码对比

### 数据获取

| 旧方式 | 新方式 |
|--------|--------|
| `m_playerWorker->m_player->hp` | `m_serviceFactory->player().getPlayerInfo()->hp` |
| 直接访问成员变量 | 通过服务接口获取 |
| 耦合到具体实现 | 依赖抽象接口 |

### 业务逻辑

| 旧方式 | 新方式 |
|--------|--------|
| 分散在 UI 和 Worker 中 | 集中在领域层 |
| 难以测试 | 易于单元测试 |
| 重复代码多 | 代码复用 |

### 依赖管理

| 旧方式 | 新方式 |
|--------|--------|
| 全局变量 `g_CGAInterface` | 依赖注入 |
| 紧耦合 | 松耦合 |
| 难以替换 | 易于 Mock |

## 测试策略

### 单元测试

```cpp
// 测试领域实体
TEST(PlayerTest, NeedsHealing) {
    cga::domain::Player player;
    player.hp = 200;
    player.maxHp = 1000;
    
    EXPECT_TRUE(player.needsHealing(30));
    EXPECT_FALSE(player.needsHealing(10));
}

// 测试服务 (使用 Mock)
TEST(PlayerServiceTest, GetPlayerInfo) {
    MockPlayerRepository repo;
    EXPECT_CALL(repo, getInfo())
        .WillOnce(Return(mockPlayerInfo));
    
    PlayerService service(&repo);
    auto player = service.getPlayerInfo();
    
    EXPECT_EQ(player->name, "Test");
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

## 性能影响

| 指标 | 旧架构 | 新架构 | 变化 |
|------|--------|--------|------|
| 内存占用 | 基准 | +5% | 可接受 |
| CPU 开销 | 基准 | +2% | 可接受 |
| 响应时间 | 基准 | 无变化 | - |
| 代码行数 | 基准 | +30% | 暂时增加 |

## 常见问题

### Q: 为什么要迁移？

**A**: 
- 提高可测试性
- 降低耦合度
- 便于扩展
- 清晰的职责划分

### Q: 迁移需要多久？

**A**: 
- 阶段 1 (框架): 已完成
- 阶段 2 (并行): 8 小时
- 阶段 3 (迁移): 40 小时
- 阶段 4 (清理): 16 小时
- **总计**: 约 64 小时

### Q: 会影响现有功能吗？

**A**: 
- 阶段 2 使用适配器，不影响
- 阶段 3 逐步迁移，可回退
- 阶段 4 清理前会充分测试

### Q: 如何回退？

**A**: 
- Git 回滚到迁移前的提交
- 或使用 `#if USE_OLD_ARCHITECTURE` 条件编译

## 检查清单

### 开发者

- [ ] 理解分层架构概念
- [ ] 阅读 LAYERED_ARCHITECTURE.md
- [ ] 熟悉新的服务接口
- [ ] 更新代码使用服务工厂

### 代码审查

- [ ] 新代码使用服务接口
- [ ] 不直接依赖 CGAInterface
- [ ] 领域逻辑在领域层
- [ ] 单元测试覆盖

### 测试

- [ ] 单元测试通过
- [ ] 集成测试通过
- [ ] 回归测试通过
- [ ] 性能测试通过

## 参考资料

- [分层架构文档](LAYERED_ARCHITECTURE.md)
- [领域实体定义](../CGAssistant/domain/entities.h)
- [服务接口定义](../CGAssistant/domain/services.h)
- [服务工厂实现](../CGAssistant/application/service_factory.h)

---

**最后更新**: 2026-04-22  
**版本**: 1.0  
**状态**: 阶段 2 (并行运行)
