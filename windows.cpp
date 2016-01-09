#include "windows.hpp"
#include <windows.h>
#include "shlobj.h"


namespace windows
{


boost::filesystem::path select_directory()
{
	boost::filesystem::path result;
	BROWSEINFO bi;
	std::memset(&bi, 0, sizeof(bi));
	bi.ulFlags   = BIF_USENEWUI;

	::OleInitialize(NULL);
	LPITEMIDLIST item(::SHBrowseForFolder(&bi));
	if(item)
	{
		wchar_t buffer[MAX_PATH];
		if(::SHGetPathFromIDList(item, buffer))
		{
			result = buffer;
		}
		CoTaskMemFree(item);
	}
	::OleUninitialize();
	
	return result;
}


}	// namespace windows