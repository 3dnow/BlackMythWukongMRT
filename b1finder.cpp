#include <iostream>
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include <iomanip>
#include <unordered_set>
#include <string>
#include <sstream>
#include <shellapi.h>

#ifndef PSAPI_VERSION
#define PSAPI_VERSION 1
#endif
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Shell32.lib")

// 函数声明
DWORD GetProcessIdByName(const wchar_t* processName);
std::vector<MEMORY_BASIC_INFORMATION> GetMemoryRegions(HANDLE hProcess);
std::vector<uintptr_t> ScanMemoryForValue(HANDLE hProcess, const std::vector<MEMORY_BASIC_INFORMATION>& regions, uint64_t value);
void DisplayProgressBar(float progress, int barWidth = 50);
std::wstring GetModuleNameForAddress(HANDLE hProcess, uintptr_t address);
void ModifyMemory(HANDLE hProcess);



int main() {


    printf("BlackMythWuKong Memory Researching Tool v0.1\nby MJ0011\nDefault: Input first value and search for next value\n  -m: modify values\n");
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argv == NULL) {
        std::wcout << L"Failed to parse command line" << std::endl;
        return 1;
    }

    if (argc > 1 && wcscmp(argv[1], L"-m") == 0) {
        // 修改内存模式
        DWORD processId = GetProcessIdByName(L"b1-Win64-Shipping.exe");
        if (processId == 0) {
            std::wcout << L"b1-Win64-Shipping.exe not found. Please start b1-Win64-Shipping and try again." << std::endl;
            LocalFree(argv);
            return 1;
        }

        HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, processId);
        if (hProcess == NULL) {
            std::wcout << L"Failed to open process." << std::endl;
            LocalFree(argv);
            return 1;
        }

        ModifyMemory(hProcess);
        CloseHandle(hProcess);
        LocalFree(argv);
        return 0;
    }

    // 等待输入第一个数值
    std::cout << "Enter the first value to scan for: ";
    uint64_t firstValue;
    std::cin >> firstValue;

    // 查找b1-Win64-Shipping.exe进程
    DWORD processId = GetProcessIdByName(L"b1-Win64-Shipping.exe");
    if (processId == 0) {
        std::cout << "b1-Win64-Shipping.exe not found." << std::endl;
        return 1;
    }

    // 打开进程
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (hProcess == NULL) {
        std::cout << "Failed to open process." << std::endl;
        return 1;
    }

    // 获取内存区域
    std::vector<MEMORY_BASIC_INFORMATION> regions = GetMemoryRegions(hProcess);

    std::cout << "Scanning memory..." << std::endl;

    // 扫描第一个值
    std::vector<uintptr_t> matchingAddresses = ScanMemoryForValue(hProcess, regions, firstValue);

    std::cout << "Found " << matchingAddresses.size() << " matches for the first value." << std::endl;

    // 等待输入第二个数值
    std::cout << "Enter the second value to scan for: ";
    uint64_t secondValue;
    std::cin >> secondValue;

    // 扫描第二个值
    std::unordered_set<uintptr_t> finalMatches;
    size_t total = matchingAddresses.size();
    for (size_t i = 0; i < total; ++i) {
        uintptr_t addr = matchingAddresses[i];
        uint64_t value;
        if (ReadProcessMemory(hProcess, (LPCVOID)addr, &value, sizeof(value), NULL)) {
            if (value == secondValue || (uint32_t)value == (uint32_t)secondValue) {
                finalMatches.insert(addr);
            }
        }
        if (i % 1000 == 0 || i == total - 1) {
            DisplayProgressBar(static_cast<float>(i) / total);
        }
    }
    std::cout << std::endl;

    // 输出结果
    std::cout << "Final matches:" << std::endl;
    for (uintptr_t addr : finalMatches) {
        std::wstring moduleName = GetModuleNameForAddress(hProcess, addr);
        std::wcout << L"0x" << std::hex << addr << std::dec << L" - Module: " << moduleName << std::endl;
    }

    CloseHandle(hProcess);
    return 0;
}
void ModifyMemory(HANDLE hProcess) {
    std::wstring addressInput;
    uint32_t value;

    while (true) {
        std::wcout << L"Enter memory address (in hex) to modify (or press Enter to exit): ";
        std::getline(std::wcin, addressInput);

        if (addressInput.empty()) {
            break;
        }

        uintptr_t address;
        std::wstringstream ss;
        ss << std::hex << addressInput;
        ss >> address;

        std::wcout << L"Enter new 32-bit value (in decimal): ";
        std::wcin >> value;

        // 清除输入缓冲
        std::wcin.clear();
        std::wcin.ignore(1000, L'\n');

        if (WriteProcessMemory(hProcess, (LPVOID)address, &value, sizeof(value), NULL)) {
           
            std::wcout << L"Memory successfully modified at address 0x" << std::hex << address << std::dec << std::endl;
        }
        else {
           
            printf("error %u\n", GetLastError());
             std::wcout << L"Failed to modify memory at address 0x" << std::hex << address << std::dec << std::endl;
        }
    }
}


