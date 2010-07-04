
//#include <filesystem/FullSourceFile.h>
//#include <filesystem/PartialFile.h>
//#include <filesystem/FullTargetFile.h>
#include <filesystem/FileObject.h>

//#include <algorithm/FullAlgorithm.h>
#include <algorithm/PartialAlgorithm.h>
//#include <algorithm/FullReconstructionAlgorithm.h>

#include <boost/filesystem/path.hpp>
using boost::filesystem::path;

#include <common/Logger.h>

const size_t chunk_size = 100 * 1024; // 100k

using namespace BT2;

int main() {
	const path workPath="test_resources";
	log4cpp::Category& logger = Logger::getLog("main");

	path file = workPath / "file.rnd";
	logger.debugStream()  << "constructing FullSourceFile for " << file;
    MetaData fulldata(file.file_string(), file.file_string(), chunk_size);
	FileObject fsf(fulldata, FileObject::COMPLETE);
	logger.debugStream()  << fsf.getMetaData().fileName;

	file = workPath / "file_new.rnd";
    MetaData newdata(fsf.getMetaData());
    newdata.diskFileName = newdata.fileName = file.file_string();
	FileObject pf(newdata, FileObject::NEW);

	logger.debugStream()  << "Source file: " << fsf.getMetaData().fileName << "(disk: "
			<< fsf.getMetaData().diskFileName << ")" << ", size: "
			<< fsf.getMetaData().fileSize << ", chunk size: "
			<< fsf.getMetaData().chunkSize << ", #chunks: "
			<< fsf.getMetaData().getMaxChunkCount();
	logger.debugStream()  << "Partial file: " << pf.getMetaData().fileName << "(disk: "
			<< pf.getMetaData().diskFileName << ")" << ", size: "
			<< pf.getMetaData().fileSize << ", chunk size: "
			<< pf.getMetaData().chunkSize << ", #chunks: "
			<< pf.getMetaData().getMaxChunkCount();

	PartialAlgorithm falg(fsf);
	PartialAlgorithm palg(pf);

	size_t i = 0;
	size_t n = pf.getMetaData().getMaxChunkCount();

	while (!palg.isDone()) {
		i++;
		Chunk c = falg.getChunk();
		if (palg.addChunk(c)) {
			if (n < 20) {
				std::cerr << "Partial matrix is: " << std::endl;
				pf.getMetaData().dump();
			}
		}
		else {
			logger.debugStream() << "Generated unusable chunk!!!";
		}

		logger.debugStream()  << "Generated " << i << " chunks so far...";
	}

	// Conclusion
	bool ok;

	if ((ok = palg.isDone())) {
		logger.infoStream() << "Successfully eliminated matrix.";
	}
	else {
		logger.errorStream() << "Matrix elimination failed!";
	}

	if (n < 30) {
		std::cerr << "Final matrix is:" << std::endl;
		pf.getMetaData().dump();
	}

	logger.infoStream() << "Total chunks transferred: " << i;
	logger.infoStream() << "# of usable chunks: " << pf.getMetaData().getChunkCount();

	if (ok) {
		logger.infoStream()  << "All done";
	}
	else {
		logger.errorStream() << "Failed!";
	}
}
