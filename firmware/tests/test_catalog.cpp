#include "core/catalog.h"

#include <cassert>
#include <iostream>

using namespace std;
using namespace core;

void test_base_catalog() {
    unique_ptr<data::Movie> m1 = make_unique<data::FullMovie>(
        "f1", 0, "", "", data::Cover(), "", "", "", 0, "");
    unique_ptr<data::Movie> m2 = make_unique<data::FullMovie>(
        "f2", 0, "", "", data::Cover(), "", "", "", 0, "");
    unique_ptr<data::Movie> m3 = make_unique<data::FullMovie>(
        "f3", 0, "", "", data::Cover(), "", "", "", 0, "");
    unique_ptr<data::Movie> m4 = make_unique<data::FullMovie>(
        "f4", 0, "", "", data::Cover(), "", "", "", 0, "");
    unique_ptr<data::Movie> m5 = make_unique<data::FullMovie>(
        "f5", 0, "", "", data::Cover(), "", "", "", 0, "");

    data::Movie* m3_ptr = m3.get();

    core::Catalog c;
    c.add(std::move(m1));
    c.add(move(m2));
    c.add(move(m3));
    c.add(move(m4));
    c.add(move(m5));
    c.save("./temp.csv");

    assert(c.size() == 5);
    assert(c.all_movies().at(1)->title() == "f2");
    const data::Movie *fi = c.get_movie("f3");
    assert(fi != nullptr && fi == m3_ptr);

    core::Catalog c1 = core::Catalog("./temp.csv");
    assert(c1.size() == 5);
    c1.remove("f4");
    assert(c1.size() == 4);
    assert(c1.all_movies().at(2)->title() == "f3");
    assert(c1.all_movies().at(3)->title() == "f5");

    remove("./temp.csv");
}



int main(void) {
    test_base_catalog();

    cout << "TEST CATALOGUE : OK" << endl;
    return 0;
}
