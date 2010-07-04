#ifndef BITTORRENT_COMMON_BITVECTOR_H
#define BITTORRENT_COMMON_BITVECTOR_H

#include <boost/dynamic_bitset.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>

#include <vector>

namespace BT2 {

#ifndef __doxygen

typedef boost::dynamic_bitset<unsigned char> BitVector;

#else

class BitVector
{
};

#endif


/*** Utiity Functions ***/

BitVector bitvector_from_string(const std::string& data, size_t bitlen = 0);
std::string bitvector_to_string(const BitVector& bits);


}

/*
 * Boost does not provide serialization for dynamic_bitset
 * The below functions should be able to rectify it
 */
namespace boost {
namespace serialization {

template<class Archive>
void save(Archive &ar, const BT2::BitVector &bv, const unsigned int version) {

    std::vector<BT2::BitVector::block_type> rawData(bv.num_blocks());
    size_t size = bv.size();

    boost::to_block_range(bv, rawData.begin());


    ar & size;

    ar & rawData;

}

template<class Archive>
void load(Archive &ar, BT2::BitVector &bv, const unsigned int version) {

    std::vector<BT2::BitVector::block_type> rawData;
    size_t size;

    ar & size;
    ar & rawData;

    bv.append(rawData.begin(), rawData.end());
    bv.resize(size);


    assert(bv.num_blocks() == rawData.size());

}



}
}

BOOST_SERIALIZATION_SPLIT_FREE(BT2::BitVector)

#ifdef __doxygen
namespace boost {
	/**
	 * @par .robin
	 * unavailable
	 */
	std::ostream& operator<<(std::ostream& os, const BT2::BitVector&);
}
#endif


#endif
