#include "samples.hpp"
#include "bresenham.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/gil/extension/io/tiff_io.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <set>


void load_inform_samples(boost::filesystem::path const& directory, samples_type& samples)
{
	char const* sample_name("Sample Name");
	char const* phenotype_field("Phenotype");
	char const* cell_id_field("Cell ID");
	char const* cell_x_field("Cell X Position");
	char const* cell_y_field("Cell Y Position");

	boost::filesystem::directory_iterator end;
	for(boost::filesystem::directory_iterator iter(directory); iter != end; ++ iter)
	{
		auto const path(iter->path());
		if(boost::iends_with(path.string(), "_cell_seg_data.txt"))
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

					if(header.count(sample_name) && header.count(cell_id_field) 
						&& header.count(cell_x_field) && header.count(cell_y_field) 
						&& header.count(phenotype_field))
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
					BOOST_THROW_EXCEPTION(load_exception() << load_detail_type(row));
				}

				std::string const sample(columns[header[sample_name]]);
				std::string const phenotype(columns[header[phenotype_field]]);
				auto cell(std::make_shared<cell_type>());
				cell->id = boost::lexical_cast<std::int64_t>(columns[header[cell_id_field]]);
				cell->x = boost::lexical_cast<std::int64_t>(columns[header[cell_x_field]]);
				cell->y = boost::lexical_cast<std::int64_t>(columns[header[cell_y_field]]);
				if(phenotype.empty())
				{
					std::cout << "\tDiscarding cell id " << cell->id << " with empty phenotype" << std::endl;
				}
				else
				{
					samples[sample][phenotype].push_back(cell);
				}
			}
		}
	}
}


std::string create_filename(std::string const& sample, std::string const& trailer)
{
	return boost::ireplace_last_copy(sample, ".im3", "") + trailer;
}


void save_inform_sample_distances(boost::filesystem::path const& directory, samples_type const& samples)
{
	for(auto const& sample: samples)
	{
		for(auto const& phenotype: sample.second)
		{
			boost::filesystem::path path(directory / create_filename(sample.first, "_distance_" + phenotype.first + ".txt"));
			std::cout << "Saving " << path << std::endl;
			boost::filesystem::ofstream stream(path, std::ios::trunc);

			stream << "Sample Name\tCell ID\tCell X Position\tCell Y Position";
			for(auto const& candidate_phenotype: sample.second)
			{
				if(candidate_phenotype.first != phenotype.first)
				{
					stream 
						<< "\t" << candidate_phenotype.first << " Min. Distance" 
						<< "\t" << candidate_phenotype.first << " Nearest Cell ID";
				}
			}
			stream << std::endl;
			
			for(auto const& cell: phenotype.second)
			{
				stream 
					<< sample.first 
					<< "\t" << cell->id 
					<< "\t" << cell->x 
					<< "\t" << cell->y;
				for(auto const& candidate_phenotype: sample.second)
				{
					if(candidate_phenotype.first != phenotype.first)
					{
						double nearest_distance(std::numeric_limits<double>::max());
						cell_ptr_type nearest_cell;
						nearest(cell, candidate_phenotype.second, nearest_distance, nearest_cell);
						stream
							<< "\t" << std::fixed << std::setprecision(0) << nearest_distance 
							<< "\t" << nearest_cell->id;
					}
				}
				stream << std::endl;
			}
		}
	}
}