DWORD GetProcessIdByName(const wchar_t* processName) {
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(entry);

    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName) == 0) {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return pid;
}

std::vector<MEMORY_BASIC_INFORMATION> GetMemoryRegions(HANDLE hProcess) {
    std::vector<MEMORY_BASIC_INFORMATION> regions;
    MEMORY_BASIC_INFORMATION mbi;
    uintptr_t address = 0;

    while (VirtualQueryEx(hProcess, (LPCVOID)address, &mbi, sizeof(mbi))) {
        if (mbi.State == MEM_COMMIT && (mbi.Protect & PAGE_READWRITE)) {
            regions.push_back(mbi);
        }
        address = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
    }

    return regions;
}

std::vector<uintptr_t> ScanMemoryForValue(HANDLE hProcess, const std::vector<MEMORY_BASIC_INFORMATION>& regions, uint64_t value) {
    std::vector<uintptr_t> matches;
    size_t totalSize = 0;
    for (const auto& region : regions) totalSize += region.RegionSize;
    size_t scannedSize = 0;

    for (const auto& region : regions) {
        std::vector<char> buffer(region.RegionSize);
        SIZE_T bytesRead;

        if (ReadProcessMemory(hProcess, region.BaseAddress, buffer.data(), region.RegionSize, &bytesRead)) {
            for (size_t i = 0; i <= bytesRead - sizeof(uint32_t); i += sizeof(uint32_t)) {
                if (*(uint32_t*)&buffer[i] == (uint32_t)value) {
                    matches.push_back((uintptr_t)region.BaseAddress + i);
                }
                if (i + sizeof(uint64_t) <= bytesRead && *(uint64_t*)&buffer[i] == value) {
                    matches.push_back((uintptr_t)region.BaseAddress + i);
                }
            }
        }

        scannedSize += region.RegionSize;
        DisplayProgressBar(static_cast<float>(scannedSize) / totalSize);
    }
    std::cout << std::endl;

    return matches;
}

void DisplayProgressBar(float progress, int barWidth) {
    std::cout << "\r[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    int percent = static_cast<int>(progress * 100.0);
    std::cout << "] " << std::setw(3) << percent << "%" << std::flush;
}

std::wstring GetModuleNameForAddress(HANDLE hProcess, uintptr_t address) {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    wchar_t szModName[MAX_PATH];

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            MODULEINFO moduleInfo;
            if (GetModuleInformation(hProcess, hMods[i], &moduleInfo, sizeof(moduleInfo))) {
                if (address >= (uintptr_t)moduleInfo.lpBaseOfDll &&
                    address < (uintptr_t)moduleInfo.lpBaseOfDll + moduleInfo.SizeOfImage) {
                    if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(wchar_t))) {
                        return szModName;
                    }
                }
            }
        }
    }

    return L"Unknown";
}
