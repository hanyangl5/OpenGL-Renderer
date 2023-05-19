#pragma once

#include <iostream>
#include <string>

namespace Log {

inline void Err(std::string errmsg) { throw std::runtime_error(errmsg.c_str()); }

inline void Log() {}

template <typename T, typename... Types> inline void Log(const T &logs, const Types &...args) {
    std::cout << logs;
    if constexpr (sizeof...(args) != 0)
        std::cout << " ";
    Log(args...);
    if constexpr (sizeof...(args) == 0)
        std::cout << std::endl;
}

} // namespace Log
