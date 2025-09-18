#include <algorithm>

#include "core/movie.h"
#include "core/sort.h"

using namespace core;
using namespace std;

void sorting::sort(vector<data::Movie *> *movies, sort_func fct) {
    std::sort(movies->begin(), movies->end(), fct);
}

/*
 * Sorting functions 
 */
sorting::sort_func sorting::sort_by_title(bool asc) {
    if (asc)
        return [](const data::Movie *m1, const data::Movie *m2)-> bool {
            return m1->title() < m2->title();
        };
    else
        return [](const data::Movie *m1, const data::Movie *m2)-> bool {
            return sort_by_title(true)(m2, m1);
        };
}

sorting::sort_func sorting::sort_by_year(bool asc) {
    if (asc)
        return [](const data::Movie *m1, const data::Movie *m2)-> bool {
            return (m1->year() < m2->year())
            || (m1->year() == m2->year() && sort_by_title(true)(m1, m2));
        };
    else
        return [](const data::Movie *m1, const data::Movie *f2)-> bool {
            return (m1->year() > f2->year())
            || (m1->year() == f2->year() && sort_by_title(true)(m1, f2));
        };
}

sorting::sort_func sorting::sort_by_category(bool asc) {
    if (asc)
        return [](const data::Movie *m1, const data::Movie *m2)-> bool {
            return (m1->category() < m2->category())
            || (m1->category() == m2->category() && sort_by_title(true)(m1, m2));
        };
    else
        return [](const data::Movie *m1, const data::Movie *m2)-> bool {
            return (m1->category() > m2->category())
            || (m1->category() == m2->category() && sort_by_title(true)(m1, m2));
        };
}

sorting::sort_func sorting::sort_by_director(bool asc) {
    if (asc)
        return [](const data::Movie *m1, const data::Movie *m2)-> bool {
            return (m1->director() < m2->director())
            || (m1->director() == m2->director() && sort_by_title(true)(m1, m2));
        };
    else
        return [](const data::Movie *m1, const data::Movie *m2)-> bool {
            return (m1->director() > m2->director())
            || (m1->director() == m2->director() && sort_by_title(true)(m1, m2));
        };
}

sorting::sort_func sorting::sort_by_duration(bool asc) {
    if (asc)
        return [](const data::Movie *m1, const data::Movie *m2)-> bool {
            return (m1->duration() < m2->duration())
            || (m1->duration() == m2->duration() && sort_by_title(true)(m1, m2));
        };
    else
        return [](const data::Movie *m1, const data::Movie *m2)-> bool {
            return (m1->duration() > m2->duration())
            || (m1->duration() == m2->duration() && sort_by_title(true)(m1, m2));
        };
}


/*************************************************************************************/

vector<data::Movie *> selection::select_by_title(
    const vector<data::Movie *> &movies, const string &val
) {
    vector<data::Movie *> vm = vector<data::Movie *>();
    for (data::Movie *m: movies)
        if (m->title() == val) vm.push_back(m);
    return vm;
}

vector<data::Movie *> selection::select_by_year(
    const vector<data::Movie *> &movies, int val, int delta
) {
    vector<data::Movie *> vm = vector<data::Movie *>();

    for (data::Movie *m: movies)
        if (val - delta <= m->year() && m->year() <= val + delta)
            vm.push_back(m);
    
    return vm;
}

vector<data::Movie *> selection::select_by_director(
    const vector<data::Movie *> &movies, const string &val
) {
    vector<data::Movie *> vm = vector<data::Movie *>();
    for (data::Movie *m: movies)
        if (m->director() == val) vm.push_back(m);
    return vm;
}

vector<data::Movie *> selection::select_by_category(
    const vector<data::Movie *> &movies, const string &val
) {
    vector<data::Movie *> vm = vector<data::Movie *>();
    for (data::Movie *m: movies)
        if (m->category() == val) vm.push_back(m);
    return vm;
}

vector<data::Movie *> selection::select_by_duration(
    const vector<data::Movie *> &movies, int val, int delta
) {
    vector<data::Movie *> vm = vector<data::Movie *>();

    for (data::Movie *m: movies)
        if (val - delta <= m->duration() && m->duration() <= val + delta) 
            vm.push_back(m);

    return vm;
}
