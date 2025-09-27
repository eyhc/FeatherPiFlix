#include "core/catalog.h"

#include <cassert>
#include <iostream>

using namespace std;
using namespace core;

void test_basic_catalog() {
    unique_ptr<data::Movie> m1 = make_unique<data::Movie>(
        "f1", 0, "", "", "", "", 0, "", data::Cover(), "");
    unique_ptr<data::Movie> m2 = make_unique<data::Movie>(
        "f2", 0, "", "", "", "", 0, "", data::Cover(), "");
    unique_ptr<data::Movie> m3 = make_unique<data::Movie>(
        "f3", 0, "", "", "", "", 0, "", data::Cover(), "");
    unique_ptr<data::Movie> m4 = make_unique<data::Movie>(
        "f4", 0, "", "", "", "", 0, "", data::Cover(), "");
    unique_ptr<data::Movie> m5 = make_unique<data::Movie>(
        "f5", 0, "", "", "", "", 0, "", data::Cover(), "");

    data::Movie* m3_ptr = m3.get();

    BasicCatalog c;
    c.add(move(m1));
    c.add(move(m2));
    c.add(move(m3));
    c.add(move(m4));
    c.add(move(m5));
    c.save("./temp.csv");

    assert(c.size() == 5);
    assert(c.all_movies().at(1).get().title() == "f2");
    const optional<data::movie_ref> fi = c.get_movie("f3");
    assert(fi.has_value() && &fi.value().get() == m3_ptr);

    BasicCatalog c1("./temp.csv");
    assert(c1.size() == 5);
    c1.remove("f4");
    assert(c1.size() == 4);
    assert(c1.all_movies().at(2).get().title() == "f3");
    assert(c1.all_movies().at(3).get().title() == "f5");

    remove("./temp.csv");
}

void test_cached_catalog() {
    unique_ptr<data::Movie> m1 = make_unique<data::Movie>(
        "f1", 0, "", "", "", "", 10, "synopsis1", data::Cover(), "");
    unique_ptr<data::Movie> m2 = make_unique<data::Movie>(
        "f2", 0, "", "", "", "", 20, "synopsis2", data::Cover(), "");
    unique_ptr<data::Movie> m3 = make_unique<data::Movie>(
        "f3", 0, "", "", "", "", 30, "synopsis3", data::Cover(), "");
    unique_ptr<data::Movie> m4 = make_unique<data::Movie>(
        "f4", 0, "", "", "", "", 40, "synopsis4", data::Cover(), "");
    unique_ptr<data::Movie> m5 = make_unique<data::Movie>(
        "f5", 0, "", "", "", "", 50, "synopsis5", data::Cover(), "");

    BasicCatalog c;
    c.add(move(m1));
    c.add(move(m2));
    c.add(move(m3));
    c.add(move(m4));
    c.add(move(m5));
    c.save("./temp.csv");

    CachedCatalog cc("./temp.csv", 2);
    assert(cc.size() == 5);
    
    assert(cc.get_movie("f4").value().get().synopsis() == "synopsis4");
    assert(!cc.is_cached("f1") 
        && !cc.is_cached("f2")
        && !cc.is_cached("f3")
        && cc.is_cached("f4")
        && !cc.is_cached("f5"));

    assert(cc.get_movie("f3").value().get().synopsis() == "synopsis3");
    assert(!cc.is_cached("f1") 
        && !cc.is_cached("f2")
        && cc.is_cached("f3")
        && cc.is_cached("f4")
        && !cc.is_cached("f5"));

    assert(cc.get_movie("f4").value().get().synopsis() == "synopsis4");
    assert(cc.get_movie("f2").value().get().synopsis() == "synopsis2");
    assert(!cc.is_cached("f1")
        && cc.is_cached("f2")
        && !cc.is_cached("f3")
        && cc.is_cached("f4")
        && !cc.is_cached("f5"));

    cc.remove("f4");
    assert(!cc.is_cached("f4"));

    c.get_movie("f1").value().get().set_synopsis("sysy");
    assert(c.get_movie("f1").value().get().synopsis() == "sysy");

    remove("./temp.csv");
}

void test_paged_cached_catalog() {
    BasicCatalog c;
    for (size_t i = 0; i < 52; i++) {
        string title = "f" + to_string(i);
        string synopsis = "synopsis" + to_string(i);
        unique_ptr<data::Movie> m = make_unique<data::Movie>(
            title, 0, "", "",  "", "", 4*(i+5), synopsis, data::Cover(), "");

        c.add(move(m));
    }
    c.save("./temp.csv");
    
    // 20 pages
    PagedCachedCatalog pcc("./temp.csv", 2);
    assert(pcc.size() == 52);

    assert(pcc.get_movie("f25").value().get().synopsis() == "synopsis25");
    for (size_t i = 0; i < 20; i++) assert(!pcc.is_cached("f"+to_string(i)));
    for (size_t i = 20; i < 30; i++) assert(pcc.is_cached("f"+to_string(i)));
    for (size_t i = 30; i < 52; i++) assert(!pcc.is_cached("f"+to_string(i)));

    assert(pcc.get_movie("f30").value().get().synopsis() == "synopsis30");
    for (size_t i = 0; i < 20; i++) assert(!pcc.is_cached("f"+to_string(i)));
    for (size_t i = 20; i < 40; i++) assert(pcc.is_cached("f"+to_string(i)));
    for (size_t i = 40; i < 52; i++) assert(!pcc.is_cached("f"+to_string(i)));

    pcc.get_movie("f20").value().get().synopsis();
    assert(pcc.get_movie("f50").value().get().synopsis() == "synopsis50");
    for (size_t i = 0; i < 20; i++) assert(!pcc.is_cached("f"+to_string(i)));
    for (size_t i = 20; i < 30; i++) assert(pcc.is_cached("f"+to_string(i)));
    for (size_t i = 40; i < 50; i++) assert(!pcc.is_cached("f"+to_string(i)));
    for (size_t i = 50; i < 52; i++) assert(pcc.is_cached("f"+to_string(i)));

    remove("./temp.csv");
}

int main(void) {
    test_basic_catalog();
    test_cached_catalog();
    test_paged_cached_catalog();

    cout << "TEST CATALOGUE : OK" << endl;
    return 0;
}
