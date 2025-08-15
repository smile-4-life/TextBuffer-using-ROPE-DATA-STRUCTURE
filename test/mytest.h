#ifndef MYTEST_H
#define MYTEST_H

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <sstream>

using namespace std;

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BOLD    "\033[1m"

struct TestCase {
    string name;
    function<string()> func;
    string expected;
};

vector<TestCase> test_cases;
int test_total = 0;
int test_fail = 0;

void REGISTER_SAMPLE(function<void()> sample, string expected, string name) {
    test_cases.push_back({
        name,
        [=]() {
            streambuf* old_buf = cout.rdbuf();
            ostringstream output;
            cout.rdbuf(output.rdbuf());

            sample();

            cout.rdbuf(old_buf);
            return output.str();
        },
        expected
    });
}

void TEST_RUNNER() {
    vector<pair<string, pair<string, string>>> failed_tests;

    for (const auto& test : test_cases) {
        test_total++;
        string got = test.func();
        while (!got.empty() && (got.back() == '\n' || got.back() == '\r')) got.pop_back();
        string exp = test.expected;

        if (got == exp) {
            cout << GREEN << "[PASS] " << RESET << test.name << '\n';
        } else {
            test_fail++;
            failed_tests.push_back({test.name, {exp, got}});
            cout << RED << "[FAIL] " << RESET << test.name << '\n';
        }
    }

    if (!failed_tests.empty()) {
        cout << '\n' << BOLD << YELLOW << "=== Failed Details ===\n" << string(40, '-') << RESET << '\n';
        for (const auto& fail : failed_tests) {
            cout << BOLD << "Test: " << fail.first << RESET << "\n\n";
            cout << BOLD << GREEN "Expected "<< RESET << "|" << RED << " Got:\n\n" << RESET;
            cout << GREEN << fail.second.first << RESET << '\n';
            cout << RED << fail.second.second << RESET << "\n\n";
            cout << string(40, '-') << '\n';
        }
    }

    cout << '\n' << BOLD
         << "Total: " << test_total
         << ", Passed: " << (test_total - test_fail)
         << ", Failed: " << test_fail
         << RESET << '\n';
}

#endif
