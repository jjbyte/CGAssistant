# CGAssistant 日志系统使用指南

## 快速开始

### 1. 基本用法

```cpp
#include "../CGALib/logger.h"

// 在初始化时调用
cga::Logger::Init("logs", spdlog::level::debug);

// 记录日志
LOG_INFO("玩家等级：{}", level);
LOG_ERROR("连接失败：{}", error_msg);
LOG_DEBUG("战斗回合：{}", round);
LOG_WARN("物品数量不足");
LOG_TRACE("详细调试信息");
```

### 2. 日志级别

| 级别 | 宏 | 用途 | 示例 |
|------|-----|------|------|
| Trace | `LOG_TRACE` | 最详细调试信息 | 函数入口/出口 |
| Debug | `LOG_DEBUG` | 调试信息 | 变量值、状态 |
| Info | `LOG_INFO` | 一般信息 | 启动、战斗开始 |
| Warning | `LOG_WARN` | 警告 | 非致命问题 |
| Error | `ERROR` | 错误 | 操作失败 |
| Critical | `LOG_CRITICAL` | 严重错误 | 崩溃、异常 |

### 3. 格式化语法

日志系统使用 `{fmt}` 库的格式化语法：

```cpp
// 基本类型
LOG_INFO("HP: {}/{} 等级：{}", hp, maxhp, level);

// 浮点数精度
LOG_INFO("坐标：{:.2f}, {:.2f}", x, y);

// 字符串
LOG_INFO("玩家名：{}", name.c_str());

// 十六进制
LOG_INFO("地址：0x{:08X}", address);

// 布尔值
LOG_INFO("状态：{}", connected ? "已连接" : "未连接");
```

## 输出目标

### 控制台输出
- 带颜色区分级别
- 格式：`[HH:MM:SS.mmm] [级别] 消息`
- 示例：<span style="color: green">[11:32:45.123] [info] 玩家 HP: 100/500</span>

### 文件输出 (logs/cga.log)
- 轮转日志，最大 10MB
- 保留最近 3 个文件
- 格式：`[YYYY-MM-DD HH:MM:SS.mmm] [线程 ID] [级别] 消息`

### 错误日志 (logs/error.log)
- 仅记录 Error 和 Critical 级别
- 永久保留，不轮转

## 高级功能

### 1. 性能分析

```cpp
// 使用作用域日志记录函数执行时间
void ExpensiveFunction() {
    LOG_SCOPE("耗时函数");
    
    // ... 耗时操作 ...
}

// 输出:
// [SCOPE START] 耗时函数
// [SCOPE END] 耗时函数 - 0.234s
```

### 2. 动态调整日志级别

```cpp
// 运行时调整
cga::Logger::SetLevel(spdlog::level::warn);  // 只记录警告及以上
cga::Logger::SetLevel(spdlog::level::debug); // 恢复详细日志
```

### 3. 条件日志

```cpp
// 避免不必要的字符串构造
if (LOG_DEBUG_ENABLED) {
    LOG_DEBUG("详细数据：{}", expensive_operation());
}

// 或使用 spdlog 内置方法
if (cga::Logger::GetFileLogger()->should_log(spdlog::level::debug)) {
    LOG_DEBUG("...");
}
```

### 4. 崩溃日志

系统会自动记录崩溃信息：

```
[CRITICAL] ========================================
[CRITICAL] 程序发生严重异常，正在生成崩溃转储...
[CRITICAL] 异常代码：0xC0000005
[CRITICAL] 异常地址：0x004A3B2C
[CRITICAL] 崩溃转储已保存：minidump.mdmp
[CRITICAL] ========================================
```

## 日志配置

### 初始化选项

```cpp
// 完整配置示例
cga::Logger::Init(
    "logs",                      // 日志目录
    spdlog::level::debug         // 日志级别
);

// 设置自动刷新间隔 (默认 30 秒)
cga::Logger::SetFlushInterval(10); // 10 秒

// 手动刷新
cga::Logger::Flush();

// 关闭日志
cga::Logger::Shutdown();
```

### 环境变量

可通过环境变量控制日志行为：

| 变量 | 说明 | 默认值 |
|------|------|--------|
| `CGA_LOG_LEVEL` | 日志级别 (0-6) | 1 (debug) |
| `CGA_LOG_DIR` | 日志目录 | logs |
| `CGA_LOG_ASYNC` | 异步日志 (0/1) | 1 |

## 最佳实践

### ✅ 推荐

```cpp
// 1. 使用有意义的日志消息
LOG_INFO("战斗开始 - 敌人数量：{}", enemy_count);

// 2. 记录关键操作
LOG_DEBUG("使用物品：{} 目标：{}", item_name, target_id);

// 3. 错误时记录上下文
LOG_ERROR("RPC 调用失败：{} 回合数：{}", error, round_count);

// 4. 性能敏感代码使用 TRACE 级别
LOG_TRACE("函数入口：{} {}", param1, param2);
```

### ❌ 避免

```cpp
// 1. 避免在循环中记录大量日志
for (int i = 0; i < 1000; i++) {
    LOG_INFO("迭代：{}", i);  // ❌ 会刷屏
}

// 2. 避免记录敏感信息
LOG_INFO("账号：{} 密码：{}", username, password);  // ❌ 危险!

// 3. 避免无意义的日志
LOG_INFO("执行到这里");  // ❌ 没有信息量
```

## 故障排查

### 问题：日志文件没有生成

**解决方案**:
1. 检查程序是否有日志目录写入权限
2. 确认 `cga::Logger::Init()` 已调用
3. 检查磁盘空间

### 问题：日志丢失

**可能原因**: 程序崩溃时日志缓冲未刷新

**解决方案**:
```cpp
// 确保关键日志后立即刷新
LOG_CRITICAL("关键操作开始");
cga::Logger::Flush();
```

### 问题：性能影响

**优化建议**:
1. 生产环境使用 `info` 级别而非 `debug`
2. 启用异步日志 (默认已启用)
3. 避免在热点代码路径记录大量日志

## 日志分析工具

### 使用 grep 过滤

```bash
# 查找所有错误
grep "\[error\]" logs/cga.log

# 查找特定功能
grep "战斗" logs/cga.log

# 统计日志级别
grep -o "\[info\]\|\[error\]\|\[warn\]" logs/cga.log | sort | uniq -c
```

### 使用 VSCode 查看

安装 `Log File Highlighter` 扩展，自动高亮日志级别。

## API 参考

### Logger 类

```cpp
namespace cga {
class Logger {
public:
    static bool Init(const std::string& log_dir = "logs", 
                     spdlog::level::level_enum level = debug);
    static void Shutdown();
    static void SetLevel(spdlog::level::level_enum level);
    static void Flush();
    static void SetFlushInterval(int seconds);
};
}
```

### 宏定义

```cpp
LOG_TRACE(format, ...)
LOG_DEBUG(format, ...)
LOG_INFO(format, ...)
LOG_WARN(format, ...)
LOG_ERROR(format, ...)
LOG_CRITICAL(format, ...)
LOG_SCOPE(title)  // 性能分析
```

---

**最后更新**: 2026-04-22  
**版本**: 1.0
