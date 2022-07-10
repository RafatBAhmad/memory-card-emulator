#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>

using namespace std;

// create a class to store memory command
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

// create vector of string to store the command
vector<string> cmdList;

void Add(string cmd, bool isQuick = false)
{
    if (isQuick)
    {
        cmdList.insert(cmdList.begin(), cmd);
    }
    cmdList.push_back(cmd);
}

void Remove()
{
    cmdList.erase(cmdList.begin());
}

void Remove(int id)
{
    cmdList.erase(cmdList.begin() + id);
}

void Execute(memoryCmd fillMemory)
{
    int index, value, EndIndex;
    string Svalue, Sindex, SEndIndex;
    string cmd = cmdList.front();
    cmdList.erase(cmdList.begin());

    // remove spaces from the string command
    cmd.erase(remove(cmd.begin(), cmd.end(), ' '), cmd.end());

    char memoryCommands = cmd.front();

    if (memoryCommands == 'r' || memoryCommands == 'R')
    {
        try
        {
            // here we convert the last character to string then by stoi we take it as integer number
            // which represents index that we need to read it's value.
            index = stoi(Sindex += cmd.back());
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << " convert string number to number so please enter a valid read cmd ex: 'read 1' " << '\n';
        }
        cout << fillMemory.Read(index) << endl;
    }

    if (memoryCommands == 'w' || memoryCommands == 'W')
    {
        try
        {
            // here we convert the last tow characters to string then by stoi we take them as integer number.
            // which the last one represents the value and the one before represents the index.
            value = stoi(Svalue += cmd.back());
            index = stoi(Sindex += cmd.back());
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << " convert string number to number so please enter a valid write cmd ex: 'write 1 6' " << '\n';
        }
        fillMemory.Write(index, value);
    }

    if (memoryCommands == 'd' || memoryCommands == 'D')
    {
        try
        {
            // here we convert the last tow characters to string then by stoi we take them as integer number.
            // which the last one represents the startIndex and the one before represents the endIndex, so we will delete all commands between these two indices and indices included.
            EndIndex = stoi(SEndIndex += cmd.back());
            index = stoi(Sindex += cmd.back());
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << " convert string number to number so please enter a valid delete cmd ex: 'delete 4 7' " << '\n';
        }
        fillMemory.Write(index, EndIndex);
    }
}

void Abort()
{
    for (string cmd : cmdList)
    {
        cout << cmd << " failed" << endl;
    }
}

int main()
{
    // create object of memoryCmd class
    memoryCmd fillMemory;

    // read commands from a file
    fstream cmdFile;
    cmdFile.open("commands.txt", ios::in);
    if (cmdFile.is_open())
    {
        string cmd;

        while (getline(cmdFile, cmd))
        {
            list<string> cmdList;
            stringstream cmdLine(cmd);
            do
            {
                string cmdPart;
                cmdLine >> cmdPart;
                cmdList.push_back(cmdPart);
            } while (cmdLine);

            if (cmdList.front() == "add" || "Add")
            {
                string cmd;
                cmdList.pop_front();
                if (cmdList.back() == "true" || "True")
                {
                    cout << "hi\n";
                    cmdList.pop_back();
                    for (auto i : cmdList)
                    {
                        cmd += i;
                    }
                    cout << cmd << endl;
                    Add(cmd, true);
                }
                else
                {
                    for (auto i : cmdList)
                    {
                        cmd += i;
                    }
                    cout << cmd << endl;
                    Add(cmd);
                }
            }
            else if (cmdList.front() == "remove" || "Remove")
            {
                cout << "I'm remove API" << endl;
            }
        }
    }

    return 0;
}