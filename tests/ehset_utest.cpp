#include "EH_set.h"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <numeric>
#include <random>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

// custom double wrapper to check set with minimal requirements
// (std::hash and std::less)
struct double_w {
    double val;

    double_w(double val) : val{val} {}
    double_w() = default;
    double_w(double_w const &) = default;
    double_w &operator=(double_w const &) = default;
};

namespace std {
template <> struct hash<double_w> {
    size_t operator()(double_w const &dw) const {
        return std::hash<double>{}(dw.val);
    }
};

template <> struct equal_to<double_w> {
    bool operator()(double_w const &lhs, double_w const &rhs) const {
        return lhs.val == rhs.val;
    }
};

} // namespace std

TEST_SUITE("EH_set") {

    TEST_CASE_TEMPLATE("DefaultConstructorEmpty", T, double, double_w) {
        EH_set<T> set{};

        CHECK_EQ(set.size(), 0);
        CHECK(set.empty());
    }

    TEST_CASE_TEMPLATE("InitListConstructor", T, double, double_w) {
        EH_set<T> set{1, 2, 3};

        CHECK_EQ(set.size(), 3);
        CHECK(set.count(1));
        CHECK(set.count(3));
        CHECK_FALSE(set.count(4));
    }

    TEST_CASE_TEMPLATE("RangeConstructor", T, double, double_w) {
        std::vector<T> vec{1, 2, 3};
        EH_set<T> set{vec.begin(), vec.end()};

        CHECK_EQ(set.size(), 3);
        CHECK(set.count(1));
        CHECK(set.count(3));
        CHECK_FALSE(set.count(4));
    }

    TEST_CASE_TEMPLATE("CopyConstructor", T, double, double_w) {
        EH_set<T> set{1, 2, 3};
        EH_set<T> copy{set};

        CHECK_EQ(copy.size(), 3);
        CHECK(copy.count(1));
        CHECK(copy.count(3));
        CHECK_FALSE(copy.count(4));
    }

    TEST_CASE_TEMPLATE("Assign", T, double, double_w) {
        EH_set<T> set{1, 2, 3};
        EH_set<T> copy{};

        REQUIRE(copy.empty());

        copy = set;

        CHECK_EQ(copy.size(), 3);
        CHECK(copy.count(1));
        CHECK(copy.count(3));
        CHECK_FALSE(copy.count(4));
    }

    TEST_CASE_TEMPLATE("Assign", T, double, double_w) {
        EH_set<T> set{1, 2, 3};
        EH_set<T> copy{};

        REQUIRE(copy.empty());

        std::swap(copy, set);

        CHECK_EQ(copy.size(), 3);
        CHECK(copy.count(1));
        CHECK(copy.count(3));
        CHECK_FALSE(copy.count(4));

        CHECK(set.empty());
    }

    TEST_CASE_TEMPLATE("AssignInitList", T, double, double_w) {
        EH_set<T> set{};

        REQUIRE(set.empty());

        set = {1, 2, 3};

        CHECK_EQ(set.size(), 3);
        CHECK(set.count(1));
        CHECK(set.count(3));
        CHECK_FALSE(set.count(4));
    }

    TEST_CASE_TEMPLATE("Clear", T, double, double_w) {
        EH_set<T> set{1, 2, 3};

        CHECK_FALSE(set.empty());
        CHECK_EQ(set.size(), 3);

        set.clear();

        CHECK(set.empty());
        CHECK_EQ(set.size(), 0);
    }

    TEST_CASE_TEMPLATE("Find", T, double, double_w) {
        EH_set<T> set{1, 2, 3};

        CHECK_EQ(set.size(), 3);

        auto it = set.find(1);
        CHECK_NE(it, set.end());
        CHECK(std::equal_to<T>{}(*it, 1));

        it = set.find(2);
        CHECK_NE(it, set.end());
        CHECK(std::equal_to<T>{}(*it, 2));

        it = set.find(3);
        CHECK_NE(it, set.end());
        CHECK(std::equal_to<T>{}(*it, 3));

        it = set.find(4);
        CHECK_EQ(it, set.end());
    }

    TEST_CASE_TEMPLATE("InsertSingleValue", T, double, double_w) {
        EH_set<T> set{};

        CHECK(set.empty());

        {
            auto [it, filled] = set.insert(1);
            CHECK(filled);
            CHECK_EQ(it, set.begin());
        }

        {
            auto [it, filled] = set.insert(2);
            CHECK(filled);
            CHECK_EQ(it, set.find(2));
        }

        {
            auto [it, filled] = set.insert(1);
            CHECK_FALSE(filled);
            CHECK_EQ(it, set.find(1));
        }
    }

    TEST_CASE_TEMPLATE("InsertInitList", T, double, double_w) {
        EH_set<T> set{};

        CHECK(set.empty());

        set.insert({1, 2, 3});
        CHECK_EQ(set.size(), 3);
        CHECK(set.count(1));
        CHECK(set.count(3));
        CHECK_FALSE(set.count(4));
    }

    TEST_CASE_TEMPLATE("InsertRange", T, double, double_w) {
        EH_set<T> set{};

        CHECK(set.empty());

        std::vector<T> vec{1, 2, 3};
        set.insert(vec.begin(), vec.end());
        CHECK_EQ(set.size(), 3);
        CHECK(set.count(1));
        CHECK(set.count(3));
        CHECK_FALSE(set.count(4));
    }

    TEST_CASE_TEMPLATE("Erase", T, double, double_w) {
        EH_set<T> set{1, 2, 3};

        CHECK(set.count(1));
        set.erase(1);
        CHECK_FALSE(set.count(1));

        CHECK_NE(set.find(2), set.end());
        set.erase(2);
        CHECK_EQ(set.find(2), set.end());
    }

    TEST_CASE_TEMPLATE("Iter", T, double, double_w) {
        EH_set<T> set{1, 2, 3};

        size_t dist = std::distance(set.begin(), set.end());
        CHECK_EQ(dist, set.size());

        std::vector<T> copy{};
        copy.reserve(set.size());

        for (const T &i : set) {
            CHECK(set.count(i));
            copy.push_back(i);
        }

        auto last = std::unique(copy.begin(), copy.end(), std::equal_to<T>{});
        copy.erase(last, copy.end());
        CHECK_MESSAGE(copy.size() == set.size(),
                      "Iterator yields duplicate values");
    }

    TEST_CASE_TEMPLATE("DuplicateValues", T, double, double_w) {
        EH_set<T> set{1, 1, 2, 2, 3};

        CHECK_EQ(set.size(), 3);

        auto it = set.find(3);
        set.insert(3);
        CHECK_EQ(it, set.find(3));
    }

    TEST_CASE_TEMPLATE("ManyValuesInsert", T, double, double_w) {
        const size_t NUM = 1'000'000;
        std::vector<T> vals(NUM);
        std::iota(vals.begin(), vals.end(), 0);
        std::shuffle(vals.begin(), vals.end(), std::default_random_engine());

        EH_set<T> set{};
        CHECK(set.empty());
        set.insert(vals.begin(), vals.end());
        CHECK_EQ(set.size(), NUM);

        CHECK(set.count(1));

        auto [it, filled] = set.insert(123'456);
        CHECK_EQ(it, set.find(123'456));
        CHECK_FALSE(filled);

        it = set.find(2'000'000);
        CHECK_EQ(it, set.end());
    }

    TEST_CASE_TEMPLATE("ManyValuesFind", T, double, double_w) {
        const size_t NUM = 100'000;
        std::vector<T> vals(NUM);
        std::iota(vals.begin(), vals.end(), 0);
        std::shuffle(vals.begin(), vals.end(), std::default_random_engine());

        EH_set<T> set{};
        CHECK(set.empty());
        set.insert(vals.begin(), vals.end());
        CHECK_EQ(set.size(), NUM);

        for (const T &i : vals) {
            CHECK_NE(set.find(i), set.end());
        }
    }

    TEST_CASE_TEMPLATE("ManyValuesErase", T, double, double_w) {
        const size_t NUM = 100'000;
        std::vector<T> vals(NUM);
        std::iota(vals.begin(), vals.end(), 0);
        std::shuffle(vals.begin(), vals.end(), std::default_random_engine());

        EH_set<T> set{};
        CHECK(set.empty());
        set.insert(vals.begin(), vals.end());
        CHECK_EQ(set.size(), NUM);

        // erase the first 10'000 elements from both containers
        for (auto it{vals.begin()}; it != vals.begin() + 10'000; ++it) {
            set.erase(*it);
            vals.erase(it);
        }

        for (const T &i : vals) {
            CHECK_NE(set.find(i), set.end());
        }
    }
}
