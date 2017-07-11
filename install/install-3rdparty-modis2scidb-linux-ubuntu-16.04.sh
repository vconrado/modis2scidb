#!/bin/bash
#
#   Copyright (C) 2017 National Institute For Space Research (INPE) - Brazil.
#
#  This file is part of modis2scidb.
#
#  modis2scidb is free software: you can
#  redistribute it and/or modify it under the terms of the
#  GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License,
#  or (at your option) any later version.
#
#  modis2scidb is distributed in the hope that
#  it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with modis2scidb. See LICENSE. If not, write to
#  e-sensing team at <esensning-team@dpi.inpe.br>.
#
#
#  Description: Install all required software for modis2scidb on Linux Ubuntu 16.04.
#
#  Author: Gilberto Ribeiro de Queiroz
#          Vitor Conrado Faria Gomes
#
#
#  Example:
#  $ MODIS2SCIDB_DEPENDENCIES_DIR="/home/gribeiro/MyLibs" ./install-3rdparty-modis2scidb-linux-ubuntu-16.04
#

echo "********************************************************************"
echo "* modis2scidb 3rd-party Libraries Installer for Linux Ubuntu 16.04 *"
echo "********************************************************************"
echo ""
sleep 1s

#
# Valid parameter val or abort script
#
function valid()
{
  if [ $1 -ne 0 ]; then
    echo $2
    echo ""
    exit
  fi
}


#
# Update Ubuntu install list
#
#sudo apt-get update
#valid $? "Error: could not update apt-get database list"



function check_and_install()
{
    PACKAGE=$1
    package_test=`dpkg -s ${PACKAGE} | grep Status`

    if [ "$package_test" != "Status: install ok installed" ]; then
      sudo  apt-get -y install ${PACKAGE}
      valid $? "Error: could not install ${PACKAGE}! Please, install ${PACKAGE}: sudo apt-get -y install ${PACKAGE}"
      echo "${PACKAGE} installed!"
    else
      echo "${PACKAGE} already installed!"
    fi

}

# pacotes python removidos
# python-setuptools python-pip python-dev python-numpy python-scipy python-matplotlib ipython ipython-notebook python-pandas python-sympy python-nose

for app in g++ zlib1g-dev swig libhdf4-dev autoconf gettext flex bison cmake cmake-qt-gui; do 
    check_and_install $app
done

#
# Check for scietl-3rdparty-linux-ubuntu-14.04.tar.gz
#
MODIS2SCIDB_3RDPARTY_PATH=$(realpath "./3rdparty-src")

if [ ! -f ./scietl-3rdparty-linux-ubuntu-14.04.tar.gz ]; then
  echo "Please, make sure to have scietl-3rdparty-linux-ubuntu-14.04.tar.gz in the current directory!"
  echo ""
  exit
fi

mkdir -p ${MODIS2SCIDB_3RDPARTY_PATH}
cd ${MODIS2SCIDB_3RDPARTY_PATH}

#
# Check installation dir
#
if [ "$MODIS2SCIDB_DEPENDENCIES_DIR" == "" ]; then
  MODIS2SCIDB_DEPENDENCIES_DIR="/opt/modis2scidb"
fi

export PATH="$PATH:$MODIS2SCIDB_DEPENDENCIES_DIR/bin"
export LD_LIBRARY_PATH="$PATH:$MODIS2SCIDB_DEPENDENCIES_DIR/lib"

echo "installing 3rd-party libraries to '$MODIS2SCIDB_DEPENDENCIES_DIR' ..."
echo ""
sleep 1s


#
# BZIP2
#
if [ ! -f "$MODIS2SCIDB_DEPENDENCIES_DIR/lib/libbz2.a" ]; then
  echo "installing bzip2..."
  echo ""
  sleep 1s
  
  if [ ! -f "bzip2-1.0.6.tar.gz" ]; then
    wget "http://www.bzip.org/1.0.6/bzip2-1.0.6.tar.gz" -O "bzip2-1.0.6.tar.gz"
    valid $? "Error: could download bzip2-1.0.6 dir!"
  fi
  
  if [ ! -d "bzip2-1.0.6" ]; then
    tar xzvf "bzip2-1.0.6.tar.gz"
    valid $? "Error: could uncompress bzip2-1.0.6 dir!"
  fi

  cd bzip2-1.0.6
  valid $? "Error: could not enter bzip2-1.0.6 dir!"

  make
  valid $? "Error: could not make BZIP2!"

  sudo make install PREFIX=$MODIS2SCIDB_DEPENDENCIES_DIR
  valid $? "Error: Could not install BZIP2!"

  cd ..
