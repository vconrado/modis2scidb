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
      // check file extension
      if (std::string(".hdf").compare(cp.extension().string()) == 0) {
        // if is a symbolic link, get canonical  path
        if (boost::filesystem::is_symlink(cp)) {
          cp = boost::filesystem::canonical(cp);
        }

        // create Modisfile
        MODISFile *modisFile = new MODISFile(cp);

        // save first file as referenceFile
        if (cube.empty()) {
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
  size_t t_size = cube.size();

  for (size_t t = 0; t < t_size; ++t) {
    for (size_t row = 0; row < MODIS_GRID_ROWS; ++row) {
      for (size_t col = 0; col < MODIS_GRID_COLS; ++col) {
        if (cube[t][row][col] != NULL) {
          delete cube[t][row][col];
        }
      }
    }
  }
  cube.clear();
}

std::vector<std::vector<std::vector<modis2scidb::MODISFile *> > >modis2scidb::
MODISSet::getCube() {
  size_t nDoys;
  size_t vSize = maxV - minV + 1;
  size_t hSize = maxH - minH + 1;

  nDoys = cube.size();


  std::vector<std::vector<std::vector<modis2scidb::MODISFile *> > > cube(
    vSize, std::vector<std::vector<modis2scidb::MODISFile *> >(
      hSize, std::vector<modis2scidb::MODISFile *>(nDoys)));

  /*
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
   */

  return cube;
}

modis2scidb::MODISFile * modis2scidb::MODISSet::getMODISFile(
  size_t x,
  size_t y,
  size_t t) {
  modis2scidb::MODISFile *mFile = NULL;

  if (t >= timeOrder.size()) {
    std::cout << "Invalid time index." << std::endl;
    return NULL;
  }
  size_t tidx   = timeOrder[t];
  size_t colidx = x / MODIS_NCOLS;
  size_t rowidx = y / MODIS_NROWS;

  if ((colidx < minH) || (colidx >= maxH) || (rowidx < minV) ||
      (rowidx >= maxV)) {
    std::cout << "Invalid x or y index." << std::endl;
    return NULL;
  }
  size_t coloffset = x % MODIS_NCOLS;
  size_t rowoffset = y % MODIS_NROWS;

  std::cout << "(" << x << "," << y << "," << t << ") = (" << colidx << ", " <<
    rowidx << ", " << tidx <<
    ") + (" << coloffset << ", " << rowoffset << ", 0) " << std::endl;

  return cube[tidx][rowidx][colidx];

  return mFile;
}

void modis2scidb::
MODISSet::add(MODISFile *modisFile) {
  std::map<size_t, size_t>::iterator it;

  size_t doy   = modisFile->getDoy();
  size_t tileV = modisFile->getTileV();
  size_t tileH = modisFile->getTileH();
  it = doyMap.find(doy);

  if (it == doyMap.end()) {
    std::vector<std::vector<modis2scidb::MODISFile *> >
    matrix(MODIS_GRID_ROWS, std::vector<modis2scidb::MODISFile *>(MODIS_GRID_COLS,
                                                                  NULL));

    doyMap[doy]          = cube.size();
    matrix[tileV][tileH] = modisFile;
    cube.push_back(matrix);

    // update timeOrder
    timeOrder.clear();

    for (const auto& doy_pair : doyMap) {
      timeOrder.push_back(doy_pair.second);
    }
  }
  else {
    cube[it->second][tileV][tileH] = modisFile;
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

  for (const auto& doy_pair : doyMap) {
    size_t t = doy_pair.second;
    std::cout << "DOY: " << doy_pair.first << " (" << timeOrder[t] << "/" <<
      t << ")"
              <<
      std::endl;

    // size_t t_size = timeOrder.size();
    //
    // for (size_t tidx = 0; tidx < t_size; ++tidx) {
    //   std::cout << "TIDX " << tidx << " " << timeOrder[tidx] << std::endl;
    //   size_t t = timeOrder[tidx];

    for (size_t row = 0; row < MODIS_GRID_ROWS; ++row) {
      for (size_t col = 0; col < MODIS_GRID_COLS; ++col) {
        if (cube[t][row][col] == NULL) {
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
