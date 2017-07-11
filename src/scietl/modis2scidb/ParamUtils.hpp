
#ifndef __MODIS2SCIDB_PARAM_UTILS_HPP__
#define __MODIS2SCIDB_PARAM_UTILS_HPP__

// STL
#include <iostream>

// Modis2SciDB
#include "Exception.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// Boost
#include <boost/program_options.hpp>

namespace modis2scidb {
// ! The list of possible input parameters
struct input_arguments {
  std::string          source_folder_name;
  std::string          target_file_name;
  uint32_t             x_chunk_size;
  uint32_t             y_chunk_size;
  uint32_t             z_chunk_size;
  std::vector<uint16_t>bands;
  bool                 verbose;
};

void handle_input_arguments(int                           argc,
                            char                        **argv,
                            modis2scidb::input_arguments *parsed_args);

void valid_args(input_arguments& args);

void print_args(modis2scidb::input_arguments args);
}
#endif /* __MODIS2SCIDB_PARAM_UTILS_HPP__*/