fi


#
# Proj4 version 4.9.1 (with proj-datumgrid version 1.5)
# Site: https://trac.osgeo.org/proj/
#
if [ ! -f "$MODIS2SCIDB_DEPENDENCIES_DIR/lib/libproj.so" ]; then
  echo "installing Proj4..."
  sleep 1s
#
  if [ ! -f "proj-4.9.1.tar.gz" ]; then
    wget "http://download.osgeo.org/proj/proj-4.9.1.tar.gz" -O "proj-4.9.1.tar.gz"
    valid $? "Error: could download proj-4.9.1.tar.gz!"
  fi
  
  if [ ! -d "proj-4.9.1" ]; then
    tar xzvf "proj-4.9.1.tar.gz"
    valid $? "Error: could uncompress proj-4.9.1.tar.gz!"
  fi

  cd proj-4.9.1
  valid $? "Error: could not enter proj-4.9.1 dir!"

  ./configure --prefix=$MODIS2SCIDB_DEPENDENCIES_DIR
  valid $? "Error: could not configure Proj4!"

  make -j 4
  valid $? "Error: could not make Proj4!"

  sudo make install
  valid $? "Error: Could not install Proj4!"

  cd ..
fi


#
# GEOS
#
if [ ! -f "$MODIS2SCIDB_DEPENDENCIES_DIR/lib/libgeos.so" ]; then
  echo "installing GEOS..."
  echo ""
  sleep 1s

  if [ ! -f "geos-3.4.2.tar.bz2" ]; then
    wget "http://download.osgeo.org/geos/geos-3.4.2.tar.bz2" -O "geos-3.4.2.tar.bz2"
    valid $? "Error: could download geos-3.4.2.tar.bz2!"
  fi

  if [ ! -d "eos-3.4.2" ]; then
    tar xjvf geos-3.4.2.tar.bz2 
    valid $? "Error: could not uncompress geos-3.4.2.tar.bz2!"
  fi

  cd geos-3.4.2
  valid $? "Error: could not enter geos-3.4.2 dir!"

  ./configure --prefix=$MODIS2SCIDB_DEPENDENCIES_DIR
  valid $? "Error: could not configure GEOS!"

  make -j 4
  valid $? "Error: could not make GEOS!"

  sudo make install
  valid $? "Error: Could not install GEOS!"

  cd ..
fi


#
# libPNG
#
if [ ! -f "$MODIS2SCIDB_DEPENDENCIES_DIR/lib/libpng.so" ]; then
  echo "installing libPNG..."
  echo ""
  sleep 1s

#

  if [ ! -f "libpng-1.5.17" ]; then
    wget "https://downloads.sourceforge.net/project/libpng/libpng15/older-releases/1.5.17/libpng-1.5.17.tar.gz?r=&ts=1498848884&use_mirror=ufpr" -O "libpng-1.5.17.tar.gz"
    valid $? "Error: could download libpng-1.5.17.tar.gz!"
  fi

  if [ ! -d "libpng-1.5.17" ]; then
    tar xzvf libpng-1.5.17.tar.gz
    valid $? "Error: could not uncompress libpng-1.5.17.tar.gz!"
  fi

  cd libpng-1.5.17
  valid $? "Error: could not enter libpng-1.5.17 dir!"

  ./configure --prefix=$MODIS2SCIDB_DEPENDENCIES_DIR
  valid $? "Error: could not configure libPNG!"

  make -j 4
  valid $? "Error: could not make libPNG!"

  sudo make install
 valid $? "Error: Could not install libPNG!"

  cd ..
fi


