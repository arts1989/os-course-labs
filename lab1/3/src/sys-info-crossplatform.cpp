#include <cstdint>
#include <iostream>
#include <stdlib.h>

#if defined(PLATFORM_POSIX) || defined(__linux__)
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#elif defined(_WIN32)
#include <VersionHelpers.h>
#include <windows.h>
#else
static_assert(false, "unrecognized platform");
#endif

class SysInfo
{
public:
	std::string GetOSName() const;
	std::string GetOSVersion() const;
	uint64_t GetFreeMemory() const;
	uint64_t GetTotalMemory() const;
	unsigned GetProcessorCount() const;
};

std::string SysInfo::GetOSName() const
{
	std::string OSname;
#if defined(PLATFORM_POSIX) || defined(__linux__)
	struct utsname name;
	if (uname(&name))
		exit(-1);

	OSname = name.sysname;
#elif defined(_WIN32)
	OSname = "Windows";
#endif
	return OSname;
}

std::string SysInfo::GetOSVersion() const
{
	std::string OSVersion;
#if defined(PLATFORM_POSIX) || defined(__linux__)
	struct utsname name;
	if (uname(&name))
		exit(-1);

	OSVersion = name.release;
#elif defined(_WIN32)
	if (IsWindows10OrGreater())
	{
		OSVersion = "10 or Greater";
	}
	else if (IsWindows8OrGreater())
	{
		OSVersion = "8 or Greater";
	}
	else if (IsWindowsVistaOrGreater())
	{
		OSVersion = "7 or Greater";
	}
	else if (IsWindowsXPOrGreater())
	{
		OSVersion = "XP or Greater";
	}
#endif
	return OSVersion;
}

uint64_t SysInfo::GetFreeMemory() const
{
#if defined(PLATFORM_POSIX) || defined(__linux__)
	struct sysinfo memory_info;
	sysinfo(&memory_info);

	return memory_info.freeram / (1024 * 1024);
#elif defined(_WIN32)
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	return statex.ullAvailPhys / (1024 * 1024);
#endif
}

uint64_t SysInfo::GetTotalMemory() const
{
#if defined(PLATFORM_POSIX) || defined(__linux__)
	struct sysinfo memory_info;
	sysinfo(&memory_info);

	return memory_info.totalram / (1024 * 1024);
#elif defined(_WIN32)
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	return statex.ullTotalPhys / (1024 * 1024);
#endif
}

unsigned SysInfo::GetProcessorCount() const
{
#if defined(PLATFORM_POSIX) || defined(__linux__)
	return get_nprocs_conf();
#elif defined(_WIN32)
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	return sysinfo.dwNumberOfProcessors;
#endif
}

int main()
{
	SysInfo info;

	std::cout << "OS: " << info.GetOSName() << " " << info.GetOSVersion() << std::endl;
	std::cout << "RAM: " << info.GetFreeMemory() << "MB / " << info.GetTotalMemory() << "MB" << std::endl;
	std::cout << "Processors: " << info.GetProcessorCount() << std::endl;

	return 0;
}
