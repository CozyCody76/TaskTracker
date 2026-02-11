#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

void executeHelp();
bool validCommand(const std::string &command);
void executeAdd(std::string item);
void executeDelete(int deleteId);
void executeViewList();
void executeUpdate(int updateId, std::string new_item);
void executeDone(int updateId);
void executeInProgress(int updateId);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "No command line arguments provided.\n";
        std::cout << "Type Help for more information.\n";
        return 1;
    }

    if (!validCommand(argv[1]))
    {
        std::cout << "Invalid command: " << argv[1] << "\n";
        std::cout << "Type Help for more information.\n";
        return 1;
    }

    
    if (std::string(argv[1]) == "help")
    {
        executeHelp();
    }
    else if (std::string(argv[1]) == "add")
    {
        std::string item = argv[2];
        executeAdd(item);
    }
    else if (std::string(argv[1]) == "delete")
    {
        int id = std::stoi(argv[2]);
        executeDelete(id);
    }
    else if(std::string(argv[1]) == "make")
    {
        std::string command = argv[2];
        int id = std::stoi(argv[3]);
        validCommand(command);
        if(command == "done"){
            executeDone(id);
        }else if(command == "inprogress"){
            executeInProgress(id);
        }
    }
    else if (std::string(argv[1]) == "update")
    {   
        if(argc < 4){
            std::cout << "Argument error";
            return 1;
        }
        int id = std::stoi(argv[2]);
        std::string item = argv[3];
        executeUpdate(id, item);
    }
    else if (std::string(argv[1]) == "list")
    {
        executeViewList();
    }
    else if (std::string(argv[1]) == "done"){
        int id = std::stoi(argv[2]);
        executeDone(id);
    }
    else
    {
        std::cout << "Unknown error occurred.\n";
    }

    return 0;
}

bool validCommand(const std::string &command)
{
    return command == "add" || command == "delete" || command == "update" || command == "list" || command == "help" || command == "done" || command == "make";
}

void executeHelp()
{
    std::cout << "Available commands:\n";
    std::cout << "  add        - Add a new item\n";
    std::cout << "  delete     - Delete an existing item\n";
    std::cout << "  update     - Update an existing item\n";
    std::cout << "  list       - List all items\n";
    std::cout << "  make       - Use 'done', 'inprogress' keywords to change the status\n";
    std::cout << "  done       - Mark an existing item Done";
    std::cout << "  inprogress - Make an existing item In Progress";
    std::cout << "  help       - Show this help message\n";
}

void executeAdd(std::string item){
    std::ifstream infile("tasks.txt");
    int lastId = 0;
    std::string line;
    while (std::getline(infile, line)) {
        if(line.empty()) continue;
        size_t pos = line.find("|");
        if(pos != std::string::npos) {
            lastId = std::stoi(line.substr(0, pos));
        }
    }
    infile.close();

    int newId = lastId + 1;

    std::ofstream outfile("tasks.txt", std::ios::app);
    outfile << newId << "|" << item << "|" << 0 << "|" << 0 << "\n";
    outfile.close();

    std::cout << "Task added with ID: " << newId << "\n";
}

void executeDelete(int deleteId){
    std::ifstream infile("tasks.txt");
    std::vector<std::string> tasks;
    std::string line;
    while(std::getline(infile, line)){
        if(line.empty()){continue;}
        size_t pos = line.find("|");
        if(pos != std::string::npos){
            int id = std::stoi(line.substr(0, pos));
            if(deleteId != id){
                tasks.push_back(line);
            }

        }
    }
    infile.close();
    std::ofstream outfile("tasks.txt", std::ios::trunc);
    for(const std::string& t: tasks){
        outfile << t << "\n";
    }
    outfile.close();
    std::cout << "Task delted with ID: " << deleteId;
}

