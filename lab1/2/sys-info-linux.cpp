#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

int main()
{

	utsname name; // стракт необязательно ++
	if (uname(&name))
		return 1; // не грейсфул шутдаун, лучше return  ++

	printf("OS: %s %s\n", name.sysname, name.release);

	struct sysinfo memoryInfo;
	auto err = sysinfo(&memoryInfo); // проверять ошибки  ++
	if (err != 0) {
		return 1;
	}

	auto freeMemory = memoryInfo.freeram / (1024 * 1024);
	auto totalMemory = memoryInfo.totalram / (1024 * 1024);

	printf("RAM: %luMB / %luMB\n", freeMemory, totalMemory);

	int processorsCount = get_nprocs_conf();

	printf("Processors: %d\n", processorsCount);

	return 0;
}
