/***************************************************************************
* Copyright (c) 2023, Hind Montassif                                  
*                                                                          
* Distributed under the terms of the BSD 3-Clause License.                 
*                                                                          
* The full license is in the file LICENSE, distributed with this software. 
****************************************************************************/

#ifndef XEUS_NELSON_CONFIG_HPP
#define XEUS_NELSON_CONFIG_HPP

// Project version
#define XEUS_NELSON_VERSION_MAJOR 0
#define XEUS_NELSON_VERSION_MINOR 3
#define XEUS_NELSON_VERSION_PATCH 0

// Composing the version string from major, minor and patch
#define XEUS_NELSON_CONCATENATE(A, B) XEUS_NELSON_CONCATENATE_IMPL(A, B)
#define XEUS_NELSON_CONCATENATE_IMPL(A, B) A##B
#define XEUS_NELSON_STRINGIFY(a) XEUS_NELSON_STRINGIFY_IMPL(a)
#define XEUS_NELSON_STRINGIFY_IMPL(a) #a

#define XEUS_NELSON_VERSION XEUS_NELSON_STRINGIFY(XEUS_NELSON_CONCATENATE(XEUS_NELSON_VERSION_MAJOR,   \
                 XEUS_NELSON_CONCATENATE(.,XEUS_NELSON_CONCATENATE(XEUS_NELSON_VERSION_MINOR,   \
                                  XEUS_NELSON_CONCATENATE(.,XEUS_NELSON_VERSION_PATCH)))))

#ifdef _WIN32
    #ifdef XEUS_NELSON_STATIC_LIB
        #define XEUS_NELSON_API
    #else
        #ifdef XEUS_NELSON_EXPORTS
            #define XEUS_NELSON_API __declspec(dllexport)
        #else
            #define XEUS_NELSON_API __declspec(dllimport)
        #endif
    #endif
#else
    #define XEUS_NELSON_API
#endif

#endif

