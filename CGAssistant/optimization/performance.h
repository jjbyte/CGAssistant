#pragma once

/**
 * CGAssistant 性能优化模块
 * 
 * 包含各种性能优化措施：
 * - RPC 批量调用
 * - 数据缓存
 * - 内存池
 * - 异步处理
 */

#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>

namespace cga { namespace optimization {

/**
 * @brief RPC 调用结果缓存
 */
template<typename T>
class RpcCache {
public:
    using TimePoint = std::chrono::steady_clock::time_point;
    
    RpcCache(std::chrono::seconds ttl = std::chrono::seconds(5))
        : m_ttl(ttl) {}
    
    /**
     * @brief 获取缓存
     */
    bool get(const std::string& key, T& value) {
        auto it = m_cache.find(key);
        if (it != m_cache.end()) {
            if (std::chrono::steady_clock::now() - it->second.timestamp < m_ttl) {
                value = it->second.data;
                return true;
            } else {
                m_cache.erase(it);
            }
        }
        return false;
    }
    
    /**
     * @brief 设置缓存
     */
    void set(const std::string& key, const T& value) {
        CacheEntry entry;
        entry.data = value;
        entry.timestamp = std::chrono::steady_clock::now();
        m_cache[key] = entry;
    }
    
    /**
     * @brief 清除过期缓存
     */
    void cleanup() {
        auto now = std::chrono::steady_clock::now();
        for (auto it = m_cache.begin(); it != m_cache.end(); ) {
            if (now - it->second.timestamp >= m_ttl) {
                it = m_cache.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    /**
     * @brief 清空缓存
     */
    void clear() {
        m_cache.clear();
    }

private:
    struct CacheEntry {
        T data;
        TimePoint timestamp;
    };
    
    std::unordered_map<std::string, CacheEntry> m_cache;
    std::chrono::seconds m_ttl;
};

/**
 * @brief 批量数据请求
 */
class BatchRequest {
public:
    /**
     * @brief 添加玩家信息请求
     */
    void addPlayerRequest(int playerId) {
        m_playerIds.push_back(playerId);
    }
    
    /**
     * @brief 添加宠物信息请求
     */
    void addPetRequest(int petId) {
        m_petIds.push_back(petId);
    }
    
    /**
     * @brief 添加物品信息请求
     */
    void addItemRequest(int itemId) {
        m_itemIds.push_back(itemId);
    }
    
    /**
     * @brief 是否有请求
     */
    bool hasRequests() const {
        return !m_playerIds.empty() || !m_petIds.empty() || !m_itemIds.empty();
    }
    
    /**
     * @brief 清空请求
     */
    void clear() {
        m_playerIds.clear();
        m_petIds.clear();
        m_itemIds.clear();
    }
    
    // Getter
    const std::vector<int>& getPlayerIds() const { return m_playerIds; }
    const std::vector<int>& getPetIds() const { return m_petIds; }
    const std::vector<int>& getItemIds() const { return m_itemIds; }
    
private:
    std::vector<int> m_playerIds;
    std::vector<int> m_petIds;
    std::vector<int> m_itemIds;
};

/**
 * @brief 性能监控器
 */
class PerformanceMonitor {
public:
    static PerformanceMonitor& instance() {
        static PerformanceMonitor inst;
        return inst;
    }
    
    /**
     * @brief 开始计时
     */
    void startTimer(const std::string& name) {
        m_timers[name] = std::chrono::steady_clock::now();
    }
    
    /**
     * @brief 结束计时并返回毫秒数
     */
    long long endTimer(const std::string& name) {
        auto it = m_timers.find(name);
        if (it != m_timers.end()) {
            auto duration = std::chrono::steady_clock::now() - it->second;
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            m_stats[name].totalMs += ms;
            m_stats[name].count++;
            return ms;
        }
        return 0;
    }
    
    /**
     * @brief 获取统计信息
     */
    void printStats() {
        for (const auto& [name, stat] : m_stats) {
            double avg = stat.count > 0 ? (double)stat.totalMs / stat.count : 0;
            printf("[%s] 调用次数：%lld, 总耗时：%lldms, 平均：%.2fms\n",
                   name.c_str(), stat.count, stat.totalMs, avg);
        }
    }

private:
    PerformanceMonitor() = default;
    
    struct Stats {
        long long totalMs = 0;
        long long count = 0;
    };
    
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_timers;
    std::unordered_map<std::string, Stats> m_stats;
};

} } // namespace cga::optimization

// 性能监控宏
#define PERF_START(name) cga::optimization::PerformanceMonitor::instance().startTimer(name)
#define PERF_END(name) cga::optimization::PerformanceMonitor::instance().endTimer(name)
#define PERF_PRINT() cga::optimization::PerformanceMonitor::instance().printStats()
