#ifndef __MODISFILE_HPP__
#define __MODISFILE_HPP__

#include <string>

class MODISFile {
public:

  MODISFile(const std::string& path);
  size_t getH() const;
  size_t getV() const;
  size_t getDOY() const;

private:

  size_t _h;
  size_t _v;
  size_t _doy;
};


#endif /*__MODISFILE_HPP__ */
