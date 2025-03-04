#include "Debug.h"

namespace Debug {
    std::ostream *debugStream;

    void Initialize(std::ostream *targetStream) {
        debugStream = targetStream;
    }
}