#include "MODISFile.hpp"

modis2scidb::MODISFile::MODISFile(boost::filesystem::path & path,
                                  std::vector<std::string>& band_names) :
  path(path),
  band_names(band_names),
  pixelSize(0) {
  fileDescriptor = modis2scidb::parse_modis_file_name(
    path.filename().string());
}

modis2scidb::MODISFile::~MODISFile() {}

modis2scidb::modis_file_descriptor modis2scidb::MODISFile::getFileDescriptor()
const {
  return fileDescriptor;
}

void modis2scidb::MODISFile::read(char *out, size_t offset, size_t size) {
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
}

size_t modis2scidb::MODISFile::getPixelSize() const {
  if (pixelSize == 0) {
    // calculate pixelSize
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
  }
  return pixelSize;
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

  for (size_t i = 0; i < band_names.size(); ++i) {
    std::cout << "Band: " << band_names[i] << std::endl;
  }
}
