#include "inform.hpp"
#include "bresenham.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/gil/extension/io/tiff_io.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <set>


double const distance_threshold_percentage(0.15);	// Maximum cell distance as percentage of min(width, height) of image


void load_inform_samples(boost::filesystem::path const& directory, categories_type& categories)
{
	boost::filesystem::directory_iterator end;
	for(boost::filesystem::directory_iterator iter(directory); iter != end; ++ iter)
	{
		auto const path(iter->path());
		if(boost::iends_with(path.string(), "_cell_seg_data.txt"))
		{
			auto const sample_field("Sample Name");
			auto const category_field("Tissue Category");
			auto const phenotype_field("Phenotype");
			auto const cell_id_field("Cell ID");
			auto const cell_x_field("Cell X Position");
			auto const cell_y_field("Cell Y Position");

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

					if(header.count(sample_field) 
						&& header.count(category_field) && header.count(phenotype_field)
						&& header.count(cell_id_field) 
						&& header.count(cell_x_field) && header.count(cell_y_field)
						)
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

				auto const sample(columns[header[sample_field]]);
				auto const category(columns[header[category_field]]);
				auto const phenotype(columns[header[phenotype_field]]);
				auto const cell(std::make_shared<cell_type>());
				cell->id = boost::lexical_cast<std::int64_t>(columns[header[cell_id_field]]);
				cell->x = boost::lexical_cast<std::int64_t>(columns[header[cell_x_field]]);
				cell->y = boost::lexical_cast<std::int64_t>(columns[header[cell_y_field]]);
				if(!phenotype.empty())
				{
					categories[category][sample].phenotypes[phenotype].push_back(cell);
				}
			}
		}
		else if(boost::iends_with(path.string(), "_cell_seg_data_summary.txt"))
		{
			auto const sample_field("Sample Name");
			auto const category_field("Tissue Category");
			auto const area_field("Tissue Category Area (pixels)");

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

					if(header.count(sample_field) 
						&& header.count(category_field) && header.count(area_field)
						)
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

				auto const sample(columns[header[sample_field]]);
				auto const category(columns[header[category_field]]);
				auto const area(columns[header[area_field]]);
				categories[category][sample].area = boost::lexical_cast<std::int64_t>(area);
			}
		}
	}
}


std::string create_filename(std::string const& sample, std::string const& trailer)
{
	return boost::ireplace_last_copy(sample, ".im3", "") + trailer;
}


