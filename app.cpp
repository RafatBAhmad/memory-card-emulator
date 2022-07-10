#include <iostream>
#include <list>
using namespace std;

class memoryCmd
{
    list<int> memory;

public:
    int Read(int LBA)
    {
        auto result = memory.begin();

        std::advance(result, LBA);

        return *result;
    }

    void Write(int LBA, int value)
    {
        auto result = memory.begin();

        std::advance(result, LBA);

        *result = value;
    }

    void Delete(int startIndex, int endIndex)
    {
        auto start = memory.begin();
        auto end = memory.begin();
        std::advance(start, startIndex);
        std::advance(end, endIndex + 1);
        memory.erase(start, end);
    }
};

int main()
{

    return 0;
}