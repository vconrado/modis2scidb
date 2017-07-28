#include "MODISFile.hpp"

modis2scidb::MODISFile::MODISFile(boost::filesystem::path& path) : path(path) {
  fileDescriptor = modis2scidb::parse_modis_file_name(
    path.filename().string());
}

modis2scidb::modis_file_descriptor modis2scidb::MODISFile::getFileDescriptor() const{
  return fileDescriptor;
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
            << "Production Date: " << fileDescriptor.production_date << std::endl;
}
