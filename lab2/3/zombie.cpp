#include <iostream>
#include <limits>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// выяснить в каком случае процесс зомби ++

int main(int argc, char* argv[])
{
	pid_t pid = fork();

	if (pid == 0)
	{
		// child
		std::cout << "Child PID: " << getpid() << std::endl;
		return EXIT_SUCCESS; // не использовать ext ++
	}
	else
	{
		// parent
		int enteredPID;
		std::cout << "Enter child PID: " << std::endl;
		std::cin >> enteredPID;

		while (!std::cin.good() || enteredPID != pid)
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "[error] invalid input, enter child PID:  " << std::endl;
			std::cin >> enteredPID;
		}

		int state;
		pid_t child_pid = waitpid(pid, &state, 0);

		std::cout << "Child PID " << child_pid << " exited normally " << std::endl;
		std::cout << "Exit status: " << WEXITSTATUS(state) << std::endl;

		return EXIT_SUCCESS;
	}
}