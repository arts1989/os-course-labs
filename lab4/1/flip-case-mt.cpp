#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <thread>
#include <vector>

char changeCase(char c)
{
	return std::isupper(c) ? std::tolower(c) : std::toupper(c);
}

int main(int argc, char* argv[])
{
	if (argv[1] == NULL || argv[2] == NULL)
	{
		std::cout << "enter names of 2 existing files" << std::endl;
		exit(EXIT_FAILURE);
	}

	auto threadHandler = [](std::string fileName) {
		std::ifstream ifs(fileName);
		std::ofstream os(fileName + ".out");

		std::string str(std::istreambuf_iterator<char>{ ifs }, {});
		std::transform(str.begin(), str.end(), str.begin(), changeCase);

		os << str;
	};

	std::vector<std::jthread> threads;
	threads.push_back(std::jthread{ threadHandler, argv[1] });
	threads.push_back(std::jthread{ threadHandler, argv[2] });
}