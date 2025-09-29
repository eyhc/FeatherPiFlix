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

vector<data::movie_ref> vm = {ref(m1), ref(m2), ref(m3)};

void title_sort() {
    core::sorting::sort(vm, core::sorting::sort_by_title(true));
    assert(&vm[0].get() == &m3 && &vm[1].get() == &m1 && &vm[2].get() == &m2);

    core::sorting::sort(vm, core::sorting::sort_by_title(false));
    assert(&vm[0].get() == &m2 && &vm[1].get() == &m1 && &vm[2].get() == &m3);
}

void year_sort() {
    core::sorting::sort(vm, core::sorting::sort_by_year(true));
    assert(&vm[0].get() == &m3 && &vm[1].get() == &m1 && &vm[2].get() == &m2);
    
    core::sorting::sort(vm, core::sorting::sort_by_year(false));
    assert(&vm[0].get() == &m1 && &vm[1].get() == &m2 && &vm[2].get() == &m3);
}

void category_sort() {
    core::sorting::sort(vm, core::sorting::sort_by_category(true));
    assert(&vm[0].get() == &m2 && &vm[1].get() == &m3 && &vm[2].get() == &m1);
    
    core::sorting::sort(vm, core::sorting::sort_by_category(false));
    assert(&vm[0].get() == &m3 && &vm[1].get() == &m1 && &vm[2].get() == &m2);
}

void director_sort() {
    core::sorting::sort(vm, core::sorting::sort_by_director(true));
    assert(&vm[0].get() == &m1 && &vm[1].get() == &m2 && &vm[2].get() == &m3);
    
    core::sorting::sort(vm, core::sorting::sort_by_director(false));
    assert(&vm[0].get() == &m3 && &vm[1].get() == &m1 && &vm[2].get() == &m2);
}

void duration_sort() {
    core::sorting::sort(vm, core::sorting::sort_by_duration(true));
    assert(&vm[0].get() == &m1 && &vm[1].get() == &m2 && &vm[2].get() == &m3);
    
    core::sorting::sort(vm, core::sorting::sort_by_duration(false));
    assert(&vm[0].get() == &m3 && &vm[1].get() == &m1 && &vm[2].get() == &m2);
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
