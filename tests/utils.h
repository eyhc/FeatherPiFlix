#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <vector>
#include <string>

#include "core/movie.h"

namespace test
{
    // using curl
    std::vector<std::byte> get_data(std::string url);

    // some real movie
    std::vector<std::unique_ptr<core::data::Movie>> create_collection();
} // namespace test

#endif