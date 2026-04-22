#include "logger.h"
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
#include <spdlog/fmt/ostr.h>
#include <filesystem>
#include <windows.h>

namespace cga {

// 静态成员初始化
std::shared_ptr<spdlog::logger> Logger::s_console_logger = nullptr;
std::shared_ptr<spdlog::logger> Logger::s_file_logger = nullptr;
std::shared_ptr<spdlog::logger> Logger::s_error_logger = nullptr;
bool Logger::s_initialized = false;

bool Logger::Init(const std::string& log_dir, spdlog::level::level_enum level)
{
    if (s_initialized) {
        return true;
    }

    try {
        // 创建日志目录
        std::filesystem::path log_path(log_dir);
        if (!std::filesystem::exists(log_path)) {
            std::filesystem::create_directories(log_path);
        }

        // 1. 控制台日志器 (带颜色)
        s_console_logger = spdlog::stdout_color_mt("console");
        s_console_logger->set_pattern("%^[%H:%M:%S.%e] [%l] %v%$");
        s_console_logger->set_level(level);

        // 2. 文件日志器 (轮转，最大 10MB，保留 3 个文件)
        std::string file_path = log_path.string() + "/cga.log";
        s_file_logger = spdlog::rotating_logger_mt<spdlog::async_factory>(
            "file", 
            file_path, 
            10485760,  // 10MB
            3          // 保留 3 个文件
        );
        s_file_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v");
        s_file_logger->set_level(level);

        // 3. 错误日志器 (单独文件)
        std::string error_path = log_path.string() + "/error.log";
        s_error_logger = spdlog::basic_logger_mt<spdlog::async_factory>("error", error_path);
        s_error_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%l] %v");
        s_error_logger->set_level(spdlog::level::err);

        // 设置全局默认日志器
        spdlog::set_default_logger(s_file_logger);

        // 设置自动刷新间隔 (每 30 秒)
        spdlog::flush_every(std::chrono::seconds(30));

        // 设置异步线程池 (可选，提高性能)
        spdlog::init_thread_pool(8192, 1);

        s_initialized = true;

        LOG_INFO("========================================");
        LOG_INFO("CGAssistant 日志系统已初始化");
        LOG_INFO("日志目录：{}", log_path.string());
        LOG_INFO("日志级别：{}", spdlog::to_string_view(level));
        LOG_INFO("========================================");

        return true;
    }
    catch (const spdlog::spdlog_ex& ex) {
        // spdlog 初始化失败，输出到 stderr
        fprintf(stderr, "日志系统初始化失败：%s\n", ex.what());
        return false;
    }
    catch (const std::exception& ex) {
        fprintf(stderr, "日志系统初始化失败：%s\n", ex.what());
        return false;
    }
}

void Logger::Shutdown()
{
    if (!s_initialized) {
        return;
    }

    LOG_INFO("日志系统正在关闭...");

    // 刷新所有缓冲
    spdlog::shutdown();

    // 释放所有日志器
    s_console_logger.reset();
    s_file_logger.reset();
    s_error_logger.reset();

    s_initialized = false;
}

void Logger::SetLevel(spdlog::level::level_enum level)
{
    if (s_console_logger) {
        s_console_logger->set_level(level);
    }
    if (s_file_logger) {
        s_file_logger->set_level(level);
    }
}

spdlog::level::level_enum Logger::GetLevel()
{
    if (s_file_logger) {
        return s_file_logger->level();
    }
    return spdlog::level::info;
}

void Logger::Flush()
{
    spdlog::dump_backtrace();
    if (s_console_logger) {
        s_console_logger->flush();
    }
    if (s_file_logger) {
        s_file_logger->flush();
    }
    if (s_error_logger) {
        s_error_logger->flush();
    }
}

void Logger::SetFlushInterval(int seconds)
{
    spdlog::flush_every(std::chrono::seconds(seconds));
}

std::shared_ptr<spdlog::logger> Logger::GetConsoleLogger()
{
    return s_console_logger;
}

std::shared_ptr<spdlog::logger> Logger::GetFileLogger()
{
    return s_file_logger;
}

std::shared_ptr<spdlog::logger> Logger::GetErrorLogger()
{
    return s_error_logger;
}

} // namespace cga
