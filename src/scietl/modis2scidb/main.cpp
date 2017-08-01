
// Modis2SciDB
#include "ParamUtils.hpp"
#include "MODISSet.hpp"

// STL
#include <iostream>
#include <string>
#include <cstdint>

// Boost


// GDAL
#include <gdal.h>

// Function prototypes
void convert(modis2scidb::input_arguments& args);


// Main
int main(int argc, char **argv) {
  GDALAllRegister();

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
      const std::string *d =
        boost::get_error_info<modis2scidb::error_description>(
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

  modis2scidb::MODISSet modisSet(folderPath, args.bands);

  if (!modisSet.validateSet()) {
    throw modis2scidb::invalid_dataset_error() << modis2scidb::error_description(
            "Invalid dataset!");
  }

  // std::vector<std::vector<std::vector<modis2scidb::MODISFile *> > > cube =
  //   modisSet.getCube();

  if (args.verbose) {
    modisSet.print();
  }


  std::cout <<
    modisSet.getMODISFile(48000, 38400,
                          0)->getFileName() << std::endl;
  std::cout <<
    modisSet.getMODISFile(48000, 38400,
                          1)->getFileName() << std::endl;
  std::cout <<
    modisSet.getMODISFile(48000, 38400,
                          2)->getFileName() << std::endl;

  std::cout <<
    modisSet.getMODISFile(48000, 38400 + 1,
                          0)->getFileName() << std::endl;
  std::cout <<
    modisSet.getMODISFile(48000, 38400 + 10,
                          1)->getFileName() << std::endl;
  std::cout <<
    modisSet.getMODISFile(48000, 38400 + 2000,
                          2)->getFileName() << std::endl;

  std::cout <<
    modisSet.getMODISFile(48000, 38400 + 4800,
                          0)->getFileName() << std::endl;
  std::cout <<
    modisSet.getMODISFile(48000, 38400 + 4800 + 1,
                          1)->getFileName() << std::endl;
  std::cout <<
    modisSet.getMODISFile(48000, 38400 + 4800 + 4800 - 1,
                          2)->getFileName() << std::endl;
}
