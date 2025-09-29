#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <vector>
#include <string>

namespace test
{
    // using curl
    std::vector<std::byte> get_data(std::string url);
} // namespace test

#endif