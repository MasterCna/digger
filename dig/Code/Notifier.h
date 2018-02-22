#ifndef __NOTIFIER_H__
#define __NOTIFIER_H__

#include <iostream>
#include <Windows.h>
#include <WinInet.h>
#include <iphlpapi.h>
#include <strsafe.h>
#include <intrin.h>

using std::string;

class CNotifier
{
public:
	string HttpRequest(
		_In_ int Method,
		_In_ LPCSTR Host,
		_In_ LPCSTR url,
		_In_ LPCSTR header,
		_In_ LPSTR data);

	void GetMacHash(__int16& mac1, __int16& mac2);
	LPTSTR GetMachineName(TCHAR &tcName);

private:
	__int16 HashMacAddress(PIP_ADAPTER_INFO info);
	
};


#endif // __NOTIFIER_H__