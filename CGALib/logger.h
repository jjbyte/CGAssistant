#pragma once

/**
 * CGAssistant 日志系统
 * 
 * 使用方法:
 *   LOG_INFO("玩家 HP: {}/{}", hp, maxhp);
 *   LOG_ERROR("RPC 调用失败：{}", error_msg);
 *   LOG_DEBUG("战斗回合：{}", round);
 *   LOG_WARN("物品数量不足");
 * 
 * 日志文件:
 *   - logs/cga.log (轮转日志，最大 10MB，保留 3 个文件)
 *   - logs/error.log (仅错误级别)
 *   - 控制台输出 (带颜色)
 */

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>
#include <string>

namespace cga {

class Logger {
public:
    /**
     * @brief 初始化日志系统
     * @param log_dir 日志目录 (默认为 "logs")
     * @param level 日志级别 (默认为 Debug)
     * @return true 初始化成功
     */
    static bool Init(const std::string& log_dir = "logs", 
                     spdlog::level::level_enum level = spdlog::level::debug);
    
    /**
     * @brief 关闭日志系统
     */
    static void Shutdown();
    
    /**
     * @brief 设置日志级别
     */
    static void SetLevel(spdlog::level::level_enum level);
    
    /**
     * @brief 获取当前日志级别
     */
    static spdlog::level::level_enum GetLevel();
    
    /**
     * @brief 刷新所有日志缓冲
     */
    static void Flush();
    
    /**
     * @brief 设置自动刷新间隔 (秒)
     */
    static void SetFlushInterval(int seconds);
    
    // 获取日志器实例
    static std::shared_ptr<spdlog::logger> GetConsoleLogger();
    static std::shared_ptr<spdlog::logger> GetFileLogger();
    static std::shared_ptr<spdlog::logger> GetErrorLogger();

private:
    static std::shared_ptr<spdlog::logger> s_console_logger;
    static std::shared_ptr<spdlog::logger> s_file_logger;
    static std::shared_ptr<spdlog::logger> s_error_logger;
    static bool s_initialized;
};

} // namespace cga

// ============================================================================
// 宏定义 - 使用示例:
//   LOG_INFO("玩家等级：{}", level);
//   LOG_ERROR("连接失败：{}", error);
// ============================================================================

#define LOG_TRACE(...) SPDLOG_CALL(trace, __VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_CALL(debug, __VA_ARGS__)
#define LOG_INFO(...)  SPDLOG_CALL(info, __VA_ARGS__)
#define LOG_WARN(...)  SPDLOG_CALL(warn, __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_CALL(error, __VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_CALL(critical, __VA_ARGS__)

// 内部宏 - 自动选择文件日志器
#define SPDLOG_CALL(level, ...) \
    do { \
        if (cga::Logger::GetFileLogger()) { \
            cga::Logger::GetFileLogger()->level(__VA_ARGS__); \
        } \
        if (cga::Logger::GetConsoleLogger() && \
            cga::Logger::GetConsoleLogger()->should_log(spdlog::level::level_enum::level)) { \
            cga::Logger::GetConsoleLogger()->level(__VA_ARGS__); \
        } \
    } while(0)

// 仅控制台输出
#define LOG_CONSOLE(level, ...) \
    do { \
        if (cga::Logger::GetConsoleLogger()) { \
            cga::Logger::GetConsoleLogger()->level(__VA_ARGS__); \
        } \
    } while(0)

// 仅文件输出
#define LOG_FILE(level, ...) \
    do { \
        if (cga::Logger::GetFileLogger()) { \
            cga::Logger::GetFileLogger()->level(__VA_ARGS__); \
        } \
    } while(0)

// 性能分析宏
#define LOG_SCOPE(title) \
    spdlog::stopwatch spdlog_sw_##__LINE__; \
    LOG_DEBUG("[SCOPE START] {}", title); \
    auto scope_guard_##__LINE__ = [&]() { \
        LOG_DEBUG("[SCOPE END] {} - {:.3f}s", title, spdlog_sw_##__LINE__); \
    }(); \
    (void)scope_guard_##__LINE__
