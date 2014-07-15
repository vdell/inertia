#include "pch.hpp"
#include "why_utilities.hpp"

std::string why::get_os_info()
{
	using namespace boost;
	using namespace std;

	string os_ver = "Operating System: Unknown";

#ifdef _WINDOWS
	DWORD dwVersion = 0;
	DWORD dwMajorVersion = 0;
	DWORD dwMinorVersion = 0;
	DWORD dwBuild = 0;

	dwVersion = GetVersion();

	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	if (dwVersion < 0x80000000)
	{
		dwBuild = (DWORD)(HIWORD(dwVersion));
	}

	os_ver = str(format("Operating System: Windows %1%.%2%(%3%)") %
		dwMajorVersion % dwMinorVersion % dwBuild);
#endif

	return os_ver;
}