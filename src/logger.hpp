#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

#ifdef DEBUG
#define INFO_LOG(...)                                   \
    fprintf(                                            \
        stderr,                                         \
        "[\x1b[0;32mINFO\x1b[0m %s %s %s::%d]::",       \
        __TIME__, __FILE__,                             \
        __func__, __LINE__                              \
    );                                                  \
    fprintf(stderr, __VA_ARGS__);                       \
    fprintf(stderr, "\n");
#else
#define INFO_LOG(...)
#endif

#endif
