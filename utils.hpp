//
// Created by Garrett on 12/29/2021.
//

#ifndef VK_BASICS_UTILS_HPP
#define VK_BASICS_UTILS_HPP

#include <functional>

// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
};

#endif //VK_BASICS_UTILS_HPP
