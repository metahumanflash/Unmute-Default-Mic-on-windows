#include <windows.h>
#include <iostream>
#include <fstream>
#include "resource.h"

bool extractResource(const HINSTANCE hInstance, int resourceID, const std::wstring& outputPath) {
    std::wcout << L"Attempting to find resource..." << std::endl;
    HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resourceID), RT_RCDATA);
    if (!hResource) {
        std::wcerr << L"Failed to find resource." << std::endl;
        return false;
    }

    std::wcout << L"Resource found, loading resource..." << std::endl;
    HGLOBAL hLoadedResource = LoadResource(hInstance, hResource);
    if (!hLoadedResource) {
        std::wcerr << L"Failed to load resource." << std::endl;
        return false;
    }

    std::wcout << L"Resource loaded, locking resource..." << std::endl;
    void* pLockedResource = LockResource(hLoadedResource);
    if (!pLockedResource) {
        std::wcerr << L"Failed to lock resource." << std::endl;
        return false;
    }

    DWORD dwResourceSize = SizeofResource(hInstance, hResource);
    if (dwResourceSize == 0) {
        std::wcerr << L"Resource size is zero." << std::endl;
        return false;
    }

    std::wcout << L"Writing resource to file: " << outputPath << std::endl;
    std::ofstream outFile(outputPath, std::ios::binary);
    outFile.write(static_cast<char*>(pLockedResource), dwResourceSize);
    outFile.close();

    std::wcout << L"Resource extraction complete." << std::endl;
    return true;
}

int main() {
    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    wcscat_s(tempPath, L"nircmd_temp.exe");

    std::wcout << L"Temporary path: " << tempPath << std::endl;

    if (extractResource(GetModuleHandle(NULL), NIRCMD_EXE, tempPath)) {
        std::wstring command = std::wstring(tempPath) + L" mutesysvolume 0 default_record";

        std::string cmd(command.begin(), command.end());

        std::wcout << L"Executing command: " << command << std::endl;
        system(cmd.c_str());

        std::wcout << L"Command executed, removing temporary file..." << std::endl;
        _wremove(tempPath);

        std::wcout << L"Temporary file removed." << std::endl;
    }
    else {
        std::wcerr << L"Error extracting nircmd.exe" << std::endl;
    }

    return 0;
}
