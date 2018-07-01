#include "inform.hpp"
#include "windows.hpp"
#include <boost/exception/diagnostic_information.hpp> 
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <set>


std::string select_phenotype_of_interest(categories_type const& categories)
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

	std::string result;
	while(phenotypes.count(result) == 0)
	{
		std::cout << "Please select your phenotype of interest: " << std::endl;
		for(auto const& phenotype: phenotypes)
		{
			std::cout << phenotype << std::endl;
		}
		std::cout << "Enter phenotype name: ";
		std::getline(std::cin, result);
	}

	std::cout << std::endl;
	return result;
}


filters_type select_filters()
{
	std::cout << "You can now add optional filters. Filters consist of a column name and a threshold value." << std::endl;
	std::cout << "Rows that contain a value below the threshold, at the specified column name, will be discarded." << std::endl;

	filters_type result;
	while(true)
	{
		std::cout << "Enter column name (empty to finish): ";
		std::string column;
		std::getline(std::cin, column);
		if(column.empty())
		{
			break;
		}
		else if(result.count(column))
		{
			std::cout << "Column already filtered" << std::endl;
			continue;
		}
		std::string threshold;
		std::cout << "Enter threshold value: ";
		std::getline(std::cin, threshold);
		result[column] = boost::lexical_cast<double>(threshold);
	}

	std::cout << std::endl;
	return result;
}


void try_directory(boost::filesystem::path const& directory)
{
	if(boost::filesystem::is_directory(directory))
	{
		categories_type categories;
		static filters_type const filters(select_filters());
		load_inform_samples(directory, categories, filters);
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
			static std::string const interest(select_phenotype_of_interest(categories));
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
		std::cout << "| tumor_phenotyping built " << __DATE__ << " " << __TIME__ << std::endl;
		std::cout << "| Latest version available at https://github.com/daderma/tumor_phenotyping" << std::endl;
		std::cout << "+-------------------------------------------------------------------------" << std::endl;
		
		std::cout << std::endl;

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
			std::cout << "For now you can manually select a directory to process..." << std::endl << std::endl;
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