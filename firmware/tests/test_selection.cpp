#include <iostream>
#include <cassert>

#include "core/sort.h"

using namespace std;
using namespace core;

data::Movie m1 = data::Movie(
    "ABCDtitre", 2020, "humour", "prodo", "reatis", "aucuns", 
    101, "", data::Cover(), ".."
);

data::Movie m2 = data::Movie(
    "EFGH", 2020, "drame", "prodo", "reatis", "aucuns",
    101, "", data::Cover(), ".."
);

data::Movie m3 = data::Movie(
    "ABCDEFGtitre", 2011, "humour", "prodos", "reatos", "merde", 
    202, "", data::Cover(), ".."
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
