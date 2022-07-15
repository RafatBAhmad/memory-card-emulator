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

class Command
{
    int id;
    int LBA;
    int data_size;
    string type;
    string cmd_str;

public:
    Command(string type, int LBA, int data_size, string cmd_str)
    {
        this->id = nextID;
        this->type = type;
        this->LBA = LBA;
        this->data_size = data_size;
        this->cmd_str = cmd_str;

        nextID++;
    }

    static int nextID;

    int getId()
    {
        return id;
    }
    int getLBA()
    {
        return LBA;
    }
    int getDataSize()
    {
        return data_size;
    }
    string getType()
    {
        return type;
    }
    string getCommand()
    {
        return cmd_str;
    }
};

// Initialize static member of class Command
int Command::nextID = 0;

class ReadCommand : public Command
{
public:
    ReadCommand(int LBA, int data_size, string cmd_str) : Command("Read", LBA, data_size, cmd_str) {}
};

class WriteCommand : public Command
{
    vector<uint8_t> data;

public:
    WriteCommand(int LBA, int data_size, vector<uint8_t> data, string cmd_str) : Command("Write", LBA, data_size, cmd_str)
    {
        this->data = data;
    }

    vector<uint8_t> getData()
    {
        return data;
    }
};

class DeleteCommand : public Command
{
public:
    DeleteCommand(int LBA, int data_size, string cmd_str) : Command("Delete", LBA, data_size, cmd_str) {}
};

class Response
{
    Command cmd;
    bool is_succeeded;

public:
    Response(Command cmd, bool is_succeeded) : cmd(cmd), is_succeeded(is_succeeded) {}

    string getStatus()
    {
        if (is_succeeded)
        {
            return "Succeeded";
        }
        return "Failed";
    }

    string getResponceString()
    {
        return to_string(cmd.getId()) + ": " + cmd.getCommand() + " " + getStatus();
    }
};

class ReadResponce : public Response
{
    vector<uint8_t> data;

public:
    ReadResponce(Command cmd, bool is_succeeded, vector<uint8_t> data) : Response(cmd, is_succeeded), data(data) {}

    vector<uint8_t> getReadData()
    {
        return data;
    }
};

void Log(Command *cmd, bool success)
{

    fstream ResFile;
    ResFile.open("response.txt", ios::app);
    if (!ResFile.is_open())
    {
        return;
    }

    Response res(*cmd, success);
    ResFile << res.getResponceString() << endl;
    ResFile.close();
}

// create a class to store memory command
class memoryCmd
{
    vector<uint8_t> memory;
    vector<bool> isReserved;

public:
    memoryCmd(int size)
    {
        memory = vector<uint8_t>(size, 0);
        isReserved = vector<bool>(size, false);
    }

    vector<uint8_t> Read(int LBA, int size)
    {
        if (LBA < 0 || LBA >= memory.size())
            return vector<uint8_t>();

        auto startAddress = memory.begin() + LBA;
        return vector<uint8_t>(startAddress, startAddress + size);
    }

    bool Write(int LBA, int size, vector<uint8_t> value)
    {
        if (LBA < 0 || LBA >= memory.size())
            return false;

        for (int i = 0; i < size; i++)
        {
            if (isReserved[LBA + i])
            {
                // this memory cell is not empty
                return false;
            }

            memory[LBA + i] = value[i];
            isReserved[LBA + i] = true;
        }
        return true;
    }

    bool Delete(int startIndex, int endIndex)
    {
        if (startIndex < 0 || startIndex >= memory.size() || startIndex > endIndex || endIndex > memory.size())
            return false;

        int index = startIndex;
        while (index < endIndex)
        {
            isReserved[index++] = false;
        }
        return true;
    }

    void Execute(Command *cmd)
    {
        bool success = false;
        if (cmd->getType() == "Write")
        {
            WriteCommand *writeCmd = (WriteCommand *)cmd;
            success = Write(writeCmd->getLBA(), writeCmd->getDataSize(), writeCmd->getData());
        }
        else if (cmd->getType() == "Read")
        {
            ReadCommand *readCmd = (ReadCommand *)cmd;
            vector<uint8_t> result = Read(readCmd->getLBA(), readCmd->getDataSize());
            success = (!result.empty());
        }
        else if (cmd->getType() == "Delete")
        {
            DeleteCommand *deleteCmd = (DeleteCommand *)cmd;
            success = Delete(deleteCmd->getLBA(), deleteCmd->getDataSize());
        }

        Log(cmd, success);
    }
};

