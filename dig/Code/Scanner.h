#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <Windows.h>
#include <iostream>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <queue>
#include "Common.h"

class CScanner
{
public:
	int ScanFile(const std::wstring& p_path, std::string &status);
};

#endif // __SCANNER_H__