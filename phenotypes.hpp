#ifndef TUMOR_PHENOTYPING__PHENOTYPES_HPP
#define TUMOR_PHENOTYPING__PHENOTYPES_HPP


#include "cells.hpp"
#include <map>


namespace phenotypes
{


typedef std::map<std::string, cells::cells_type> phenotypes_type;


}	// namespace phenotypes


#endif	// TUMOR_PHENOTYPING__PHENOTYPES_HPP