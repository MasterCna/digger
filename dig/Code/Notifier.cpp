#include "Notifier.h"

#define POST 1
#define GET 0

#pragma comment (lib, "urlmon.lib")
#pragma comment (lib, "Iphlpapi.lib")

string CNotifier::HttpRequest(
	_In_ int Method,
	_In_ LPCSTR Host,
	_In_ LPCSTR url,
	_In_ LPCSTR header,
	_In_ LPSTR data)
{
	//Retrieve default http user agent
	char httpUseragent[512];

	char buffer[4096];
	DWORD read;

	DWORD szhttpUserAgent = sizeof(httpUseragent);
	ObtainUserAgentString(0, httpUseragent, &szhttpUserAgent);

	char m[5];

	if (Method == GET)
		strcpy_s(m, "GET");
	else
		strcpy_s(m, "POST");

	HINTERNET internet = InternetOpenA(httpUseragent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (internet != NULL)
	{
		HINTERNET connect = InternetConnectA(internet, Host, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
		if (connect != NULL)
		{
			HINTERNET request = HttpOpenRequestA(connect, m, url, "HTTP/1.1", NULL, NULL,
				INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_IGNORE_CERT_CN_INVALID |
				INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
				INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
				INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
				INTERNET_FLAG_NO_AUTH |
				INTERNET_FLAG_NO_CACHE_WRITE |
				INTERNET_FLAG_NO_UI |
				INTERNET_FLAG_PRAGMA_NOCACHE |
				INTERNET_FLAG_RELOAD, NULL);

			if (request != NULL)
			{
				int datalen = 0;
				if (data != NULL) datalen = strlen(data);
				int headerlen = 0;
				if (header != NULL) headerlen = strlen(header);

				HttpSendRequestA(request, header, headerlen, data, datalen);
				InternetReadFile(request, buffer, sizeof(buffer) / sizeof(buffer[0]), &read);

				/* End http requests */
				HttpEndRequest(request, NULL, 0, 0);
				InternetCloseHandle(request);
			}
		}

		InternetCloseHandle(connect);
	}
	InternetCloseHandle(internet);

	return std::string(buffer, read);
}
__int16 CNotifier::HashMacAddress(PIP_ADAPTER_INFO info)
{
	__int16 hash = 0;
	for (__int32 i = 0; i < info->AddressLength; i++)
	{
		hash += (info->Address[i] << ((i & 1) * 8));
	}
	return hash;
}

void CNotifier::GetMacHash(__int16& mac1, __int16& mac2)
{
	IP_ADAPTER_INFO AdapterInfo[32];
	DWORD dwBufLen = sizeof(AdapterInfo);

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	if (dwStatus != ERROR_SUCCESS)
		return; // no adapters.

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	mac1 = HashMacAddress(pAdapterInfo);
	if (pAdapterInfo->Next)
		mac2 = HashMacAddress(pAdapterInfo->Next);

	// sort the mac addresses. We don't want to invalidate
	// both macs if they just change order.
	if (mac1 > mac2)
	{
		__int16 tmp = mac2;
		mac2 = mac1;
		mac1 = tmp;
	}
}
