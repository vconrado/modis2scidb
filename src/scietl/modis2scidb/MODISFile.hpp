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

  MODISFile(boost::filesystem::path& path);
  ~MODISFile();

  void                    print() const;
  std::string             getDataType() const;
  size_t                  getDoy() const;
  size_t                  getTileV() const;
  size_t                  getTileH() const;
  size_t                  getPixelSize(
    const std::vector<std::string>& band_names) const;
  std::vector<std::string>getBands() const;
  std::vector<std::string>getBands(std::vector<uint16_t>& indices)
  const;
  size_t                  read(const std::vector<std::string>& band_names,
                               size_t                          offset,
                               size_t                          len,
                               char                           *out);

private:

  boost::filesystem::path path;
  modis2scidb::modis_file_descriptor fileDescriptor;
};
} // end namespace modis2scidb


#endif /*__MODISFILE_HPP__ */
