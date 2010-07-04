
|make-source-files| = ${foreach f,$2,$1/$(f)}
|make-objects| = ${foreach f,${patsubst %.cc,%.o,$1},${OBJDIR}/${f}}
|filter-so|	= ${filter-out %.$(SO_EXT),$1}

UNAME = ${shell uname -s}

CXX = g++-4.0
LDSHARED = g++-4.0 -shared
CP = cp -f

INCLUDE_PATH = src/betterp2p
EXT_PATH = ext
BOOST_INCLUDE_PATH ?= -I/usr/local/include/boost-1_36 -I/usr/include/boost
INCLUDES = -I${INCLUDE_PATH} -I/opt/local/include ${BOOST_INCLUDE_PATH}
CXXFLAGS = $(INCLUDES) -g -O0 -Wall

LDFLAGS = -L$(LIBDIR) -L/opt/local/lib
BOOST_LIBS = -lboost_filesystem-mt -lboost_serialization-mt -lboost_system-mt -lboost_thread-mt
LOG4CPP_LIBS = -llog4cpp
LDFLAGS += $(BOOST_LIBS) $(LOG4CPP_LIBS)

TARGET = bit2
SO_EXT = so
ifeq ($(UNAME),Darwin)
	LDSHARED = g++ -dynamiclib
	SO_EXT = dylib
	BOOST_LIBS = -lboost_filesystem-mt -lboost_serialization-mt -lboost_system-mt -lboost_thread-mt
endif
GRIFFIN_SCRIPT ?= griffin
WITH_ROBIN ?= 1

ifneq ($(WITH_ROBIN),0)
	ROBIN_FILES = 4robin.o
endif

SRCDIR = .
OBJDIR = obj
LIBDIR = lib
#INSTALL_PATH = /usr/local
INSTALL_PATH = ./install_pkg

MODULES = COMMON FILESYSTEM ALGORITHM CONTROL
BP2P_LIB = libbp2p.$(SO_EXT)
BP2P_OUTLIB = $(LIBDIR)/$(BP2P_LIB)

# --- COMMON --- #
COMMON_DIR = src/betterp2p/common
COMMON_SRC = MetaData.cc Logger.cc utility.cc BitVector.cc
COMMON_H = ${patsubst %.cc,%.h,$(COMMON_SRC)}

COMMON_OBJ = ${call |make-objects|,${call |make-source-files|,$(COMMON_DIR),$(COMMON_SRC)}}
COMMON_OUTLIB = $(LIBDIR)/$(COMMON_LIB)

# --- FILESYSTEM --- #
FILESYSTEM_DIR = src/betterp2p/filesystem
FILESYSTEM_SRC = Chunk.cc MetaFile.cc FileObject.cc
FILESYSTEM_H = ${patsubst %.cc,%.h,$(FILESYSTEM_SRC)}

FILESYSTEM_OBJ = ${call |make-objects|,${call |make-source-files|,$(FILESYSTEM_DIR),$(FILESYSTEM_SRC)}}
FILESYSTEM_OUTLIB = $(LIBDIR)/$(FILESYSTEM_LIB)

# --- ALGORITHM --- #
ALGORITHM_DIR = src/betterp2p/algorithm
ALGORITHM_SRC = PartialAlgorithm.cc
ALGORITHM_H = ${patsubst %.cc,%.h,$(ALGORITHM_SRC)}

ALGORITHM_OBJ = ${call |make-objects|,${call |make-source-files|,$(ALGORITHM_DIR),$(ALGORITHM_SRC)}}
ALGORITHM_OUTLIB = $(LIBDIR)/$(ALGORITHM_LIB)

# --- CONTROL --- #
CONTROL_DIR = src/betterp2p/control
CONTROL_SRC = FileHandler.cc
CONTROL_H = ${patsubst %.cc,%.h,$(CONTROL_SRC)}

CONTROL_OBJ = ${call |make-objects|,${call |make-source-files|,$(CONTROL_DIR),$(CONTROL_SRC)}}
CONTROL_OUTLIB = $(LIBDIR)/$(CONTROL_LIB)

