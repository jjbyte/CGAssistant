# RPC 超时配置文档

## 概述

CGAssistant 现在使用统一的 RPC 超时配置，防止 RPC 调用永久阻塞，并提供自动重试机制。

## 配置参数

### 超时时间

| 操作类型 | 超时时间 | 说明 |
|----------|----------|------|
| `DEFAULT_TIMEOUT_MS` | 5000ms | 默认超时 (5 秒) |
| `CONNECT_TIMEOUT_MS` | 10000ms | 连接超时 (10 秒) |
| `BATTLE_TIMEOUT_MS` | 3000ms | 战斗操作 (3 秒) |
| `MAP_TIMEOUT_MS` | 3000ms | 地图操作 (3 秒) |
| `ITEM_TIMEOUT_MS` | 3000ms | 物品操作 (3 秒) |

### 重试配置

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `MAX_RETRY_COUNT` | 3 | 最大重试次数 |
| `RETRY_DELAY_MS` | 100ms | 初始重试延迟 |
| `RETRY_BACKOFF_MULTIPLIER` | 2 | 退避倍数 (指数退避) |
| `MAX_CONNECTION_ATTEMPTS` | 5 | 最大连接尝试次数 |
| `CONNECTION_RETRY_DELAY_MS` | 500ms | 连接重试延迟 |

## 重试策略

使用**指数退避**策略：

```
重试 1: 等待 100ms
重试 2: 等待 200ms
重试 3: 等待 400ms
```

公式：`delay = RETRY_DELAY_MS * (2 ^ retryCount)`

## 异常类型

### RpcTimeoutException

RPC 调用超时。

```cpp
try {
    cgaInterface->GetPlayerInfo(info);
} catch (const cga::RpcTimeoutException& e) {
    LOG_ERROR("RPC 超时：{}", e.method());
}
```

### RpcConnectionException

RPC 连接失败。

```cpp
try {
    cgaInterface->Connect(port);
} catch (const cga::RpcConnectionException& e) {
    LOG_ERROR("连接失败：{}", e.what());
}
```

### RpcCallException

RPC 调用失败 (非超时)。

```cpp
try {
    cgaInterface->BattleNormalAttack(target);
} catch (const cga::RpcCallException& e) {
    LOG_ERROR("调用失败：{} - {}", e.method(), e.error());
}
```

## 日志输出

### 连接日志

```
[11:32:45.123] [info] 正在连接 RPC 服务器 - 端口：4396 超时：10000ms
[11:32:45.456] [info] RPC 连接成功 - 端口：4396
[11:32:46.789] [warn] RPC 连接失败 (尝试 1/5) - 错误：Connection refused
[11:32:47.289] [debug] 等待 500ms 后重试...
```

### 调用日志

```
[11:32:48.123] [debug] RPC 调用成功 - Initialize
[11:32:49.456] [trace] RPC 调用超时 (尝试 1/3) - IsInGame
[11:32:49.556] [trace] RPC 调用超时 (尝试 2/3) - IsInGame
[11:32:49.756] [error] RPC 调用失败 - IsInGame: Timeout expired
```

## 使用示例

### 基本使用

```cpp
#include "rpc_config.h"

// 使用默认超时
auto timeout = cga::RpcConfig::GetTimeout(
    cga::RpcConfig::TimeoutLevel::Normal);  // 5000ms

// 计算重试延迟
int delay = cga::RpcConfig::GetRetryDelay(0);  // 100ms
delay = cga::RpcConfig::GetRetryDelay(1);      // 200ms
delay = cga::RpcConfig::GetRetryDelay(2);      // 400ms

// 检查是否应该重试
if (cga::RpcConfig::ShouldRetry(retryCount)) {
    // 重试逻辑
}
```

### 自定义超时

```cpp
// 快速操作 (1 秒)
auto fastTimeout = cga::RpcConfig::GetTimeout(
    cga::RpcConfig::TimeoutLevel::Fast);

// 慢速操作 (10 秒)
auto slowTimeout = cga::RpcConfig::GetTimeout(
    cga::RpcConfig::TimeoutLevel::Slow);
```

## 配置修改

如需修改超时参数，编辑 `CGALib/rpc_config.h`:

```cpp
struct RpcConfig {
    static constexpr int DEFAULT_TIMEOUT_MS = 5000;  // 修改这里
    static constexpr int MAX_RETRY_COUNT = 3;        // 修改这里
    // ...
};
```

## 性能影响

- **超时检测**: 几乎无影响 (使用 RPC 库内置机制)
- **重试机制**: 增加最多 700ms 延迟 (100+200+400)
- **日志记录**: 轻微影响 (异步日志)

## 故障排查

### 问题：频繁超时

**可能原因**:
1. 游戏进程卡死
2. RPC 服务器过载
3. 系统资源不足

**解决方案**:
```cpp
// 临时增加超时
static constexpr int DEFAULT_TIMEOUT_MS = 10000;  // 改为 10 秒
```

### 问题：重试过多

**可能原因**: 网络不稳定或服务器问题

**解决方案**:
```cpp
// 减少重试次数
static constexpr int MAX_RETRY_COUNT = 2;  // 改为 2 次
```

### 问题：连接失败

**检查步骤**:
1. 确认游戏进程运行正常
2. 确认端口未被占用
3. 检查防火墙设置

**日志分析**:
```bash
# 查找所有连接错误
grep "RPC 连接失败" logs/cga.log

# 统计超时次数
grep "RPC 调用超时" logs/cga.log | wc -l
```

## 最佳实践

### ✅ 推荐

```cpp
// 1. 捕获所有异常
try {
    cgaInterface->SomeOperation();
} catch (const std::exception& e) {
    LOG_ERROR("操作失败：{}", e.what());
}

// 2. 记录重试日志
LOG_WARN("重试操作 ({}次)", retryCount);

// 3. 设置合理超时
auto timeout = std::chrono::milliseconds(5000);
```

### ❌ 避免

```cpp
// 1. 无限等待 (无超时)
cgaInterface->SomeOperation();  // ❌ 可能永久阻塞

// 2. 忽略异常
try {
    cgaInterface->SomeOperation();
} catch (...) {}  // ❌ 吞掉所有错误

// 3. 过短超时
auto timeout = std::chrono::milliseconds(100);  // ❌ 太短
```

---

**最后更新**: 2026-04-22  
**版本**: 1.0