#
# Independent JPEG Group version v9a
# Site: http://www.ijg.org
#
if [ ! -f "$MODIS2SCIDB_DEPENDENCIES_DIR/lib/libjpeg.so" ]; then
  echo "installing Independent JPEG Group Library..."
  sleep 1s

  if [ ! -f "jpegsrc.v9a.tar.gz" ]; then
    wget "http://www.ijg.org/files/jpegsrc.v9a.tar.gz" -O "jpegsrc.v9a.tar.gz"
    valid $? "Error: could download jpegsrc.v9a.tar.gz!"
  fi

  if [ ! -d "jpegsrc.v9a" ]; then
    tar xzvf jpegsrc.v9a.tar.gz
    valid $? "Error: could not uncompress jpegsrc.v9a.tar.gz!"
  fi

  cd jpeg-9a
  valid $? "Error: could not enter jpeg-9a dir!"

  ./configure --prefix=$MODIS2SCIDB_DEPENDENCIES_DIR
  valid $? "Error: could not configure JPEG!"

  make -j 4
  valid $? "Error: could not make JPEG!"

  sudo make install
  valid $? "Error: Could not install JPEG!"

  cd ..
fi


#
# TIFF
#
if [ ! -f "$MODIS2SCIDB_DEPENDENCIES_DIR/lib/libtiff.so" ]; then
  echo "installing TIFF..."
  echo ""
  sleep 1s

 if [ ! -f "tiff-4.0.3.tar.gz" ]; then
    wget "http://download.osgeo.org/libtiff/old/tiff-4.0.3.tar.gz" -O "tiff-4.0.3.tar.gz"
    valid $? "Error: could download tiff-4.0.3.tar.gz!"
  fi

  if [ ! -d "tiff-4.0.3" ]; then
    tar xzvf tiff-4.0.3.tar.gz
    valid $? "Error: could not uncompress tiff-4.0.3.tar.gz!"
  fi

  cd tiff-4.0.3
  valid $? "Error: could not enter tiff-4.0.3!"

  ./configure --enable-cxx --with-jpeg-include-dir=$MODIS2SCIDB_DEPENDENCIES_DIR/include --with-jpeg-lib-dir=$MODIS2SCIDB_DEPENDENCIES_DIR/lib --prefix=$MODIS2SCIDB_DEPENDENCIES_DIR
  valid $? "Error: could not configure TIFF!"

  make -j 4
  valid $? "Error: could not make TIFF!"

  sudo make install
  valid $? "Error: Could not install TIFF!"

  cd ..
fi


#
# GeoTIFF
#
if [ ! -f "$MODIS2SCIDB_DEPENDENCIES_DIR/lib/libgeotiff.so" ]; then
  echo "installing GeoTIFF..."
  echo ""
  sleep 1s


  if [ ! -f "libgeotiff-1.4.0.tar.gz" ]; then
    wget "http://download.osgeo.org/geotiff/libgeotiff/libgeotiff-1.4.0.tar.gz" -O "libgeotiff-1.4.0.tar.gz"
    valid $? "Error: could download libgeotiff-1.4.0.tar.gz!"
  fi

  if [ ! -d "libgeotiff-1.4.0" ]; then
    tar xzvf libgeotiff-1.4.0.tar.gz
    valid $? "Error: could not uncompress libgeotiff-1.4.0.tar.gz!"
  fi

  cd libgeotiff-1.4.0
  valid $? "Error: could not enter libgeotiff-1.4.0!"

  ./configure --with-jpeg=$MODIS2SCIDB_DEPENDENCIES_DIR --with-zlib --with-libtiff=$MODIS2SCIDB_DEPENDENCIES_DIR --with-proj=$MODIS2SCIDB_DEPENDENCIES_DIR --prefix=$MODIS2SCIDB_DEPENDENCIES_DIR
  valid $? "Error: could not configure GeoTIFF!"

  make -j 4
  valid $? "Error: could not make GeoTIFF!"

  sudo make install
  valid $? "Error: Could not install GeoTIFF!"

  cd ..
fi

