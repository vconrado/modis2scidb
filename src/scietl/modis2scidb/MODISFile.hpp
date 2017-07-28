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

// Boost
#include "boost/filesystem.hpp"

namespace modis2scidb {
class MODISFile {
public:

  MODISFile(boost::filesystem::path& path);

  void                               print() const;

  // TODO acessar diretamente os atributos
  modis2scidb::modis_file_descriptor getFileDescriptor() const;

  // size_t                             getH() const;
  // size_t                             getV() const;
  // size_t                             getDOY() const;

private:

  boost::filesystem::path path;

  modis2scidb::modis_file_descriptor fileDescriptor;
};
} // end namespace modis2scidb


#endif /*__MODISFILE_HPP__ */
