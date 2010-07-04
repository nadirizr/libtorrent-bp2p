// std::vector< ::BT2::BitVector >: location specification may be missing?
#include "src/betterp2p/common/MetaData.h"
#include "src/betterp2p/control/FileHandler.h"
#include "src/betterp2p/algorithm/PartialAlgorithm.h"
#include "src/betterp2p/filesystem/Chunk.h"
#include "src/betterp2p/common/BitVector.h"
#include "src/betterp2p/filesystem/FileObject.h"
#include "src/betterp2p/filesystem/MetaFile.h"
#include <memory>
struct RegData
{
	const char *name;
	const char *type;
	const RegData *prototype;
	void *sym;
};

struct PascalString
{
	unsigned long size;
	const char *chars;
	char buffer[1];
};

#ifdef SPECIAL_CONVERSION_OPERATION
# define CONVOP(type,self) static_cast<type>(*self)
#else
# define CONVOP(type,self) self->operator type()
#endif

#ifdef _WINDLL
# define EXPORT __declspec(dllexport)
#else
# define EXPORT
#endif

#if defined(__GNUC__) && defined(__i386__)
# define __CDECL __attribute__((cdecl))
#elif defined(_WIN32)
# define __CDECL __cdecl
#else
# define __CDECL
#endif

typedef void* basic_block;
typedef void* scripting_element;

typedef void* xfer_float;

template < typename TO, typename FROM > TO union_cast(FROM v) { union { FROM f; TO t; } u; u.f = v; return u.t; }

extern bool (*__robin_callback)(scripting_element twin, RegData *signature, basic_block args[], basic_block *result, bool isPure);
bool (*__robin_callback)(scripting_element twin, RegData *signature, basic_block args[], basic_block *result, bool isPure) = 0;


