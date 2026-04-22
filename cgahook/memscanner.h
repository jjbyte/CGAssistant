#pragma once

/**
 * CGAssistant 内存特征码扫描模块
 * 
 * 用于自动查找游戏内存地址，避免硬编码偏移
 * 游戏小版本更新后无需重新适配
 */

#include <windows.h>
#include <string>
#include <vector>
#include <cstdint>

namespace cga {

/**
 * @brief 内存特征码匹配结果
 */
struct PatternMatch {
    uintptr_t address;      // 匹配地址
    size_t offset;          // 在模块中的偏移
    bool found;             // 是否找到
    
    PatternMatch() : address(0), offset(0), found(false) {}
};

/**
 * @brief 内存扫描器
 */
class MemoryScanner {
public:
    /**
     * @brief 初始化扫描器
     * @param moduleHandle 模块句柄 (NULL 表示当前进程)
     * @return true 初始化成功
     */
    static bool Initialize(HMODULE moduleHandle = NULL);
    
    /**
     * @brief 关闭扫描器
     */
    static void Shutdown();
    
    /**
     * @brief 获取模块基址
     * @return 模块基址
     */
    static uintptr_t GetModuleBase();
    
    /**
     * @brief 获取模块大小
     * @return 模块大小
     */
    static size_t GetModuleSize();
    
    /**
     * @brief 扫描特征码
     * @param pattern 特征码字符串 (如 "8B 0D ?? ?? ?? ??")
     * @param mask 掩码字符串 (如 "xx?????????"，x 表示精确匹配，? 表示通配符)
     * @param offset 地址偏移 (用于计算实际地址)
     * @return 匹配结果
     * 
     * 示例:
     *   PatternMatch match = ScanPattern("8B 0D ?? ?? ?? ??", "xx??????", 2);
     *   if (match.found) {
     *       DWORD* ptr = *(DWORD**)(match.address + match.offset);
     *   }
     */
    static PatternMatch ScanPattern(const char* pattern, const char* mask, int offset = 0);
    
    /**
     * @brief 扫描特征码 (支持多个结果)
     * @param pattern 特征码
     * @param mask 掩码
     * @param maxResults 最大结果数
     * @return 所有匹配结果
     */
    static std::vector<PatternMatch> ScanPatternMulti(const char* pattern, const char* mask, size_t maxResults = 10);
    
    /**
     * @brief 从地址读取相对偏移
     * @param address 起始地址
     * @param offset 偏移量
     * @return 计算后的地址
     * 
     * 示例:
     *   // 指令：8B 05 12 34 56 78  ->  mov eax, [0x78563412]
     *   uintptr_t ptr = ReadRelativeOffset(address, 2);
     */
    static uintptr_t ReadRelativeOffset(uintptr_t address, int offset = 0);
    
    /**
     * @brief 从地址读取指针链
     * @param baseAddress 基址
     * @param offsets 偏移数组
     * @return 最终地址
     * 
     * 示例:
     *   // 读取 [ [base + 0x10] + 0x20 ]
     *   std::vector<int> offsets = {0x10, 0x20};
     *   uintptr_t ptr = ReadPointerChain(baseAddress, offsets);
     */
    static uintptr_t ReadPointerChain(uintptr_t baseAddress, const std::vector<int>& offsets);
    
    /**
     * @brief 在内存中查找字节序列
     * @param buffer 内存缓冲区
     * @param bufferSize 缓冲区大小
     * @param pattern 特征码
     * @param mask 掩码
     * @return 匹配地址 (相对于 buffer)
     */
    static uintptr_t FindPattern(const void* buffer, size_t bufferSize, const char* pattern, const char* mask);
    
    /**
     * @brief 验证内存是否可读
     * @param address 地址
     * @param size 大小
     * @return true 可读
     */
    static bool IsMemoryReadable(uintptr_t address, size_t size);
    
    /**
     * @brief 获取模块导出函数地址
     * @param moduleName 模块名
     * @param functionName 函数名
     * @return 函数地址
     */
    static uintptr_t GetExportFunctionAddress(const char* moduleName, const char* functionName);

private:
    static HMODULE s_moduleHandle;
    static uintptr_t s_moduleBase;
    static size_t s_moduleSize;
    static bool s_initialized;
};

/**
 * @brief 地址解析器 - 用于解析复杂的地址计算
 */
class AddressResolver {
public:
    /**
     * @brief 解析指针偏移地址
     * @param moduleBase 模块基址
     * @param pattern 特征码
     * @param mask 掩码
     * @param offset 特征码内偏移
     * @param dereference 是否解引用
     * @return 解析后的地址
     */
    static uintptr_t ResolvePointer(uintptr_t moduleBase, const char* pattern, const char* mask, 
                                    int offset = 0, bool dereference = true);
    
    /**
     * @brief 解析函数调用地址
     * @param moduleBase 模块基址
     * @param pattern CALL 指令特征码
     * @param mask 掩码
     * @return 调用目标地址
     */
    static uintptr_t ResolveCall(uintptr_t moduleBase, const char* pattern, const char* mask);
    
    /**
     * @brief 解析数据引用地址
     * @param moduleBase 模块基址
     * @param pattern MOV 指令特征码
     * @param mask 掩码
     * @return 数据地址
     */
    static uintptr_t ResolveDataRef(uintptr_t moduleBase, const char* pattern, const char* mask);
};

// 常用特征码定义
namespace Patterns {
    // 玩家基址特征码示例
    // 指令：8B 0D ?? ?? ?? ??  ->  mov ecx, [playerBase]
    constexpr const char* PLAYER_BASE_PATTERN = "8B 0D ?? ?? ?? ??";
    constexpr const char* PLAYER_BASE_MASK = "xx??????";
    
    // 地图名称特征码示例
    // 指令：8D 05 ?? ?? ?? ??  ->  lea eax, [mapName]
    constexpr const char* MAP_NAME_PATTERN = "8D 05 ?? ?? ?? ??";
    constexpr const char* MAP_NAME_MASK = "xx??????";
    
    // 战斗状态特征码示例
    constexpr const char* BATTLE_STATUS_PATTERN = "83 3D ?? ?? ?? ?? ??";
    constexpr const char* BATTLE_STATUS_MASK = "xx?????x";
}

} // namespace cga
