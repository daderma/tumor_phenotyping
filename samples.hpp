#pragma once


#include "phenotypes.hpp"



struct sample_type
{
	std::int64_t area;	// Pixels
	phenotypes_type phenotypes;
};


typedef std::map<std::string, sample_type> samples_type;
