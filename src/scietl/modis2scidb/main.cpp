
// Modis2SciDB
#include "ParamUtils.hpp"
#include "MODISSet.hpp"

// STL
#include <iostream>
#include <string>
#include <cstdint>

// Boost


void convert(modis2scidb::input_arguments& args);

int  main(int argc, char **argv) {
  modis2scidb::input_arguments parsed_args;

  try {
    handle_input_arguments(argc, argv, &parsed_args);

    if (parsed_args.verbose) {
      print_args(parsed_args);
    }

    valid_args(parsed_args);

    convert(parsed_args);
  } catch (const modis2scidb::exception& e) {
    std::cerr << "\n\nmodis2scidb finished with errors!\n";

    if (e.what() != 0) {
      const std::string *d = boost::get_error_info<modis2scidb::error_description>(
        e);
      std::cerr << "An unexpected error has occurried: " << "(" << *d << ")" <<
      std::endl;
    }
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void convert(modis2scidb::input_arguments& args) {
  modis2scidb::MODISSet modisSet(args.source_folder_name);
}
