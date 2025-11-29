#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <cmath>      
#include <cctype>
#include <functional>
#include <cstddef>
#include <vector>
#include <numeric>
#include "gtest/gtest.h"


template<typename T>
inline void MoveSwap(T& A, T& B) {
    T Tmp = std::move(A);
    A = std::move(B);
    B = std::move(Tmp);
}

template<typename T, typename Compare>
void InsertionSort(T* First, T* Last, Compare Comp) {
    if (First >= Last) {
        return;
    }
    for (T* It = First + 1; It < Last; ++It) {
        T Key = std::move(*It);
        T* J = It;
        while (J > First && Comp(Key, *(J - 1))) {
            *J = std::move(*(J - 1));
            --J;
        }
        *J = std::move(Key);
    }
}

template<typename T, typename Compare>
T* MedianOfThree(T* A, T* B, T* C, Compare Comp) {
    if (Comp(*A, *B)) {
        if (Comp(*B, *C)) {
            return B;
        }
        else if (Comp(*A, *C)) {
            return C;
        }
        else {
            return A;
        }
    }
    else {
        if (Comp(*A, *C)) {
            return A;
        }
        else if (Comp(*B, *C)) {
            return C;
        }
        else {
            return B;
        }
    }
}

template<typename T, typename Compare>
void QuickHybridSort(T* First, T* Last, Compare Comp, std::ptrdiff_t InsertionThreshold = 16) {
    if (First == nullptr || Last == nullptr) {
        return;
    }
    while (Last - First > InsertionThreshold) {
        T* Mid = First + (Last - First - 1) / 2;
        T* PivotPtr = MedianOfThree(First, Mid, Last - 1, Comp);
        T PivotValue = *PivotPtr;
        T* I = First - 1;
        T* J = Last;
        while (true) {
            do {
                ++I;
            } while (Comp(*I, PivotValue));
            do {
                --J;
            } while (Comp(PivotValue, *J));
            if (I >= J) {
                break;
            }
            if (I != J) {
                MoveSwap(*I, *J);
            }
        }
        T* Partition = J;
        auto LeftSize = Partition - First + 1;
        auto RightSize = Last - (Partition + 1);
        if (LeftSize < RightSize) {
            if (LeftSize > 0) {
                QuickHybridSort(First, Partition + 1, Comp, InsertionThreshold);
            }
            First = Partition + 1;
        }
        else {
            if (RightSize > 0) {
                QuickHybridSort(Partition + 1, Last, Comp, InsertionThreshold);
            }
            Last = Partition + 1;
        }
    }
    InsertionSort(First, Last, Comp);
}

template<typename T, typename Compare>
void QuickSort(T* First, T* Last, Compare Comp) {
    if (First == nullptr || Last == nullptr) {
        return;
    }

    T* Mid = First + (Last - First - 1) / 2;
    T* PivotPtr = MedianOfThree(First, Mid, Last - 1, Comp);
    T PivotValue = *PivotPtr;
    T* I = First - 1;
    T* J = Last;
    while (true) {
        do {
            ++I;
        } while (Comp(*I, PivotValue));
        do {
            --J;
        } while (Comp(PivotValue, *J));
        if (I >= J) {
            break;
        }
        if (I != J) {
            MoveSwap(*I, *J);
        }
    }
    T* Partition = J;
    auto LeftSize = Partition - First + 1;
    auto RightSize = Last - (Partition + 1);
    if (LeftSize < RightSize) {
        if (LeftSize > 0) {
            QuickSort(First, Partition + 1, Comp);
        }
        First = Partition + 1;
    }
    else {
        if (RightSize > 0) {
            QuickSort(Partition + 1, Last, Comp);
        }
        Last = Partition + 1;
    }
}


template<typename T, typename Compare>
bool IsSortedBy(T* First, T* Last, Compare Comp) {
    if (First == Last) {
        return true;
    }
    for (T* It = First + 1; It < Last; ++It) {
        if (Comp(*It, *(It - 1))) {
            return false;
        }
    }
    return true;
}

std::vector<std::size_t> GenerateSampleSizes(std::size_t MinN, std::size_t MaxN, std::size_t Count) {
    std::vector<std::size_t> Result;
    if (Count <= 1 || MinN >= MaxN) {
        Result.push_back(MinN);
        return Result;
    }
    double Step = static_cast<double>(MaxN - MinN) / static_cast<double>(Count - 1);
    for (std::size_t I = 0; I < Count; ++I) {
        std::size_t Value = static_cast<std::size_t>(std::round(MinN + Step * I));
        if (Value < 1) {
            Value = 1;
        }
        Result.push_back(Value);
    }
    return Result;
}

