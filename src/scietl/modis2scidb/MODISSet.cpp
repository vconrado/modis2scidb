#include "MODISSet.hpp"

modis2scidb::MODISSet::MODISSet() {}

modis2scidb::MODISSet::MODISSet(std::string& folderPath) {
  if (!folderPath.empty()) {
    boost::filesystem::path fpath(folderPath);
    boost::filesystem::recursive_directory_iterator end;

    for (boost::filesystem::recursive_directory_iterator i(fpath); i != end;
         ++i) {
      const boost::filesystem::path cp = (*i);
      std::cout << cp.string() << std::endl;
    }
  }
}
