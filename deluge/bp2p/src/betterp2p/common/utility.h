
#ifndef COMMON_UTILITY_H_
#define COMMON_UTILITY_H_

#include <boost/random.hpp>

namespace BT2 {

    // TODO(mishas): check what mt19937 returns
    boost::mt19937::result_type getRandom();

} // namespace BT2

#endif // COMMON_UTILITY_H_
