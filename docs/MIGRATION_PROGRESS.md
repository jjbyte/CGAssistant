# 架构迁移实施记录

## 已完成工作

### 阶段 1: 基础框架 ✅

- [x] 领域层 (8 个实体 + 5 个接口)
- [x] 应用层 (5 个服务实现)
- [x] 基础设施层 (5 个仓库实现)
- [x] 服务工厂和依赖注入
- [x] Worker 适配器 (向后兼容)

### 阶段 2: 模块迁移 🔄

#### 玩家模块 (50% 完成)

**已完成**:
- [x] playerform.h - 添加服务工厂支持
- [x] playerform.cpp - 添加 InitializeWithServices() 方法
- [x] playerform.cpp - 添加 UpdatePlayerInfoNew() 方法
- [x] playerform.cpp - 添加 UpdatePetsInfoNew() 方法
- [x] playerform.cpp - 添加 UpdateSkillsInfoNew() 方法

**待完成**:
- [ ] 修改 OnNotifyGetPlayerInfo() 支持双架构
- [ ] 修改 OnNotifyGetPetsInfo() 支持双架构
- [ ] 修改 OnNotifyGetSkillsInfo() 支持双架构
- [ ] 测试新旧架构切换

#### 战斗模块 (0% 完成)

**待完成**:
- [ ] autobattleform.h - 添加服务工厂支持
- [ ] autobattleform.cpp - 实现新战斗逻辑
- [ ] battlesetting.cpp - 迁移到新架构
- [ ] 测试自动战斗功能

#### 地图模块 (0% 完成)

**待完成**:
- [ ] mapform.h - 添加服务工厂支持
- [ ] mapform.cpp - 使用 MapService
- [ ] mypaintmap.cpp - 支持新数据格式
- [ ] 测试地图显示和寻路

#### 其他模块 (0% 完成)

**待完成**:
- [ ] chatform.cpp - 使用 ChatService
- [ ] scriptform.cpp - 使用新服务
- [ ] itemform.cpp - 使用 ItemService
- [ ] accountform.cpp - 使用新服务

## 代码示例

### 玩家模块迁移

```cpp
// playerform.h - 添加新架构支持
class PlayerForm : public QWidget {
private:
    // 旧架构 (向后兼容)
    CPlayerWorker *m_worker;
    
    // 新架构
    std::shared_ptr<cga::application::ServiceFactory> m_serviceFactory;
    cga::PlayerWorkerAdapter* m_workerAdapter;
    
public:
    void InitializeWithServices(
        std::shared_ptr<cga::application::ServiceFactory> serviceFactory
    );
    
private:
    void UpdatePlayerInfoNew();
    void UpdatePetsInfoNew();
    void UpdateSkillsInfoNew();
};

// playerform.cpp - 实现
void PlayerForm::InitializeWithServices(...)
{
    m_serviceFactory = serviceFactory;
    
    // 创建 Worker 适配器保持兼容
    m_workerAdapter = new cga::PlayerWorkerAdapter(...);
    
    // 连接信号
    connect(m_workerAdapter, &PlayerWorkerAdapter::NotifyGetPlayerInfo,
            this, &PlayerForm::OnNotifyGetPlayerInfo);
}

void PlayerForm::UpdatePlayerInfoNew()
{
    auto player = m_serviceFactory->player().getPlayerInfo();
    if (player) {
        ui->label_name->setText(player->name);
        ui->label_level->setText(QString("Lv.%1").arg(player->level));
        // ...
    }
}
```

## 迁移步骤

### 1. 修改头文件

```cpp
// 添加新架构包含
#include "application/service_factory.h"
#include "worker_adapters.h"

// 添加成员变量
std::shared_ptr<ServiceFactory> m_serviceFactory;
PlayerWorkerAdapter* m_workerAdapter;

// 添加初始化方法
void InitializeWithServices(std::shared_ptr<ServiceFactory> factory);
```

### 2. 修改构造函数

```cpp
PlayerForm::PlayerForm(...) 
    : m_worker(worker),
      m_serviceFactory(nullptr),
      m_workerAdapter(nullptr)
{
    // ...
}
```

### 3. 实现 InitializeWithServices

```cpp
void PlayerForm::InitializeWithServices(...)
{
    m_serviceFactory = factory;
    m_workerAdapter = new PlayerWorkerAdapter(...);
    
    // 连接适配器信号
    connect(m_workerAdapter, &PlayerWorkerAdapter::NotifyGetPlayerInfo,
            this, &PlayerForm::OnNotifyGetPlayerInfo);
}
```

### 4. 添加新架构方法

```cpp
void PlayerForm::UpdatePlayerInfoNew()
{
    auto player = m_serviceFactory->player().getPlayerInfo();
    // 更新 UI...
}
```

### 5. 修改现有槽函数支持双架构

```cpp
void PlayerForm::OnNotifyGetPlayerInfo(...)
{
    if (m_serviceFactory) {
        UpdatePlayerInfoNew();  // 新架构
    } else {
        // 旧架构逻辑
    }
}
```

## 测试计划

### 单元测试