double TimeInsertionWorstCase(std::size_t N, std::size_t NumRepeats) {
    std::vector<int> Base;
    Base.reserve(N);
    for (std::size_t I = 0; I < N; ++I) {
        Base.push_back(static_cast<int>(N - I));
    }
    double Total = 0.0;
    for (std::size_t R = 0; R < NumRepeats; ++R) {
        std::vector<int> Arr = Base;
        auto Start = std::chrono::high_resolution_clock::now();
        InsertionSort(Arr.data(), Arr.data() + static_cast<std::ptrdiff_t>(Arr.size()), std::less<int>());
        auto End = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> Elapsed = End - Start;
        Total += Elapsed.count();
        if (!IsSortedBy(Arr.data(), Arr.data() + static_cast<std::ptrdiff_t>(Arr.size()), std::less<int>())) {
            std::cerr << "InsertionSort produced unsorted result for N=" << N << std::endl;
            std::exit(1);
        }
    }
    return Total;
}

double TimeQuickWorstCase(std::size_t N, std::size_t NumRepeats) {
    std::vector<int> Base;
    Base.reserve(N);
    for (std::size_t I = 0; I < N; ++I) {
        Base.push_back(static_cast<int>(N - I));
    }
    MakeMedianOfThreeKiller(N);
    double Total = 0.0;
    for (std::size_t R = 0; R < NumRepeats; ++R) {
        std::vector<int> Arr = Base;
        auto Start = std::chrono::high_resolution_clock::now();
        QuickSort(Arr.data(), Arr.data() + static_cast<std::ptrdiff_t>(Arr.size()), std::less<int>());
        auto End = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> Elapsed = End - Start;
        Total += Elapsed.count();
        if (!IsSortedBy(Arr.data(), Arr.data() + static_cast<std::ptrdiff_t>(Arr.size()), std::less<int>())) {
            std::cerr << "QuickSort produced unsorted result for N=" << N << std::endl;
            std::exit(1);
        }
    }
    return Total;
}

std::string BuildCoordsString(const std::vector<std::size_t>& Sizes, const std::vector<double>& Values) {
    std::ostringstream Oss;
    Oss << "[";
    for (std::size_t I = 0; I < Sizes.size(); ++I) {
        Oss << "[";
        Oss << Sizes[I] << ",";
        Oss << std::fixed << std::setprecision(6) << Values[I];
        Oss << "]";
        if (I + 1 < Sizes.size()) {
            Oss << ",";
        }
    }
    Oss << "]";
    return Oss.str();
}

std::vector<int> MakeMedianOfThreeKiller(std::size_t N) {
    std::vector<int> Result(N, 0);
    std::vector<std::size_t> Positions(N);
    std::iota(Positions.begin(), Positions.end(), 0u);

    int Low = 1;                      
    int High = static_cast<int>(N);   

    std::function<void(const std::vector<std::size_t>&)> Build;
    Build = [&](const std::vector<std::size_t>& P) -> void {
        std::size_t m = P.size();
        if (m == 0) return;
        if (m == 1) {
            Result[P[0]] = Low++;
            return;
        }
        if (m == 2) {
            Result[P[0]] = Low++;
            Result[P[1]] = Low++;
            return;
        }
        std::size_t First = P.front();
        std::size_t Mid = P[m / 2];
        std::size_t Last = P.back();

        Result[First] = Low++;
        Result[Mid] = Low++;
        Result[Last] = High--;

        std::vector<std::size_t> Rem;
        Rem.reserve(m > 3 ? m - 3 : 0);
        for (std::size_t i = 0; i < m; ++i) {
            std::size_t pos = P[i];
            if (pos == First || pos == Mid || pos == Last) continue;
            Rem.push_back(pos);
        }
        Build(Rem);
        };

    Build(Positions);
    return Result;
}


bool EndsWithIgnoreCase(const std::string& Str, const std::string& Suffix) {
    if (Str.size() < Suffix.size()) {
        return false;
    }
    auto itStr = Str.end();
    auto itSuf = Suffix.end();
    for (std::size_t i = 0; i < Suffix.size(); ++i) {
        char a = *(--itStr);
        char b = *(--itSuf);
        if (std::tolower(static_cast<unsigned char>(a)) != std::tolower(static_cast<unsigned char>(b))) {
            return false;
        }
    }
    return true;
}

