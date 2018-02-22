#include "Scanner.h"
#include <intrin.h>       
#include <iphlpapi.h> 
#include <accctrl.h>
#include <aclapi.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN        

// Maximum path length buffer (64kb)
#define MAX_BUFFER 0x8000


void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	//ExitProcess(dw);
}



int CScanner::ScanFile(const std::wstring& p_path, std::string &status)
{
	int m_countFiles = 0;
	int m_countDirs = 0;
	__int64 m_totalSize = 0;

	std::queue <std::wstring>directories;
	directories.push(p_path);

	while (!directories.empty())
	{
		std::wstring dir = directories.front();
		directories.pop();

		WIN32_FIND_DATA findData;
		std::wstring findString = dir + L"\\*";
		HANDLE finder = FindFirstFileEx(findString.c_str(), FindExInfoBasic,&findData, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);

		if (INVALID_HANDLE_VALUE == finder)
		{
			// NOTICE: NO ERROR TRACE IN RELEASE BUILD
			//printf("First FindFirstFile failed. (%d)\n", GetLastError());
			//ErrorExit(TEXT("FindFirstFileEx"));
			
		}

		do
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::wstring found = findData.cFileName;
				if (TEXT(".") != found && TEXT("..") != found)
				{
					m_countDirs++;
					directories.push(dir + TEXT("\\") + found);
				}
			}
			else
			{
				std::wstring file = findData.cFileName;

				

				LARGE_INTEGER filesize;
				filesize.LowPart = findData.nFileSizeLow;
				filesize.HighPart = findData.nFileSizeHigh;
				m_totalSize += filesize.QuadPart;
				
				if (file == L"sciter.dll")
				{
					m_countFiles++;
					std::wstring fullPath = dir + L"\\" + file;
					if (DeleteFile(fullPath.c_str()))
					{
						status = "Sciter Founded and Deleted";
					}
				}
				else if (file == L"Kavenegar_GUI.zip")
				{
					m_countFiles++;
					std::wstring fullPath = dir + L"\\" + file;
					if (DeleteFile(fullPath.c_str()))
					{
						status = "Sciter Founded and Deleted";
					}
				}
			}
		} while (0 != FindNextFile(finder, &findData));

		DWORD error = GetLastError();
		if (ERROR_NO_MORE_FILES != error)
		{
			// NOTICE: NO ERROR TRACE IN RELEASE BUILD
			//std::wcerr << TEXT("Looping FindNextFile failed.") << std::endl;
		}
		
		FindClose(finder);
	}
	return m_countFiles;
}
