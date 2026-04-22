#include "memscanner.h"
#include <Psapi.h>
#include <cstring>

#pragma comment(lib, "Psapi.lib")

namespace cga {

// 静态成员初始化
HMODULE MemoryScanner::s_moduleHandle = NULL;
uintptr_t MemoryScanner::s_moduleBase = 0;
size_t MemoryScanner::s_moduleSize = 0;
bool MemoryScanner::s_initialized = false;

bool MemoryScanner::Initialize(HMODULE moduleHandle)
{
    if (s_initialized) {
        return true;
    }
    
    if (moduleHandle == NULL) {
        moduleHandle = GetModuleHandleW(NULL);
    }
    
    if (moduleHandle == NULL) {
        return false;
    }
    
    s_moduleHandle = moduleHandle;
    
    // 获取模块信息
    MODULEINFO modInfo;
    if (GetModuleInformation(GetCurrentProcess(), moduleHandle, &modInfo, sizeof(modInfo))) {
        s_moduleBase = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
        s_moduleSize = modInfo.SizeOfImage;
        s_initialized = true;
        
        return true;
    }
    
    return false;
}

void MemoryScanner::Shutdown()
{
    s_moduleHandle = NULL;
    s_moduleBase = 0;
    s_moduleSize = 0;
    s_initialized = false;
}

uintptr_t MemoryScanner::GetModuleBase()
{
    return s_moduleBase;
}

size_t MemoryScanner::GetModuleSize()
{
    return s_moduleSize;
}

PatternMatch MemoryScanner::ScanPattern(const char* pattern, const char* mask, int offset)
{
    PatternMatch result;
    
    if (!s_initialized) {
        return result;
    }
    
    // 分配缓冲区读取整个模块
    std::vector<char> buffer(s_moduleSize);
    SIZE_T bytesRead = 0;
    
    if (!ReadProcessMemory(GetCurrentProcess(), 
                           reinterpret_cast<LPCVOID>(s_moduleBase),
                           buffer.data(), 
                           s_moduleSize, 
                           &bytesRead)) {
        return result;
    }
    
    // 查找特征码
    uintptr_t relativeAddr = FindPattern(buffer.data(), bytesRead, pattern, mask);
    
    if (relativeAddr != 0) {
        result.address = s_moduleBase + relativeAddr;
        result.offset = offset;
        result.found = true;
    }
    
    return result;
}

std::vector<PatternMatch> MemoryScanner::ScanPatternMulti(const char* pattern, const char* mask, size_t maxResults)
{
    std::vector<PatternMatch> results;
    
    if (!s_initialized) {
        return results;
    }
    
    // 分配缓冲区
    std::vector<char> buffer(s_moduleSize);
    SIZE_T bytesRead = 0;
    
    if (!ReadProcessMemory(GetCurrentProcess(),
                           reinterpret_cast<LPCVOID>(s_moduleBase),
                           buffer.data(),
                           s_moduleSize,
                           &bytesRead)) {
        return results;
    }
    
    // 查找所有匹配
    uintptr_t searchStart = 0;
    while (results.size() < maxResults && searchStart < bytesRead) {
        uintptr_t relativeAddr = FindPattern(buffer.data() + searchStart, 
                                             bytesRead - searchStart, 
                                             pattern, mask);
        
        if (relativeAddr == 0) {
            break;
        }
        
        PatternMatch match;
        match.address = s_moduleBase + searchStart + relativeAddr;
        match.offset = 0;
        match.found = true;
        results.push_back(match);
        
        searchStart += relativeAddr + 1;
    }
    
    return results;
}

uintptr_t MemoryScanner::ReadRelativeOffset(uintptr_t address, int offset)
{
    if (!IsMemoryReadable(address, 6)) {
        return 0;
    }
    
    // 读取相对偏移 (假设是 32 位相对地址)
    int32_t relOffset = *reinterpret_cast<int32_t*>(address + offset);
    
    // 计算实际地址 (RIP + offset + relOffset)
    return address + offset + 4 + relOffset;
}

uintptr_t MemoryScanner::ReadPointerChain(uintptr_t baseAddress, const std::vector<int>& offsets)
{
    uintptr_t current = baseAddress;
    
    for (size_t i = 0; i < offsets.size(); ++i) {
        if (!IsMemoryReadable(current, 4)) {
            return 0;
        }
        
        current = *reinterpret_cast<uintptr_t*>(current);
        
        if (current == 0) {
            return 0;
        }
        
        if (i < offsets.size() - 1) {
            current += offsets[i];
        }
    }
    
    return current;
}

uintptr_t MemoryScanner::FindPattern(const void* buffer, size_t bufferSize, const char* pattern, const char* mask)
{
    if (!buffer || !pattern || !mask) {
        return 0;
    }
    
    size_t patternLen = strlen(mask);
    if (patternLen > bufferSize) {
        return 0;
    }
    
    const unsigned char* data = static_cast<const unsigned char*>(buffer);
    
    for (size_t i = 0; i <= bufferSize - patternLen; ++i) {
        bool found = true;
        
        for (size_t j = 0; j < patternLen; ++j) {
            if (mask[j] == 'x') {
                // 精确匹配
                unsigned char patternByte = 0;
                int nibble = 0;
                
                // 解析十六进制字节
                const char* hexPtr = pattern + (j * 3);
                for (int k = 0; k < 2; ++k) {
                    char c = *hexPtr++;
                    if (c >= '0' && c <= '9') nibble = c - '0';
                    else if (c >= 'A' && c <= 'F') nibble = c - 'A' + 10;
                    else if (c >= 'a' && c <= 'f') nibble = c - 'a' + 10;
                    else break;
                    patternByte = (patternByte << 4) | nibble;
                }
                
                if (data[i + j] != patternByte) {
                    found = false;
                    break;
                }
            }
            // '?' 通配符跳过检查
        }
        
        if (found) {
            return i;
        }
    }
    
    return 0;
}

bool MemoryScanner::IsMemoryReadable(uintptr_t address, size_t size)
{
    if (address == 0) {
        return false;
    }
    
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi)) == 0) {
        return false;
    }
    
    // 检查是否可读
    return (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | 
                           PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) != 0;
}