namespace Robin_4a39201f {

template <class T>
struct SameClass {
	static T same(T i) { return i; }
};

long long *touchup(long long val)
{
	return new long long(val);
}
long long touchdown(long long* val)
{
	return *std::auto_ptr<long long>(val);
}

const double *touchup(double val)
{
	return new double(val);
}
double touchdown(const double* val)
{
	return *std::auto_ptr<const double>(val);
}

unsigned long long *touchup(unsigned long long val)
{
	return new unsigned long long(val);
}
unsigned long long touchdown(unsigned long long* val)
{
	return *std::auto_ptr<unsigned long long>(val);
}

const void *touchup(float val)
{
	return union_cast<void*>(val);
}
float touchdown(const void* val)
{
	return union_cast<float>(val);
}

BT2::BitVector* __CDECL ctor_1() { return new BT2::BitVector; }
void __CDECL dtor_1(BT2::BitVector *self) { delete self; }
/*
 * ::BT2::FileHandler::FileHandler
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:30
 */
::BT2::FileHandler* __CDECL routine_2r1(const std::string &arg0)
{
	return new ::BT2::FileHandler(arg0);
}
RegData routine_2r1_proto[] = {
	{"diskFileName", "&std::string", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileHandler::FileHandler
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:37
 */
::BT2::FileHandler* __CDECL routine_3r1(const ::BT2::MetaData &arg0)
{
	return new ::BT2::FileHandler(arg0);
}
RegData routine_3r1_proto[] = {
	{"metadata", "&BT2::MetaData", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileHandler::FileHandler
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:37
 */
::BT2::FileHandler* __CDECL routine_3r2(const ::BT2::MetaData &arg0, ::BT2::FileObject::FileState arg1)
{
	return new ::BT2::FileHandler(arg0, arg1);
}
RegData routine_3r2_proto[] = {
	{"metadata", "&BT2::MetaData", 0},
	{"state", "#BT2::FileObject::FileState", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileHandler::getMetaData
 * returns reference(const ::BT2::MetaData)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:42
 */
const ::BT2::MetaData &routine_4r0(const ::BT2::FileHandler *self)
{
	return self->getMetaData();
}
RegData routine_4r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileHandler::getMetaData
 * returns reference(::BT2::MetaData)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:43
 */
::BT2::MetaData &routine_5r0(::BT2::FileHandler *self)
{
	return self->getMetaData();
}
RegData routine_5r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileHandler::getFilename
 * returns reference(const std::string)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:48
 */
const std::string &routine_6r0(const ::BT2::FileHandler *self)
{
	return self->getFilename();
}
RegData routine_6r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileHandler::getFileObject
 * returns reference(::BT2::FileObject)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:53
 */
::BT2::FileObject &routine_7r0(::BT2::FileHandler *self)
{
	return self->getFileObject();
}
RegData routine_7r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileHandler::getAlgorithm
 * returns reference(::BT2::PartialAlgorithm)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:58
 */
::BT2::PartialAlgorithm &routine_8r0(::BT2::FileHandler *self)
{
	return self->getAlgorithm();
}
RegData routine_8r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileHandler::getChunk
 * returns ::BT2::Chunk
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:65
 */
::BT2::Chunk *routine_9r0(::BT2::FileHandler *self)
{
	return new ::BT2::Chunk(self->getChunk());
}
RegData routine_9r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileHandler::putChunk
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:76
 */
bool routine_10r1(::BT2::FileHandler *self, const ::BT2::Chunk &arg0)
{
	return self->putChunk(arg0);
}
RegData routine_10r1_proto[] = {
	{"c", "&BT2::Chunk", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileHandler::isEndGame
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:81
 */
bool routine_11r0(::BT2::FileHandler *self)
{
	return self->isEndGame();
}
RegData routine_11r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileHandler::isDone
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/control/FileHandler.h:86
 */
bool routine_12r0(::BT2::FileHandler *self)
{
	return self->isDone();
}
RegData routine_12r0_proto[] = {
	{ 0,0,0,0 }
};

void __CDECL dtor_13(BT2::FileHandler *self) { delete self; }
/*
 * ::BT2::Chunk::Chunk
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/Chunk.h:28
 */
::BT2::Chunk* __CDECL routine_14r3(size_t &arg0, size_t &arg1, const std::string &arg2)
{
	return new ::BT2::Chunk(arg0, arg1, arg2);
}
RegData routine_14r3_proto[] = {
	{"index", "&size_t", 0},
	{"maxChunks", "&size_t", 0},
	{"data", "&std::string", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::Chunk::Chunk
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/Chunk.h:33
 */
::BT2::Chunk* __CDECL routine_15r2(::BT2::BitVector &arg0, const std::string &arg1)
{
	return new ::BT2::Chunk(arg0, arg1);
}
RegData routine_15r2_proto[] = {
	{"functional", "&BT2::BitVector", 0},
	{"data", "&std::string", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::Chunk::operator^
 * returns ::BT2::Chunk
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/Chunk.h:40
 */
::BT2::Chunk *routine_16r1(const ::BT2::Chunk *self, const ::BT2::Chunk &arg0)
{
	return new ::BT2::Chunk(self->operator^(arg0));
}
RegData routine_16r1_proto[] = {
	{"rhs", "&BT2::Chunk", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::Chunk::operator^=
 * returns reference(::BT2::Chunk)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/Chunk.h:41
 */
::BT2::Chunk &routine_17r1(::BT2::Chunk *self, const ::BT2::Chunk &arg0)
{
	return self->operator^=(arg0);
}
RegData routine_17r1_proto[] = {
	{"lhs", "&BT2::Chunk", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::Chunk::getData
 * returns reference(const std::string)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/Chunk.h:46
 */
const std::string &routine_18r0(const ::BT2::Chunk *self)
{
	return self->getData();
}
RegData routine_18r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::Chunk::getFunctional
 * returns reference(const ::BT2::BitVector)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/Chunk.h:50
 */
const ::BT2::BitVector &routine_19r0(const ::BT2::Chunk *self)
{
	return self->getFunctional();
}
RegData routine_19r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::Chunk::getData
 * returns reference(std::string)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/Chunk.h:54
 */
std::string &routine_20r0(::BT2::Chunk *self)
{
	return self->getData();
}
RegData routine_20r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::Chunk::getFunctional
 * returns reference(::BT2::BitVector)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/Chunk.h:58
 */
::BT2::BitVector &routine_21r0(::BT2::Chunk *self)
{
	return self->getFunctional();
}
RegData routine_21r0_proto[] = {
	{ 0,0,0,0 }
};

void __CDECL dtor_22(BT2::Chunk *self) { delete self; }
/*
 * ::BT2::MetaFile::MetaFile
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:29
 */
::BT2::MetaFile* __CDECL routine_23r1(const std::string &arg0)
{
	return new ::BT2::MetaFile(arg0);
}
RegData routine_23r1_proto[] = {
	{"baseFileName", "&std::string", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaFile::MetaFile
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:36
 */
::BT2::MetaFile* __CDECL routine_24r2(const std::string &arg0, const std::string &arg1)
{
	return new ::BT2::MetaFile(arg0, arg1);
}
RegData routine_24r2_proto[] = {
	{"baseFileName", "&std::string", 0},
	{"diskFileName", "&std::string", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaFile::MetaFile
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:40
 */
::BT2::MetaFile* __CDECL routine_25r1(::BT2::MetaData *arg0)
{
	return new ::BT2::MetaFile(arg0);
}
RegData routine_25r1_proto[] = {
	{"metadata", ">BT2::MetaData", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaFile::isInitialized
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:50
 */
bool routine_26r0(const ::BT2::MetaFile *self)
{
	return self->isInitialized();
}
RegData routine_26r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaFile::getMetaData
 * returns reference(::BT2::MetaData)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:55
 */
::BT2::MetaData &routine_27r0(::BT2::MetaFile *self)
{
	return self->getMetaData();
}
RegData routine_27r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaFile::getMetaData
 * returns reference(const ::BT2::MetaData)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:59
 */
const ::BT2::MetaData &routine_28r0(const ::BT2::MetaFile *self)
{
	return self->getMetaData();
}
RegData routine_28r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaFile::getFileName
 * returns std::string
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:64
 */
std::string *routine_29r0(const ::BT2::MetaFile *self)
{
	return new std::string(self->getFileName());
}
RegData routine_29r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaFile::persist
 * returns void
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:69
 */
void routine_30r0(::BT2::MetaFile *self)
{
	self->persist();
}
RegData routine_30r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaFile::checkMetaFileExists
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:77
 */
bool routine_31r1(::BT2::MetaFile *self, const std::string &arg0)
{
	return self->checkMetaFileExists(arg0);
}
RegData routine_31r1_proto[] = {
	{"baseFileName", "&std::string", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaFile::getMetaFilePathForFile
 * returns std::string
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:86
 */
std::string *routine_32r1(::BT2::MetaFile *self, const std::string &arg0)
{
	return new std::string(self->getMetaFilePathForFile(arg0));
}
RegData routine_32r1_proto[] = {
	{"baseFileName", "&std::string", 0},
	{ 0,0,0,0 }
};

void __CDECL dtor_33(BT2::MetaFile *self) { delete self; }
/*
 * ::BT2::MetaData::FileData::FileData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:55
 */
::BT2::MetaData::FileData* __CDECL routine_34r0()
{
	return new ::BT2::MetaData::FileData();
}
RegData routine_34r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::FileData::FileData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:55
 */
::BT2::MetaData::FileData* __CDECL routine_34r1(const std::string &arg0)
{
	return new ::BT2::MetaData::FileData(arg0);
}
RegData routine_34r1_proto[] = {
	{"_path", "&std::string", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::FileData::FileData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:55
 */
::BT2::MetaData::FileData* __CDECL routine_34r2(const std::string &arg0, size_t &arg1)
{
	return new ::BT2::MetaData::FileData(arg0, arg1);
}
RegData routine_34r2_proto[] = {
	{"_path", "&std::string", 0},
	{"_offset", "&size_t", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::FileData::FileData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:55
 */
::BT2::MetaData::FileData* __CDECL routine_34r3(const std::string &arg0, size_t &arg1, size_t &arg2)
{
	return new ::BT2::MetaData::FileData(arg0, arg1, arg2);
}
RegData routine_34r3_proto[] = {
	{"_path", "&std::string", 0},
	{"_offset", "&size_t", 0},
	{"_size", "&size_t", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::FileData::FileData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:55
 */
::BT2::MetaData::FileData* __CDECL routine_34r4(const std::string &arg0, size_t &arg1, size_t &arg2, size_t &arg3)
{
	return new ::BT2::MetaData::FileData(arg0, arg1, arg2, arg3);
}
RegData routine_34r4_proto[] = {
	{"_path", "&std::string", 0},
	{"_offset", "&size_t", 0},
	{"_size", "&size_t", 0},
	{"_index", "&size_t", 0},
	{ 0,0,0,0 }
};

/*
 * var ::BT2::MetaData::FileData::path
 * of type std::string
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:57
 */
std::string &data_get_35f(::BT2::MetaData::FileData *self) { return self->path; }
/*
 * var ::BT2::MetaData::FileData::offset
 * of type size_t
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:58
 */
size_t &data_get_36f(::BT2::MetaData::FileData *self) { return self->offset; }
/*
 * var ::BT2::MetaData::FileData::size
 * of type size_t
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:59
 */
size_t &data_get_37f(::BT2::MetaData::FileData *self) { return self->size; }
/*
 * var ::BT2::MetaData::FileData::index
 * of type size_t
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:60
 */
size_t &data_get_38f(::BT2::MetaData::FileData *self) { return self->index; }
void __CDECL dtor_39(BT2::MetaData::FileData *self) { delete self; }
/*
 * ::BT2::PartialAlgorithm::PartialAlgorithm
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/algorithm/PartialAlgorithm.h:27
 */
::BT2::PartialAlgorithm* __CDECL routine_40r1(::BT2::FileObject &arg0)
{
	return new ::BT2::PartialAlgorithm(arg0);
}
RegData routine_40r1_proto[] = {
	{"file", ">BT2::FileObject", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::PartialAlgorithm::canAddFunctional
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/algorithm/PartialAlgorithm.h:36
 */
bool routine_41r1(::BT2::PartialAlgorithm *self, const ::BT2::BitVector &arg0)
{
	return self->canAddFunctional(arg0);
}
RegData routine_41r1_proto[] = {
	{"functional", "&BT2::BitVector", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::PartialAlgorithm::addChunk
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/algorithm/PartialAlgorithm.h:45
 */
bool routine_42r1(::BT2::PartialAlgorithm *self, const ::BT2::Chunk &arg0)
{
	return self->addChunk(arg0);
}
RegData routine_42r1_proto[] = {
	{"c", "&BT2::Chunk", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::PartialAlgorithm::isEndGame
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/algorithm/PartialAlgorithm.h:52
 */
bool routine_43r0(::BT2::PartialAlgorithm *self)
{
	return self->isEndGame();
}
RegData routine_43r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::PartialAlgorithm::getChunk
 * returns ::BT2::Chunk
 * from /home/nadir/Work/bp2p/current/src/betterp2p/algorithm/PartialAlgorithm.h:60
 */
::BT2::Chunk *routine_44r0(::BT2::PartialAlgorithm *self)
{
	return new ::BT2::Chunk(self->getChunk());
}
RegData routine_44r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::PartialAlgorithm::isDone
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/algorithm/PartialAlgorithm.h:65
 */
bool routine_45r0(::BT2::PartialAlgorithm *self)
{
	return self->isDone();
}
RegData routine_45r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::PartialAlgorithm::wrapUp
 * returns void
 * from /home/nadir/Work/bp2p/current/src/betterp2p/algorithm/PartialAlgorithm.h:71
 */
void routine_46r0(::BT2::PartialAlgorithm *self)
{
	self->wrapUp();
}
RegData routine_46r0_proto[] = {
	{ 0,0,0,0 }
};

void __CDECL dtor_47(BT2::PartialAlgorithm *self) { delete self; }
/*
 * ::BT2::FileObject::FileObject
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:34
 */
::BT2::FileObject* __CDECL routine_48r1(const std::string &arg0)
{
	return new ::BT2::FileObject(arg0);
}
RegData routine_48r1_proto[] = {
	{"filename", "&std::string", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::FileObject
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:52
 */
::BT2::FileObject* __CDECL routine_49r1(const ::BT2::MetaData &arg0)
{
	return new ::BT2::FileObject(arg0);
}
RegData routine_49r1_proto[] = {
	{"metadata", "&BT2::MetaData", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::FileObject
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:52
 */
::BT2::FileObject* __CDECL routine_49r2(const ::BT2::MetaData &arg0, ::BT2::FileObject::FileState arg1)
{
	return new ::BT2::FileObject(arg0, arg1);
}
RegData routine_49r2_proto[] = {
	{"metadata", "&BT2::MetaData", 0},
	{"state", "#BT2::FileObject::FileState", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::getMetaData
 * returns reference(const ::BT2::MetaData)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:63
 */
const ::BT2::MetaData &routine_50r0(const ::BT2::FileObject *self)
{
	return self->getMetaData();
}
RegData routine_50r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::getMetaData
 * returns reference(::BT2::MetaData)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:64
 */
::BT2::MetaData &routine_51r0(::BT2::FileObject *self)
{
	return self->getMetaData();
}
RegData routine_51r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::getFunctionals
 * returns template-instantiation(std::vector,::BT2::BitVector)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:72
 */
std::vector< ::BT2::BitVector > *routine_52r0(const ::BT2::FileObject *self)
{
	return new std::vector< ::BT2::BitVector >(self->getFunctionals());
}
RegData routine_52r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::getFunctionalMatrix
 * returns reference(const template-instantiation(std::vector,::BT2::BitVector))
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:79
 */
const std::vector< ::BT2::BitVector > &routine_53r0(const ::BT2::FileObject *self)
{
	return self->getFunctionalMatrix();
}
RegData routine_53r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::getChunk
 * returns ::BT2::Chunk
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:87
 */
::BT2::Chunk *routine_54r1(::BT2::FileObject *self, int arg0)
{
	return new ::BT2::Chunk(self->getChunk(arg0));
}
RegData routine_54r1_proto[] = {
	{"chunk_num", "int", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::getChunk
 * returns ::BT2::Chunk
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:95
 */
::BT2::Chunk *routine_55r1(::BT2::FileObject *self, const std::vector< ::BT2::BitVector > &arg0)
{
	return new ::BT2::Chunk(self->getChunk(arg0));
}
RegData routine_55r1_proto[] = {
	{"functionals", "&std::vector< BT2::BitVector >", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::getChunk
 * returns ::BT2::Chunk
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:105
 */
::BT2::Chunk *routine_56r2(::BT2::FileObject *self, const ::BT2::BitVector &arg0, const ::BT2::BitVector &arg1)
{
	return new ::BT2::Chunk(self->getChunk(arg0, arg1));
}
RegData routine_56r2_proto[] = {
	{"a", "&BT2::BitVector", 0},
	{"b", "&BT2::BitVector", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::putChunk
 * returns void
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:113
 */
void routine_57r1(::BT2::FileObject *self, const ::BT2::Chunk &arg0)
{
	self->putChunk(arg0);
}
RegData routine_57r1_proto[] = {
	{"c", "&BT2::Chunk", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::putCombinedChunks
 * returns void
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:121
 */
void routine_58r1(::BT2::FileObject *self, const std::vector< ::BT2::BitVector > &arg0)
{
	self->putCombinedChunks(arg0);
}
RegData routine_58r1_proto[] = {
	{"functionals", "&std::vector< BT2::BitVector >", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::putCombinedChunks
 * returns void
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:130
 */
void routine_59r2(::BT2::FileObject *self, const std::vector< ::BT2::BitVector > &arg0, const ::BT2::Chunk &arg1)
{
	self->putCombinedChunks(arg0, arg1);
}
RegData routine_59r2_proto[] = {
	{"functionals", "&std::vector< BT2::BitVector >", 0},
	{"chunk", "&BT2::Chunk", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::completeFiles
 * returns void
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:137
 */
void routine_60r0(::BT2::FileObject *self)
{
	self->completeFiles();
}
RegData routine_60r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::closeFiles
 * returns void
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:144
 */
void routine_61r0(::BT2::FileObject *self)
{
	self->closeFiles();
}
RegData routine_61r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::reopenFiles
 * returns void
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:150
 */
void routine_62r0(::BT2::FileObject *self)
{
	self->reopenFiles();
}
RegData routine_62r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::FileObject::persist
 * returns void
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/FileObject.h:156
 */
void routine_63r0(::BT2::FileObject *self)
{
	self->persist();
}
RegData routine_63r0_proto[] = {
	{ 0,0,0,0 }
};

void __CDECL dtor_64(BT2::FileObject *self) { delete self; }
/*
 * ::BT2::MetaData::MetaData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:86
 */
::BT2::MetaData* __CDECL routine_65r0()
{
	return new ::BT2::MetaData();
}
RegData routine_65r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::MetaData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:94
 */
::BT2::MetaData* __CDECL routine_66r3(const std::string &arg0, const std::string &arg1, size_t &arg2)
{
	return new ::BT2::MetaData(arg0, arg1, arg2);
}
RegData routine_66r3_proto[] = {
	{"fileName", "&std::string", 0},
	{"diskFileName", "&std::string", 0},
	{"chunkSize", "&size_t", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::MetaData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:94
 */
::BT2::MetaData* __CDECL routine_66r4(const std::string &arg0, const std::string &arg1, size_t &arg2, size_t &arg3)
{
	return new ::BT2::MetaData(arg0, arg1, arg2, arg3);
}
RegData routine_66r4_proto[] = {
	{"fileName", "&std::string", 0},
	{"diskFileName", "&std::string", 0},
	{"chunkSize", "&size_t", 0},
	{"fileSize", "&size_t", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::MetaData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:94
 */
::BT2::MetaData* __CDECL routine_66r5(const std::string &arg0, const std::string &arg1, size_t &arg2, size_t &arg3, const std::vector< ::BT2::BitVector > &arg4)
{
	return new ::BT2::MetaData(arg0, arg1, arg2, arg3, arg4);
}
RegData routine_66r5_proto[] = {
	{"fileName", "&std::string", 0},
	{"diskFileName", "&std::string", 0},
	{"chunkSize", "&size_t", 0},
	{"fileSize", "&size_t", 0},
	{"funcs", "&std::vector< BT2::BitVector >", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::MetaData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:102
 */
::BT2::MetaData* __CDECL routine_67r4(const std::string &arg0, const std::string &arg1, size_t &arg2, const std::vector< ::BT2::MetaData::FileData > &arg3)
{
	return new ::BT2::MetaData(arg0, arg1, arg2, arg3);
}
RegData routine_67r4_proto[] = {
	{"fileName", "&std::string", 0},
	{"diskFileName", "&std::string", 0},
	{"chunkSize", "&size_t", 0},
	{"containedFiles", "&std::vector< BT2::MetaData::FileData >", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::MetaData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:102
 */
::BT2::MetaData* __CDECL routine_67r5(const std::string &arg0, const std::string &arg1, size_t &arg2, const std::vector< ::BT2::MetaData::FileData > &arg3, const std::vector< ::BT2::BitVector > &arg4)
{
	return new ::BT2::MetaData(arg0, arg1, arg2, arg3, arg4);
}
RegData routine_67r5_proto[] = {
	{"fileName", "&std::string", 0},
	{"diskFileName", "&std::string", 0},
	{"chunkSize", "&size_t", 0},
	{"containedFiles", "&std::vector< BT2::MetaData::FileData >", 0},
	{"funcs", "&std::vector< BT2::BitVector >", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::MetaData
 * returns 
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:106
 */
::BT2::MetaData* __CDECL routine_68r1(const ::BT2::MetaData &arg0)
{
	return new ::BT2::MetaData(arg0);
}
RegData routine_68r1_proto[] = {
	{"other", "&BT2::MetaData", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::isSingleFile
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:111
 */
bool routine_69r0(const ::BT2::MetaData *self)
{
	return self->isSingleFile();
}
RegData routine_69r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::getContainedFilePath
 * returns std::string
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:119
 */
std::string *routine_70r1(const ::BT2::MetaData *self, int arg0)
{
	return new std::string(self->getContainedFilePath(arg0));
}
RegData routine_70r1_proto[] = {
	{"index", "int", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::getFunctionals
 * returns template-instantiation(std::vector,::BT2::BitVector)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:124
 */
std::vector< ::BT2::BitVector > *routine_71r0(const ::BT2::MetaData *self)
{
	return new std::vector< ::BT2::BitVector >(self->getFunctionals());
}
RegData routine_71r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::getMaxChunkCount
 * returns size_t
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:129
 */
size_t *routine_72r0(const ::BT2::MetaData *self)
{
	return new size_t(self->getMaxChunkCount());
}
RegData routine_72r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::getChunkCount
 * returns size_t
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:134
 */
size_t *routine_73r0(const ::BT2::MetaData *self)
{
	return new size_t(self->getChunkCount());
}
RegData routine_73r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::getCompletePieces
 * returns const ::BT2::BitVector
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:140
 */
const ::BT2::BitVector *routine_74r0(const ::BT2::MetaData *self)
{
	return new const ::BT2::BitVector(self->getCompletePieces());
}
RegData routine_74r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::isComplete
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:145
 */
bool routine_75r0(const ::BT2::MetaData *self)
{
	return self->isComplete();
}
RegData routine_75r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaData::dump
 * returns void
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:150
 */
void routine_76r0(const ::BT2::MetaData *self)
{
	self->dump();
}
RegData routine_76r0_proto[] = {
	{ 0,0,0,0 }
};

/*
 * var ::BT2::MetaData::EMPTY_FUNCTIONAL
 * of type const ::BT2::BitVector
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:30
 */
const ::BT2::BitVector &data_get_77f(::BT2::MetaData *self) { return self->EMPTY_FUNCTIONAL; }
/*
 * var ::BT2::MetaData::fileName
 * of type std::string
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:38
 */
std::string &data_get_78f(::BT2::MetaData *self) { return self->fileName; }
/*
 * var ::BT2::MetaData::diskFileName
 * of type std::string
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:43
 */
std::string &data_get_79f(::BT2::MetaData *self) { return self->diskFileName; }
/*
 * var ::BT2::MetaData::chunkSize
 * of type size_t
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:48
 */
size_t &data_get_80f(::BT2::MetaData *self) { return self->chunkSize; }
/*
 * var ::BT2::MetaData::containedFiles
 * of type template-instantiation(std::vector,::BT2::MetaData::FileData)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:66
 */
std::vector< ::BT2::MetaData::FileData > &data_get_81f(::BT2::MetaData *self) { return self->containedFiles; }
/*
 * var ::BT2::MetaData::fileSize
 * of type size_t
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:71
 */
size_t &data_get_82f(::BT2::MetaData *self) { return self->fileSize; }
/*
 * var ::BT2::MetaData::chunkFunctionals
 * of type template-instantiation(std::vector,::BT2::BitVector)
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:78
 */
std::vector< ::BT2::BitVector > &data_get_83f(::BT2::MetaData *self) { return self->chunkFunctionals; }
void __CDECL dtor_84(BT2::MetaData *self) { delete self; }
/*
 * ::BT2::MetaFile::checkMetaFileExists
 * returns bool
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:77
 */
bool routine_31s1(const std::string &arg0)
{
	return ::BT2::MetaFile::checkMetaFileExists(arg0);
}
RegData routine_31s1_proto[] = {
	{"baseFileName", "&std::string", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::MetaFile::getMetaFilePathForFile
 * returns std::string
 * from /home/nadir/Work/bp2p/current/src/betterp2p/filesystem/MetaFile.h:86
 */
std::string *routine_32s1(const std::string &arg0)
{
	return new std::string(::BT2::MetaFile::getMetaFilePathForFile(arg0));
}
RegData routine_32s1_proto[] = {
	{"baseFileName", "&std::string", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::bitvector_from_string
 * returns ::BT2::BitVector
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/BitVector.h:28
 */
::BT2::BitVector *routine_85s1(const std::string &arg0)
{
	return new ::BT2::BitVector(::BT2::bitvector_from_string(arg0));
}
RegData routine_85s1_proto[] = {
	{"data", "&std::string", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::bitvector_from_string
 * returns ::BT2::BitVector
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/BitVector.h:28
 */
::BT2::BitVector *routine_85s2(const std::string &arg0, size_t &arg1)
{
	return new ::BT2::BitVector(::BT2::bitvector_from_string(arg0, arg1));
}
RegData routine_85s2_proto[] = {
	{"data", "&std::string", 0},
	{"bitlen", "&size_t", 0},
	{ 0,0,0,0 }
};

/*
 * ::BT2::bitvector_to_string
 * returns std::string
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/BitVector.h:29
 */
std::string *routine_86s1(const ::BT2::BitVector &arg0)
{
	return new std::string(::BT2::bitvector_to_string(arg0));
}
RegData routine_86s1_proto[] = {
	{"bits", "&BT2::BitVector", 0},
	{ 0,0,0,0 }
};

/*
 * var ::BT2::MetaData::EMPTY_FUNCTIONAL
 * of type const ::BT2::BitVector
 * from /home/nadir/Work/bp2p/current/src/betterp2p/common/MetaData.h:30
 */
const ::BT2::BitVector &data_get_77s(::BT2::MetaData *self) { return self->EMPTY_FUNCTIONAL; }
/*
 * enum ::BT2::FileObject::FileState
 */
int const_8831815 = (int)::BT2::FileObject::UNKNOWN;
int const_25062038 = (int)::BT2::FileObject::NEW;
int const_8889804 = (int)::BT2::FileObject::INCOMPLETE;
int const_26611461 = (int)::BT2::FileObject::COMPLETE;
RegData enumerated_15789782[] = {
	{ "UNKNOWN", 0, 0, (void*)&const_8831815 },
	{ "NEW", 0, 0, (void*)&const_25062038 },
	{ "INCOMPLETE", 0, 0, (void*)&const_8889804 },
	{ "COMPLETE", 0, 0, (void*)&const_26611461 },
	{ 0,0,0,0 }
};

RegData scope_18992192[] = {
{ "*", "constructor", 0, (void*)&ctor_1},
{ ".", "destructor", 0, (void*)&dtor_1},
	{ 0,0,0,0 }
};

RegData scope_24529889[] = {
{ "*" , "constructor", routine_2r1_proto+0, (void*)&routine_2r1},
{ "*" , "constructor", routine_3r1_proto+0, (void*)&routine_3r1},
{ "*" , "constructor", routine_3r2_proto+0, (void*)&routine_3r2},
{"getMetaData", "&BT2::MetaData", routine_4r0_proto+0, (void*)&routine_4r0},
{"getMetaData", "&BT2::MetaData", routine_5r0_proto+0, (void*)&routine_5r0},
{"getFilename", "&std::string", routine_6r0_proto+0, (void*)&routine_6r0},
{"getFileObject", "&BT2::FileObject", routine_7r0_proto+0, (void*)&routine_7r0},
{"getAlgorithm", "&BT2::PartialAlgorithm", routine_8r0_proto+0, (void*)&routine_8r0},
{"getChunk", "*BT2::Chunk", routine_9r0_proto+0, (void*)&routine_9r0},
{"putChunk", "bool", routine_10r1_proto+0, (void*)&routine_10r1},
{"isEndGame", "bool", routine_11r0_proto+0, (void*)&routine_11r0},
{"isDone", "bool", routine_12r0_proto+0, (void*)&routine_12r0},
{ ".", "destructor", 0, (void*)&dtor_13},
	{ 0,0,0,0 }
};

RegData scope_32782069[] = {
{ "*" , "constructor", routine_14r3_proto+0, (void*)&routine_14r3},
{ "*" , "constructor", routine_15r2_proto+0, (void*)&routine_15r2},
{"operator^", "*BT2::Chunk", routine_16r1_proto+0, (void*)&routine_16r1},
{"operator^=", "&BT2::Chunk", routine_17r1_proto+0, (void*)&routine_17r1},
{"getData", "&std::string", routine_18r0_proto+0, (void*)&routine_18r0},
{"getFunctional", "&BT2::BitVector", routine_19r0_proto+0, (void*)&routine_19r0},
{"getData", "&std::string", routine_20r0_proto+0, (void*)&routine_20r0},
{"getFunctional", "&BT2::BitVector", routine_21r0_proto+0, (void*)&routine_21r0},
{ ".", "destructor", 0, (void*)&dtor_22},
	{ 0,0,0,0 }
};

RegData scope_5462872[] = {
{ "*" , "constructor", routine_23r1_proto+0, (void*)&routine_23r1},
{ "*" , "constructor", routine_24r2_proto+0, (void*)&routine_24r2},
{ "*" , "constructor", routine_25r1_proto+0, (void*)&routine_25r1},
{"isInitialized", "bool", routine_26r0_proto+0, (void*)&routine_26r0},
{"getMetaData", "&BT2::MetaData", routine_27r0_proto+0, (void*)&routine_27r0},
{"getMetaData", "&BT2::MetaData", routine_28r0_proto+0, (void*)&routine_28r0},
{"getFileName", "*std::string", routine_29r0_proto+0, (void*)&routine_29r0},
{"persist", "void", routine_30r0_proto+0, (void*)&routine_30r0},
{"checkMetaFileExists", "bool", routine_31r1_proto+0, (void*)&routine_31r1},
{"getMetaFilePathForFile", "*std::string", routine_32r1_proto+0, (void*)&routine_32r1},
{ ".", "destructor", 0, (void*)&dtor_33},
	{ 0,0,0,0 }
};

RegData scope_16821027[] = {
{ "*" , "constructor", routine_34r0_proto+0, (void*)&routine_34r0},
{ "*" , "constructor", routine_34r1_proto+0, (void*)&routine_34r1},
{ "*" , "constructor", routine_34r2_proto+0, (void*)&routine_34r2},
{ "*" , "constructor", routine_34r3_proto+0, (void*)&routine_34r3},
{ "*" , "constructor", routine_34r4_proto+0, (void*)&routine_34r4},
{ ".data_path", "&std::string", 0, (void*)&data_get_35f },
{ ".data_offset", "&size_t", 0, (void*)&data_get_36f },
{ ".data_size", "&size_t", 0, (void*)&data_get_37f },
{ ".data_index", "&size_t", 0, (void*)&data_get_38f },
{ ".", "destructor", 0, (void*)&dtor_39},
	{ 0,0,0,0 }
};

RegData scope_14093690[] = {
{ "*" , "constructor", routine_40r1_proto+0, (void*)&routine_40r1},
{"canAddFunctional", "bool", routine_41r1_proto+0, (void*)&routine_41r1},
{"addChunk", "bool", routine_42r1_proto+0, (void*)&routine_42r1},
{"isEndGame", "bool", routine_43r0_proto+0, (void*)&routine_43r0},
{"getChunk", "*BT2::Chunk", routine_44r0_proto+0, (void*)&routine_44r0},
{"isDone", "bool", routine_45r0_proto+0, (void*)&routine_45r0},
{"wrapUp", "void", routine_46r0_proto+0, (void*)&routine_46r0},
{ ".", "destructor", 0, (void*)&dtor_47},
	{ 0,0,0,0 }
};

RegData scope_25771774[] = {
{ "*" , "constructor", routine_48r1_proto+0, (void*)&routine_48r1},
{ "*" , "constructor", routine_49r1_proto+0, (void*)&routine_49r1},
{ "*" , "constructor", routine_49r2_proto+0, (void*)&routine_49r2},
{"getMetaData", "&BT2::MetaData", routine_50r0_proto+0, (void*)&routine_50r0},
{"getMetaData", "&BT2::MetaData", routine_51r0_proto+0, (void*)&routine_51r0},
{"getFunctionals", "*std::vector< BT2::BitVector >", routine_52r0_proto+0, (void*)&routine_52r0},
{"getFunctionalMatrix", "&std::vector< BT2::BitVector >", routine_53r0_proto+0, (void*)&routine_53r0},
{"getChunk", "*BT2::Chunk", routine_54r1_proto+0, (void*)&routine_54r1},
{"getChunk", "*BT2::Chunk", routine_55r1_proto+0, (void*)&routine_55r1},
{"getChunk", "*BT2::Chunk", routine_56r2_proto+0, (void*)&routine_56r2},
{"putChunk", "void", routine_57r1_proto+0, (void*)&routine_57r1},
{"putCombinedChunks", "void", routine_58r1_proto+0, (void*)&routine_58r1},
{"putCombinedChunks", "void", routine_59r2_proto+0, (void*)&routine_59r2},
{"completeFiles", "void", routine_60r0_proto+0, (void*)&routine_60r0},
{"closeFiles", "void", routine_61r0_proto+0, (void*)&routine_61r0},
{"reopenFiles", "void", routine_62r0_proto+0, (void*)&routine_62r0},
{"persist", "void", routine_63r0_proto+0, (void*)&routine_63r0},
{ ".", "destructor", 0, (void*)&dtor_64},
	{ 0,0,0,0 }
};

RegData scope_21356612[] = {
{ "*" , "constructor", routine_65r0_proto+0, (void*)&routine_65r0},
{ "*" , "constructor", routine_66r3_proto+0, (void*)&routine_66r3},
{ "*" , "constructor", routine_66r4_proto+0, (void*)&routine_66r4},
{ "*" , "constructor", routine_66r5_proto+0, (void*)&routine_66r5},
{ "*" , "constructor", routine_67r4_proto+0, (void*)&routine_67r4},
{ "*" , "constructor", routine_67r5_proto+0, (void*)&routine_67r5},
{ "*" , "constructor", routine_68r1_proto+0, (void*)&routine_68r1},
{"isSingleFile", "bool", routine_69r0_proto+0, (void*)&routine_69r0},
{"getContainedFilePath", "*std::string", routine_70r1_proto+0, (void*)&routine_70r1},
{"getFunctionals", "*std::vector< BT2::BitVector >", routine_71r0_proto+0, (void*)&routine_71r0},
{"getMaxChunkCount", "*size_t", routine_72r0_proto+0, (void*)&routine_72r0},
{"getChunkCount", "*size_t", routine_73r0_proto+0, (void*)&routine_73r0},
{"getCompletePieces", "*BT2::BitVector", routine_74r0_proto+0, (void*)&routine_74r0},
{"isComplete", "bool", routine_75r0_proto+0, (void*)&routine_75r0},
{"dump", "void", routine_76r0_proto+0, (void*)&routine_76r0},
{ ".data_EMPTY_FUNCTIONAL", "&BT2::BitVector", 0, (void*)&data_get_77f },
{ ".data_fileName", "&std::string", 0, (void*)&data_get_78f },
{ ".data_diskFileName", "&std::string", 0, (void*)&data_get_79f },
{ ".data_chunkSize", "&size_t", 0, (void*)&data_get_80f },
{ ".data_containedFiles", "&std::vector< BT2::MetaData::FileData >", 0, (void*)&data_get_81f },
{ ".data_fileSize", "&size_t", 0, (void*)&data_get_82f },
{ ".data_chunkFunctionals", "&std::vector< BT2::BitVector >", 0, (void*)&data_get_83f },
{ ".", "destructor", 0, (void*)&dtor_84},
	{ 0,0,0,0 }
};


}  // end of Robin_4a39201f namespace

using namespace Robin_4a39201f;

extern "C" EXPORT RegData entry[];

RegData entry[] = {
	{"BT2::FileObject::FileState", "enum", enumerated_15789782},
	{"BT2::MetaData::ChunkFunctionals", "=std::vector< BT2::BitVector >", 0},
	{"BT2::FileHandler", "class", scope_24529889},
	{"BT2::BitVector", "class", scope_18992192},
	{"BT2::Chunk", "class", scope_32782069},
	{"BT2::MetaFile", "class", scope_5462872},
	{"BT2::MetaData::FileData", "class", scope_16821027},
	{"BT2::MetaData", "class", scope_21356612},
	{"BT2::FileObject", "class", scope_25771774},
	{"BT2::PartialAlgorithm", "class", scope_14093690},
{"BT2::MetaFile::checkMetaFileExists", "bool", routine_31s1_proto+0, (void*)&routine_31s1},
{"BT2::MetaFile::getMetaFilePathForFile", "*std::string", routine_32s1_proto+0, (void*)&routine_32s1},
{"BT2::bitvector_from_string", "*BT2::BitVector", routine_85s1_proto+0, (void*)&routine_85s1},
{"BT2::bitvector_from_string", "*BT2::BitVector", routine_85s2_proto+0, (void*)&routine_85s2},
{"BT2::bitvector_to_string", "*std::string", routine_86s1_proto+0, (void*)&routine_86s1},
{ ".data_BT2::MetaData::EMPTY_FUNCTIONAL", "&BT2::BitVector", 0, (void*)&data_get_77s },
	{ 0,0,0,0 }
};

