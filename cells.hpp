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


typedef std::shared_ptr<cell_type> cell_ptr_type;
typedef std::list<cell_ptr_type> cells_type;


template<typename Range>
void nearest(cell_ptr_type const& target, Range const& range, std::string const& phenotype, double& nearest_distance, cell_ptr_type& nearest_cell)
{
	for(auto const& candidate: range)
	{
		if(candidate->phenotype == phenotype)
		{
			std::int64_t const x_distance(target->x - candidate->x);
			std::int64_t const y_distance(target->y - candidate->y);
			double const distance(std::sqrt((x_distance * x_distance) + (y_distance * y_distance)));
			if(distance < nearest_distance)
			{
				nearest_distance = distance;
				nearest_cell = candidate;
			}
		}
	}
}


#endif	// TUMOR_PHENOTYPING_CELL_HPP