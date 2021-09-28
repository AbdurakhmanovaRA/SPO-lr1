﻿#define WINVER 0x0502
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <locale.h>
#include <iostream>

using namespace std;

#define INFO_BUF_SIZE 32767
#define BUFSIZE MAX_PATH
#define FILESYSNAMEBUFSIZE MAX_PATH
#define TOTALBYTES 8192

//2.1, 2.2


void counter()
{
    LARGE_INTEGER freq, tick1, tick2;
    QueryPerformanceCounter(&tick1);
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&tick2);
    double count = (double(tick2.QuadPart - tick1.QuadPart) * 1000000 / freq.QuadPart);
    printf("Рабочая частота ЦП: %u Hz \n", freq);
    printf("Количество тактов ЦП потраченных на выполнение пункта 2.1: %f мкс\n", count);

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
            printf("Total size: %I64u  bytes\n", total.QuadPart);
            printf("Free space: %I64u  bytes\n", free.QuadPart);
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
    TCHAR ValueName[TOTALBYTES];
    DWORD chValueName = TOTALBYTES;
    LPBYTE  lpData = (LPBYTE)malloc(TOTALBYTES);
    int i = 0;

    printf("\nStartup programs:\n");
    RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey);
    while (RegEnumValue(hKey, i, ValueName, &chValueName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
    {
        chValueName = TOTALBYTES;
        RegQueryValueEx(hKey, ValueName, NULL, NULL, lpData, &chValueName);
        printf("%d) %s:  %s\n", i + 1, ValueName, lpData);
        i++;
    }
    RegCloseKey(hKey);


    counter();
    return 0;
}

