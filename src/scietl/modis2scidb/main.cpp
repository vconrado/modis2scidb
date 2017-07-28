
// Modis2SciDB
#include "ParamUtils.hpp"
#include "MODISSet.hpp"

// STL
#include <iostream>
#include <string>
#include <cstdint>

// Boost


// Function prototypes
void convert(modis2scidb::input_arguments& args);


// Main
int main(int argc, char **argv) {
  modis2scidb::input_arguments parsed_args;

  try {
    handle_input_arguments(argc, argv, &parsed_args);

    if (parsed_args.verbose) {
      print_args(parsed_args);
    }

    valid_args(parsed_args);

    convert(parsed_args);
  } catch (const modis2scidb::exception& e) {
    std::cerr << std::endl << argv[0] << " finished with errors!" << std::endl;

    if (e.what() != 0) {
      const std::string *d = boost::get_error_info<modis2scidb::error_description>(
        e);
      std::cerr << "An unexpected error has occurried: " << "" << *d << "" <<
      std::endl << std::endl;
    }
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void convert(modis2scidb::input_arguments& args) {
  boost::filesystem::path folderPath(args.source_folder_name);
  modis2scidb::MODISSet   modisSet(folderPath);

  if (!modisSet.validateSet()) {
    throw modis2scidb::invalid_dataset_error() << modis2scidb::error_description(
            "Invalid dataset!");
  }

  if (args.verbose) {
    modisSet.print();
  }
}
