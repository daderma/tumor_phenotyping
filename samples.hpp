#ifndef TUMOR_PHENOTYPING__SAMPLES_HPP
#define TUMOR_PHENOTYPING__SAMPLES_HPP


#include "phenotypes.hpp"
#include <boost/filesystem.hpp>
#include <boost/exception/all.hpp>


namespace samples
{


typedef std::map<std::string, phenotypes::phenotypes_type> samples_type;


struct load_exception : virtual std::exception, virtual boost::exception {};
typedef boost::error_info<struct load_detail_, std::string> load_detail_type;


// File formats and naming consistent with inForm
void load_inform_samples(boost::filesystem::path const& directory, samples_type& samples);
void save_inform_sample_nearest(boost::filesystem::path const& directory, samples_type const& samples);
void save_inform_sample_nearest_composites(boost::filesystem::path const& directory, samples_type const& samples);
void save_inform_sample_neighbor_composites(boost::filesystem::path const& directory, samples_type const& samples);
void save_inform_phenotype_nearest(boost::filesystem::path const& directory, samples_type const& samples);
void save_inform_phenotype_summary(boost::filesystem::path const& directory, samples_type const& samples);


}	// namespace samples


#endif	// TUMOR_PHENOTYPING__SAMPLES_HPP