
#include "utility.h"
#include <boost/random.hpp>
#include <ctime>

namespace BT2 {

boost::mt19937::result_type getRandom() {
    static boost::mt19937 rng(static_cast<unsigned int>(std::time(0)));

    return rng();
}

} // namespace BT2
