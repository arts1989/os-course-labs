#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdatomic.h>
#include <thread>

#if defined(PLATFORM_POSIX) || defined(__linux__)
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#elif defined(_WIN32)
#include <VersionHelpers.h>
#include <windows.h>
#else
#error unrecognized platform
#endif

using namespace std::chrono_literals;

#define FILE_NAME "temp.txt"

std::atomic_bool flag = false;

void SignalHandler(int s)
{
	if (s == SIGINT || s == SIGTERM)
	{
		std::cout << "Process " << getpid() << " has received signal #" << s << "\n";
		flag.store(true);
	}
}

int main()
{
	signal(SIGINT, SignalHandler);
	signal(SIGTERM, SignalHandler);

	std::ofstream(FILE_NAME);
	std::cout << "file " << FILE_NAME << " created" << std::endl;

	int counter = 0;
	while (counter < 100)
	{
		if (flag.load())
		{
			std::filesystem::remove(FILE_NAME);
			std::cout << "file " << FILE_NAME << " removed" << std::endl;
			std::cout << "Done" << std::endl;
			return 1;
		}
		std::cout << counter << std::endl;
		counter++;
		std::this_thread::sleep_for(1000ms);
	}

	std::filesystem::remove(FILE_NAME);
	std::cout << "file " << FILE_NAME << " removed" << std::endl;
	std::cout << "Done" << std::endl;

	return 0;
}
