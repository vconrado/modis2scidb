#ifndef __MODISFILE_HPP__
#define __MODISFILE_HPP__


// STL
#include <string>
#include <iostream>
#include <ostream>

// Modis2SciDB
#include "Utils.hpp"
#include "Types.hpp"
#include "Exception.hpp"
#include "TimeUtils.hpp"

// Boost
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

// GDAL
#include <gdal.h>

namespace modis2scidb {
class MODISFile {
public:

  MODISFile(boost::filesystem::path & path,
            std::vector<std::string>& band_names);
  ~MODISFile();

  void                               print() const;

  // TODO acessar diretamente os atributos
  modis2scidb::modis_file_descriptor getFileDescriptor() const;
  size_t                             getPixelSize() const;
  void                               read(char  *out,
                                          size_t offset,
                                          size_t len);

private:

  boost::filesystem::path path;
  modis2scidb::modis_file_descriptor fileDescriptor;
  std::vector<std::string>band_names;
  size_t pixelSize;
};
} // end namespace modis2scidb


#endif /*__MODISFILE_HPP__ */
