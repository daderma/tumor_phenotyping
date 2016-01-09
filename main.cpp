#include "samples.hpp"
#include "windows.hpp"
#include <boost/exception/diagnostic_information.hpp> 
#include <iostream>


void try_directory(boost::filesystem::path const& directory)
{
	if(boost::filesystem::is_directory(directory))
	{
		using namespace samples;
	
		samples_type samples;
		load_inform_samples(directory, samples);
		if(samples.empty())
		{
			boost::filesystem::directory_iterator end;
			for(boost::filesystem::directory_iterator iter(directory); iter != end; ++ iter)
			{
				try_directory(iter->path());
			}
		}
		else
		{
			save_inform_sample_nearest(directory, samples);
			save_inform_sample_nearest_composites(directory, samples);
			save_inform_sample_neighbor_composites(directory, samples);
			save_inform_phenotype_nearest(directory, samples);
			save_inform_phenotype_summary(directory, samples);
		}
	}
	else
	{
		std::cout << "Not a directory: " << directory << std::endl;
	}
}


int main(int argc, char* argv[])
{
	try
	{
		if(argc > 1)
		{
			for(int i(1); i < argc; ++ i)
			{
				try_directory(argv[i]);
			}
		}
		else
		{
			std::cout << "This program processes a collection of inForm samples located in a directory. Example:" << std::endl;
			std::cout << "samples" << std::endl << "\tset 1" << std::endl << "\tset 2" << std::endl << "\tset 3" << std::endl << "\t..." << std::endl << std::endl;
			std::cout << "You can pass the directories you wish to process on the command line. If you provide a directory" << std::endl;
			std::cout << "containing sub-directories (i.e. \"samples\" above), the sub-directories will be processed as well." << std::endl << std::endl;
			std::cout << "For now you can manually select a directory to process..." << std::endl;
			try_directory(windows::select_directory());
		}
		return 0;
	}

	catch(...)
	{
		std::cout << boost::current_exception_diagnostic_information() << std::endl;
		return 1;
	}
}