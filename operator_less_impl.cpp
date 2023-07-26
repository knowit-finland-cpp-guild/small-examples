#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <signal.h>

/****************************************************

* C++ code illustrating:

* Incorrect implementation of operator<
    - The right implementation works when called
    - The wrong implementation fails when called
    - Fail occurs at sorting of vector with keys with
      wrong operator< (KeyWrong)
* Operator selection is based on structs inherited
  from parent (KeyBase)
* Insertion/printing/sorting of elements in vector
  based on a function pointer
    - Selected based on usage of KeyRight or KeyWrong
* Simple argument parsing from command line
* Simple handling of a segfault

****************************************************/

// Enumeration for right and wrong keys
enum class Comparison { RIGHT, WRONG };

class KeyRight;
class KeyWrong;

// Count of increments in key's 1st, 2nd and 3rd elements
int aSteps = 13;
int bSteps = 5;
int cSteps = 17;

// Key data type in scalar form
using Value = double;

// Multipliers for conversion from 3 elements to scalar form
Value aMultiply = 1.0/aSteps;
Value bMultiply = aMultiply/bSteps;
Value cMultiply = bMultiply/cSteps;

// How many keys are to be placed in vector
int nSamp = 30;

// Base struct for keys
struct KeyBase
{
public:
    KeyBase(const int a, const int b, const int c) :
        a(a), b(b), c(c), val(a*aMultiply+b*bMultiply+c*cMultiply)
    {}

    virtual bool operator<(const KeyBase& rhs) const = 0;

    friend std::ostream& operator<<(
            std::ostream& os, const KeyBase& key)
    {
        return os << "(" << key.getA() << ", " << key.getB() << ", "
                  <<        key.getC() << ") " << key.val;
    }

    int getA() const { return a; }
    int getB() const { return b; }
    int getC() const { return c; }

protected:
    int a;      // Element 1 in key
    int b;      // Element 2 in key
    int c;      // Element 3 in key
    Value val;  // Key in scalar form
};

// Descendant of base with right implementation of operator<
struct KeyRight : public KeyBase
{
public:
    KeyRight(const int a, const int b, const int c) :
        KeyBase(a, b, c)
    {}

    bool operator<(const KeyBase& rhs) const override
    {
        return
            a == rhs.getA()
                ? b == rhs.getB()
                    ? c <  rhs.getC()
                    : b <  rhs.getB()
                : a <  rhs.getA();
    }
};

// Descendant of base with wrong implementation of operator<
struct KeyWrong : public KeyBase
{
public:
    KeyWrong(const int a, const int b, const int c) :
        KeyBase(a, b, c)
    {}

    bool operator<(const KeyBase& rhs) const override
    {
        if (a < rhs.getA())
            return true;
        if (b < rhs.getB())
            return true;
        if (c < rhs.getC())
            return true;
        return false;
    }
};

// Holder class for vectors with either right or wrong operator<
class VecKey
{
private:
    using InsertFunction = std::function<void(VecKey&, int, int, int)>;
    using PrintFunction = std::function<void(VecKey const&)>;
    using SortFunction = std::function<void(VecKey&)>;

public:
    VecKey(const Comparison comp) : comp(comp)
    {
        if (comp == Comparison::RIGHT)
        {
            // Select which function pointers to use with right keys
            insertFunction = &VecKey::insertRight;
            printFunction = &VecKey::printRight;
            sortFunction = &VecKey::sortRight;
        }
        else
        {
            // Select which function pointers to use with wrong keys
            insertFunction = &VecKey::insertWrong;
            printFunction = &VecKey::printWrong;
            sortFunction = &VecKey::sortWrong;
        }
    }

    void insert(int a, int b, int c)
    {
        insertFunction(*this, a, b, c);
    }

    void print() const
    {
        printFunction(*this);
    }

    void sort()
    {
        sortFunction(*this);
    }

private:
    VecKey();

    void insertRight(int a, int b, int c)
    {
        vRight.emplace_back(a, b, c);
    }

    void insertWrong(int a, int b, int c)
    {
        vWrong.emplace_back(a, b, c);
    }

    void printRight() const
    {
        int n = 0;
        for (const auto& x : vRight)
            std::cout << ++n << ": " << x << std::endl;
    }

    void printWrong() const
    {
        int n = 0;
        for (const auto& x : vWrong)
            std::cout << ++n << ": " << x << std::endl;
    }

    void sortRight()
    {
        std::sort(vRight.begin(), vRight.end());
    }

    void sortWrong()
    {
        std::sort(vWrong.begin(), vWrong.end());
    }

    Comparison comp;
    std::vector<KeyRight> vRight;
    std::vector<KeyWrong> vWrong;
    InsertFunction insertFunction;
    PrintFunction printFunction;
    SortFunction sortFunction;
};

// Determine the desired run, right or wrong
bool parseArgs(const int argc, char *argv[], Comparison& comp)
{
    std::string sr{"comparison-right"};
    std::string sw{"comparison-wrong"};
    bool flag{};
    auto argRight{std::make_pair(sr, flag)};
    auto argWrong{std::make_pair(sw, flag)};

    if (argc != 2)
        ;
    else if (std::string(argv[1]) == argRight.first)
        argRight.second = true;
    else if (std::string(argv[1]) == argWrong.first)
        argWrong.second = true;

    if (argRight.second == argWrong.second)
    {
        std::cout << "Incompatible arguments. Use examples:"
                  << std::endl;
        std::cout << argv[0] << " " << argRight.first << std::endl;
        std::cout << argv[0] << " " << argWrong.first << std::endl;
        return false;
    }

    comp = Comparison::RIGHT;
    if (argWrong.second)
        comp = Comparison::WRONG;

    return true;
}

// Handle possible segfault in sorting vector
void handler(int sig)
{
    std::cout << "Segmentation fault: " << sig << std::endl;

    exit(sig);
}

int main(int argc, char *argv[])
{
    // Prepare for possible segfault
    signal(SIGSEGV, handler);

    Comparison comp;

    // Determine the desired run
    if (!parseArgs(argc, argv, comp))
        return 1;

    // Initialize vector holder class for either right or wrong key
    VecKey vk(comp);

    std::cout << "Inserting" << std::endl;
    // Populate the right or wrong vector in the vector holder class
    for (int n = 1; n <= nSamp; ++n)
    {
        int a = n % aSteps;
        int b = n % bSteps;
        int c = n % cSteps;
        vk.insert(a, b, c);
    }
    // Print the right or wrong vector in initial order
    vk.print();

    std::cout << "Sorting" << std::endl;
    // Sort the right or wrong vector
    vk.sort();
    // Print the right or wrong vector in sorted order
    vk.print();
}

