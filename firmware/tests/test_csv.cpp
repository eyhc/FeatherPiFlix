#include "core/csv.h"

#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;
using namespace core;

void test_write() {
    stringbuf sb;
    ostream out(&sb);

    csv::write_row(out, {"A", "b", "c"});
    csv::write(out, {
        {"1", "abc", "&éàçê"}, 
        {"154", "efg\nh", "bon..."},
        {"48", "\"go\r\non'", "ok"}
    });

    assert(sb.str().find(
        "\"A\",\"b\",\"c\"\n"
        "\"1\",\"abc\",\"&éàçê\"\n"
        "\"154\",\"efg\nh\",\"bon...\"\n"
        "\"48\",\"\"\"go\r\non'\",\"ok\""
    ) != string::npos);
}

void test_read() {
    stringbuf sb;
    ostream out(&sb);

    vector<vector<string>> data = {
        {"1", "abc", "&éàçê"}, 
        {"154", "efg\nh", "bon..."},
        {"48", "\"go\r\non'", "ok"}
    };

    csv::write_row(out, {"A", "b", "c"});
    csv::write(out, data);
    out.flush();

    istream in(&sb);

    int i = 0;
    csv::row_callback rc = [&i, data](vector<string> r) {
        if (i == 0)      assert(r == vector<string>({"A", "b", "c"}));
        else if (i == 1) assert(r == data[0]);
        else if (i == 2) assert(r == data[1]);
        else if (i == 3) assert(r == data[2]);
        else             assert(false);
        i++;
    };

    csv::read(in, rc);
    assert(i == 4);
}

#define restart(in) {in.clear(); in.seekg(in.beg);}

void test_get_field() {
    vector<vector<string>> data = {
        {"1", "abc", "&éàçê"}, 
        {"154", "efg\nh", "bon..."},
        {"48", "\"go\r\non'", "ok"}
    };

    stringbuf sb;
    ostream out(&sb);
    csv::write_row(out, {"A", "b", "c"});
    csv::write(out, data);
    out.flush();

    istream in(&sb);

    auto res = csv::get_field(in, "48", "A", "c");
    assert(res.has_value() && res.value() == "ok");

    restart(in);
    res = csv::get_field(in, "48", "A", "C");
    assert(!res.has_value());

    restart(in);
    res = csv::get_field(in, "49", "A", "c");
    assert(!res.has_value());

    restart(in);
    res = csv::get_field(in, "154", "A", "c");
    assert(res.has_value() && res.value() == "bon...");

    restart(in);
    res = csv::get_field(in, "bon...", "c", "A");
    assert(res.has_value() && res.value() == "154");

    restart(in);
    res = csv::get_field(in, "\"go\r\non'", "b", "c");
    assert(res.has_value() && res.value() == "ok");
}

void test_edit_field() {
    vector<vector<string>> data = {
        {"1", "abc", "&éàçê"}, 
        {"154", "efg\nh", "bon..."},
        {"48", "\"go\r\non'", "ok"}
    };

    stringstream ss1, ss2, ss3;

    csv::write_row(ss1, {"A", "b", "c"});
    csv::write(ss1, data);

    restart(ss1);
    csv::edit_field(ss1, ss2, "154", "efgh", "A", "b");
    restart(ss2);
    csv::edit_field(ss2, ss3, "ok", "non", "c", "b");

    restart(ss3);
    size_t counter = 0;
    auto rc = [&](vector<string> row) {
        for (const auto &cell: row) {
            switch (counter) {
                case 0: assert(cell == "A"); break;
                case 1: assert(cell == "b"); break;
                case 2: assert(cell == "c"); break;
                case 3: assert(cell == "1"); break;
                case 4: assert(cell == "abc"); break;
                case 5: assert(cell == "&éàçê"); break;
                case 6: assert(cell == "154"); break;
                case 7: assert(cell == "efgh"); break;
                case 8: assert(cell == "bon..."); break;
                case 9: assert(cell == "48"); break;
                case 10: assert(cell == "non"); break;
                case 11: assert(cell == "ok"); break;
                default: assert(false); break;
            }
            counter++;
        }
    };

    csv::read(ss3, rc);
    assert(counter == 12);
}

int main(void) {
    test_write();
    test_read();
    test_get_field();
    test_edit_field();

    cout << "TEST CSV : OK" << endl;
    return 0;
}