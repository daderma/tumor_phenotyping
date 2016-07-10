#include "cells.hpp"


double distance(cell_ptr_type const& cell, cell_ptr_type const& candidate)
{
	std::int64_t const x_distance(cell->x - candidate->x);
	std::int64_t const y_distance(cell->y - candidate->y);
	return std::sqrt((x_distance * x_distance) + (y_distance * y_distance));
}


void nearest(cell_ptr_type const& cell, cells_type const& candidates, double& nearest_distance, cell_ptr_type& nearest_cell)
{
	for(auto const& candidate: candidates)
	{
		double const candidate_distance(distance(cell, candidate));
		if(candidate_distance < nearest_distance)
		{
			nearest_distance = candidate_distance;
			nearest_cell = candidate;
		}
	}
}
