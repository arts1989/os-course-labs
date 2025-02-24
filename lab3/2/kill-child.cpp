#include <chrono>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std::chrono_literals;

void SignalHandler(int s)
{
	if (s == SIGTERM)
	{
		std::cout << "Process " << getpid() << " was terminated" << std::endl;
		exit(EXIT_SUCCESS);
	}
}

int main()
{
	signal(SIGTERM, SignalHandler);

	std::string command;
	std::vector<pid_t> spawnedPIDs;

	while (true)
	{
		std::cout << "Enter command (exit, spawn or kill): " << std::endl;
		std::cin >> command;

		if (command == "spawn")
		{
			pid_t pid = fork();
			switch (pid)
			{
			case -1:
				std::cout << "Error create process" << std::endl;
				return EXIT_FAILURE;
			case 0:
				while (true)
				{
					std::cout << "Child process " << getpid() << " is still alive" << std::endl;
					std::this_thread::sleep_for(5000ms);
				}
			default:
				spawnedPIDs.push_back(pid);
			}
		}
		else if (command == "kill")
		{
			if (spawnedPIDs.size() > 0)
			{
				for (pid_t spawnedPID : spawnedPIDs)
				{
					kill(spawnedPID, SIGTERM);
				}
				spawnedPIDs.clear();
			}
			else
			{
				std::cout << "No children to kill" << std::endl;
			}
		}
		else if (command == "exit")
		{
			for (pid_t spawnedPID : spawnedPIDs)
			{
				kill(spawnedPID, SIGTERM);
			}
			return EXIT_SUCCESS;
		}
	}
}
