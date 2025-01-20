#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

int main()
{
	struct utsname name;
	if (uname(&name))
		exit(-1);

	printf("OS: %s %s\n", name.sysname, name.release);

	struct sysinfo memoryInfo;
	sysinfo(&memoryInfo);

	auto freeMemory = memoryInfo.freeram / (1024 * 1024);
	auto totalMemory = memoryInfo.totalram / (1024 * 1024);

	printf("RAM: %luMB / %luMB\n", freeMemory, totalMemory);

	int processorsCount = get_nprocs_conf();

	printf("Processors: %d\n", processorsCount);

	return 0;
}
