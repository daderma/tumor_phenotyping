#include "inform.hpp"
#include "windows.hpp"
#include <boost/exception/diagnostic_information.hpp> 
#include <iostream>
#include <set>


void select_phenotype_of_interest(categories_type const& categories, std::string& phenotype)
{
	std::set<std::string> phenotypes;
	for(auto const& category: categories)
	{
		for(auto const& sample: category.second)
		{
			for(auto const& phenotype: sample.second.phenotypes)
			{
				phenotypes.insert(phenotype.first);
			}
		}
	}

	while(true)
	{
		std::cout << "Please select your phenotype of interest: " << std::endl;
		for(auto const& phenotype: phenotypes)
		{
			std::cout << phenotype << std::endl;
		}
		std::cout << "> ";
		std::getline(std::cin, phenotype);
		if(phenotypes.count(phenotype))
		{
			std::cout << std::endl;
			return;
		}
	}
}


void try_directory(boost::filesystem::path const& directory)
{
	if(boost::filesystem::is_directory(directory))
	{
		categories_type categories;
		load_inform_samples(directory, categories);
		if(categories.empty())
		{
			boost::filesystem::directory_iterator end;
			for(boost::filesystem::directory_iterator iter(directory); iter != end; ++ iter)
			{
				try_directory(iter->path());
			}
		}
		else
		{
			std::string interest;
			select_phenotype_of_interest(categories, interest);
			for(auto const& category: categories)
			{
				boost::filesystem::create_directories(directory / category.first);
				save_inform_sample_nearest(directory, category, interest);
				save_inform_sample_nearest_composites(directory, category, interest);
				save_inform_sample_neighbor_composites(directory, category, interest);
				save_inform_phenotype_nearest(directory, category, interest);
			}
			save_inform_phenotype_summary(directory, categories, interest);
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