class CommandsQueue
{
private:
    // create vector of Command to store the command
    vector<Command *> cmdList;

public:
    void Add(Command *cmd, bool isQuick = false)
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

    void Remove(string id_string)
    {
        try
        {
            // here we convert the last character to string then by stoi we take it as integer number
            // which represents index that we need to remove it.
            uint8_t id = stoi(id_string);

            for (uint i = 0; i < cmdList.size(); i++)
            {
                if (cmdList[i]->getId() == id)
                {
                    cmdList.erase(cmdList.begin() + id);
                    break;
                }
            }
        }
        catch (const exception &e)
        {
            cerr << e.what() << " convert string number to number so please enter a valid remove cmd ex: 'remove 1' " << endl;
        }
    }

    Command *GetNextCommand()
    {
        if (cmdList.empty())
            return nullptr;

        Command *cmd = cmdList.front();
        cmdList.erase(cmdList.begin()); // Delete first command.
        return cmd;
    }

    void Abort()
    {
        while (true)
        {
            Command *nextCmd = GetNextCommand();
            if (nextCmd == nullptr)
            {
                break;
            }

            Log(nextCmd, false);
        }
    }
};

vector<string> ReadFile(string path)
{
    fstream file;

    file.open(path, ios::in);
    if (!file.is_open())
    {
        cerr << "Connot open file" << path << endl;
        return vector<string>();
    }

    vector<string> lines;
    string line;
    while (getline(file, line))
    {
        if (!line.empty())
        {
            lines.push_back(line);
        }
    }

    file.close();

    if (lines.empty())
    {
        cerr << path << " file is empty" << endl;
    }

    return lines;
}

list<string> SplitCommandToList(string cmd)
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

    return cmdList;
}

bool StrCompare(string s1, string s2)
{
    if (s1.size() != s2.size())
        return false;
    for (uint i = 0; i < s1.size(); i++)
        if (tolower(s1[i]) != tolower(s2[i]))
            return false;
    return true;
}

Command *StringToCommand(string cmdStr)
{
    Command *command;

    list<string> cmdList = SplitCommandToList(cmdStr);
    if (cmdList.size() < 3)
    {
        cerr << "Invalid commands: " << cmdStr << endl;
        return nullptr;
    }

    string op = cmdList.front();
    cmdList.pop_front();
    int LBA = atoi(cmdList.front().c_str());
    cmdList.pop_front();
    int size = atoi(cmdList.front().c_str());
    cmdList.pop_front();

    if (StrCompare(op, "read"))
    {
        return new ReadCommand(LBA, size, cmdStr);
    }

    if (StrCompare(op, "write"))
    {
        vector<uint8_t> data;
        while (!cmdList.empty())
        {
            data.push_back(atoi(cmdList.front().c_str()));
            cmdList.pop_front();
        }

        return new WriteCommand(LBA, size, data, cmdStr);
    }

    if (StrCompare(op, "delete"))
    {
        return new DeleteCommand(LBA, size, cmdStr);
    }

    return nullptr;
}

int main()
{
    const string QUICK_STR = "quick";

    // create object of memoryCmd class
    memoryCmd fillMemory(100);
    // create a priority queue for the commands
    CommandsQueue cmdQueue;
    // read commands from a file
    vector<string> fileLines = ReadFile("commands.txt");

    for (auto &cmdLine : fileLines)
    {
        int quickPos = cmdLine.find(QUICK_STR);
        bool isQuick = (quickPos != -1);
        if (isQuick)
        {
            // remove 'quick' from cmdLine
            cmdLine.erase(quickPos, QUICK_STR.length());
        }

        int spacePos = cmdLine.find(" ");
        string queueOperator = cmdLine.substr(0, spacePos);
        string cmd = cmdLine.substr(spacePos + 1);
        bool hasArguments = (spacePos != -1);

        if (StrCompare(queueOperator, "add"))
        {
            Command *command = StringToCommand(cmd);
            if (command != nullptr)
            {
                cmdQueue.Add(command, isQuick);
            }
        }
        else if (StrCompare(queueOperator, "remove"))
        {
            if (hasArguments)
            {
                cmdQueue.Remove(cmd);
            }
            else
            {
                cmdQueue.Remove();
            }
        }
        else if (StrCompare(queueOperator, "execute"))
        {
            fillMemory.Execute(cmdQueue.GetNextCommand());
        }
        else if (StrCompare(queueOperator, "abort"))
        {
            cmdQueue.Abort();
        }
        else
        {
            cerr << "Invalid queue operator: " << cmdLine << endl;
        }
    }

    return 0;
}