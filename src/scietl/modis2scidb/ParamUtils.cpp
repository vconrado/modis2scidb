#include "ParamUtils.hpp"

void modis2scidb::handle_input_arguments(
  int    argc,
  char **argv,
  modis2scidb::input_arguments *
  parsed_args) {
  std::string bands_str;

  parsed_args->x_chunk_size = 0;
  parsed_args->y_chunk_size = 0;
  parsed_args->z_chunk_size = 0;

  boost::program_options::options_description options_all(
    "MODIS to SciDB's Multidimensional Array Conversion Options");
  options_all.add_options()

  //  ("version", "Prints MODIS to SciDB conversion tool version.\n")
    ("help,h", "Prints help message.\n")
    ("verbose,v",
    "Turns on verbose mode: some information about the conversion progress.\n")
    ("dir,d",
    boost::program_options::value<std::string>(&parsed_args->source_folder_name),
    "The source folder with a collection of HDF files to convert to SciDB's load format.\n")
    ("output,o",
    boost::program_options::value<std::string>(&parsed_args->target_file_name),
    "The target file to store the SciDB data file.\n")
    (",x", boost::program_options::value<uint32_t>(&parsed_args->x_chunk_size),
    "The chunk size in horizontal dimension.\n")
    (",y", boost::program_options::value<uint32_t>(&parsed_args->y_chunk_size),
    "The chunk size in vertical dimension.\n")
    (",t", boost::program_options::value<uint32_t>(&parsed_args->z_chunk_size),
    "The chunk size in time dimension.\n")
    ("bands,b", boost::program_options::value<std::string>(&bands_str),
    "A list of comma separated spectral bands to extract data, starting from 0: \"3,4\".\n");
  boost::program_options::variables_map options;
  boost::program_options::store(boost::program_options::parse_command_line(
                                  argc,
                                  argv,
                                  options_all),
                                options);
  boost::program_options::notify(options);

  if (options.count("help")) {
    std::cout << options_all << std::endl;

    exit(EXIT_SUCCESS);
  }
  parsed_args->verbose = options.count("verbose") ? true : false;

  if (bands_str.size()) {
    parsed_args->bands = modis2scidb::parse_bands(bands_str);
  }
}

void modis2scidb::valid_args(modis2scidb::input_arguments& args) {
  if (args.source_folder_name.empty()) {
    throw modis2scidb::invalid_arg_value() << modis2scidb::error_description(
            "missing source folder name!");
  }

  if (!boost::filesystem::exists(args.source_folder_name)) {
    throw modis2scidb::invalid_dir_error() << modis2scidb::error_description(
            "could not find source folder!");
  }

  if (args.target_file_name.empty()) {
    throw modis2scidb::invalid_arg_value() << modis2scidb::error_description(
            "missing output file name!");
  }

  if (boost::filesystem::exists(args.target_file_name)) {
    throw modis2scidb::invalid_arg_value() << modis2scidb::error_description(
            "can not overwrite an existing output file!");
  }

  if ((args.x_chunk_size < 1) ||
      ((args.y_chunk_size < 1) || (args.z_chunk_size < 1))) {
    throw modis2scidb::invalid_arg_value() << modis2scidb::error_description(
            "all the chunk dimensions must be a positive number!");
  }

  // TODO verificar as bandas pedidas, considerando o produto
}

void modis2scidb::print_args(modis2scidb::input_arguments args) {
  std::cout << "Source folder:\t" << args.source_folder_name << std::endl;
  std::cout << "Target file:\t" << args.target_file_name << std::endl;
  std::cout << "1st Chunk Size:\t" << args.x_chunk_size << std::endl;
  std::cout << "2st Chunk Size:\t" << args.y_chunk_size << std::endl;
  std::cout << "3rd Chunk Size:\t" << args.z_chunk_size << std::endl;
  std::cout << "Bands:\t";
  std::size_t num_bands = args.bands.size();

  for (std::size_t i = 0; i != num_bands; ++i) {
    std::cout << args.bands[i] << ", ";
  }
  std::cout << std::endl;
  std::cout << "Verbose:\t" << args.verbose << std::endl;
}
