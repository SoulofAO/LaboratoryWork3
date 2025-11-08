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
#include "gtest/gtest.h"

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
void sort(T* First, T* Last, Compare comp) {
    if (First == nullptr || Last == nullptr) {
        return;
    }

    const std::ptrdiff_t INSERTION_THRESHOLD = 16;

    while (Last - First > static_cast<std::ptrdiff_t>(INSERTION_THRESHOLD)) {
        T* Mid = First + (Last - First - 1) / 2;
        T* PivotPtr = median_of_three(First, Mid, Last - 1, comp);
        T PivotValue = *PivotPtr; 

        T* I = First - 1;
        T* J = Last;
        while (true) {
            do {
                ++I;
            } while (comp(*I, PivotValue));

            do {
                --J;
            } while (comp(PivotValue, *J));

            if (I >= J) {
                break;
            }

            if (I != J) {
                move_swap(*I, *J);
            }
        }

        T* Partition = J;
        auto LeftSize = Partition - First + 1;     
        auto RightSize = Last - (Partition + 1);   

        if (LeftSize < RightSize) {
            if (LeftSize > 0) {
                sort(First, Partition + 1, comp);
            }
            First = Partition + 1;
        }
        else {
            if (RightSize > 0) {
                sort(Partition + 1, Last, comp);
            }
            Last = Partition + 1;
        }
    }
    insertion_sort(First, Last, comp);
}



//Измерить время сортировки вставками и Qiucksort. Построить графики. Худшие случаи. 
template<typename T, typename Compare>
bool is_sorted_by(T* first, T* last, Compare comp) {
    if (first == last) return true;
    for (T* it = first + 1; it < last; ++it) {
        if (comp(*it, *(it - 1))) return false;
    }
    return true;
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
