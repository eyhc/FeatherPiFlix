#include "core/sort.h"

#include <iostream>
#include <cassert>

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

void title_sort() {
    core::sorting::sort(&vm, core::sorting::sort_by_title(true));
    assert(vm[0] == &m3 && vm[1] == &m1 && vm[2] == &m2);

    core::sorting::sort(&vm, core::sorting::sort_by_title(false));
    assert(vm[0] == &m2 && vm[1] == &m1 && vm[2] == &m3);
}

void year_sort() {
    core::sorting::sort(&vm, core::sorting::sort_by_year(true));
    assert(vm[0] == &m3 && vm[1] == &m1 && vm[2] == &m2);
    
    core::sorting::sort(&vm, core::sorting::sort_by_year(false));
    assert(vm[0] == &m1 && vm[1] == &m2 && vm[2] == &m3);
}

void category_sort() {
    core::sorting::sort(&vm, core::sorting::sort_by_category(true));
    assert(vm[0] == &m2 && vm[1] == &m3 && vm[2] == &m1);
    
    core::sorting::sort(&vm, core::sorting::sort_by_category(false));
    assert(vm[0] == &m3 && vm[1] == &m1 && vm[2] == &m2);
}

void director_sort() {
    core::sorting::sort(&vm, core::sorting::sort_by_director(true));
    assert(vm[0] == &m1 && vm[1] == &m2 && vm[2] == &m3);
    
    core::sorting::sort(&vm, core::sorting::sort_by_director(false));
    assert(vm[0] == &m3 && vm[1] == &m1 && vm[2] == &m2);
}

void duration_sort() {
    core::sorting::sort(&vm, core::sorting::sort_by_duration(true));
    assert(vm[0] == &m1 && vm[1] == &m2 && vm[2] == &m3);
    
    core::sorting::sort(&vm, core::sorting::sort_by_duration(false));
    assert(vm[0] == &m3 && vm[1] == &m1 && vm[2] == &m2);
}

int main(void)
{
    title_sort();
    year_sort();
    category_sort();
    director_sort();
    duration_sort();

    cout << "TEST SORT : OK" << endl;
    return 0;
}
