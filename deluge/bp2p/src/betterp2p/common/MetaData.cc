#include "MetaData.h"

#include <boost/filesystem.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

namespace BT2 {

const std::string MetaData::METADATA_MAGIC("B2MD");
const BitVector MetaData::EMPTY_FUNCTIONAL(0);

MetaData::MetaData()
    : chunkSize(0), fileSize(0)
{ }

MetaData::MetaData(const std::string& aFileName, const std::string& aDiskFileName,
            size_t aChunkSize, size_t aFileSize, const std::vector<BitVector>& aFuncs)
    : fileName(aFileName), diskFileName(aDiskFileName), chunkSize(aChunkSize),
      containedFiles(0), fileSize(aFileSize), chunkFunctionals(aFuncs)
{ }

MetaData::MetaData(const std::string& aFileName, const std::string& aDiskFileName,
        size_t aChunkSize, const std::vector<MetaData::FileData>& aContainedFiles,
        const std::vector<BitVector>& aFuncs)
    : fileName(aFileName), diskFileName(aDiskFileName), chunkSize(aChunkSize),
      containedFiles(aContainedFiles), fileSize(0), chunkFunctionals(aFuncs)
{
    for (size_t i = 0; i < containedFiles.size(); ++i) {
        fileSize += containedFiles[i].size;
        containedFiles[i].index = i;
    }
}

MetaData::MetaData(const MetaData &other)
    : fileName(other.fileName), diskFileName(other.diskFileName),
      chunkSize(other.chunkSize), containedFiles(other.containedFiles),
      fileSize(other.fileSize), chunkFunctionals(other.chunkFunctionals)
{ }

bool MetaData::isSingleFile() const {
    return containedFiles.empty();
}

std::string MetaData::getContainedFilePath(int index) const {
    if (isSingleFile()) {
        return diskFileName;
    }
    return (boost::filesystem::path(diskFileName) / containedFiles[index].path).string();
}

vector<BitVector> MetaData::getFunctionals() const {
    vector<BitVector> funcs;
    for (size_t i = 0; i < chunkFunctionals.size(); ++i) {
        if (chunkFunctionals[i] != MetaData::EMPTY_FUNCTIONAL) {
            funcs.push_back(chunkFunctionals[i]);
        }
    }

    return funcs;
}

size_t MetaData::getMaxChunkCount() const {
    return chunkFunctionals.size();
}

size_t MetaData::getChunkCount() const {
	size_t count = 0;
    for (size_t i = 0; i < chunkFunctionals.size(); ++i) {
        if (chunkFunctionals[i] != MetaData::EMPTY_FUNCTIONAL) {
            count++;
        }
    }
    return count;
}

const BitVector MetaData::getCompletePieces() const {
	BitVector pieces(chunkFunctionals.size());
    for (size_t i = 0; i < chunkFunctionals.size(); ++i) {
    	if (chunkFunctionals[i].count() == 1) {
    		pieces[i] = 1;
    	}
    }
    return pieces;
}

bool MetaData::isComplete() const
{
    return (getChunkCount() == getMaxChunkCount());
}

void MetaData::dump() const
{
    int padding = 0;
    for (size_t i = chunkFunctionals.size(); i > 0; i /= 10, padding++);
	for (size_t i = 0; i < chunkFunctionals.size(); ++i) {
        std::ostringstream s;
        s << std::setw(padding) << std::setfill('0') << i << std::ends;
		std::cerr << "[" << s.str() << "] " << chunkFunctionals[i] << std::endl;
	}
}

} /* namespace BT2 */
