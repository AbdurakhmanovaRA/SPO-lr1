#define WINVER 0x0502
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <locale.h>
#include <iostream>

using namespace std;

#define MAX_KEY_LENGTH 255
#define INFO_BUF_SIZE 32767
#define BUFSIZE MAX_PATH
#define FILESYSNAMEBUFSIZE MAX_PATH
#define TOTALBYTES    8192

//CPU performance measurement
double freq = 0.0;
__int64 tick = 0;

void counter()
{
    LARGE_INTEGER CNT;
    QueryPerformanceFrequency(&CNT);
    freq = double(CNT.QuadPart);
    printf("CPU performance frequency: %u Hz \n", CNT);
    QueryPerformanceCounter(&CNT);
    tick = CNT.QuadPart;
}

double tickCount()
{
    LARGE_INTEGER CNT;
    QueryPerformanceCounter(&CNT);
    double count = double((CNT.QuadPart - tick) * 1000000 / freq) ;
    return count;
}

int main()
{
    setlocale(LC_ALL, "RUS");
    TCHAR  infoBuf[INFO_BUF_SIZE];
    DWORD  bufCharCount = INFO_BUF_SIZE;
    //1)display version
    DWORD dwVersion = GetVersion();
    DWORD dwMajorVer = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    DWORD dwMinorVer = (DWORD)(HIBYTE(LOWORD(dwVersion)));
    DWORD dwBuild = (DWORD)(HIWORD(dwVersion));
    printf("Version is %d.%d (%d)\n", dwMajorVer, dwMinorVer, dwBuild);
    //2)system directory

    GetSystemDirectory(infoBuf, sizeof(infoBuf));
    printf("System directory: %s\n", infoBuf);

    //3)computer name
    bufCharCount = INFO_BUF_SIZE;
    GetComputerName(infoBuf, &bufCharCount);
    printf("Computer name: %s\n", infoBuf);

    //user name
    bufCharCount = INFO_BUF_SIZE;
    GetUserName(infoBuf, &bufCharCount);
    printf("Username: %s\n", infoBuf);

    //4)disks' volume

    char buffer[MAX_PATH];
    DWORD BufLn = MAX_PATH;
    char Names[MAX_PATH + 1];
    ULARGE_INTEGER total, available, free;

    HANDLE search = FindFirstVolume(Names, sizeof(buffer));


    do {
        printf("Volume path: %s\n", Names);
        GetVolumePathNamesForVolumeName(Names, buffer, BufLn, &BufLn);
        char* path = buffer;
        printf("Mount point: %s\n", path);
        
        if (GetDiskFreeSpaceEx(buffer, &available, &total, &free) != 0)
        {
            printf("Total size: %u  bytes\n", total.QuadPart);
            printf("Free space: %u  bytes\n", free.QuadPart);
        }
        else
        {
            printf("Information unavailable\n");
        }

    } while (FindNextVolume(search, Names, sizeof(buffer)));
    if (GetLastError() != ERROR_NO_MORE_FILES) {
        int code = GetLastError();
        printf("Function  failed! Error %d\n", code);
    }
    FindVolumeClose(search);

    //5)Startup programs

    HKEY hKey;
    TCHAR ValueName[MAX_KEY_LENGTH];
    DWORD chValueName;
    DWORD retCode;

    DWORD BufferSize = TOTALBYTES;
    PPERF_DATA_BLOCK PerfData = (PPERF_DATA_BLOCK)malloc(BufferSize);
    DWORD cbData = BufferSize;
    int i = 0;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        printf("\nStartup programs:\n");
    }
    else printf("\nFunction RegOpenKeyEx() failed!\n");

    while (TRUE) {
        chValueName = sizeof(ValueName);
        retCode = RegEnumValue(hKey, i, ValueName, &chValueName, NULL, NULL, NULL, NULL);
        if (retCode == ERROR_SUCCESS)
        {
            RegQueryValueEx(hKey, ValueName, NULL, NULL, (LPBYTE)PerfData, &cbData);
            printf("%d) %s:  %s\n", (i + 1), ValueName, PerfData);
            i++;
        }
        else break;
    }

    RegCloseKey(hKey);


    //CPU performance measurement
    counter();
    cout << "CPU clock count: " << tickCount() << " us\n";
    return 0;

}

