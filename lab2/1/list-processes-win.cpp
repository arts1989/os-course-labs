#include <algorithm>
#include <iostream>
#include <psapi.h>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

struct ProcessInfo
{
	DWORD PID;
	string fileName;
	SIZE_T memUsage;
};

BOOL EnumProcesses(
	DWORD* lpidProcess,
	DWORD cb,
	LPDWORD lpcbNeeded);

BOOL GetProcessMemoryInfo(
	HANDLE Process,
	PPROCESS_MEMORY_COUNTERS ppsmemCounters,
	DWORD cb);

vector<ProcessInfo> listProcessIDs()
{
	vector<ProcessInfo> processInfo;

	DWORD processIDs[1024], bytes;

	BOOL processList = EnumProcesses(processIDs, sizeof(processIDs), &bytes);
	if (!processList)
		throw "EnumProcesses fails";

	for (DWORD i = 0; i < bytes / sizeof(DWORD); i++)
	{
		HANDLE hProcess = OpenProcess(
			PROCESS_ALL_ACCESS,
			FALSE,
			processIDs[i]
		);

		if (hProcess)
		{
			PROCESS_MEMORY_COUNTERS pmc;
			GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));

			TCHAR Buffer[MAX_PATH];
			GetModuleFileNameEx(hProcess, 0, Buffer, MAX_PATH);

			processInfo.insert(processInfo.begin(), ProcessInfo{ processIDs[i], string(Buffer), pmc.WorkingSetSize });
		}

		CloseHandle(hProcess);
	}

	return processInfo;
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
