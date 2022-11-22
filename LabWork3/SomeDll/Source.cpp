#include "Header.h"
#include <iostream>
#include <algorithm>
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <fstream>

using namespace std;

int Sum(int a, int b)
{
	return a + b;
}

int Sub(int a, int b)
{
	return a % b;
}

void Hacked()
{
	cout << "\nDLL IS HERE!!!!!!!!!!-_-" << endl;
}
/*
	REPLACE DOCS
	HANDLE process = GetCurrentProcess() - DESCRIPTOR OF CURRENT PROCESS
	size_t len = strlen(data) - LENGHT OF REPLACING STRING
	size_t replacementLength = strlen(replacement) - STRING LENGHT FOR REPLACING
	SYSTEM_INFO si -
	GetSystemInfo(&si) - Get all information about current system
	MEMORY_BASIC_INFORMATION info -
	std::vector<char> chunk - MAKE BUFFER
	while (p < si.lpMaximumApplicationAddress) - GOING THROW ALL MEMORY
		//                   proc  addr  memory        size
		(VirtualQueryEx(process, p, &info, sizeof(info)) == sizeof(info) ) // Retrieves information about a range of pages within the virtual address space of a specified process
			if (info.State == MEM_COMMIT && info.AllocationProtect == PAGE_READWRITE) // allocate memory for read and write as well as to change it
				p = (char*)info.BaseAddress; // set up borders to change
				chunk.resize(info.RegionSize);
				SIZE_T bytesRead;
					try
						// trying to change
						if (ReadProcessMemory(process, p, &chunk[0], info.RegionSize, &bytesRead)) // read as a blocks
							for (size_t i = 0; i < (bytesRead - len); ++i)
								if (memcmp(data, &chunk[i], len) == 0) // comapre the blocks and make sure that everything done correctly
									char* ref = (char*)p + i; // replace here
									for (int j = 0; j < replacementLength; j++) {
										ref[j] = replacement[j];
									}
									ref[replacementLength] = 0;
					catch (std::bad_alloc& e) // if the error happens with allocate memory (buffer)
*/
void Replace(const char* data, const char* replacement)
{
	HANDLE process = GetCurrentProcess(); 
	size_t len = strlen(data); 
	size_t replacementLength = strlen(replacement);

	if (process) 
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si); 

		MEMORY_BASIC_INFORMATION info;
		std::vector<char> chunk; 
		char* p = 0;
		while (p < si.lpMaximumApplicationAddress)
		{
			if (VirtualQueryEx(process, p, &info, sizeof(info)) == sizeof(info) )
			{
				if (info.State == MEM_COMMIT && info.AllocationProtect == PAGE_READWRITE) 
				{
					p = (char*)info.BaseAddress; 
					chunk.resize(info.RegionSize);
					SIZE_T bytesRead;

					try 
					{ 
						if (ReadProcessMemory(process, p, &chunk[0], info.RegionSize, &bytesRead)) 
						{
							for (size_t i = 0; i < (bytesRead - len); ++i)
							{
								if (memcmp(data, &chunk[i], len) == 0) 
								{
									char* ref = (char*)p + i; 
									for (int j = 0; j < replacementLength; j++) {
										ref[j] = replacement[j];
									}
									ref[replacementLength] = 0;
								}
							}
						}
					}
					catch (std::bad_alloc& e)	
					{ 
					}
				}
				p += info.RegionSize;
			}
		}
	} 
}