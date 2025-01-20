#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

using namespace std;

#define PROC_DIRECTORY "/proc/"

struct ProcessInfo
{
	int PID;
	string fileName;
	double memUsage;
};

bool isNumber(const string& str)
{
	return all_of(str.begin(), str.end(), [](unsigned char c) {
		return isdigit(c);
	});
}

string getExecutableName(string pid)
{
	string command, exe;

	command = "/proc/" + pid + "/comm";
	ifstream buffer(command);

	buffer >> exe;
	buffer.close();

	return exe;
}

double getMemoryUsage(string pid)
{
	int tSize = 0, resident = 0, share = 0;

	string command = "/proc/" + pid + "/statm";
	ifstream buffer(command);

	buffer >> tSize >> resident >> share;
	buffer.close();

	return share * (sysconf(_SC_PAGE_SIZE) / 1024);
}

vector<ProcessInfo> listProcessIDs()
{
	struct dirent* entity = NULL;
	DIR* procDir = NULL;
	vector<ProcessInfo> processList{};

	procDir = opendir(PROC_DIRECTORY);
	if (procDir == NULL)
		throw "Couldn't open the /proc";

	while (entity = readdir(procDir))
	{
		if (entity->d_type == DT_DIR)
		{
			string pid = string(entity->d_name);
			if (isNumber(pid))
			{
				processList.insert(processList.begin(), ProcessInfo{ stoi(entity->d_name), getExecutableName(pid), getMemoryUsage(pid) });
			}
		}
	}

	return processList;
}

int main()
{
	try
	{
		vector<ProcessInfo> processList = listProcessIDs();

		cout << "PID" << "\t" << "Memory" << "\t" << "File" << endl;

		for (ProcessInfo info : processList)
		{
			cout << info.PID << "\t" << info.memUsage << "\t" << info.fileName << endl;
		}
	}
	catch (const char* err)
	{
		cout << err << endl;
	}

	return EXIT_SUCCESS;
}
