#include "MODISFile.hpp"

modis2scidb::MODISFile::MODISFile(boost::filesystem::path& path) :
  path(path) {
  fileDescriptor = modis2scidb::parse_modis_file_name(
    path.filename().string());
}

modis2scidb::MODISFile::~MODISFile() {}

std::string modis2scidb::MODISFile::getDataType() const {
  return fileDescriptor.data_type_name;
}

size_t modis2scidb::MODISFile::getDoy() const {
  return fileDescriptor.doy;
}

size_t modis2scidb::MODISFile::getTileV() const {
  return fileDescriptor.tileV;
}

size_t modis2scidb::MODISFile::getTileH() const {
  return fileDescriptor.tileH;
}

size_t modis2scidb::MODISFile::read(const std::vector<std::string>& band_names,
                                    size_t                          offset,
                                    size_t                          size,
                                    char                           *out) {
  for (const auto& band_name : band_names) {
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
  return 0;
}

size_t modis2scidb::MODISFile::getPixelSize(
  const std::vector<std::string>& band_names)
const {
  size_t pixelSize = 0;

  for (const auto& band_name : band_names) {
    boost::format subdataset(band_name);
    subdataset.bind_arg(1, path.string());

    GDALDatasetH dataset = GDALOpen(subdataset.str().c_str(), GA_ReadOnly);

    if (dataset == 0) {
      boost::format err_msg(
        "could not open subdataset: '%1%', for input hdf file: '%2%'!");
      throw modis2scidb::gdal_error() <<
            modis2scidb::error_description((err_msg % subdataset.str() %
                                            path.string()).str());
    }

    GDALRasterBandH band = GDALGetRasterBand(dataset, 1);

    if (band == 0) {
      GDALClose(dataset);
      boost::format err_msg("could not access band: %1%!");
      throw modis2scidb::gdal_error() <<
            modis2scidb::error_description((err_msg % band_name).str());
    }
    GDALDataType pixel_type = GDALGetRasterDataType(band);

    pixelSize += modis2scidb::num_bytes(pixel_type);
    GDALClose(dataset);
  }
  return pixelSize;
}

std::vector<std::string>modis2scidb::MODISFile::getBands() const {
  return modis2scidb::extract_subdatasets_pattern_names(path.string());
}

std::vector<std::string>modis2scidb::MODISFile::getBands(
  std::vector<uint16_t>& indices) const {
  std::vector<std::string> selectedBandNames;
  std::vector<std::string> bandNames = getBands();

  size_t numBands = indices.size();

  for (std::size_t i = 0; i != numBands; ++i) {
    if (indices[i] < bandNames.size()) {
      selectedBandNames.push_back(bandNames[indices[i]]);
    }
    else {
      throw modis2scidb::invalid_arg_value() <<
            modis2scidb::error_description("band number is invalid!");
    }
  }
  return selectedBandNames;
}

void modis2scidb::MODISFile::print() const {
  std::cout << "File Name: " << path.filename().string() << std::endl
            << "Data Type: " << fileDescriptor.data_type_name << std::endl
            << "Acquisition Date: " << fileDescriptor.acquisition_date <<
    std::endl
            << "Year: " << fileDescriptor.year << std::endl
            << "DOY: " << fileDescriptor.doy << std::endl
            << "Tile: " << fileDescriptor.tile << std::endl
            << "TileH: " << fileDescriptor.tileH << std::endl
            << "TileV: " << fileDescriptor.tileV << std::endl
            << "Collection Version: " << fileDescriptor.collection_version <<
    std::endl
            << "Production Date: " << fileDescriptor.production_date << std::endl
            << "Band Names: " << std::endl;

  std::vector<std::string> band_names = getBands();

  for (size_t i = 0; i < band_names.size(); ++i) {
    std::cout << "\t#" << i << ": " << band_names[i] << std::endl;
  }
}
