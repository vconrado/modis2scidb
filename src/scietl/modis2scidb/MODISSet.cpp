#include "MODISSet.hpp"


modis2scidb::MODISSet::MODISSet(boost::filesystem::path& folderPath) {
  addFiles(folderPath);
}

void modis2scidb::MODISSet::addFiles(boost::filesystem::path& folderPath) {
  if (!boost::filesystem::is_directory(folderPath)) {
    throw modis2scidb::invalid_dir_error() << modis2scidb::error_description(
            "invalid dir path!");
  }

  boost::filesystem::recursive_directory_iterator end;

  for (boost::filesystem::recursive_directory_iterator i(folderPath); i != end;
       ++i) {
    boost::filesystem::path cp = (*i);

    if (boost::filesystem::is_regular_file(cp)) {
      try {
        if (std::string(".hdf").compare(cp.extension().string()) == 0) {
          MODISFile *modisFile = new MODISFile(cp);
          add(modisFile);
        }
      } catch (const modis2scidb::exception& e) {
        if (e.what() != 0) {
          const std::string *d =
            boost::get_error_info<modis2scidb::error_description>(
              e);
          std::cerr <<
          "An error has occurried while parsing modis file name: " <<
          "" << *d << "" <<
          std::endl << std::endl;
        }
      }
    }
  }
}

modis2scidb::MODISSet::~MODISSet() {
  // TODO delete grid and MODISFiles
}

// creating grid

void modis2scidb::MODISSet::add(MODISFile *modisFile) {
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
}

bool modis2scidb::MODISSet::validateSet() {
  // TODO Implementar
  // Verificar se é um cubo de dados sem falhas
  return true;
}

void modis2scidb::MODISSet::print() const {
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
