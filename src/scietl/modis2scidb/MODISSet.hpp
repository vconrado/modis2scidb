

#ifndef __MODIS2SCIDB_MODISSET_HPP__
#define __MODIS2SCIDB_MODISSET_HPP__

// STL
#include <iostream>
#include <string>
#include <map>

// Modis2SciDB
#include "MODISFile.hpp"

// Boost
#include "boost/filesystem.hpp"

namespace modis2scidb {
class MODISSet {
public:

  MODISSet();
  MODISSet(std::string& folderPath);
  bool add(const MODISFile& modisFile);
  bool isValid() const;
  void print() const;

private:

  // std::map<size_t, std::vector<std::vector> > > _grid;
};
}

#endif /*__MODIS2SCIDB_MODISSET_HPP__ */
