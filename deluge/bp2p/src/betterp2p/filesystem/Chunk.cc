#include "Chunk.h"


#include <iostream>
using namespace std;

namespace {

    std::string &operator^=(std::string &lhs, const std::string &rhs) {
        for(size_t i=0; i<lhs.size(); ++i) {
            lhs[i] ^= rhs[i];
        }

        return lhs;
    }

    std::string operator^(const std::string &lhs, const std::string &rhs) {
        std::string ret(lhs);
        ret ^= rhs;
        return ret;
    }
}

namespace BT2 {

Chunk::Chunk(size_t index, size_t maxChunks, const std::string &data)
    : m_data(data),
      m_functional(maxChunks)
{
    m_functional[index] = 1;
}

Chunk::Chunk(BitVector functional, const std::string &data)
    : m_data(data),
      m_functional(functional)
{ }


Chunk &Chunk::operator^=(const Chunk& rhs) {
    m_functional ^= rhs.m_functional;
    m_data ^= rhs.m_data;

    return *this;
}

Chunk Chunk::operator^(const Chunk& rhs) const {
    Chunk ret(*this);
    return ret ^= rhs;
}

const std::string &Chunk::getData() const {
    return m_data;
}

const BitVector &Chunk::getFunctional() const {
    return m_functional;
}

std::string &Chunk::getData() {
    return m_data;
}

BitVector &Chunk::getFunctional() {
    return m_functional;
}

} /* namespace BT2 */