void executeViewList(){
    std::cout << "---View Tasks---" << "\n";
    std::ifstream infile("tasks.txt");
    std::string line;
    while(std::getline(infile, line)){
        if(line.empty()){continue;}
        size_t pos1 = line.find("|");
        size_t pos2 = line.find("|", pos1 + 1);
        size_t pos3 = line.find("|", pos2 + 1);
        if(pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos){
            continue;
        }

        int id = std::stoi(line.substr(0, pos1));
        std::string item = line.substr(pos1 + 1, pos2 - pos1 - 1);
        int completed = std::stoi(line.substr(pos2 + 1, pos2 - pos1 - 1));
        int inProgress = std::stoi(line.substr(pos3 + 1));
        std::cout << id << " " << item << " " << (completed ? "[x]" : "[ ]") << (inProgress ? " <- In Progress" : "") << "\n";
    }
    infile.close();
    std::cout << "----------------" << "\n";
}

void executeUpdate(int updateId, std::string new_item){
    std::ifstream infile("tasks.txt");
    std::vector<std::string> tasks;
    std::string line;
    bool found = false;
    
    while(std::getline(infile, line)){
        if(line.empty()) continue;
        size_t pos1 = line.find("|");
        size_t pos2 = line.find("|" , pos1 + 1);
        
        if(pos1 == std::string::npos || pos2 == std::string::npos){
            continue;
        }

        int id = std::stoi(line.substr(0, pos1));
        std::string item = line.substr(pos1 + 1, pos2 - pos1 - 1);
        int completed = std::stoi(line.substr(pos2 + 1));

        if(updateId == id){
            item = new_item;
            found = true;
        }

        std::stringstream ss;
        ss << id << "|" << item << "|" << completed;
        tasks.push_back(ss.str());
    }

    infile.close();
    std::ofstream outfile("tasks.txt");
    for(const std::string& task: tasks){
        outfile << task << "\n";
    }
    outfile.close();


    if(found){
        std::cout << "Task at ID " << updateId << " updated." << "\n";
    }else{
        std::cout << "Id doesn't exist in the list." << "\n";
    }
}  

void executeDone(int updateId){
    std::ifstream infile("tasks.txt");
    std::vector<std::string> tasks;
    std::string line;
    bool found = false;
    
    while(std::getline(infile, line)){
        if(line.empty()) continue;
        size_t pos1 = line.find("|");
        size_t pos2 = line.find("|" , pos1 + 1);
        
        if(pos1 == std::string::npos || pos2 == std::string::npos){
            continue;
        }
        int id = std::stoi(line.substr(0, pos1));
        std::string item = line.substr(pos1 + 1, pos2 - pos1 - 1);
        int completed = std::stoi(line.substr(pos2 + 1));

        if(updateId == id){
            completed = 1;
            found = true;
        }
        std::stringstream ss;
        ss << id << "|" << item << "|" << completed << "|" << 0;
        tasks.push_back(ss.str());
    }
    infile.close();
    std::ofstream outfile("tasks.txt");
    for(const std::string& task: tasks){
        outfile << task << "\n";
    }
    outfile.close();

    if(found){
        std::cout << "Task at ID " << updateId << " marked as Done." << "\n";
    }else{
        std::cout << "Id doesn't exist in the list." << "\n";
    }
}

void executeInProgress(int updateId){
    std::ifstream infile("tasks.txt");
    std::vector<std::string> tasks;
    std::string line;
    bool found = false;
    
    while(std::getline(infile, line)){
        if(line.empty()) continue;
        size_t pos1 = line.find("|");
        size_t pos2 = line.find("|" , pos1 + 1);
        size_t pos3 = line.find("|", pos2 + 1);
        
        if(pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos){
            continue;
        }
        int id = std::stoi(line.substr(0, pos1));
        std::string item = line.substr(pos1 + 1, pos2 - pos1 - 1);
        int completed = std::stoi(line.substr(pos2 + 1, pos2 - pos1 - 1));
        int inProgress = std::stoi(line.substr(pos3 + 1));

        if(updateId == id){
            if(completed){
                std::cout << "The taks is already completed.\n";
                return;
            }
            found = true;
            inProgress = 1;
        }
        std::stringstream ss;
        ss << id << "|" << item << "|" << completed << "|" << inProgress;
        tasks.push_back(ss.str());
    }
    infile.close();
    std::ofstream outfile("tasks.txt");
    for(const std::string& task: tasks){
        outfile << task << "\n";
    }
    outfile.close();

    if(found){
        std::cout << "Task at ID " << updateId << " marked as In Progress." << "\n";
    }else{
        std::cout << "Id doesn't exist in the list." << "\n";
    }
}