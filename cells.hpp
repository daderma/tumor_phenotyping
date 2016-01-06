#ifndef TUMOR_PHENOTYPING_CELL_HPP
#define TUMOR_PHENOTYPING_CELL_HPP


#include <boost/filesystem.hpp>
#include <cstdint>
#include <list>


struct cell_type
{
	boost::filesystem::path path;
	std::int64_t id;
	std::int64_t x; 
	std::int64_t y;
	std::string phenotype;
};


typedef std::list<cell_type> cells_type;


#endif	// TUMOR_PHENOTYPING_CELL_HPP