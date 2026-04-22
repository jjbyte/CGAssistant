#pragma once

/**
 * CGAssistant RPC 超时配置
 * 
 * 统一配置所有 RPC 调用的超时时间和重试策略
 */

#include <chrono>
#include <cstdint>

namespace cga {

struct RpcConfig {
    // 默认超时时间 (毫秒)
    static constexpr int DEFAULT_TIMEOUT_MS = 5000;        // 5 秒
    static constexpr int CONNECT_TIMEOUT_MS = 10000;       // 连接 10 秒
    static constexpr int BATTLE_TIMEOUT_MS = 3000;         // 战斗操作 3 秒
    static constexpr int MAP_TIMEOUT_MS = 3000;            // 地图操作 3 秒
    static constexpr int ITEM_TIMEOUT_MS = 3000;           // 物品操作 3 秒
    
    // 重试配置
    static constexpr int MAX_RETRY_COUNT = 3;              // 最大重试次数
    static constexpr int RETRY_DELAY_MS = 100;             // 重试延迟 (毫秒)
    static constexpr int RETRY_BACKOFF_MULTIPLIER = 2;     // 退避倍数
    
    // 连接配置
    static constexpr int MAX_CONNECTION_ATTEMPTS = 5;      // 最大连接尝试
    static constexpr int CONNECTION_RETRY_DELAY_MS = 500;  // 连接重试延迟
    
    // 超时级别
    enum class TimeoutLevel {
        Fast = 1000,      // 快速操作 (1 秒)
        Normal = 5000,    // 普通操作 (5 秒)
        Slow = 10000,     // 慢速操作 (10 秒)
        VerySlow = 30000  // 极慢操作 (30 秒)
    };
    
    /**
     * @brief 获取超时时间
     * @param level 超时级别
     * @return 超时毫秒数
     */
    static inline std::chrono::milliseconds GetTimeout(TimeoutLevel level) {
        return std::chrono::milliseconds(static_cast<int>(level));
    }
    
    /**
     * @brief 计算重试延迟 (指数退避)
     * @param retryCount 当前重试次数 (从 0 开始)
     * @return 延迟毫秒数
     */
    static inline int GetRetryDelay(int retryCount) {
        return RETRY_DELAY_MS * (1 << retryCount);  // 指数退避：100ms, 200ms, 400ms...
    }
    
    /**
     * @brief 是否应该重试
     * @param retryCount 当前重试次数
     * @return true 应该重试
     */
    static inline bool ShouldRetry(int retryCount) {
        return retryCount < MAX_RETRY_COUNT;
    }
};

/**
 * @brief RPC 超时异常
 */
class RpcTimeoutException : public std::runtime_error {
public:
    explicit RpcTimeoutException(const std::string& method)
        : std::runtime_error("RPC timeout: " + method)
        , method_(method) {}
    
    const std::string& method() const { return method_; }
    
private:
    std::string method_;
};

/**
 * @brief RPC 连接异常
 */
class RpcConnectionException : public std::runtime_error {
public:
    explicit RpcConnectionException(const std::string& message)
        : std::runtime_error("RPC connection error: " + message) {}
};

/**
 * @brief RPC 调用异常
 */
class RpcCallException : public std::runtime_error {
public:
    RpcCallException(const std::string& method, const std::string& error)
        : std::runtime_error("RPC call failed: " + method + " - " + error)
        , method_(method)
        , error_(error) {}
    
    const std::string& method() const { return method_; }
    const std::string& error() const { return error_; }
    
private:
    std::string method_;
    std::string error_;
};

} // namespace cga
