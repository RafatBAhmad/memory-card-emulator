#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <cstring>

using namespace std;

// create a class to store memory command
class memoryCmd
{
    list<int> memory;

public:
    int Read(int LBA)
    {
        auto result = memory.begin();

        advance(result, LBA);

        return *result;
    }

    void Write(int LBA, int value)
    {
        auto result = memory.begin();

        advance(result, LBA);

        *result = value;
    }

    void Delete(int startIndex, int endIndex)
    {
        auto start = memory.begin();
        auto end = memory.begin();
        advance(start, startIndex);
        advance(end, endIndex + 1);
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
    else
    {
        cmdList.push_back(cmd);
    }
}

void Remove()
{
    cmdList.erase(cmdList.begin());
}

void Remove(string id)
{
    int index;
    try
    {
        // here we convert the last character to string then by stoi we take it as integer number
        // which represents index that we need to remove it.
        index = stoi(id);
        cmdList.erase(cmdList.begin() + index);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << " convert string number to number so please enter a valid remove cmd ex: 'remove 1' " << '\n';
    }
}

void Execute(memoryCmd fillMemory)
{
    int index, value, EndIndex;
    string Svalue, Sindex, SEndIndex; // S for string
    string cmd = cmdList.front();

    list<string> l_cmd;
    stringstream cmdLine(cmd);

    cmdList.erase(cmdList.begin()); // Delete first command.

    while (cmdLine)
    {
        string cmdPart;
        cmdLine >> cmdPart;
        l_cmd.push_back(cmdPart);
    }
    l_cmd.pop_back();
    cout << cmd << endl;
    if (l_cmd.front() == "read" || l_cmd.front() == "Read")
    {
        try
        {
            // here we convert the last character to string then by stoi we take it as integer number
            // which represents index that we need to read it's value.
            index = stoi(Sindex += l_cmd.back());
            cout << " The result is: " << fillMemory.Read(index) << endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << " convert string number to number so please enter a valid read cmd ex: 'read 1' " << '\n';
        }
    }

    if (l_cmd.front() == "write" || l_cmd.front() == "Write")
    {
        try
        {
            // here we convert the last tow characters to string then by stoi we take them as integer number.
            // which the last one represents the value and the one before represents the index.
            value = stoi(Svalue += l_cmd.back());
            l_cmd.pop_back();
            index = stoi(Sindex += l_cmd.back());
            fillMemory.Write(index, value);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << " convert string number to number so please enter a valid write cmd ex: 'write 1 6' " << '\n';
        }
    }

    if (l_cmd.front() == "delete" || l_cmd.front() == "Delete")
    {
        try
        {
            // here we convert the last tow characters to string then by stoi we take them as integer number.
            // which the last one represents the startIndex and the one before represents the endIndex, so we will delete all commands between these two indices and indices included.
            EndIndex = stoi(SEndIndex += l_cmd.back());
            l_cmd.pop_back();
            index = stoi(Sindex += l_cmd.back());
            fillMemory.Delete(index, EndIndex);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << " convert string number to number so please enter a valid delete cmd ex: 'delete 4 7' " << '\n';
        }
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
            while (cmdLine)
            {
                string cmdPart;
                cmdLine >> cmdPart;
                cmdList.push_back(cmdPart);
            }
            cmdList.pop_back();
            if (cmdList.front() == "add" || cmdList.front() == "Add")
            {
                string cmd;
                cmdList.pop_front();
                if (cmdList.back() == "true" || cmdList.back() == "True")
                {
                    cmdList.pop_back();
                    for (auto i : cmdList)
                    {
                        cmd += ' ' + i;
                    }
                    Add(cmd, true);
                }
                else
                {
                    for (auto i : cmdList)
                    {
                        cmd += ' ' + i;
                    }
                    Add(cmd);
                }
            }
            else if (cmdList.front() == "remove" || cmdList.front() == "Remove")
            {

                if (cmdList.size() == 1)
                {
                    Remove();
                }
                else if (cmdList.size() == 2)
                {
                    Remove(cmdList.back());
                }
                else if (cmdList.size() > 2)
                {
                    cout << "Bad remove cmd, the good remove cmd comes in these two style => 'remove' or 'remove 2'" << endl;
                }
            }
            else if (cmdList.front() == "execute" || cmdList.front() == "Execute")
            {
                Execute(fillMemory);
            }
            else if (cmdList.front() == "abort" || cmdList.front() == "Abort")
            {
                cout << "I'm abort API" << endl;
            }
        }
        cmdFile.close();
    }
    return 0;
}