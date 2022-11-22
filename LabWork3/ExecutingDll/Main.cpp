#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <tlhelp32.h>

#include "..\\SomeDll\\Header.h"
#pragma comment(lib, "..\\x64\\Debug\\SomeDll.lib")

using namespace std;

typedef int Func(int, int);

const int n = 5;
/*
	GetProcessIdByProcessName DOCS
	DWORD processId = 0 - HERE WE MAKE VARIABLE FOR PROCESS (int32)
	HANDLE hSnapshot - HERE WE TAKE PHOTO OF CURRENT WORKING PROCESS OF FILE SYSTEM
	PROCESSENTRY32 processEntry - STRUCTURE OF SINGLE PROCESS
	ZeroMemory - MAKE IT EQUAL ZERO
	processEntry.dwSize = sizeof(processEntry) - MAKE IT NEW SIZE 
	while (Process32Next(hSnapshot, &processEntry) && !isFound) - IN CYCLE WE ARE LOOKING FOR ENTERING PROCESS
*/

DWORD GetProcessIdByProcessName(string processName)
{
	DWORD processId = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry; 

	ZeroMemory(&processEntry, sizeof(processEntry)); 
	processEntry.dwSize = sizeof(processEntry);

	bool isFound = false;
	while (Process32Next(hSnapshot, &processEntry) && !isFound)
	{
		if (!processName.compare(processEntry.szExeFile)) 
		{
			processId = processEntry.th32ProcessID; 
			isFound = true; 
		}
	}

	return processId; 
}

void main()
{
/*
	main DOCS
	HMODULE dll = 0 -  An HMODULE is just the DLL's base address
	FOR DYNAMIC IMPORT
	LoadLibrary("SomeDll"))) - LOOKING FOR DLL AND DOWNLOAD IT
	Func* _Sum, * _Sub - DELEGATES
	_Sum = (Func*)GetProcAddress(dll, MAKEINTRESOURCE(4)) - WRITE DOWN LINK ON METHODS FROM DYMANIC DLL
	FreeLibrary(dll) - DELETE DATA OF DYNAMIC DLL
*/
	cout << "BEFORE REPLACING\n" << endl;
	string strings[n] = {
	"Vse obernulis no pozdno uzhe u menya drugie dela",
	"Vse dumayt mne povezlo chto proverim? DAVAITE. Nazhmite",
	"Pause?",
	"I YA OKAZHUS V TO GE VREMYA S TAKIM GE USPEHOM NA TEH GE MESTAH"
	};

	for (int i = 0; i < n; ++i)
		cout << strings[i].c_str() << endl;

	cout << "AFTER REPLACING\n" << endl;

	Replace(strings[2].c_str(), "RESTART");

	for (int i = 0; i < n; ++i)
		cout << strings[i].c_str() << endl;

	cout << "STATIC IMPORT DLL: " << endl << "6 + 6 = " << Sum(6, 6) << endl << "12 % 11 = " << Sub(12, 11) << endl;

	HMODULE dll = 0;
	if ((dll = LoadLibrary("SomeDll")))
	{ 
		Func* _Sum, * _Sub;
		_Sum = (Func*)GetProcAddress(dll, MAKEINTRESOURCE(4)); 
		_Sub = (Func*)GetProcAddress(dll, MAKEINTRESOURCE(3));
		cout << "\nDYNAMIC IMPORT DLL: " << endl << "7 + 7 = " << _Sum(7, 7) << endl << "26 % 12 = " << _Sub(26, 12) << endl;
		FreeLibrary(dll);
	}
	else
		printf("Cant load Dll");


	cout << "\nENTER PROCESS NAME(LAST TASK): " << endl;
	std::string AppName;
	std::cin >> AppName;
	DWORD pid = GetProcessIdByProcessName(AppName + ".exe");
	cout << "PID: ";
	cout << pid << endl;
/*
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid) - WHERE PROCESS_ALL_ACCESS - WE HAVE ACCESS TO PROCESS
	FALSE - INHERIT CURRENT PROCESS; pid - PROCESS ID
	hRemoteProcess != NULL - IF WE HAVE PERMISSIONS
	LPVOID threadFunction = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA") 
	- TAKE FUNCTION ADDRESS ON DYNAMIC LOADING FROM KERNEL32
	LPVOID argumentAddress = VirtualAllocEx(hRemoteProcess, NULL, argument.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)
	- GIVE PLACE FOR LIBRARY IN FOUNDED PROCESS
	WriteProcessMemory(hRemoteProcess, (LPVOID)argumentAddress, argument.c_str(), argument.length() + 1, NULL)
	- INSERT IN FOUNDED PROCESS OUR LIBRARY	
	if (CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)threadFunction, (LPVOID)argumentAddress, 0, NULL))
	- BUILT NEW THREAD FOR LIBRARY
*/

	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (hRemoteProcess != NULL) 
	{
		cout << "\nPID finded" << endl;

		LPVOID threadFunction = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
		string argument("InjectionDll.dll");

		LPVOID argumentAddress = VirtualAllocEx(hRemoteProcess, NULL, argument.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		WriteProcessMemory(hRemoteProcess, (LPVOID)argumentAddress, argument.c_str(), argument.length() + 1, NULL);

		if (CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)threadFunction, (LPVOID)argumentAddress, 0, NULL))
		{
			Sleep(1000);
			cout << "Creating thread" << endl;
			CloseHandle(hRemoteProcess);
		}
		else
			cout << "Cant create thread" << endl;
	}
	else
		cout << "Cant find PID" << endl;

	cout << "\nPress any button to exit." << endl;
	_getch();
}