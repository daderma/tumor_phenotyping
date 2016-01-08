#include "samples.hpp"
#include <boost/exception/diagnostic_information.hpp> 
#include <iostream>


int main(int argc, char* argv[])
{
	try
	{
		auto const directory(boost::filesystem::current_path() / "samples");
		samples_type samples;
		load_inform_samples(directory, samples);
		save_inform_distances(directory, samples);
		save_inform_phenotype_summary(directory, samples);
		return 0;
	}

	catch(...)
	{
		std::cout << boost::current_exception_diagnostic_information() << std::endl;
		return 1;
	}
}