# --- LIBTORRENT + PLUGIN --- #
LIBTORRENT_DIR = ..
LIBTORRENT_INCLUDE = $(LIBTORRENT_DIR)/include/libtorrent
LIBTORRENT_LIBDIR = $(LIBTORRENT_DIR)/build/lib.*
LIBTORRENT_LIB = libtorrent.$(SO_EXT)
LIBTORRENT_OUTLIB = $(LIBDIR)/$(LIBTORRENT_LIB)

# --- SMOKETEST --- #
SMOKETEST_DIR = src/tests/smoke
SMOKETEST_SRC = test.cc
SMOKETEST_H = ${patsubst %.cc,%.h,$(SMOKETEST_SRC)}

SMOKETEST_OBJ = ${call |make-objects|,${call |make-source-files|,$(SMOKETEST_DIR),$(SMOKETEST_SRC)}}


# --- BUILD TARGETS --- #
#$(LIBDIR)/$(TARGET) : $(BP2P_OUTLIB) $(LIBTORRENT_OUTLIB) $(SMOKETEST_OBJ)
#	$(CXX) $(LDFLAGS) -lbp2p -o $@ $(SMOKETEST_OBJ)

$(LIBTORRENT_OUTLIB): $(BP2P_OUTLIB)
	(cd $(LIBTORRENT_DIR); CC=$(CXX) python setup.py build)
	$(CP) $(LIBTORRENT_LIBDIR)/$(LIBTORRENT_LIB) $(LIBDIR)

$(BP2P_OUTLIB): ${foreach m, $(MODULES), $($(m)_OBJ)} $(ROBIN_FILES)
	mkdir -p $(LIBDIR)
	$(LDSHARED) $(LDFLAGS) -o $@ ${call |filter-so|,$^}

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	mkdir -p ${dir $@}
	$(CXX) $(CXXFLAGS) -c $< -o $@

4robin.cc: 4robin_include_list 
	$(GRIFFIN_SCRIPT) --include $(INCLUDES) --in `cat 4robin_include_list \
	    | sed "s/4ROBIN_DEPS://" | sed 's/\\\\//' | grep -v "boost"` --out ./4robin.cc

4robin_include_list: ${call |make-source-files|,$(ALGORITHM_DIR),$(ALGORITHM_H)} ${call |make-source-files|,$(FILESYSTEM_DIR),$(FILESYSTEM_H)} ${call |make-source-files|,$(CONTROL_DIR),$(CONTROL_H)}
	$(CXX) $(CXXFLAGS) -MM -MT 4ROBIN_DEPS $^ -o $@
	echo " $^" >> $@

.PHONY : libtorrent install clean test
libtorrent:
	(cd $(LIBTORRENT_DIR); CC=$(CXX) python setup.py build)
	$(CP) $(LIBTORRENT_LIBDIR)/$(LIBTORRENT_LIB) $(LIBDIR)

install: $(BP2P_OUTLIB) $(LIBTORRENT_OUTLIB)
	$(CP) $(BP2P_OUTLIB) $(INSTALL_PATH)/lib
	$(CP) $(LIBTORRENT_OUTLIB) $(INSTALL_PATH)/lib
	$(CP) -r $(LIBTORRENT_INCLUDE) $(INSTALL_PATH)/include

clean:
	rm -rf $(LIBDIR)/$(TARGET)
	rm -rf $(BP2P_OUTLIB)
	rm -rf ${foreach m,$(MODULES),$($(m)_OBJ)}
	rm -rf 4robin.cc 4robin.o xml/ module.tag 4robin_include_list
	rm -rf $(LIBTORRENT_OUTLIB)
	(cd $(LIBTORRENT_DIR); python setup.py clean)
	rm -rf $(LIBTORRENT_LIBDIR)

TEST_TEMP_DIR ?= test_resources
test:
	mkdir -p ${TEST_TEMP_DIR}
	dd if=/dev/urandom of=${TEST_TEMP_DIR}/file.rnd bs=102400 count=26
	$(LIBDIR)/bit2
	rm -f ${TEST_TEMP_DIR}/file.rnd ${TEST_TEMP_DIR}/file.rnd.meta
	rm -f ${TEST_TEMP_DIR}/file_new.rnd ${TEST_TEMP_DIR}/file_new.rnd.meta
	rm -f ${TEST_TEMP_DIR}/file_new.rnd.partial ${TEST_TEMP_DIR}/file_new.rnd.partial.meta
	rm -rf ${TEST_TEMP_DIR}

