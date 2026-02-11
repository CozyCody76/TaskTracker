#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

struct Task {
    int id;
    std::string item;
    int completed;
    int inProgress;
};

void executeHelp();
bool validCommand(const std::string &command);
void executeAdd(std::string item);
void executeDelete(int deleteId);
void executeViewList();
void executeUpdate(int updateId, std::string new_item);
void executeDone(int updateId);
void executeInProgress(int updateId);
void saveTasks(std::vector<Task> task);
std::vector<Task> loadTasks();


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


Task praseTask(const std::string& line){
    size_t pos1 = line.find("|");
    size_t pos2 = line.find("|", pos1 + 1);
    size_t pos3 = line.find("|", pos2 + 1);
    
    if(pos1 == std::string::npos || pos2 == std::string::npos || pos3 == std::string::npos){
        throw std::runtime_error("Invalid task format");
    }

    int id = std::stoi(line.substr(0, pos1));
    std::string item = line.substr(pos1 + 1, pos2 - pos1 - 1);
    int completed = std::stoi(line.substr(pos2 + 1, pos3 - pos2 - 1));
    int inProgress = std::stoi(line.substr(pos3 + 1));

    Task task;
    task.id = id;
    task.item = item;
    task.completed = completed;
    task.inProgress = inProgress;

    return task;
}

std::string stringTask(Task task){
    std::string id = std::to_string(task.id);
    std::string item = task.item;
    std::string completed = std::to_string(task.completed);
    std::string inProgress = std::to_string(task.inProgress);
    return id + "|" + item + "|" + completed + "|" + inProgress + "\n";
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
    std::vector<Task> tasks = loadTasks();

    int lastId = tasks.back().id;
    int newId = lastId + 1;

    Task new_task;
    new_task.id = newId;
    new_task.item = item;
    new_task.completed = 0;
    new_task.inProgress = 0;
    std::string task_string = stringTask(new_task);

    std::ofstream outfile("tasks.txt", std::ios::app);
    outfile << task_string;
    outfile.close();

    std::cout << "Task added with ID: " << newId << "\n";
}

void executeDelete(int deleteId){
    std::vector<Task> tasks = loadTasks();
    std::vector<Task> new_tasks;
    bool found = false;
    for(const Task& task: tasks){
        if(task.id != deleteId){
            new_tasks.push_back(task);
            found = true;
        }
    }
    saveTasks(new_tasks);
    if(found){
        std::cout << "Task delted with ID: " << deleteId << "\n";
    }else{
        std::cout << "No task is found at ID: " << deleteId << "\n";
    }
}

void executeViewList(){
    std::vector<Task> tasks = loadTasks();

    std::cout << "---View Tasks---" << "\n";
    for(const Task& task: tasks){
        std::cout << task.id << " " << task.item << " " << (task.completed ? "[x]" : "[ ]") << (task.inProgress ? " <- In Progress" : "") << "\n";
    }
    std::cout << "----------------" << "\n";
}

void executeUpdate(int updateId, std::string new_item){
    std::vector<Task> tasks = loadTasks();
    bool found = false;

    for(Task& task: tasks){
        if(task.id == updateId){
            task.item = new_item;
            found = true;
        }
    }

    saveTasks(tasks);

    if(found){
        std::cout << "Task at ID " << updateId << " updated." << "\n";
    }else{
        std::cout << "Id doesn't exist in the list." << "\n";
    }
}  

void executeDone(int updateId){
    std::vector<Task> tasks = loadTasks();
    bool found = false;

    for(Task& task: tasks){
        if(task.id == updateId){
            task.completed = 1;
            task.inProgress = 0;
            found = true;
        }
    }

    saveTasks(tasks);

    if(found){
        std::cout << "Task at ID " << updateId << " marked as Done." << "\n";
    }else{
        std::cout << "Id doesn't exist in the list." << "\n";
    }
}

void executeInProgress(int updateId){
    std::vector<Task> tasks = loadTasks();
    bool found = false;

    for(Task& task: tasks){
         if(task.id == updateId){
            if(task.completed){
                std::cout << "The taks is already completed.\n";
                return;
            }
            found = true;
            task.inProgress = 1;
        }
    }

    saveTasks(tasks);

    if(found){
        std::cout << "Task at ID " << updateId << " marked as In Progress." << "\n";
    }else{
        std::cout << "Id doesn't exist in the list." << "\n";
    }
}

void saveTasks(std::vector<Task> tasks){
    if(tasks.empty()) return;
    std::ofstream outfile("tasks.txt", std::ios::trunc);
    for(const Task& task: tasks){
        std::string task_string = stringTask(task);
        outfile << task_string;
    }
    outfile.close();
}

std::vector<Task> loadTasks(){
    std::vector<Task> tasks;
    std::ifstream infile("tasks.txt");
    std::string line;
    while(std::getline(infile, line)){
        if(line.empty()) continue;
        Task task = praseTask(line);
        tasks.push_back(task);
    }
    infile.close();
    return tasks;
}