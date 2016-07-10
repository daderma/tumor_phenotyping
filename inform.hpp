#pragma once


#include "categories.hpp"
#include <boost/filesystem.hpp>
#include <boost/exception/all.hpp>


struct load_exception : virtual std::exception, virtual boost::exception {};
typedef boost::error_info<struct load_detail_, std::string> load_detail_type;


// File formats and naming consistent with inForm
void load_inform_samples(boost::filesystem::path const& directory, categories_type& categories);
void save_inform_sample_nearest(boost::filesystem::path const& directory, categories_type::value_type const& category, std::string const& interest);
void save_inform_sample_nearest_composites(boost::filesystem::path const& directory, categories_type::value_type const& category, std::string const& interest);
void save_inform_sample_neighbor_composites(boost::filesystem::path const& directory, categories_type::value_type const& category, std::string const& interest);
void save_inform_phenotype_nearest(boost::filesystem::path const& directory, categories_type::value_type const& category, std::string const& interest);
void save_inform_phenotype_summary(boost::filesystem::path const& directory, categories_type const& categories, std::string const& interest);
