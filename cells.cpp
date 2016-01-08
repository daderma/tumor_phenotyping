#include "cells.hpp"


void nearest(cell_ptr_type const& cell, cells_type const& candidates, double& nearest_distance, cell_ptr_type& nearest_cell)
{
	for(auto const& candidate: candidates)
	{
		std::int64_t const x_distance(cell->x - candidate->x);
		std::int64_t const y_distance(cell->y - candidate->y);
		double const distance(std::sqrt((x_distance * x_distance) + (y_distance * y_distance)));
		if(distance < nearest_distance)
		{
			nearest_distance = distance;
			nearest_cell = candidate;
		}
	}
}