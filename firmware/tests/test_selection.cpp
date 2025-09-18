#include <iostream>
#include <cassert>

#include "core/sort.h"

using namespace std;
using namespace core;

data::FullMovie m1 = data::FullMovie(
    "ABCDtitre", 2020, "prodo", "humour", data::Cover(),
    "reatis", "aucuns", "", 101, ".."
);

data::FullMovie m2 = data::FullMovie(
    "EFGH", 2020, "prodo", "drame", data::Cover(),
    "reatis", "aucuns", "", 105, ".."
);

data::FullMovie m3 = data::FullMovie(
    "ABCDEFGtitre", 2011, "prodos", "humour", data::Cover(),
    "reatos", "merde", "", 202, ".."
);

vector<data::Movie *> vm = {&m1, &m2, &m3};

int main(void)
{
    vector<data::Movie *> res;
    res = selection::select_by_title(vm, "ABCDEFGtitre");
    assert(res.size() == 1);
    assert(res[0]->title() == "ABCDEFGtitre");

    res = selection::select_by_year(vm, 2020);
    assert(res.size() == 2);
    assert(res[0] == &m1);
    assert(res[1] == &m2);

    res = selection::select_by_category(vm, "humour");
    assert(res.size() == 2);
    assert(res[0] == &m1);
    assert(res[1] == &m3);

    res = selection::select_by_duration(vm, 100, 15);
    assert(res.size() == 2);
    assert(res[0] == &m1);
    assert(res[1] == &m2);

    res = selection::select_by_director(vm, "prodo");
    assert(res.size() == 0);

    res = selection::select_by_director(vm, "reatis");
    assert(res.size() == 2);
    assert(res[0] == &m1);
    assert(res[1] == &m2);

    cout << "TEST SELECTION : OK" << endl;
    return 0;
}
