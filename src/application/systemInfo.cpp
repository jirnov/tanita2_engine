/*
 Движок "Танита2".
 Вывод в лог информации о системе.
 */
#include <precompiled.h>
#include "systemInfo.h"

namespace
{

// Определение версии OS.
// Код взят из MSDN.
std::wstring getOsVersion()
{
    typedef void (WINAPI *PGNSI)( LPSYSTEM_INFO );
    typedef BOOL (WINAPI *PGPI)( DWORD, DWORD, DWORD, DWORD, PDWORD );
    
    OSVERSIONINFOEX osvi;
    SYSTEM_INFO si;
    PGNSI pGNSI;
    PGPI pGPI;
    DWORD dwType;
    std::wostringstream versionString;
    
    ZeroMemory(&si, sizeof(SYSTEM_INFO));
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    check_win32(GetVersionEx((OSVERSIONINFO *) &osvi));
    
    // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
    pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
                                  "GetNativeSystemInfo");
    if (NULL != pGNSI)
        pGNSI(&si);
    else
        GetSystemInfo(&si);
    
    if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4)
    {
        versionString << "Microsoft ";
        
        // Test for the specific product.
        if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
        {
            if (osvi.wProductType == VER_NT_WORKSTATION)
                versionString << L"Windows Vista ";
            else
                versionString << L"Windows Server 2008 ";
            pGPI = (PGPI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),
                                        "GetProductInfo");
            pGPI(6, 0, 0, 0, &dwType);
            
            switch (dwType)
            {
            case PRODUCT_ULTIMATE:
                versionString << L"Ultimate Edition";
                break;
            case PRODUCT_HOME_PREMIUM:
                versionString << L"Home Premium Edition";
                break;
            case PRODUCT_HOME_BASIC:
                versionString << L"Home Basic Edition";
                break;
            case PRODUCT_ENTERPRISE:
                versionString << L"Enterprise Edition";
                break;
            case PRODUCT_BUSINESS:
                versionString << L"Business Edition";
                break;
            case PRODUCT_STARTER:
                versionString << L"Starter Edition";
                break;
            case PRODUCT_CLUSTER_SERVER:
                versionString << L"Cluster Server Edition";
                break;
            case PRODUCT_DATACENTER_SERVER:
                versionString << L"Datacenter Edition";
                break;
            case PRODUCT_DATACENTER_SERVER_CORE:
                versionString << L"Datacenter Edition (core installation)";
                break;
            case PRODUCT_ENTERPRISE_SERVER:
                versionString << L"Enterprise Edition";
                break;
            case PRODUCT_ENTERPRISE_SERVER_CORE:
                versionString << L"Enterprise Edition (core installation)";
                break;
            case PRODUCT_ENTERPRISE_SERVER_IA64:
                versionString << L"Enterprise Edition for Itanium-based Systems";
                break;
            case PRODUCT_SMALLBUSINESS_SERVER:
                versionString << L"Small Business Server";
                break;
            case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
                versionString << L"Small Business Server Premium Edition";
                break;
            case PRODUCT_STANDARD_SERVER:
                versionString << L"Standard Edition";
                break;
            case PRODUCT_STANDARD_SERVER_CORE:
                versionString << L"Standard Edition (core installation)";
                break;
            case PRODUCT_WEB_SERVER:
                versionString << L"Web Server Edition";
                break;
            }
            if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                versionString << L", 64-bit";
            else
                if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
                    versionString << L", 32-bit";
        }
        if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
        {
            if (GetSystemMetrics(SM_SERVERR2))
                versionString << L"Windows Server 2003 R2, ";
            else if (osvi.wSuiteMask == VER_SUITE_STORAGE_SERVER)
                versionString << L"Windows Storage Server 2003";
            else if(osvi.wProductType == VER_NT_WORKSTATION &&
                    si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
            {
                versionString << L"Windows XP Professional x64 Edition";
            }
            else
                versionString << L"Windows Server 2003, ";
            
            // Test for the server type.
            if (osvi.wProductType != VER_NT_WORKSTATION)
            {
                if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
                {
                    if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
                        versionString << L"Datacenter Edition for Itanium-based Systems";
                    else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
                        versionString << L"Enterprise Edition for Itanium-based Systems";
                }
                else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                {
                    if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
                        versionString << L"Datacenter x64 Edition";
                    else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
                        versionString << L"Enterprise x64 Edition";
                    else
                        versionString << L"Standard x64 Edition";
                }
                else
                {
                    if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
                        versionString << L"Compute Cluster Edition";
                    else if(osvi.wSuiteMask & VER_SUITE_DATACENTER)
                        versionString << L"Datacenter Edition";
                    else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
                        versionString << L"Enterprise Edition";
                    else if (osvi.wSuiteMask & VER_SUITE_BLADE )
                        versionString << L"Web Edition";
                    else
                        versionString << L"Standard Edition";
                }
            }
        }
        if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
        {
            versionString << L"Windows XP ";
            if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
                versionString << L"Home Edition";
            else
                versionString << L"Professional";
        }
        if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
        {
            versionString << L"Windows 2000 ";
            if (osvi.wProductType == VER_NT_WORKSTATION)
                versionString << L"Professional";
            else
            {
                if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
                    versionString << L"Datacenter Server";
                else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
                    versionString << L"Advanced Server";
                else
                    versionString << L"Server";
            }
        }
        // Display service pack (if any) and build number.
        versionString << " " << osvi.szCSDVersion << L" (Build "
                      << (osvi.dwBuildNumber & 0xFFFF) << L')';
    }
    else
    {
        return L"Unknown platform";
    }
    return versionString.str();
}

// Определение объема оперативной памяти.
std::wstring getMemoryInfo()
{
    MEMORYSTATUS memoryStatus;
    ZeroMemory(&memoryStatus,sizeof(MEMORYSTATUS));
    memoryStatus.dwLength = sizeof (MEMORYSTATUS);
    ::GlobalMemoryStatus(&memoryStatus);
    
    std::wostringstream memInfo;
    memInfo << (memoryStatus.dwTotalPhys / 1024 / 1024) << L"Mb installed, "
            << (memoryStatus.dwAvailPhys / 1024 / 1024) << L"Mb free.";
    return memInfo.str();
}

// Определение типа процессора.
std::wstring getCpuInfo()
{
    HKEY key;
    if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                      L"Hardware\\Description\\System\\CentralProcessor\\0",
                                      0, KEY_READ, &key))
        return L"Unknown.";
    
    const ULONG BUFFER_SIZE = 100;
    ULONG size = BUFFER_SIZE;
    wchar_t cpuName[BUFFER_SIZE] = L"Unknown";
    RegQueryValueEx(key, L"ProcessorNameString", NULL, NULL,
                    reinterpret_cast<LPBYTE>(cpuName), &size);
    RegCloseKey(key);
    
    std::wostringstream info;
    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo); 
    info << siSysInfo.dwNumberOfProcessors << L" x " << cpuName;
    return info.str();
}

} // namespace

// Вывод информации о компьютере в лог.
void application::printSystemInformation()
{
    logger()->info() << "OS: "  << getOsVersion();
    logger()->info() << "RAM: " << getMemoryInfo();
    logger()->info() << "CPU: " << getCpuInfo();
}
