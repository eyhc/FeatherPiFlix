#include <algorithm>

#include "core/movie.h"
#include "core/sort.h"

using namespace core;
using namespace std;

void sorting::sort(vector<data::movie_ref> &movies, sort_func fct) {
    std::sort(movies.begin(), movies.end(), fct);
}

/*
 * Sorting functions 
 */
sorting::sort_func sorting::sort_by_title(bool asc) {
    if (asc)
        return [](const data::movie_ref m1, const data::movie_ref m2)-> bool {
            return m1.get().title() < m2.get().title();
        };
    else
        return [](const data::movie_ref m1, const data::movie_ref m2)-> bool {
            return sort_by_title(true)(m2, m1);
        };
}

sorting::sort_func sorting::sort_by_year(bool asc) {
    if (asc)
        return [](const data::movie_ref m1, const data::movie_ref m2)-> bool {
            return (m1.get().year() < m2.get().year())
            || (m1.get().year() == m2.get().year() 
                && sort_by_title(true)(m1, m2));
        };
    else
        return [](const data::movie_ref m1, const data::movie_ref f2)-> bool {
            return (m1.get().year() > f2.get().year())
            || (m1.get().year() == f2.get().year() 
                && sort_by_title(true)(m1, f2));
        };
}

sorting::sort_func sorting::sort_by_category(bool asc) {
    if (asc)
        return [](const data::movie_ref m1, const data::movie_ref m2)-> bool {
            return (m1.get().category() < m2.get().category())
            || (m1.get().category() == m2.get().category() 
                && sort_by_title(true)(m1, m2));
        };
    else
        return [](const data::movie_ref m1, const data::movie_ref m2)-> bool {
            return (m1.get().category() > m2.get().category())
            || (m1.get().category() == m2.get().category() 
                && sort_by_title(true)(m1, m2));
        };
}

sorting::sort_func sorting::sort_by_director(bool asc) {
    if (asc)
        return [](const data::movie_ref m1, const data::movie_ref m2)-> bool {
            return (m1.get().director() < m2.get().director())
            || (m1.get().director() == m2.get().director() 
                && sort_by_title(true)(m1, m2));
        };
    else
        return [](const data::movie_ref m1, const data::movie_ref m2)-> bool {
            return (m1.get().director() > m2.get().director())
            || (m1.get().director() == m2.get().director() 
                && sort_by_title(true)(m1, m2));
        };
}

sorting::sort_func sorting::sort_by_duration(bool asc) {
    if (asc)
        return [](const data::movie_ref m1, const data::movie_ref m2)-> bool {
            return (m1.get().duration() < m2.get().duration())
            || (m1.get().duration() == m2.get().duration()
                && sort_by_title(true)(m1, m2));
        };
    else
        return [](const data::movie_ref m1, const data::movie_ref m2)-> bool {
            return (m1.get().duration() > m2.get().duration())
            || (m1.get().duration() == m2.get().duration()
                && sort_by_title(true)(m1, m2));
        };
}


/******************************************************************************/

vector<data::movie_ref> selection::select_by_title(
    const vector<data::movie_ref> &movies, const string &val
) {
    vector<data::movie_ref> vm = vector<data::movie_ref>();
    for (data::movie_ref m: movies)
        if (m.get().title() == val) vm.push_back(m);
    return vm;
}

vector<data::movie_ref> selection::select_by_year(
    const vector<data::movie_ref> &movies, int val, int delta
) {
    vector<data::movie_ref> vm = vector<data::movie_ref>();

    for (data::movie_ref m: movies)
        if (val - delta <= m.get().year() && m.get().year() <= val + delta)
            vm.push_back(m);
    
    return vm;
}

vector<data::movie_ref> selection::select_by_director(
    const vector<data::movie_ref> &movies, const string &val
) {
    vector<data::movie_ref> vm = vector<data::movie_ref>();
    for (data::movie_ref m: movies)
        if (m.get().director() == val) vm.push_back(m);
    return vm;
}

vector<data::movie_ref> selection::select_by_category(
    const vector<data::movie_ref> &movies, const string &val
) {
    vector<data::movie_ref> vm = vector<data::movie_ref>();
    for (data::movie_ref m: movies)
        if (m.get().category() == val) vm.push_back(m);
    return vm;
}

vector<data::movie_ref> selection::select_by_duration(
    const vector<data::movie_ref> &movies, int val, int delta
) {
    vector<data::movie_ref> vm = vector<data::movie_ref>();

    for (data::movie_ref m: movies)
        if (
            val - delta <= m.get().duration() 
            && m.get().duration() <= val + delta
        )
            vm.push_back(m);

    return vm;
}
