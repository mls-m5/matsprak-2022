#pragma once

#include <iostream>

namespace logging {

inline bool isVerbose = true;

}

#define vout                                                                   \
    if (logging::isVerbose)                                                    \
    std::cout