#
# ICU
#
if [ ! -f "$MODIS2SCIDB_DEPENDENCIES_DIR/lib/libicuuc.so" ]; then
  echo "installing ICU..."
  echo ""
  sleep 1s

  if [ ! -f "icu4c-52_1-src.tgz" ]; then
    wget "http://download.icu-project.org/files/icu4c/52.1/icu4c-52_1-src.tgz" -O "icu4c-52_1-src.tgz"
    valid $? "Error: could download icu4c-52_1-src.tgz!"
  fi

  if [ ! -d "icu/source" ]; then
    tar xzvf icu4c-52_1-src.tgz
    valid $? "Error: could not uncompress icu4c-52_1-src.tgz!"
  fi

  cd icu/source
  valid $? "Error: could not enter icu/source!"

  chmod +x runConfigureICU configure install-sh
  valid $? "Error: could not set runConfigureICU to execute mode!"

  CPPFLAGS="-DU_USING_ICU_NAMESPACE=0 -DU_CHARSET_IS_UTF8=1 -DU_NO_DEFAULT_INCLUDE_UTF_HEADERS=1" ./runConfigureICU Linux/gcc --prefix=$MODIS2SCIDB_DEPENDENCIES_DIR
  valid $? "Error: could not runConfigureICU!"

  make -j 4
  valid $? "Error: could not make ICU!"

  #make check
  #valid $? "Error: could not check ICU build!"

  sudo make install
  valid $? "Error: Could not install ICU!"

  cd ../..
fi


#
# Boost
#
if [ ! -f "$MODIS2SCIDB_DEPENDENCIES_DIR/lib/libboost_thread.so" ]; then
  echo "installing boost..."
  echo ""
  sleep 1s

  if [ ! -f "boost_1_64_0.tar.gz" ]; then
    wget "https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.gz" -O "boost_1_64_0.tar.gz"
    valid $? "Error: could download boost_1_64_0.tar.gz!"
  fi

  if [ ! -d "boost_1_64_0" ]; then
    tar xzvf boost_1_64_0.tar.gz
    valid $? "Error: could not uncompress boost_1_64_0.tar.gz!"
  fi

  cd boost_1_64_0
  valid $? "Error: could not enter boost_1_64_0!"


#--with-python=/home/vconrado/anaconda3/bin/python --with-python-root=/home/vconrado/anaconda3/
  ./bootstrap.sh --prefix=$MODIS2SCIDB_DEPENDENCIES_DIR --with-python=/usr/bin/python --with-python-root=/usr
  valid $? "Error: could not configure Boost!"
  
  export CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH:/usr/include/python2.7/"
  sudo ./b2 runtime-link=shared link=shared variant=release threading=multi --prefix=$MODIS2SCIDB_DEPENDENCIES_DIR -sICU_PATH=$MODIS2SCIDB_DEPENDENCIES_DIR -sICONV_PATH=/usr -sBZIP2_INCLUDE=$MODIS2SCIDB_DEPENDENCIES_DIR/include -sBZIP2_LIBPATH=$MODIS2SCIDB_DEPENDENCIES_DIR/lib install
  valid $? "Error: could not make boost"

  cd ..
fi
#
# HDF4
#
if [ ! -f "$MODIS2SCIDB_DEPENDENCIES_DIR/lib/libmfhdf.a" ]; then
  echo "installing HDF4..."
  echo ""
  sleep 1s
  
  if [ ! -f "hdf-4.2.9.tar.gz" ]; then
    wget "https://support.hdfgroup.org/ftp/HDF/releases/HDF4.2.9/src/hdf-4.2.9.tar.gz" -O "hdf-4.2.9.tar.gz"
    valid $? "Error: could download hdf-4.2.9.tar.gz!"
  fi

  if [ ! -d "hdf-4.2.9" ]; then
    tar xzvf hdf-4.2.9.tar.gz
    valid $? "Error: could not uncompress hdf-4.2.9.tar.gz!"
  fi

  cd hdf-4.2.9
  valid $? "Error: could not enter hdf-4.2.9!"

  CFLAGS=-fPIC ./configure --prefix=$MODIS2SCIDB_DEPENDENCIES_DIR --with-zlib --with-jpeg=$MODIS2SCIDB_DEPENDENCIES_DIR --enable-netcdf --disable-fortran
  valid $? "Error: could not configure hdf-4!"

  make -j 4
  valid $? "Error: could not make hdf-4"

  sudo make install
  valid $? "Error: Could not install hdf-4"

  cd ..
fi


