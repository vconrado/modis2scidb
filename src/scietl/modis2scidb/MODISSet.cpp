#include "MODISSet.hpp"

modis2scidb::MODISSet::MODISSet(boost::filesystem::path& folderPath,
                                std::vector<uint16_t>  & bands_nums) :
  minV(MODIS_GRID_ROWS), maxV(0), minH(MODIS_GRID_COLS), maxH(0),
  referenceFile(NULL) {
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
        // create Modisfile
        MODISFile *modisFile = new MODISFile(cp);

        // save first file as referenceFile
        if (grid.empty()) {
          referenceFile = modisFile;
          bandNames     = referenceFile->getBands(bands_nums);
        }
        else {
          // check if the current file has the same data type as the
          // referenceFile
          if (referenceFile->getDataType().compare(
                modisFile->getDataType()) != 0) {
            delete modisFile;
            throw modis2scidb::invalid_data_type_error() <<
                  modis2scidb::error_description(
                    "invalid data type. Expecting " + referenceFile->getDataType() +
                    " !");
          }
        }

        // add file to the dataset
        add(modisFile);
      }
    }
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

  size_t doy   = modisFile->getDoy();
  size_t tileV = modisFile->getTileV();
  size_t tileH = modisFile->getTileH();
  it = grid.find(doy);

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
  // comprar MODISFiles (tamanho, bandas, etc)
  return true;
}

void modis2scidb::MODISSet::print() const {
  std::cout << "MODISSet:" << std::endl;
  std::cout << "Bouding Box: (V0,H0,Vf,Hf) = ("
            << minV << ", "
            << minH << ", "
            << maxV << ", "
            << maxH << ")" << std::endl;


  std::cout << "Bands: " << std::endl;

  for (size_t i = 0; i < bandNames.size(); ++i) {
    std::cout << "\t#" << i << ": " << bandNames[i] << std::endl;
  }
  std::cout << "Pixel Size: " << referenceFile->getPixelSize(bandNames) <<
    std::endl;

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
