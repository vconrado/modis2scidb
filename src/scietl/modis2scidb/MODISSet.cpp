#include "MODISSet.hpp"

modis2scidb::MODISSet::MODISSet(boost::filesystem::path& folderPath,
                                std::vector<uint16_t>  & bands_nums) :
  minV(MODIS_GRID_ROWS), maxV(0), minH(MODIS_GRID_COLS), maxH(0),
  defaultReferenceDataType("") {
  addFiles(folderPath, bands_nums);
}

void modis2scidb::MODISSet::addFiles(boost::filesystem::path& folderPath,
                                     std::vector<uint16_t>  & bands_nums) {
  if (!boost::filesystem::is_directory(folderPath)) {
    throw modis2scidb::invalid_dir_error() << modis2scidb::error_description(
            "invalid dir path!");
  }

  boost::filesystem::recursive_directory_iterator end;

  for (boost::filesystem::recursive_directory_iterator it(folderPath); it != end;
       ++it) {
    boost::filesystem::path cp = (*it);

    if (boost::filesystem::is_regular_file(cp)) {
      // if is a symbolic link, get canonical  path
      if (boost::filesystem::is_symlink(cp)) {
        cp = boost::filesystem::canonical(cp);
      }

      // check file extension
      if (std::string(".hdf").compare(cp.extension().string()) == 0) {
        // get informations from the first file
        if (grid.empty()) {
          extractDefaultInformationFromFile(cp, bands_nums);
        }

        // create Modisfile
        MODISFile *modisFile = new MODISFile(cp, band_names);

        // check if the current file has the same data type as the first one
        if (defaultReferenceDataType.compare(modisFile->getFileDescriptor().
                                             data_type_name) != 0) {
          delete modisFile;
          throw modis2scidb::invalid_data_type_error() <<
                modis2scidb::error_description(
                  "invalid data type. Expecting " + defaultReferenceDataType +
                  " !");
        }

        // add file to the dataset
        add(modisFile);
      }
    }
  }
}

void modis2scidb::MODISSet::extractDefaultInformationFromFile(
  boost::filesystem::path& file, std::vector<uint16_t>& bands_nums) {
  band_names.clear();

  // 1. Getting the names of selected bands
  // 1.1 Getting bands names by bands numbers
  std::vector<std::string> all_band_names =
    modis2scidb::extract_subdatasets_pattern_names(file.string());
  std::size_t num_bands = bands_nums.size();

  // 1.2 Getting bands names and calculating pixel size from
  // selected bands
  for (std::size_t i = 0; i != num_bands; ++i) {
    if (bands_nums[i] >= all_band_names.size()) {
      throw modis2scidb::invalid_arg_value() <<
            modis2scidb::error_description("band number is invalid!");
    }
    else {
      band_names.push_back(all_band_names[bands_nums[i]]);
      std::cout << "Band: " << all_band_names[bands_nums[i]] << std::endl;
    }
  }

  // 2. Getting DataTypeName from fileName
  modis2scidb::modis_file_descriptor mfd = modis2scidb::parse_modis_file_name(
    file.filename().string());
  defaultReferenceDataType = mfd.data_type_name;

  // 3. Getting pixelSize

  boost::format subdataset(band_name);

  subdataset.bind_arg(1, path.string());

  std::cout << std::endl << "#### Band Path: " << subdataset.str() << std::endl;
  GDALDatasetH dataset = GDALOpen(subdataset.str().c_str(), GA_ReadOnly);

  if (dataset == 0) {
    boost::format err_msg(
      "could not open subdataset: '%1%', for input hdf file: '%2%'!");
    throw modis2scidb::gdal_error() <<
          modis2scidb::error_description((err_msg % subdataset.str() %
                                          path.string()).str());
  }
  else {
    std::cout << "Opened dataset " << std::endl;
  }

  GDALRasterBandH band = GDALGetRasterBand(dataset, 1);

  if (band == 0) {
    GDALClose(dataset);
    boost::format err_msg("could not access band: %1%!");
    throw modis2scidb::gdal_error() <<
          modis2scidb::error_description((err_msg % band_name).str());
  }
  else {
    std::cout << "Opened band " << GDALGetRasterBandXSize(band) << " " <<
      GDALGetRasterBandYSize(band) << std::endl;
  }
  GDALClose(dataset);
  }

}

