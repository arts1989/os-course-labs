#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <filesystem>
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

struct ProcessInfo {
  int PID;
  string fileName;
  double memUsage;
};

bool isNumber(const string &str) {
  return all_of(str.begin(), str.end(),
                [](unsigned char c) { return isdigit(c); });
}

string getExecutableName(string pid)
{
  string command, executableName;

  command =
      "/proc/" + pid + "/comm"; // поправить нейминги exe => executableName ++
  ifstream buffer(command); // проверить результат чтения из потока
  if (!buffer) {
    throw "Error reading buffer";
  }

  buffer >> executableName; // проверить если имя процесса содержит пробел
  // buffer.close(); // необязательно, RAII почистит

  return executableName;
}

double getMemoryUsage(string pid) {
  int tSize = 0, resident = 0, share = 0;

  string command = "/proc/" + pid + "/statm";
  ifstream buffer(command);
  if (!buffer) {
    throw "Error reading buffer";
  }

  buffer >> tSize >> resident >> share;
  // buffer.close(); // необязательно, RAII почистит

  return share * (sysconf(_SC_PAGE_SIZE) / 1024);
}

vector<ProcessInfo> listProcessIDs() {
	struct dirent *entity = NULL;
	DIR *procDir = NULL;
	vector<ProcessInfo> processList{};

	std::string path = "/proc/";
	for (const auto &entry : filesystem::directory_iterator(path)) {
		if (filesystem::is_directory(entry) && isNumber(entry.path().filename())) {
			string pid = entry.path().filename();
			processList.insert(
				processList.begin(),
				ProcessInfo{
					stoi(pid),
					getExecutableName(pid),
					getMemoryUsage(pid)
				}
			);
		}
	}

  //	 переписал opendir на std::filesystem
  //	while (entity = readdir(procDir))
  //	{
  //		if (entity->d_type == DT_DIR)
  //		{
  //			string pid = entity->d_name;
  //
  //		}
  //	}
  //	 procDir = opendir(PROC_DIRECTORY);
  //	 if (procDir == NULL)
  //		throw "Couldn't open the /proc";
  //       closedir(procDir);  // закрыть иначе течет ++

  return processList;
}

int main() {
	try
	{
		vector<ProcessInfo> processList = listProcessIDs();

		cout << "PID" << "\t" << "Memory" << "\t" << "File" << endl;

		for (ProcessInfo info : processList) {
		  cout << info.PID << "\t" << info.memUsage << "\t" << info.fileName
			   << endl;
		}
	}
	catch (...)
	{
		cout << "error happened" << endl;
	}

  	return EXIT_SUCCESS;
}
