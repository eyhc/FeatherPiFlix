#include "core/cover.h"

using namespace std;
using namespace core::data;

Cover::Cover()
{
    _normal = "./assets/default_normal.jpg";
    _square = "./assets/default_square.jpg";
}

Cover::Cover(
    const filesystem::path &normal_path,
    const filesystem::path &square_path
) {
    _normal = normal_path;
    _square = square_path;
}

filesystem::path Cover::normal_path() const { return _normal; }
filesystem::path Cover::square_path() const { return _square; }

void Cover::set_normal_path(const filesystem::path &path) { _normal = path; }
void Cover::set_square_path(const filesystem::path &path) { _square = path; }

std::string Cover::to_string() const {
    return "Cover : " + _normal.generic_string() + " | " + _square.generic_string();
}
