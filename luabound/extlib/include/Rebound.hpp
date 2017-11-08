/**
 * Copyright Sean Moss (c) 2017
 * Licensed under the GNU GPL v3 license, the text of which can be found in the LICENSE file in
 *     this repository. If a copy of this license was not included, it can be found at 
 *     <http://www.gnu.org/licenses/>.
 *
 * This file provides a C++ interface to the Rebound C API. Unfortunately, the Rebound header
 *     file is not C++ friendly (mostly because of the use of the `restrict` keyword), so this
 *     header allows for safe C++ use. Do not include "rebound.h" directly. 
 */

#ifndef REBOUND_CPLUSPLUS_INTERFACE_
#define REBOUND_CPLUSPLUS_INTERFACE_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Since `restrict` is a C99 keyword only, it is not core to C++ and is the main source of the
//     compatibility issues with using the C API in C++. However, most C++ compilers do support
//     the `__restrict__` extension keyword, which is functionally equivalent. So a simple 
//     redefinition is enough to fix this issue. This should work on all major compilers, but
//     please report if you come across one that does not support this.
#define restrict __restrict__

#include <rebound.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // REBOUND_CPLUSPLUS_INTERFACE_