

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

  MODISSet(boost::filesystem::path& folderPath,
           std::vector<uint16_t>  & num_bands);
  ~MODISSet();
  void                                                             add(
    MODISFile *modisFile);
  void                                                             print() const;
  bool                                                             validateSet();
  std::vector<std::vector<std::vector<modis2scidb::MODISFile *> > >getCube();
  modis2scidb::MODISFile                                         * getMODISFile(
    size_t x,
    size_t y,
    size_t t);

private:

  void addFiles(boost::filesystem::path& folderPath,
                std::vector<uint16_t>  & num_bands);


  std::vector<std::vector<std::vector<modis2scidb::MODISFile *> > >cube;
  std::map<size_t, size_t>doyMap;
  std::vector<size_t>timeOrder;
  std::vector<std::string>bandNames;
  size_t read(size_t l,
              size_t c,
              char  *out);
  size_t minV, maxV;
  size_t minH, maxH;
  modis2scidb::MODISFile *referenceFile;
};
} // end namespace modis2scidb


#endif /*__MODIS2SCIDB_MODISSET_HPP__ */