void save_inform_sample_nearest(boost::filesystem::path const& directory, categories_type::value_type const& category, std::string const& interest)
{
	for(auto const& sample: category.second)
	{
		for(auto const& phenotype: sample.second.phenotypes)
		{
			if(phenotype.first != interest)
			{
				continue;
			}

			auto const destination(directory / category.first / create_filename(sample.first, "_nearest_" + phenotype.first + ".csv"));
			std::cout << "Saving " << destination << std::endl;
			boost::filesystem::ofstream stream(destination, std::ios::trunc);

			stream << "sep=\t" << std::endl;
			stream << "Sample Name\tCell ID\tCell X Position\tCell Y Position";
			for(auto const& candidate_phenotype: sample.second.phenotypes)
			{
				if(candidate_phenotype.first != phenotype.first)
				{
					stream 
						<< "\t" << candidate_phenotype.first << " Nearest Distance" 
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
				for(auto const& candidate_phenotype: sample.second.phenotypes)
				{
					if(candidate_phenotype.first != phenotype.first)
					{
						double nearest_distance(std::numeric_limits<double>::max());
						cell_ptr_type nearest_cell;
						nearest(cell, candidate_phenotype.second, nearest_distance, nearest_cell);
						stream
							<< "\t" << boost::io::group(std::fixed, std::setprecision(0), nearest_distance)
							<< "\t" << nearest_cell->id;
					}
				}
				stream << std::endl;
			}
		}
	}
}


auto const red(boost::gil::rgb8_pixel_t(255, 0, 0));
auto const green(boost::gil::rgb8_pixel_t(0, 255, 0));


template<typename Pixel, typename View>
void mark(cell_ptr_type const& cell, Pixel const& pixel, View& view)
{
	bresenham_line(cell->x - 3, cell->y - 3, cell->x + 3, cell->y + 3, pixel, view);
	bresenham_line(cell->x - 3, cell->y + 3, cell->x + 3, cell->y - 3, pixel, view);
}


void save_inform_sample_nearest_composites(boost::filesystem::path const& directory, categories_type::value_type const& category, std::string const& interest)
{
	for(auto const& sample: category.second)
	{
		auto const source((directory / create_filename(sample.first, "_composite_image.tif")));
		if(!boost::filesystem::is_regular_file(source))
		{
			std::cout << "Skipping missing " << source << std::endl;
			continue;
		}
		boost::gil::rgb8_image_t original;
		boost::gil::tiff_read_image(source.string(), original);
		double const distance_threshold_pixels(std::min(original.height(), original.width()) * distance_threshold_percentage);

		for(auto const& phenotype: sample.second.phenotypes)
		{
			if(phenotype.first != interest)
			{
				continue;
			}

			for(auto const& candidate_phenotype: sample.second.phenotypes)
			{
				if(candidate_phenotype.first == phenotype.first)
				{
					continue;
				}

				boost::gil::rgb8_image_t image(original);
				auto view(boost::gil::view(image));
				auto const white(boost::gil::rgb8_pixel_t(255, 255, 255));
				for(auto const& cell: phenotype.second)
				{
					double nearest_distance(std::numeric_limits<double>::max());
					cell_ptr_type nearest_cell;
					nearest(cell, candidate_phenotype.second, nearest_distance, nearest_cell);
					if(nearest_distance < distance_threshold_pixels)
					{
						bresenham_line(cell->x, cell->y, nearest_cell->x, nearest_cell->y, white, view);
					}
				}

				for(auto const& cell: phenotype.second)
				{
					mark(cell, red, view);
					for(auto const& candidate_cell: candidate_phenotype.second)
					{
						mark(candidate_cell, green, view);
					}
				}

				auto const destination(directory / category.first / create_filename(sample.first, "_nearest_" + phenotype.first + "_" + candidate_phenotype.first + ".tif"));
				std::cout << "Saving " << destination << std::endl;
				boost::gil::tiff_write_view(destination.string(), view);
			}
		}
	}
}


void save_inform_sample_neighbor_composites(boost::filesystem::path const& directory, categories_type::value_type const& category, std::string const& interest)
{
	for(auto const& sample: category.second)
	{
		auto const source(directory / create_filename(sample.first, "_composite_image.tif"));
		if(!boost::filesystem::is_regular_file(source))
		{
			std::cout << "Skipping missing " << source << std::endl;
			continue;
		}
		boost::gil::rgb8_image_t original;
		boost::gil::tiff_read_image(source.string(), original);
		double const distance_threshold_pixels(std::min(original.height(), original.width()) * distance_threshold_percentage);

		for(auto const& phenotype: sample.second.phenotypes)
		{
			if(phenotype.first != interest)
			{
				continue;
			}

			for(auto const& candidate_phenotype: sample.second.phenotypes)
			{
				if(candidate_phenotype.first == phenotype.first)
				{
					continue;
				}

				boost::gil::rgb8_image_t image(original);
				auto view(boost::gil::view(image));
				for(auto const& cell: phenotype.second)
				{
					for(auto const& candidate_cell: candidate_phenotype.second)
					{
						double const candidate_distance(distance(cell, candidate_cell));
						if(candidate_distance < distance_threshold_pixels)
						{
							double const intensity((distance_threshold_pixels - candidate_distance) / distance_threshold_pixels);
							auto const gradient(boost::gil::rgb8_pixel_t(
								127 + static_cast<boost::gil::bits8>(128 * intensity), 
								127 + static_cast<boost::gil::bits8>(128 * intensity), 
								127 + static_cast<boost::gil::bits8>(128 * intensity)
								));
							bresenham_line(cell->x, cell->y, candidate_cell->x, candidate_cell->y, gradient, view);
						}
					}
				}

				for(auto const& cell: phenotype.second)
				{
					mark(cell, red, view);
					for(auto const& candidate_cell: candidate_phenotype.second)
					{
						mark(candidate_cell, green, view);
					}
				}

				auto const destination(directory / category.first / create_filename(sample.first, "_neighbor_" + phenotype.first + "_" + candidate_phenotype.first + ".tif"));
				std::cout << "Saving " << destination << std::endl;
				boost::gil::tiff_write_view(destination.string(), view);
			}
		}
	}
}


void save_inform_phenotype_nearest(boost::filesystem::path const& directory, categories_type::value_type const& category, std::string const& interest)
{
	std::set<std::string> phenotypes;
	for(auto const& sample: category.second)
	{
		for(auto const& phenotype: sample.second.phenotypes)
		{
			phenotypes.insert(phenotype.first);
		}
	}

	for(auto const& phenotype: phenotypes)
	{
		if(phenotype != interest)
		{
			continue;
		}

		auto const destination(directory / category.first / ("phenotype_nearest_" + phenotype + ".csv"));
		std::cout << "Saving " << destination << std::endl;
		boost::filesystem::ofstream stream(destination, std::ios::trunc);

		stream << "sep=\t" << std::endl;
		stream << "Sample Name\tCell ID\tCell X Position\tCell Y Position";
		for(auto const& candidate_phenotype: phenotypes)
		{
			if(candidate_phenotype != phenotype)
			{
				stream 
					<< "\t" << candidate_phenotype << " Nearest Distance" 
					<< "\t" << candidate_phenotype << " Nearest Cell ID";
			}
		}
		stream << std::endl;

		for(auto const& sample: category.second)
		{
			if(sample.second.phenotypes.count(phenotype))
			{
				for(auto const& cell: sample.second.phenotypes.at(phenotype))
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
							if(sample.second.phenotypes.count(candidate_phenotype))
							{
								nearest(cell, sample.second.phenotypes.at(candidate_phenotype), nearest_distance, nearest_cell);
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


void save_inform_phenotype_summary(boost::filesystem::path const& directory, categories_type const& categories, std::string const& interest)
{
	std::set<std::string> phenotypes;
	for(auto const& category: categories)
	{
		for(auto const& sample: category.second)
		{
			for(auto const& phenotype: sample.second.phenotypes)
			{
				phenotypes.insert(phenotype.first);
			}
		}
	}

	auto const destination(directory / "phenotype_summary.csv");
	std::cout << "Saving " << destination << std::endl;
	boost::filesystem::ofstream stream(destination, std::ios::trunc);

	stream << "sep=\t" << std::endl;
	stream << "Sample Name\tTissue Category";
	for(auto const& phenotype: phenotypes)
	{
		stream << "\t" << phenotype << " Cells";
		stream << "\t" << phenotype << " Cells/Pixel";
	}
	for(auto const& phenotype: phenotypes)
	{
		if(phenotype != interest)
		{
			continue;
		}

		for(auto const& candidate_phenotype: phenotypes)
		{
			if(candidate_phenotype != phenotype)
			{
				stream << "\t" << phenotype << "/" << candidate_phenotype << " Nearest Mean";
				stream << "\t" << phenotype << "/" << candidate_phenotype << " Nearest Median";
				stream << "\t" << phenotype << "/" << candidate_phenotype << " Nearest Std. Dev.";
			}
		}
	}
	stream << std::endl;

	for(auto const& category: categories)
	{
		for(auto const& sample: category.second)
		{
			stream << sample.first << "\t" << category.first;
			for(auto const& phenotype: phenotypes)
			{
				std::size_t cells(0);
				if(sample.second.phenotypes.count(phenotype))
				{
					cells = sample.second.phenotypes.at(phenotype).size();
				}
				stream << "\t" << cells;
				stream << "\t" << "=" << cells << "/" << sample.second.area;
			}

			for(auto const& phenotype: phenotypes)
			{
				if(phenotype != interest)
				{
					continue;
				}

				for(auto const& candidate_phenotype: phenotypes)
				{
					if(candidate_phenotype != phenotype)
					{
						if(sample.second.phenotypes.count(phenotype) && sample.second.phenotypes.count(candidate_phenotype))
						{
							boost::accumulators::accumulator_set<
								double, 
								boost::accumulators::stats<
									boost::accumulators::tag::median,
									boost::accumulators::tag::lazy_variance							
								>
							> accumulator;
							for(auto const& cell: sample.second.phenotypes.at(phenotype))
							{
								double nearest_distance(std::numeric_limits<double>::max());
								cell_ptr_type nearest_cell;
								nearest(cell, sample.second.phenotypes.at(candidate_phenotype), nearest_distance, nearest_cell);
								accumulator(nearest_distance);
							}
							stream << "\t" << boost::io::group(std::fixed, std::setprecision(2), boost::accumulators::mean(accumulator));
							stream << "\t" << boost::io::group(std::fixed, std::setprecision(2), boost::accumulators::median(accumulator));
							stream << "\t" << boost::io::group(std::fixed, std::setprecision(2), std::sqrt(boost::accumulators::variance(accumulator)));
						}
						else
						{
							stream << "\t" << "\t" << "\t";
						}
					}
				}
			}

			stream << std::endl;
		}
	}
}
