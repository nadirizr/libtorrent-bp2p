#include "BitVector.h"

#include <iostream>
using namespace std;

namespace BT2 {


BitVector bitvector_from_string(const std::string& data, size_t bitlen)
{
    if (bitlen == 0) {
        // if no bitlen given, assume all bits are used
        bitlen = data.length() * BitVector::bits_per_block;
    }
    BitVector b(data.length() * BitVector::bits_per_block);
    boost::from_block_range(
        (const unsigned char*)data.c_str(),
        (const unsigned char*)(data.c_str()+data.length()),
        b);
    b.resize(bitlen);
    return b;
}

std::string bitvector_to_string(const BitVector& bits)
{
    std::string s(bits.num_blocks(), '\0');
    boost::to_block_range(
        bits,
        const_cast<unsigned char*>((unsigned char*)s.c_str()));
    return s;
}


} // namespace BT2

