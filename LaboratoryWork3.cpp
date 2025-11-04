// ���������: ���������� � GoogleTest (gtest) ��� ������� ������.
// ������ ���������� (linux):
// g++ -std=c++17 quicksort_tests.cpp -isystem /usr/local/include -pthread -lgtest -lgtest_main -o quicksort_tests
// ����� ./quicksort_tests

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <random>
#include <string>
#include <vector>
#include <type_traits>
#include <utility>
#include <iostream>

#ifdef UNIT_TESTS
#include <gtest/gtest.h>
#endif

template<typename T>
inline void move_swap(T& a, T& b) {
    T tmp = std::move(a);
    a = std::move(b);
    b = std::move(tmp);
}

template<typename T, typename Compare>
void insertion_sort(T* first, T* last, Compare comp) {
    if (first >= last) return;
    for (T* it = first + 1; it < last; ++it) {
        T key = std::move(*it);
        T* j = it;
        while (j > first && comp(key, *(j - 1))) {
            *j = std::move(*(j - 1));
            --j;
        }
        *j = std::move(key);
    }
}

template<typename T, typename Compare>
T* median_of_three(T* a, T* b, T* c, Compare comp) {
    if (comp(*a, *b)) {
        if (comp(*b, *c))
            return b;
        else if (comp(*a, *c))
            return c;
        else
            return a;
    }
    else {
        if (comp(*a, *c))
            return a;
        else if (comp(*b, *c))
            return c;
        else
            return b;
    }
}

template<typename T, typename Compare>
void sort(T* first, T* last, Compare comp) {
    if (first == nullptr || last == nullptr) return;
    const std::ptrdiff_t INSERTION_THRESHOLD = 16;

    while (last - first > static_cast<std::ptrdiff_t>(INSERTION_THRESHOLD)) {
        T* mid = first + (last - first - 1) / 2;
        T* piv_ptr = median_of_three(first, mid, last - 1, comp);
        if (piv_ptr != last - 1) move_swap(*piv_ptr, *(last - 1));

        T pivot = std::move(*(last - 1));
        T* i = first;
        for (T* j = first; j < last - 1; ++j) {
            if (comp(*j, pivot)) {
                if (i != j) move_swap(*i, *j);
                ++i;
            }
        }
        move_swap(*i, *(last - 1));
        T* partition = i;

        auto left_size = partition - first;
        auto right_size = last - (partition + 1);

        if (left_size < right_size) {
            if (left_size > 0) sort(first, partition, comp);
            first = partition + 1;
        }
        else {
            if (right_size > 0) sort(partition + 1, last, comp);
            last = partition;
        }
    }

    insertion_sort(first, last, comp);
}

#ifdef UNIT_TESTS

template<typename T, typename Compare>
bool is_sorted_by(T* first, T* last, Compare comp) {
    if (first == last) return true;
    for (T* it = first + 1; it < last; ++it) {
        if (comp(*it, *(it - 1))) return false;
    }
    return true;
}

TEST(QuickSortGeneric, EmptyAndSingle) {
    int a_empty[0];
    sort(a_empty, a_empty, [](int a, int b) { return a < b; });
    EXPECT_TRUE(is_sorted_by(a_empty, a_empty, [](int a, int b) { return a < b; }));

    int a1[1] = { 42 };
    sort(a1, a1 + 1, [](int a, int b) { return a < b; });
    EXPECT_TRUE(is_sorted_by(a1, a1 + 1, [](int a, int b) { return a < b; }));
}

TEST(QuickSortGeneric, AlreadySortedAndReverse) {
    int asc[10];
    for (int i = 0; i < 10; ++i) asc[i] = i;
    sort(asc, asc + 10, [](int a, int b) { return a < b; });
    EXPECT_TRUE(is_sorted_by(asc, asc + 10, [](int a, int b) { return a < b; }));

    int desc[10];
    for (int i = 0; i < 10; ++i) desc[i] = 10 - i;
    sort(desc, desc + 10, [](int a, int b) { return a < b; });
    EXPECT_TRUE(is_sorted_by(desc, desc + 10, [](int a, int b) { return a < b; }));
}

TEST(QuickSortGeneric, Duplicates) {
    int dup[12] = { 5,1,5,5,2,2,5,1,1,2,5,5 };
    sort(dup, dup + 12, [](int a, int b) { return a < b; });
    EXPECT_TRUE(is_sorted_by(dup, dup + 12, [](int a, int b) { return a < b; }));
}

TEST(QuickSortGeneric, RandomLarge) {
    const int N = 1000;
    std::vector<int> v(N);
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int i = 0; i < N; ++i) v[i] = dist(rng);
    sort(v.data(), v.data() + v.size(), [](int a, int b) { return a < b; });
    EXPECT_TRUE(std::is_sorted(v.begin(), v.end()));
}

struct Item {
    int key;
    std::string name;
    Item() : key(0), name() {}
    Item(int k, const std::string& s) : key(k), name(s) {}
};

TEST(QuickSortGeneric, NonTrivialType) {
    Item arr[6] = { {3,"a"}, {1,"b"}, {4,"c"}, {1,"d"}, {2,"e"}, {0,"f"} };
    sort(arr, arr + 6, [](const Item& a, const Item& b) { return a.key < b.key; });
    EXPECT_TRUE(is_sorted_by(arr, arr + 6, [](const Item& a, const Item& b) { return a.key < b.key; }));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#else

int main() {
    std::cout << "QuickSort implementation demo (no unit tests compiled).\n";
    int a[] = { 5,2,9,1,5,6,3,0,4,8,7 };
    sort(a, a + sizeof(a) / sizeof(a[0]), [](int a, int b) { return a < b; });
    for (int x : a) std::cout << x << ' ';
    std::cout << '\n';
    return 0;
}
#endif