modis2scidb::MODISSet::~MODISSet() {
  for (const auto& matrix_pair : grid) {
    for (size_t row = 0; row < MODIS_GRID_ROWS; ++row) {
      for (size_t col = 0; col < MODIS_GRID_COLS; ++col) {
        if (matrix_pair.second[row][col] != NULL) {
          delete matrix_pair.second[row][col];
        }
      }
    }
  }
  grid.clear();
}

std::vector<std::vector<std::vector<modis2scidb::MODISFile *> > >modis2scidb::
MODISSet::getCube() {
  size_t nDoys;
  size_t vSize = maxV - minV + 1;
  size_t hSize = maxH - minH + 1;

  nDoys = grid.size();


  std::vector<std::vector<std::vector<modis2scidb::MODISFile *> > > cube(
    vSize, std::vector<std::vector<modis2scidb::MODISFile *> >(
      hSize, std::vector<modis2scidb::MODISFile *>(nDoys)));

  if (nDoys == 0) {
    return cube;
  }

  // std::cout << "VSize: " << cube.size() << std::endl;
  // std::cout << "HSize: " << cube[0].size() << std::endl;
  // std::cout << "DoySize: " << cube[0][0].size() << std::endl;
  size_t t = 0;

  for (const auto& matrix_pair : grid) {
    std::vector<std::vector<modis2scidb::MODISFile *> > matrix =
      matrix_pair.second;

    for (size_t v = minV; v <= maxV; ++v) {
      for (size_t h = minH; h <= maxH; ++h) {
        cube[v - minV][h - minH][t] = matrix[v][h];
      }
    }
    ++t;
  }

  return cube;
}

void modis2scidb::
MODISSet::add(MODISFile *modisFile) {
  std::map<uint32_t,
           std::vector<std::vector<modis2scidb::MODISFile *> > >::iterator it;

  it = grid.find(modisFile->getFileDescriptor().doy);
  size_t tileV = modisFile->getFileDescriptor().tileV;
  size_t tileH = modisFile->getFileDescriptor().tileH;
  size_t doy   = modisFile->getFileDescriptor().doy;

  if (it == grid.end()) {
    std::vector<std::vector<modis2scidb::MODISFile *> >
    matrix(MODIS_GRID_ROWS, std::vector<modis2scidb::MODISFile *>(MODIS_GRID_COLS,
                                                                  NULL));

    // TODO remover codigo duplicado
    matrix[tileV][tileH] = modisFile;
    grid[doy]            =  matrix;
  }
  else {
    std::vector<std::vector<modis2scidb::MODISFile *> > matrix = it->second;
    matrix[tileV][tileH] = modisFile;
    grid[doy]            =  matrix;
  }


  // modisFile->print();

  // updating min{V|H}
  maxV = (tileV > maxV) ? tileV : maxV;
  minV = (tileV < minV) ? tileV : minV;
  maxH = (tileH > maxH) ? tileH : maxH;
  minH = (tileH < minH) ? tileH : minH;
}

bool modis2scidb::MODISSet::validateSet() {
  // TODO Implementar
  // Verificar se é um cubo de dados sem falhas
  return true;
}

void modis2scidb::MODISSet::print() const {
  std::cout << "MODISSet:" << std::endl;
  std::cout << "Bouding Box: (V0,H0,Vf,Hf) = ("
            << minV << ", "
            << minH << ", "
            << maxV << ", "
            << maxH << ")" << std::endl;

  for (const auto& matrix_pair : grid) {
    std::cout << "DOY: " << matrix_pair.first << std::endl;

    for (size_t row = 0; row < MODIS_GRID_ROWS; ++row) {
      for (size_t col = 0; col < MODIS_GRID_COLS; ++col) {
        if (matrix_pair.second[row][col] == NULL) {
          std::cout << "* ";
        }
        else {
          std::cout << "# ";
        }
      }
      std::cout << std::endl;
    }
  }
}
