#/* 
#Copyright: A. Zakai ('Kripken') <kripkensteiner@gmail.com> http://6thsenseless.blogspot.com
#
#2006-15-9
#
#This code is licensed under the terms of the GNU General Public License (GPL),
#version 2 or above; See /usr/share/common-licenses/GPL , or see
#http://www.fsf.org/licensing/licenses/gpl.html
#*/

print "============================"
print "Creating libtorrent for BP2P"
print "============================"

from distutils.core import setup, Extension
from glob import glob

BP2P_PATH = "../bp2p"
_include_dirs = ['./include', './include/libtorrent', BP2P_PATH + "/src", BP2P_PATH + "/src/betterp2p", '/usr/local/include/boost-1_36']
_libraries = ['boost_filesystem', 'boost_date_time',
              'boost_program_options', 'boost_regex',
              'boost_serialization', 'boost_thread',
              'boost_python', 'z', 'ssl', 'crypto',
              'pthread', 'bp2p']
_library_dirs = [BP2P_PATH + '/lib']
_sources = glob("src/*.c*") + glob("src/kademlia/*.c*") + glob("bindings/python/src/*.c*") + glob(BP2P_PATH + "/src/libtorrent_plugin/*.c*")
_compile_args = ['-DPACKAGE_NAME=\"libtorrent-rasterbar\"', '-DPACKAGE_TARNAME=\"libtorrent-rasterbar\"', '-DPACKAGE_VERSION=\"0.14.4\"', '-DPACKAGE_STRING=\"libtorrent-rasterbar\ 0.14.4\"', '-DPACKAGE_BUGREPORT=\"arvid@cs.umu.se\"', '-DPACKAGE=\"libtorrent-rasterbar\"', '-DVERSION=\"0.14.4\"', '-DSTDC_HEADERS=1', '-DHAVE_SYS_TYPES_H=1', '-DHAVE_SYS_STAT_H=1', '-DHAVE_STDLIB_H=1', '-DHAVE_STRING_H=1', '-DHAVE_MEMORY_H=1', '-DHAVE_STRINGS_H=1', '-DHAVE_INTTYPES_H=1', '-DHAVE_STDINT_H=1', '-DHAVE_UNISTD_H=1', '-DHAVE_DLFCN_H=1', '-D_FILE_OFFSET_BITS=64', '-DHAVE_PTHREAD=1', '-DHAVE_BOOST_SYSTEM=/\*\*/', '-DHAVE_BOOST_ASIO=/\*\*/', '-DHAVE_BOOST=/\*\*/', '-DHAVE_BOOST_FILESYSTEM=/\*\*/', '-DHAVE_BOOST_THREAD=/\*\*/', '-DHAVE_BOOST_REGEX=/\*\*/', '-DHAVE_BOOST_PROGRAM_OPTIONS=/\*\*/', '-DTORRENT_USE_OPENSSL=/\*\*/', '-DHAVE_LIBZ=1', '-DTORRENT_BUILDING_SHARED=/\*\*/']

libtorrent_module = Extension(
        'libtorrent',
        extra_compile_args = _compile_args,
        include_dirs = _include_dirs,
        libraries = _libraries,
        library_dirs = _library_dirs,
        sources = _sources); 

setup (name = 'libtorrent-bp2p',
       version = '1.3.6/0.0.1',
       description = 'C++ torrent library (Sourceforge, not Rakshasa), including extensions for BP2P',
       ext_modules = [libtorrent_module])
