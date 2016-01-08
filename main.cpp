#include "samples.hpp"
#include <boost/exception/diagnostic_information.hpp> 
#include <iostream>
#include <windows.h>
#include "shlobj.h"


std::wstring select_directory()
{
	std::wstring result;
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


int main(int argc, char* argv[])
{
	try
	{
		std::cout << "Select a folder containing inForm sample data." << std::endl;

		std::wstring directory(select_directory());
		if(!directory.empty())
		{
			samples_type samples;
			load_inform_samples(directory, samples);
			save_inform_sample_distances(directory, samples);
			save_inform_phenotype_summary(directory, samples);
			save_inform_phenotype_distances(directory, samples);
		}
		return 0;
	}

	catch(...)
	{
		std::cout << boost::current_exception_diagnostic_information() << std::endl;
		return 1;
	}
}