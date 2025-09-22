#include "core/cover.h"
#include "core/movie.h"
#include "core/utils.h"

#include <fstream>
#include <iostream>
#include <cassert>

using namespace std;
using namespace core;

void test_cover() {
    data::Cover img("normal.png", "square.png");
    assert(img.to_string() == "Cover : normal.png | square.png");

    img.set_normal_path("normal.jpg");
    img.set_square_path("square.jpg");

    assert(img.normal_path() == "normal.jpg");
    assert(img.square_path() == "square.jpg");
}

void test_movie_accessors() {
    data::FullMovie m(
        "flix", 2015, "toto", "drama", data::Cover("path1.png", "path2.png"),
        "titi", "", "empty movie", 135, "data/movie.mp4"
    );

    assert(m.title() == "flix");
    assert(m.year() == 2015);
    assert(m.producer() == "toto");
    assert(m.category() == "drama");
    assert(m.cover().normal_path() == "path1.png");
    assert(m.cover().square_path() == "path2.png");
    assert(m.director() == "titi");
    assert(m.actors().empty());
    assert(m.synopsis() == "empty movie");
    assert(m.duration() == 135);
    assert(m.duration_str() == "2h 15min");
    assert(m.video_file() == "data/movie.mp4");
}

void test_movie_to_string() {
    data::FullMovie m(
        "Flix", 2015, "", "drama", data::Cover("path1.png", "path2.png"),
        "toto", "", "empty movie", 135, "data/movie.mp4"
    );

    assert(m.to_string() == "Flix - toto - drama - 2015");

    m.set_producer("titi");
    m.set_director("");
    assert(m.to_string() == "Flix - titi - drama - 2015");
}

void test_movie_mutators() {
    data::FullMovie m(
        "flix", 2015, "toto", "drama", data::Cover("path1.png", "path2.png"),
        "titi", "", "empty movie", 135, "data/movie.mp4"
    );

    string long_summary = "abcdefghijklmnopqrstuvwxyz012_\
        abcdefghijklmnopqrstuvwxyz012_\
        abcdefghijklmnopqrstuvwxyz012_\
        abcdefghijklmnopqrstuvwxyz012_";

    m.set_year(1850);
    m.set_producer("tata");
    m.set_category("comedy");
    m.set_cover(data::Cover());
    m.set_director("toto");
    m.set_actors("jack & john");
    m.set_synopsis(long_summary);
    m.set_duration(25);
    m.set_video_file("movie2.mp4");

    assert(m.title() == "flix");
    assert(m.year() == 1850);
    assert(m.producer() == "tata");
    assert(m.category() == "comedy");
    assert(m.cover().normal_path() == data::Cover().normal_path());
    assert(m.cover().square_path() == data::Cover().square_path());
    assert(m.director() == "toto");
    assert(m.actors() == "jack & john");
    assert(m.synopsis() == long_summary);
    assert(m.duration() == 25);
    assert(m.duration_str() == "25min");
    assert(m.video_file() == "movie2.mp4");
}

void test_movie_equality() {
    data::FullMovie m(
        "Flix", 2015, "", "drama", data::Cover("path1.png", "path2.png"),
        "toto", "", "empty movie", 135, "data/movie.mp4"
    );

    assert(m.equals(m));

    data::FullMovie m2(
        "Flix", 2015, "", "drama", data::Cover("path1.png", "path2.png"),
        "toto", "", "empty movie", 135, "data/movie.mp4"
    );

    assert(&m != &m2);
    assert(m.equals(m2));

    data::FullMovie m3(
        "PiFlix", 2015, "", "drama", data::Cover("path1.png", "path2.png"),
        "toto", "", "empty movie", 135, "data/movie.mp4"
    );
    assert(!m3.equals(m));
}

void test_lazy_synopsis() {
    const string synopsis1 = "Long time ago...";
    const string synopsis2 = "efg\nh";
    const string synopsis3 = "\"go\r\nnot'";

    // create correct csv files
    const string file = "synopsis.csv";
    ofstream out(file);
    csv::write_row(out, {"title", "synopsis", "other"});
    csv::write(out, {
        {"f1", synopsis1, "bad"}, 
        {"f2", synopsis2, "good..\n."},
        {"f3", synopsis3, "ok"}
    });
    out.close();

    const string file2 = "synopsis2.csv";
    out.open(file2);
    csv::write_row(out, {"synopsis", "other", "title"});
    csv::write(out, {
        {synopsis1, "bad", "f4"}, 
        {synopsis2, "good..\n.", "f5"},
    });
    out.close();

    /// create lazy movies with previous file
    auto f1 = data::LazyMovie(
        "f1", 2015, "", "", data::Cover(), "", "", file, 10, "");
    auto f2 = data::LazyMovie(
        "f2", 2015, "", "", data::Cover(), "", "", file, 10, "");
    auto f3 = data::LazyMovie(
        "f3", 2015, "", "", data::Cover(), "", "", file, 10, "");
    auto f4 = data::LazyMovie(
        "f4", 2015, "", "", data::Cover(), "", "", file2, 10, "");
    auto f5 = data::LazyMovie(
        "f5", 2015, "", "", data::Cover(), "", "", file2, 10, "");

    // check synopsis correctness
    assert(f1.synopsis() == synopsis1);
    assert(f2.synopsis() == synopsis2);
    assert(f3.synopsis() == synopsis3);
    assert(f4.synopsis() == synopsis1);
    assert(f5.synopsis() == synopsis2);

    f2.set_synopsis("toto titi \n tata");
    assert(f2.synopsis() == "toto titi \n tata");
    remove(file.c_str());
    remove(file2.c_str());
}

int main(void) {
    test_cover();
    test_movie_accessors();
    test_movie_to_string();
    test_movie_mutators();
    test_movie_equality();
    test_lazy_synopsis();

    cout << "TEST DATA : OK"  << endl;
    return 0;
}
