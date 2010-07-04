#ifndef BITTORRENT2_FILESYSTEM_CHUNK_H
#define BITTORRENT2_FILESYSTEM_CHUNK_H

#include <stddef.h>
#include <string>

#include <common/BitVector.h>

namespace BT2 {

/**
 * Represents a single chunk of a file
 *
 * The chunk may be a linear combination of multiple logical
 * file chunks, this is denoted as a bitset with bits set
 * at logical positions of the chunk data
 */
class Chunk {

public:
    /**
     * Creates a new chunk at the specified index, for a file with given number of chunks
     *
     * @param index		chunk index
     * @param maxChunks		number of chunks in the file
     * @param data		data of the chunk
     */
    Chunk(size_t index, size_t maxChunks, const std::string &data);

    /**
     * Constructs a chunk from an existing bitvector and data
     */
    Chunk(BitVector functional, const std::string &data);

    /**
     * The ^ operator performs a bitwise-XOR on the chunk's functional vector
     * and byte-wise ^ on the data
     *
     */
    Chunk operator^(const Chunk& rhs) const;
    Chunk &operator^=(const Chunk& lhs);

    /**
     * Returns the chunk's data
     */
    const std::string &getData() const;
    /**
     * Returns the chunk's bit vector
     */
    const BitVector &getFunctional() const;
    /**
     * Returns the chunk's data
     */
    std::string &getData();
    /**
     * Returns the chunk's bit vector
     */
    BitVector &getFunctional();


private:
    /**
     * Chunk's data
     */
    std::string m_data;
    /**
     * Bitvector with bits set in the indices
     * of logical chunks that yielded the data in
     * this chunk
     */
    BitVector m_functional;
}; /* Chunk */


} /* namespace BT2 */

#endif