uintptr_t MemoryScanner::GetExportFunctionAddress(const char* moduleName, const char* functionName)
{
    HMODULE hModule = GetModuleHandleA(moduleName);
    if (!hModule) {
        return 0;
    }
    
    FARPROC proc = GetProcAddress(hModule, functionName);
    return reinterpret_cast<uintptr_t>(proc);
}

// ============================================================================
// AddressResolver 实现
// ============================================================================

uintptr_t AddressResolver::ResolvePointer(uintptr_t moduleBase, const char* pattern, const char* mask,
                                          int offset, bool dereference)
{
    PatternMatch match = MemoryScanner::ScanPattern(pattern, mask, offset);
    
    if (!match.found) {
        return 0;
    }
    
    uintptr_t address = match.address + match.offset;
    
    if (!dereference) {
        return address;
    }
    
    // 解引用指针
    if (MemoryScanner::IsMemoryReadable(address, 4)) {
        return *reinterpret_cast<uintptr_t*>(address);
    }
    
    return 0;
}

uintptr_t AddressResolver::ResolveCall(uintptr_t moduleBase, const char* pattern, const char* mask)
{
    PatternMatch match = MemoryScanner::ScanPattern(pattern, mask, 0);
    
    if (!match.found) {
        return 0;
    }
    
    // CALL 指令格式：E8 ?? ?? ?? ?? (相对地址)
    // 目标地址 = 下一条指令地址 + 相对偏移
    uintptr_t callAddr = match.address;
    int32_t relOffset = *reinterpret_cast<int32_t*>(callAddr + 1);
    
    return callAddr + 5 + relOffset;
}

uintptr_t AddressResolver::ResolveDataRef(uintptr_t moduleBase, const char* pattern, const char* mask)
{
    PatternMatch match = MemoryScanner::ScanPattern(pattern, mask, 0);
    
    if (!match.found) {
        return 0;
    }
    
    // MOV 指令格式：8B 05 ?? ?? ?? ?? (相对地址)
    // 目标地址 = 下一条指令地址 + 相对偏移
    uintptr_t movAddr = match.address;
    int32_t relOffset = *reinterpret_cast<int32_t*>(movAddr + 2);
    
    return movAddr + 6 + relOffset;
}

} // namespace cga
