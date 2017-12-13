/**
 * @file debug.hpp
 * @brief Custom debug utilities
 */

#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <string.h>

/** Show custom debug messages */
#define VERBOSE
/** Current filename string */
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef VERBOSE
#define debugPrint(x) do { std::cout << x; } while (0)
#else
#define debugPrint(x) do {} while (0)
#endif

#ifdef VERBOSE
#define debugPrintTrace(x) do {\
    std::cout << __FILENAME__ << ":" << __LINE__ << ":" << __func__ << ": " << \
    x << "\n";\
} while (0)
#else
#define debugPrintTrace(x) do {} while (0)
#endif

/** Display error and where it occurred */
#define errPrintTrace(x) do {\
    std::cout << "[ERROR] " << __FILENAME__ << ":" << __LINE__ << ":" << __func__ << ": " << \
    x << "\n";\
} while (0)

#endif