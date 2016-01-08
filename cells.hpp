#ifndef TUMOR_PHENOTYPING__CELLS_HPP
#define TUMOR_PHENOTYPING__CELLS_HPP


#include <cstdint>
#include <list>
#include <memory>


struct cell_type
{
	std::int64_t id;
	std::int64_t x; 
	std::int64_t y;
};


typedef std::shared_ptr<cell_type> cell_ptr_type;
typedef std::list<cell_ptr_type> cells_type;


void nearest(cell_ptr_type const& cell, cells_type const& candidates, double& nearest_distance, cell_ptr_type& nearest_cell);


#endif	// TUMOR_PHENOTYPING__CELLS_HPP