#include <VersionHelpers.h>
#include <cstdlib>
#include <iostream>
#include <windows.h>

int main()
{
	std::string osVersion;
	if (IsWindows10OrGreater())
	{
		osVersion = "Windows 10 or Greater";
	}
	else if (IsWindows8OrGreater())
	{
		osVersion = "Windows 8 or Greater";
	}
	else if (IsWindowsVistaOrGreater())
	{
		osVersion = "Windows 7 or Greater";
	}
	else if (IsWindowsXPOrGreater())
	{
		osVersion = "Windows XP or Greater";
	}

	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	int availRAM = statex.ullAvailPhys / (1024 * 1024);
	int totalRAM = statex.ullTotalPhys / (1024 * 1024);
	int countCPU = sysinfo.dwNumberOfProcessors;

	std::cout << "OS: " << osVersion << std::endl;
	std::cout << "RAM: " << availRAM << "MB / " << totalRAM << "MB" << std::endl;
	std::cout << "Processors: " << countCPU << std::endl;

	return 0;
}