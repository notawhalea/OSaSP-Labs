#include <Windows.h>
#include <malloc.h>
#include <iostream>
#include <string>
#include <atlstr.h>
#include <fstream>

#define SIZE_STR 260

using namespace std;
/*MAIN DOCS
    HKEY hKey = NULL - HERE WE OPEN KEY FOR ACCESS TO REESTR
    HKEY temp = NULL - CREATION OF INSERTED KEY
    LSTATUS status = RegDeleteKey(hKey, L"NewKey") - DELETE KEY
    IF THE SECTION CONTAINS ONLY VALUES THEN EVERYTHING WILL BE DELETED
    AND IF THERE ARE A SUBSECTIONS THEN NO
*/
int main()
{
    std::cout << "Program has started\n" << std::endl;
    HKEY hKey = NULL;
    if (RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\RegisteredApplications", &hKey) != ERROR_SUCCESS)
        return 1;

    std::cout << "Access key was created successfully\n" << std::endl;
    HKEY temp = NULL;
    LSTATUS stat = RegCreateKey(hKey, L"NewKey", &temp);  
    if (stat == ERROR_SUCCESS)
    {
        std::cout << "Default key was created successfully\n" << std::endl;

        LSTATUS status = RegDeleteKey(hKey, L"NewKey");
        RegCloseKey(temp);

        std::cout << "Default key was deleted successfully\n" << std::endl;
    }
/* Creating a inserted key with specific parameters DOCS
    CREATE A SUBKEY AND DEFAULT VALUES, REG_SZ - STRING
    LSTATUS status0 = RegSetValue(hKey, L"Test", REG_SZ, L"Message", 7 * sizeof(WCHAR))
    CREATION VALUE FOR SELECTED KEY
    BYTE* data = (BYTE*)calloc(4, 1) - DWORD
    CREATE PAIR KEY:VALUE
    LSTATUS status1 = RegSetValueEx(hKey, L"PARAM", NULL, REG_DWORD, data, 4)
*/

    LSTATUS status0 = RegSetValue(hKey, L"Test", REG_SZ, L"Message", 7 * sizeof(WCHAR)); 

    if (status0 == ERROR_SUCCESS)
    {
        std::cout << "Key with params was created successfully\n" << std::endl;

        LSTATUS status = RegDeleteKey(hKey, L"Test");

        std::cout << "Key with params was deleted successfully\n" << std::endl;
    }

    BYTE* data = (BYTE*)calloc(4, 1); 
    data[1] = 1;

    LSTATUS status1 = RegSetValueEx(hKey, L"PARAM", NULL, REG_DWORD, data, 4);
    std::cout << "Value for access Key was created successfully\n" << std::endl;
    LSTATUS status2 = RegDeleteValue(hKey, L"PARAM");
    std::cout << "Value for access Key was deleted successfully\n" << std::endl;

    DWORD iIndex = 0;
    LPWSTR wstr = (LPWSTR)calloc(SIZE_STR, sizeof(WCHAR));

    std::cout << "List of keys inside the access key:\n" << std::endl;

    RegSetValue(hKey, L"Hello", REG_SZ, L"Hello", 5 * sizeof(WCHAR));
    RegSetValue(hKey, L"Mir", REG_SZ, L"Mir", 3 * sizeof(WCHAR));
 
    
    while (RegEnumKey(hKey, iIndex, wstr, SIZE_STR) == ERROR_SUCCESS)
    {
        std::cout << CW2A(wstr) << std::endl;
        ++iIndex;
    }

    iIndex = 0;
    DWORD value = SIZE_STR;
    while (RegEnumValueW(hKey, iIndex, wstr, &value, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
    {
        std::cout << CW2A(wstr) << std::endl;
        ++iIndex;
    }

    std::cout << "\nTrying to search specific key (World):\n" << std::endl;

    byte flag = 0;
    iIndex = 0;
    while (RegEnumKey(hKey, iIndex, wstr, SIZE_STR) == ERROR_SUCCESS)
    {
        if (strcmp(CW2A(wstr), "World") == 0)
        {
            std::cout << "Key named <World> was found!" << std::endl;
            flag = 1;
        }
        ++iIndex;
    }
     if(flag == 0)
         std::cout << "Key named <World> was not found!" << std::endl;

     /*SAVING KEYS IN FILE*/

     string path = "keysStorage.txt";
     ofstream fout;
     fout.open(path);

     if (!fout.is_open())
     {
         std::cout << "Error trying to open file!" << std::endl;
     }
     else
     {
         iIndex = 0;
         while (RegEnumKey(hKey, iIndex, wstr, SIZE_STR) == ERROR_SUCCESS)
         {
             fout << CW2A(wstr);
             fout << "\n";
             ++iIndex;
         }
     }

     fout.close();

    RegDeleteKey(hKey, L"Hello");
    RegDeleteKey(hKey, L"Mir");
    
    RegCloseKey(hKey);
    return 0;
}