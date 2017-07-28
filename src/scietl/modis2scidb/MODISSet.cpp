#include "MODISSet.hpp"

modis2scidb::MODISSet::MODISSet(boost::filesystem::path& folderPath) :
  minV(MODIS_GRID_ROWS), maxV(0), minH(MODIS_GRID_COLS), maxH(0) {
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

  // updating min{V|H}
  if (tileV > maxV) {
    maxV = tileV;
  }

  if (tileV < minV) {
    minV = tileV;
  }

  if (tileH > maxH) {
    maxH = tileH;
  }

  if (tileH < minH) {
    minH = tileH;
  }
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