```cpp
TEST(PlayerFormTest, InitializeWithServices) {
    auto form = new PlayerForm(oldWorker, battleWorker);
    auto factory = ServiceFactory::Create(cga);
    
    form->InitializeWithServices(factory);
    
    // 验证服务工厂已设置
    // 验证适配器已创建
}
```

### 集成测试

```cpp
TEST(IntegrationTest, PlayerWorkflow) {
    // 1. 创建主窗口
    auto mainWindow = new MainWindow();
    
    // 2. 使用新架构初始化
    mainWindow->UseNewArchitecture();
    
    // 3. 验证功能正常
    ASSERT_TRUE(mainWindow->IsUsingNewArchitecture());
}
```

### 回归测试

- [ ] 玩家信息显示正常
- [ ] 宠物列表显示正常
- [ ] 技能列表显示正常
- [ ] 设置保存/加载正常
- [ ] 自动战斗功能正常
- [ ] 地图显示正常
- [ ] 聊天功能正常

## 时间估算

| 模块 | 预计时间 | 实际时间 | 状态 |
|------|----------|----------|------|
| 玩家模块 | 8h | 4h | 50% |
| 战斗模块 | 12h | 0h | 0% |
| 地图模块 | 8h | 0h | 0% |
| 其他模块 | 12h | 0h | 0% |
| 测试 | 16h | 0h | 0% |
| **总计** | **56h** | **4h** | **7%** |

## 遇到的问题

### 问题 1: 信号槽兼容性

**问题**: 新架构不使用信号槽，但 UI 层依赖信号

**解决**: 使用 Worker 适配器模式，在内部调用服务，外部保持信号槽接口

### 问题 2: 数据格式转换

**问题**: 领域实体和旧数据结构不同

**解决**: 在适配器中进行数据转换，保持 UI 层不变

### 问题 3: 循环依赖

**问题**: 某些模块互相依赖

**解决**: 使用接口解耦，通过服务工厂注入

## 下一步计划

1. **完成玩家模块** (4 小时)
   - 修改所有 OnNotify* 方法
   - 添加单元测试
   - 验证功能正常

2. **开始战斗模块** (12 小时)
   - 修改 autobattleform.h/.cpp
   - 迁移 battlesetting.cpp
   - 测试自动战斗

3. **迁移地图模块** (8 小时)
   - 修改 mapform.h/.cpp
   - 测试地图功能

4. **完成其他模块** (12 小时)
   - 逐个迁移剩余模块
   - 集成测试

5. **清理旧代码** (16 小时)
   - 移除 Worker 类
   - 移除适配器
   - 清理冗余代码

---

## 快速开始

### 使用新架构

```cpp
// 1. 在主窗口创建服务工厂
#include "application/service_factory.h"

auto factory = cga::application::ServiceFactory::Create(g_CGAInterface);

// 2. 初始化各个 Form
playerForm->InitializeWithServices(factory);
battleForm->InitializeWithServices(factory);
mapForm->InitializeWithServices(factory);

// 3. 直接使用服务
auto& player = factory->player();
auto playerInfo = player.getPlayerInfo();

auto& battle = factory->battle();
battle.normalAttack(target);
```

### 保持旧架构 (默认)

```cpp
// 无需修改，现有代码继续工作
auto playerForm = new PlayerForm(playerWorker, battleWorker);
auto battleForm = new AutoBattleForm(battleWorker, playerWorker);
```

---

### 地图模块 (5% 完成)

**已完成**:
- [x] mapform_new.h - 创建新架构头文件模板

**待完成**:
- [ ] 修改 mapform.h 添加服务工厂支持
- [ ] 修改 mapform.cpp 使用 MapService
- [ ] 实现 UpdateMapInfoNew 方法
- [ ] 测试地图显示和寻路

### 其他模块 (0% 完成)

**待完成**:
- [ ] chatform.cpp - 使用 ChatService
- [ ] scriptform.cpp - 使用新服务
- [ ] itemform.cpp - 使用 ItemService
- [ ] accountform.cpp - 使用新服务

---

### 其他模块 (0% 完成)

**待完成**:
- [ ] chatform.cpp - 使用 ChatService
- [ ] scriptform.cpp - 使用新服务
- [ ] itemform.cpp - 使用 ItemService
- [ ] accountform.cpp - 使用新服务

---

## 快速开始

### 使用新架构

```cpp
// 1. 在主窗口创建服务工厂
#include "application/service_factory.h"

auto factory = cga::application::ServiceFactory::Create(g_CGAInterface);

// 2. 初始化各个 Form
playerForm->InitializeWithServices(factory);
battleForm->InitializeWithServices(factory);
mapForm->InitializeWithServices(factory);

// 3. 直接使用服务
auto& player = factory->player();
auto playerInfo = player.getPlayerInfo();

auto& battle = factory->battle();
battle.normalAttack(target);

auto& map = factory->map();
auto mapName = map.getMapName();
auto [x, y] = map.getPosition();
```

### 保持旧架构 (默认)

```cpp
// 无需修改，现有代码继续工作
auto playerForm = new PlayerForm(playerWorker, battleWorker);
auto battleForm = new AutoBattleForm(battleWorker, playerWorker);
auto mapForm = new MapForm();
```

---

**最后更新**: 2026-04-22 21:45  
**状态**: 阶段 2 (进行中)  
**完成度**: 35%