void save_inform_sample_composites(boost::filesystem::path const& directory, samples_type const& samples)
{
	double const maximum_distance_percent(0.10);	// Maximum cell distance as percentage of max(width, height) of image

	for(auto const& sample: samples)
	{
		boost::gil::rgb8_image_t original;
		boost::gil::tiff_read_image((directory / create_filename(sample.first, "_composite_image.tif")).string(), original);

		for(auto const& phenotype: sample.second)
		{
			for(auto const& candidate_phenotype: sample.second)
			{
				if(candidate_phenotype.first == phenotype.first)
				{
					continue;
				}

				boost::gil::rgb8_image_t image(original);
				auto view(boost::gil::view(image));
				for(auto const& cell: phenotype.second)
				{
					bresenham_line(cell->x, cell->y, cell->x + 3, cell->y, view);
					bresenham_line(cell->x, cell->y, cell->x - 3, cell->y, view);
					bresenham_line(cell->x, cell->y, cell->x, cell->y + 3, view);
					bresenham_line(cell->x, cell->y, cell->x, cell->y - 3, view);

					double nearest_distance(std::numeric_limits<double>::max());
					cell_ptr_type nearest_cell;
					nearest(cell, candidate_phenotype.second, nearest_distance, nearest_cell);
					if(nearest_distance < std::max(view.height(), view.width()) * maximum_distance_percent)
					{
						bresenham_line(cell->x, cell->y, nearest_cell->x, nearest_cell->y, view);
					}
				}

				boost::filesystem::path path(directory / create_filename(sample.first, "_distance_" + phenotype.first + "_" + candidate_phenotype.first + ".tif"));
				std::cout << "Saving " << path << std::endl;
				boost::gil::tiff_write_view(path.string(), view);
			}
		}
	}
}



void save_inform_phenotype_distances(boost::filesystem::path const& directory, samples_type const& samples)
{
	std::set<std::string> phenotypes;
	for(auto const& sample: samples)
	{
		for(auto const& phenotype: sample.second)
		{
			phenotypes.insert(phenotype.first);
		}
	}

	for(auto const& phenotype: phenotypes)
	{
		boost::filesystem::path path(directory / ("phenotype_distance_" + phenotype + ".txt"));
		std::cout << "Saving " << path << std::endl;
		boost::filesystem::ofstream stream(path, std::ios::trunc);

		stream << "Sample Name\tCell ID\tCell X Position\tCell Y Position";
		for(auto const& candidate_phenotype: phenotypes)
		{
			if(candidate_phenotype != phenotype)
			{
				stream 
					<< "\t" << candidate_phenotype << " Min. Distance" 
					<< "\t" << candidate_phenotype << " Nearest Cell ID";
			}
		}
		stream << std::endl;

		for(auto const& sample: samples)
		{
			if(sample.second.count(phenotype))
			{
				for(auto const& cell: sample.second.at(phenotype))
				{
					stream 
						<< sample.first 
						<< "\t" << cell->id 
						<< "\t" << cell->x 
						<< "\t" << cell->y;

					for(auto const& candidate_phenotype: phenotypes)
					{
						if(candidate_phenotype != phenotype)
						{
							double nearest_distance(std::numeric_limits<double>::max());
							cell_ptr_type nearest_cell;
							if(sample.second.count(candidate_phenotype))
							{
								nearest(cell, sample.second.at(candidate_phenotype), nearest_distance, nearest_cell);
							}

							if(nearest_cell)
							{
								stream
									<< "\t" << std::fixed << std::setprecision(0) << nearest_distance 
									<< "\t" << nearest_cell->id;
							}
							else
							{
								stream << "\t" << "\t";
							}
						}
					}
					stream << std::endl;
				}
			}
		}
	}
}


void save_inform_phenotype_summary(boost::filesystem::path const& directory, samples_type const& samples)
{
	boost::filesystem::path path(directory / "phenotype_summary.txt");
	std::cout << "Saving " << path << std::endl;
	boost::filesystem::ofstream stream(path, std::ios::trunc);
	
	std::set<std::string> phenotypes;
	for(auto const& sample: samples)
	{
		for(auto const& phenotype: sample.second)
		{
			phenotypes.insert(phenotype.first);
		}
	}

	stream << "Sample Name";
	for(auto const& phenotype: phenotypes)
	{
		stream << "\t" << phenotype << " Cells";
	}
	stream << std::endl;

	for(auto const& sample: samples)
	{
		stream << sample.first;
		for(auto const& phenotype: phenotypes)
		{
			std::size_t cells(0);
			if(sample.second.count(phenotype))
			{
				cells = sample.second.at(phenotype).size();
			}
			stream << "\t" << cells;
		}
		stream << std::endl;
	}
}