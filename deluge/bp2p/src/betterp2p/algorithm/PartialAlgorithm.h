#ifndef BITTORRENT2_ALGORITHM_PARTIALALGORITHM_H
#define BITTORRENT2_ALGORITHM_PARTIALALGORITHM_H

#include <stddef.h>
#include <vector>

#include <common/BitVector.h>
#include <filesystem/FileObject.h>

namespace BT2 {

class Chunk;

/**
 * The PartialAlgorithm knows how to handle partial files.
 *
 * The algorithm wraps getChunk and putChunk of the partial file
 */
class PartialAlgorithm {

public:
    /**
     * Creates a partial file algorithm
     * The FileObject instance is assumed to be managed by somebody else,
     * the algorithm only holds a reference to it
     */
    PartialAlgorithm(FileObject &file);

    virtual ~PartialAlgorithm();

    /**
     * Checks a-priory if specified functional may potentially provide
     * independent information to the file.
     * @param functional functional coefficients
     */
    bool canAddFunctional(const BitVector& functional);

    /**
     * Tries to add a chunk to the partial file
     *
     * @param c chunk to add
     * @return true if the chunk contained useful data
     *         false if the chunk was linearly-dependent on existing chunks
     */
    bool addChunk(const Chunk &c);

    /**
     * Endgame has different rules, clients should not be penalized for sending linearly
     * dependent chunks (or the penalty should be different)
     * @return		true if the file is in endgame mode (i.e misses only a few chunks)
     */
    bool isEndGame();

    /**
     * Constructs a chunk from a random set of functionals.
     * The returned chunk is a combination of existing chunks of the file.
     *
     * If there are no good chunks, a chunk with popcount 0 is returned.
     */
    Chunk getChunk();

    /**
     * Returns true if the matrix has enough information for elimination.
     */
    bool isDone();

    /**
     * This function must be called only if isDone().
     * It will do matrix elimination and mark the file as complete.
     */
    void wrapUp();

private:
    bool semiReduce(const BitVector& functional,
                    std::vector<BitVector>& to_xor_functionals) const;

    bool isUpperTriangular();
    bool isIdentityMatrix();

    void gaussElimination();

private:
    /**
     * Constant for number of remaining chunks below which the endgame mode is activated
     */
    static const size_t END_GAME_CONSTANT;

private:

    /**
     * File which the algorithm operates on
     */
    FileObject& m_file;

    /**
     * Number of generated chunks, used to determine how many functionals
     * we should cram into the chunk
     */
    size_t m_numGeneratedChunks;

    /**
     * Returns true if elimination could be performed
     */
    size_t m_wasEliminated;
}; /* class PartialAlgorithm */

} /* namespace BT2 */

#endif /* BITTORRENT2_ALGORITHM_PARTIALALGORITHM_H */
