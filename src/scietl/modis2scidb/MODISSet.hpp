

#ifndef __MODIS2SCIDB_MODISSET_HPP__
#define __MODIS2SCIDB_MODISSET_HPP__

// STL
#include <iostream>
#include <string>
#include <map>
#include <vector>

// Modis2SciDB
#include "MODISFile.hpp"
#include "Types.hpp"

// Boost
#include "boost/filesystem.hpp"

namespace modis2scidb {
class MODISSet {
public:

  MODISSet(boost::filesystem::path& folderPath);
  ~MODISSet();
  void add(
    MODISFile *modisFile);
  void print() const;
  bool validateSet();

private:

  void addFiles(boost::filesystem::path& folderPath);
  std::map<uint32_t, std::vector<std::vector<modis2scidb::MODISFile *> > > grid;
};
} // end namespace modis2scidb


#endif /*__MODIS2SCIDB_MODISSET_HPP__ */