std::string QuoteArg(const std::string& Arg) {
    std::string Out;
    Out.reserve(Arg.size() + 2);
    Out.push_back('"');
    for (char C : Arg) {
        if (C == '"') {
            Out.push_back('\\');
            Out.push_back('"');
        }
        else {
            Out.push_back(C);
        }
    }
    Out.push_back('"');
    return Out;
}

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
    QuickHybridSort(asc, asc + 10, [](int a, int b) { return a < b; });
    EXPECT_TRUE(is_sorted_by(asc, asc + 10, [](int a, int b) { return a < b; }));

    int desc[10];
    for (int i = 0; i < 10; ++i) desc[i] = 10 - i;
    QuickHybridSort(desc, desc + 10, [](int a, int b) { return a < b; });
    EXPECT_TRUE(is_sorted_by(desc, desc + 10, [](int a, int b) { return a < b; }));
}

TEST(QuickSortGeneric, Duplicates) {
    int dup[12] = { 5,1,5,5,2,2,5,1,1,2,5,5 };
    QuickHybridSort(dup, dup + 12, [](int a, int b) { return a < b; });
    EXPECT_TRUE(is_sorted_by(dup, dup + 12, [](int a, int b) { return a < b; }));
}

TEST(QuickSortGeneric, RandomLarge) {
    const int N = 1000;
    std::vector<int> v(N);
    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> dist(-1000, 1000);
    for (int i = 0; i < N; ++i) v[i] = dist(rng);
    QuickHybridSort(v.data(), v.data() + v.size(), [](int a, int b) { return a < b; });
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
    QuickHybridSort(arr, arr + 6, [](const Item& a, const Item& b) { return a.key < b.key; });
    EXPECT_TRUE(is_sorted_by(arr, arr + 6, [](const Item& a, const Item& b) { return a.key < b.key; }));
}

static const std::size_t DefaultMinN = 4;
static const std::size_t DefaultMaxN = 50;
static const std::size_t DefaultNumRepeats = 20000;
static const std::ptrdiff_t DefaultInsertionThreshold = 16;
static const std::string ScriptPath = "G:\\LaboratoryWork3\\LaboratoryWork3\\ShowGraphs.exe";

int main(int argc, char** argv) {
    std::size_t MinN = DefaultMinN;
    std::size_t MaxN = DefaultMaxN;
	std::size_t NumPoints = DefaultMaxN - DefaultMinN + 1;
    std::size_t NumRepeats = DefaultNumRepeats;

    std::vector<std::size_t> Sizes = GenerateSampleSizes(MinN, MaxN, NumPoints);
    std::vector<double> TimesInsertion;
    std::vector<double> TimesQuick;
    TimesInsertion.reserve(Sizes.size());
    TimesQuick.reserve(Sizes.size());
    double MaxTime = 0.0;
    for (std::size_t N : Sizes) {
        double TIns = TimeInsertionWorstCase(N, NumRepeats);
        double TQ = TimeQuickWorstCase(N, NumRepeats);
        TimesInsertion.push_back(TIns);
        TimesQuick.push_back(TQ);
        if (TIns > MaxTime) {
            MaxTime = TIns;
        }
        if (TQ > MaxTime) {
            MaxTime = TQ;
        }
    }
    double MaxY = MaxTime * 1.10;
    if (MaxY <= 0.0) {
        MaxY = 1.0;
    }
    std::string CoordsInsertion = BuildCoordsString(Sizes, TimesInsertion);
    std::string CoordsQuick = BuildCoordsString(Sizes, TimesQuick);

    std::ostringstream Command;

    Command << ScriptPath << " ";
    Command << MinN << " " << MaxN << " " << 0 << " " << MaxY << " ";
    Command << 2 << " ";
    Command << QuoteArg(CoordsInsertion) << " ";
    Command << QuoteArg(std::string("Insertion Sort (worst-case)")) << " ";
    Command << QuoteArg(CoordsQuick) << " ";
    Command << QuoteArg(std::string("Quick Sort"));

    std::string CmdStr = Command.str();
    std::cout << "Executing: " << CmdStr << std::endl;
    int Result = std::system(CmdStr.c_str());
    return Result;
}
