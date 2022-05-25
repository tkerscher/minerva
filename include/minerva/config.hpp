#pragma once

#if !defined(MINERVA_STATIC)

#if defined(_WIN32) || defined(__CYGWIN__)

#define MINERVA_API_EXPORT __declspec(dllexport)
#define MINERVA_API_IMPORT __declspec(dllimport)

#else

#define MINERVA_API_EXPORT __attribute__ ((visibility("default")))
#define MINERVA_API_IMPORT __attribute__ ((visibility("default")))

#endif

#else

//Static build needs no attributes
#define MINERVA_API_EXPORT
#define MINERVA_API_IMPORT

#endif

//Import/Export switch
#if defined(MINERVA_EXPORTS)

#define MINERVA_API MINERVA_API_EXPORT

#else

#define MINERVA_API MINERVA_API_IMPORT

#endif

//DEBUG macro
#if !defined(NDEBUG)

#define MINERVA_DEBUG

#endif
