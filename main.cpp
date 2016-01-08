#include "samples.hpp"
#include <boost/exception/diagnostic_information.hpp> 
#include <iostream>

/*
void generate(boost::filesystem::path const& directory, cells_type& cells)
{

}
*/

int main(int argc, char* argv[])
{
	try
	{
		auto const directory(boost::filesystem::current_path() / "samples");
		samples_type samples;
		load_inform_samples(directory, samples);
		std::cout << "Samples:" << std::endl;
		for(auto const& sample: samples)
		{
			std::cout << "\t" << sample.first << std::endl;			
			for(auto const& phenotype: sample.second)
			{
				std::cout << "\t\t" << phenotype.first << ": " << phenotype.second.size() << " cell(s)" << std::endl;
			}
		}
		save_inform_distances(directory, samples);
		return 0;
	}

	catch(...)
	{
		std::cout << boost::current_exception_diagnostic_information() << std::endl;
		return 1;
	}
}