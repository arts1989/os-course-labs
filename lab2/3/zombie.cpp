#include <iostream>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int status, enteredPID;
	pid_t pid;

	pid = fork();
	if (pid == 0)
	{
		std::cout << "Child PID: " << getpid() << std::endl;
		exit(EXIT_SUCCESS);
	}
	else
	{
		std::cout << "Enter child PID: " << std::endl;
		std::cin >> enteredPID;

		while (!std::cin.good() || enteredPID != pid)
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "[error] invalid input, enter child PID:  " << std::endl;
			std::cin >> enteredPID;
		}

		if (WEXITSTATUS(status))
		{
			std::cout << "Child PID " << pid << " exited normally " << std::endl;
		}

		exit(EXIT_SUCCESS);
	}
}