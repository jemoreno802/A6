#include "set.hpp"

int main() {

    // Some miscellaneous code to get you started on testing your sets.
    // The following should work:

    std_simple_set<int> R;
    R += 3;
    cout << "3 is " << (R.contains(3) ? "" : "not ") << "in R\n";
    cout << "5 is " << (R.contains(5) ? "" : "not ") << "in R\n";

    simple_set<double>* S = new std_simple_set<double>();
    *S += 3.14;
    cout << "pi is " << (S->contains(3.14) ? "" : "not ") << "in S\n";
    cout << "e is " << (S->contains(2.718) ? "" : "not ") << "in S\n";

    std_simple_set<string> U;
    U += "hello";
    cout << "\"hello\" is " << (U.contains("hello") ? "" : "not ") << "in U\n";
    cout << "\"foo\" is " << (U.contains("foo") ? "" : "not ") << "in U\n";

    range<string> r1("a", true, "f", true);
    cout << "\"b\" is " << (r1.contains("b") ? "" : "not ") << "in r1\n";
    cout << "\"aaa\" is " << (r1.contains("aaa") ? "" : "not ") << "in r1\n";
    cout << "\"faa\" is " << (r1.contains("faa") ? "" : "not ") << "in r1\n";

    range<const char*, lexico_less> r2("a", true, "f", true);
    cout << "\"b\" is " << (r2.contains("b") ? "" : "not ") << "in r2\n";
    cout << "\"aaa\" is " << (r2.contains("aaa") ? "" : "not ") << "in r2\n";
    cout << "\"faa\" is " << (r2.contains("faa") ? "" : "not ") << "in r2\n";

    // The following will not work correctly yet:

    range_set<int>* X = new std_range_set<int>();
    *X += range<int>(5, true, 8, false);
    if (X->contains(4)) cout << "4 is in X\n";
    if (X->contains(5)) cout << "5 is in X\n";      // should print
    if (X->contains(6)) cout << "6 is in X\n";      // should print
    if (X->contains(7)) cout << "7 is in X\n";      // should print
    if (X->contains(8)) cout << "8 is in X\n";
    if (X->contains(9)) cout << "9 is in X\n";
    *X -= range<int>(6, true, 10, false);
    if (X->contains(4)) cout << "4 is now in X\n";
    if (X->contains(5)) cout << "5 is now in X\n";      // should print
    if (X->contains(6)) cout << "6 is now in X\n";
    if (X->contains(7)) cout << "7 is now in X\n";
    if (X->contains(8)) cout << "8 is now in X\n";
    if (X->contains(9)) cout << "9 is now in X\n";

    simple_set<weekday>* V = new carray_simple_set<weekday>(mon, (weekday)5);

    hashed_simple_set<int, cast_to_int<int>> H(100);

    // bin_search_simple_set<double> J(100);

    carray_simple_set<int>* test = new carray_simple_set<int>(1,84);
    (*test).print();
    *test += 2;
    *test += 63;
    (*test).print();
    *test -= 2;
    *test -= 63;
    (*test).print();

    hashed_simple_set<double>* hashtest = new hashed_simple_set<double>(10);
    cout << "has 4.f: " << hashtest->contains(4.f) << std::endl;
    *hashtest += 5.f;
    *hashtest += 4.f;
    cout << "has 4.f: " << hashtest->contains(4.f) << std::endl;
    *hashtest -= 4.f;
    cout << "has 4.f: " << hashtest->contains(4.f) << std::endl;

    bin_search_simple_set<int>* bin_test = new bin_search_simple_set<int>(5);
    *bin_test += 2;
    *bin_test += -1;
    *bin_test += 5;
    *bin_test += 3;
    *bin_test += 4;
    *bin_test -= 3;
    *bin_test += 6;
    *bin_test -= 3;
    
    cout << "bin search set has 3:" << bin_test->contains(3);
    
    range<int> range1(1.f, true, 10.f, true);
    range<int> range2(10.f, true, 15.f, true);
    range<int> range3(8.f, true, 12.f, true);
    carray_range_set<int>* testrange = new carray_range_set<int>(1, 100);
    *testrange += range1;
    *testrange += range2;
    cout << testrange->contains(10);
    *testrange -= range3;
    cout << testrange->contains(10);
}
