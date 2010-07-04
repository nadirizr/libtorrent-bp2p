#include "PartialAlgorithm.h"

#include <filesystem/Chunk.h>
#include <common/BitVector.h>
#include <common/utility.h>
#include <common/Logger.h>

#include <iostream>
using namespace std;

namespace BT2 {

// - for some reason, putting these in the header causes link-time
//   failure
const size_t PartialAlgorithm::END_GAME_CONSTANT = 2;


PartialAlgorithm::PartialAlgorithm(FileObject &file) :
    m_file(file), m_numGeneratedChunks(0), m_wasEliminated(false) {
}

PartialAlgorithm::~PartialAlgorithm() { };

bool PartialAlgorithm::canAddFunctional(const BitVector& functional) {
	std::vector<BitVector> to_xor_functionals;
	return semiReduce(functional, to_xor_functionals);
}

bool PartialAlgorithm::addChunk(const Chunk &c) {
	//Logger::getLog("PartialAlgorithm").debugStream() << "Add chunk " << c.getFunctional();

    std::vector<BitVector> to_xor_functionals;

    bool indep = semiReduce(c.getFunctional(), to_xor_functionals);

    if (indep)
    	m_file.putCombinedChunks(to_xor_functionals, c);

    cerr << "// --> PartialAlgorithm: was chunk helpful? " << indep << endl;
    return indep;
}

bool PartialAlgorithm::isEndGame() {
    return (m_file.getMetaData().getChunkCount() ==
            (m_file.getMetaData().getMaxChunkCount() - PartialAlgorithm::END_GAME_CONSTANT));
}

Chunk PartialAlgorithm::getChunk() {
    // if we have no chunks to give, send a blank one
    if (m_file.getMetaData().getChunkCount() == 0) {
        return Chunk(BitVector(m_file.getMetaData().getMaxChunkCount()),
                     std::string(m_file.getMetaData().chunkSize, '\0'));
    }

    int randint = getRandom();
    size_t numChunks;

    std::vector<BitVector> functionals = m_file.getFunctionals();

    // 1:2 - combine 2 chunks
    // 1:4 - combine 4 chunks
    // 1:8 - combine 8 chunks
    // 1:16 - combine 16 chunks
    if ((randint & 0x1) == 0x0) {
        numChunks = 2;
    } else if ((randint & 0x3) == 0x1) {
        numChunks = 4;
    } else if ((randint & 0x7) == 0x3) {
        numChunks = 8;
    } else if ((randint & 0xF) == 0x7) {
        numChunks = 16;
    } else {
        // at least 2 chunks, at most maxChunks - 1
        size_t maxChunks = m_file.getMetaData().getChunkCount();
        numChunks = getRandom() % (maxChunks-2) + 2;
    }
    // 1:2 (independently of the above) - one more chunk
    if ((randint & 0x100) != 0) numChunks++;
    // Make sure numChunks <= total number of chunks
    numChunks = ((numChunks - 1) % functionals.size()) + 1;

    std::vector<BitVector> chosen_funcs(numChunks);

    for (size_t i = 0; i < numChunks; ++i) {
    	size_t wanted_rand_chunk = (getRandom() % (functionals.size() - i)) + i;
    	chosen_funcs[i] = functionals[wanted_rand_chunk];
    	functionals[wanted_rand_chunk] = functionals[i];
    }

    return m_file.getChunk(chosen_funcs);
}


bool PartialAlgorithm::isDone() {
    return m_wasEliminated || isUpperTriangular();
}

void PartialAlgorithm::wrapUp() {
    assert(isDone());
    if(!m_wasEliminated) {
        gaussElimination();
        if (!m_wasEliminated) {
            throw std::runtime_error("Couldn't reduce a triangular matrix");
        }
        m_file.completeFiles();
    }
}

/**
 * Tries to xor-combine the given functional with existing functionals,
 * such as the resulting functional is d(f)-unique compared to the other
 * functionals.
 *
 * e.g.
 * If you have these functionals:
 * f1 = 001010000
 * f2 = 000001011
 *
 * and you get this functional:
 * f  = 001001110
 *
 * Then the reduced form is
 * f' = f ^ f1 = 000011110
 *
 * @param functional new functional
 * @param to_xor_functionals (output) functionals that have to be xor-ed
 *   to the specified functional in order to get the reduced form
 * @return true if reduction was successful - meaning the functional is linearly
 *   independent of the existing ones, false otherwise.
 */
bool PartialAlgorithm::semiReduce(const BitVector& functional,
		std::vector<BitVector>& to_xor_functionals) const
{
	BitVector func = functional;

    const std::vector<BitVector>& matrix = m_file.getFunctionalMatrix();
    size_t setbit = func.find_first();

    while ((setbit != BitVector::npos) && (setbit < matrix.size())) {
    	if (matrix[setbit] == MetaData::EMPTY_FUNCTIONAL) {
    		// TODO(corwin): We might want to continue xoring here so the
    		//  functionals would be sparser. but not sure it's good...
    		break;
    	}
        //Logger::getLog("PartialAlgorithm").debugStream() << "Combine with (setbit=" << setbit << ") " << matrix[setbit];
    	to_xor_functionals.push_back(matrix[setbit]);
    	func ^= matrix[setbit];
    	setbit = func.find_next(setbit);
    }

    if (setbit == BitVector::npos) {
        // either there were no bits set, or we never broke from the loop
        // because chunk contained no useful data
        return false;
    }
	return true;
}

void PartialAlgorithm::gaussElimination() {
    Logger::getLog("PartialAlgorithm").debugStream() << "Eliminating the matrix";

    const MetaData::ChunkFunctionals &matrix = m_file.getMetaData().chunkFunctionals;

    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix.size(); ++j) {
            if (j != i && matrix[j][i] == 1) {
            	// l[j] <- l[j] ^ l[i]
            	std::vector<BitVector> xor_these(2);
            	xor_these[0] = matrix[j];
            	xor_these[1] = matrix[i];
            	m_file.putCombinedChunks(xor_these);
            }
        }
    }

    assert(isIdentityMatrix());

    m_wasEliminated = true;
}

/**
 * (internal) Check if the functionals form an upper diagonal matrix
 * of rank n.
 */
bool PartialAlgorithm::isUpperTriangular() {
    const MetaData::ChunkFunctionals& matrix = m_file.getMetaData().chunkFunctionals;

    for(size_t i = 0; i < matrix.size(); ++i) {
        if (matrix[i] == MetaData::EMPTY_FUNCTIONAL) {
            Logger::getLog("PartialAlgorithm").debugStream()
                << "row " << i << " in matrix is empty!";
            return false;
        }
        if (matrix[i].find_first() != i) {
            Logger::getLog("PartialAlgorithm").debugStream()
                << "row " << i << " in matrix is not in form!";
            return false;
        }
    }

    Logger::getLog("PartialAlgorithm").debugStream()
        << "no rows were empty!";
    return true;
}

bool PartialAlgorithm::isIdentityMatrix() {
  const MetaData::ChunkFunctionals& matrix = m_file.getMetaData().chunkFunctionals;

  for (size_t i = 0; i < matrix.size(); ++i) {
    for (size_t j = 0; j < matrix[i].size(); ++j) {
      if (((i != j) && (matrix[i][j] == 1)) ||
          ((i == j) && (matrix[i][j] == 0))) {
        return false;
      }
    }
  }
  return true;
}

} /* namespace BT2 */


