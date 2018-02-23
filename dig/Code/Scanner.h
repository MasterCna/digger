/****************************************************************************\
* Created: 1/9/2018 by Ali Sepehri-Amin
* Version: 0.9.0000
* Copyright © Tehranbytes Team.
* History: -
* Description: -
\****************************************************************************/

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <Windows.h>
#include <iostream>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <queue>

class CScanner
{
public:
	int ScanFile(const std::wstring& p_path, std::string &status);
};

#endif // __SCANNER_H__