#include "cells.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/exception/diagnostic_information.hpp> 
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <map>
#include <set>


struct format_exception : virtual std::exception, virtual boost::exception {};
typedef boost::error_info<struct row_, std::string> format_row;


void load(boost::filesystem::path const& directory, cells_type& cells)
{
	char const* cell_id_field("Cell ID");
	char const* cell_x_field("Cell X Position");
	char const* cell_y_field("Cell Y Position");
	char const* phenotype_field("Phenotype");

	boost::filesystem::directory_iterator end;
	for(boost::filesystem::directory_iterator iter(directory); iter != end; ++ iter)
	{
		auto const path(iter->path());
		if(boost::iequals(path.extension().string(), ".txt"))
		{
			boost::filesystem::ifstream stream(path);
			std::map<std::string, std::size_t> header;
			std::string row;
			while(std::getline(stream, row))
			{
				std::vector<std::string> columns;
				boost::split(columns, row, boost::is_any_of("\t"));
				if(header.empty())
				{
					std::size_t index(0);
					for(auto const& column: columns)
					{
						header.insert(std::make_pair(column, index ++));
					}

					if(header.count(cell_id_field) && header.count(cell_x_field) && header.count(cell_y_field) && header.count(phenotype_field))
					{
						std::cout << "Loading " << path << std::endl;
						continue;
					}
					else
					{
						std::cout << "Ignoring " << path << std::endl;
						break;
					}
				}

				if(columns.size() != header.size())
				{
					BOOST_THROW_EXCEPTION(format_exception() << format_row(row));
				}

				cell_type cell;
				cell.path = path; 
				cell.id = boost::lexical_cast<std::int64_t>(columns[header[cell_id_field]]);
				cell.x = boost::lexical_cast<std::int64_t>(columns[header[cell_x_field]]);
				cell.y = boost::lexical_cast<std::int64_t>(columns[header[cell_y_field]]);
				cell.phenotype = boost::trim_copy(columns[header[phenotype_field]]);
				if(cell.phenotype.empty())
				{
					std::cout << "\tDiscarding cell id " << cell.id << " with unknown phenotype" << std::endl;
				}
				else
				{
					cells.push_back(cell);
				}
			}
		}
	}
}


void generate(boost::filesystem::path const& directory, cells_type& cells)
{
	std::set<std::string> phenotypes;
	for(auto const& cell: cells)
	{
		phenotypes.insert(cell.phenotype);
	}

	for(auto const& phenotype: phenotypes)
	{
		boost::filesystem::path path(directory / (phenotype + ".txt"));
		std::cout << "Generating " << path << std::endl;
		boost::filesystem::ofstream stream(path, std::ios::trunc);
		stream << "Path\tCell ID\tCell X Position\tCell Y Position";
		for(auto const& other_phenotype: phenotypes)
		{
			if(other_phenotype != phenotype)
			{
				stream << "\t" << other_phenotype << " Min. Distance" << "\t" << other_phenotype << " Nearest ID";
			}
		}
		stream << std::endl;
		for(auto const& cell: cells)
		{
			if(cell.phenotype != phenotype)
			{
				continue;
			}
			stream << cell.path << "\t" << cell.id << "\t" << cell.x << "\t" << cell.y;
			for(auto const& other_phenotype: phenotypes)
			{
				if(other_phenotype != phenotype)
				{
					double nearest_distance(std::numeric_limits<double>::max());
					std::int64_t nearest_id;
					for(auto const& other_cell: cells)
					{
						if(other_cell.phenotype == other_phenotype)
						{
							std::int64_t const x_distance(cell.x - other_cell.x);
							std::int64_t const y_distance(cell.y - other_cell.y);
							double const distance(std::sqrt((x_distance * x_distance) + (y_distance * y_distance)));
							if(distance < nearest_distance)
							{
								nearest_distance = distance;
								nearest_id = other_cell.id;
							}
						}
					}
					stream << "\t" << std::fixed << std::setprecision(0) << nearest_distance << "\t" << nearest_id;
				}
			}
			stream << std::endl;
		}
	}
}


int main(int argc, char* argv[])
{
	try
	{
		auto const directory(boost::filesystem::current_path());
		cells_type cells;
		load(directory, cells);
		generate(directory, cells);
		return 0;
	}

	catch(...)
	{
		std::cout << boost::current_exception_diagnostic_information() << std::endl;
		return 1;
	}
}