#
# GDAL
#
if [ ! -f "$MODIS2SCIDB_DEPENDENCIES_DIR/lib/libgdal.so" ]; then
  echo "installing GDAL/OGR..."
  echo ""
  sleep 1s
  
  if [ ! -f "gdal-2.1.3.tar.gz" ]; then
    wget "http://download.osgeo.org/gdal/2.1.3/gdal-2.1.3.tar.gz" -O "gdal-2.1.3.tar.gz"
  fi
  
  if [ ! -d "gdal-2.1.3" ]; then
    tar xzvf gdal-2.1.3.tar.gz
    valid $? "Error: could not uncompress gdal-2.1.3.tar.gz!"
  fi

  cd gdal-2.1.3
  valid $? "Error: could not enter gdal-2.1.3.tar.gz!"

#  CXXFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0"
CXXFLAGS="-I$MODIS2SCIDB_DEPENDENCIES_DIR/include" LDFLAGS=-L$MODIS2SCIDB_DEPENDENCIES_DIR/lib ./configure --with-png=$MODIS2SCIDB_DEPENDENCIES_DIR --with-libtiff=$MODIS2SCIDB_DEPENDENCIES_DIR --with-geotiff=$MODIS2SCIDB_DEPENDENCIES_DIR --with-jpeg=$MODIS2SCIDB_DEPENDENCIES_DIR  --with-gif --with-ecw=yes --with-expat=yes --with-geos=$MODIS2SCIDB_DEPENDENCIES_DIR/bin/geos-config --with-threads --without-python --prefix=$MODIS2SCIDB_DEPENDENCIES_DIR --with-hdf4=$MODIS2SCIDB_DEPENDENCIES_DIR --without-netcdf 
  valid $? "Error: could not configure gdal!"

  make -j 4 -s
  valid $? "Error: could not make gdal"

  sudo make install
  valid $? "Error: Could not install gdal"

  cd ..
fi

#
# pydev
#
if [ ! -d "/usr/local/lib/python2.7/dist-packages/pyhdf-0.8.3-py2.7-linux-x86_64.egg" ]; then
  echo "installing pyhdf..."
  echo ""
  sleep 1s

  if [ ! -f "pyhdf-0.8.3.tar.gz" ]; then
    wget "https://downloads.sourceforge.net/project/pysclint/pyhdf/0.8.3/pyhdf-0.8.3.tar.gz?r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Fpysclint%2Ffiles%2Fpyhdf%2F0.8.3%2F&ts=1498849109&use_mirror=ufpr" -O "pyhdf-0.8.3.tar.gz"
  fi
  
  if [ ! -d "pyhdf-0.8.3/pyhdf" ]; then
    tar xzvf pyhdf-0.8.3.tar.gz
    valid $? "Error: could not uncompress pyhdf-0.8.3.tar.gz!"
  fi

  cd pyhdf-0.8.3/pyhdf
  valid $? "Error: could not enter pyhdf-0.8.3/pyhdf!"

  swig -python hdfext.i
  valid $? "Error: could not use swig for pyhdf!"
  
  cd ..
  valid $? "Error: could not enter pyhdf-0.8.3!"

  sudo INCLUDE_DIRS=/usr/include/hdf LIBRARY_DIRS=/usr/lib NOSZIP=1 python setup.py install
  valid $? "Error: could not configure gdal!"

  cd ..
fi


# Generating source.sh para o cmake
echo "export BOOST_ROOT=${MODIS2SCIDB_DEPENDENCIES_DIR}" >> ../build/cmake/source.sh
echo "export BOOST_INCLUDEDIR=${MODIS2SCIDB_DEPENDENCIES_DIR}/include" >> ../build/cmake/source.sh
echo "export GDAL_DIR=${MODIS2SCIDB_DEPENDENCIES_DIR}" >> ../build/cmake/source.sh

# Apagando arquivos temporarios
rm -rf ${MODIS2SCIDB_3RDPARTY_PATH}

#
# Finished!
#
clear
echo "********************************************************************"
echo "* modis2scidb 3rd-party Libraries Installer for Linux Ubuntu 16.04 *"
echo "********************************************************************"
echo ""
echo "finished successfully!"

