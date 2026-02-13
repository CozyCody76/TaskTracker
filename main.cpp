#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <algorithm>

enum Status{
    Pending,
    Done,
    InProgress,
};

struct Task {
    int id;
    std::string item;
    Status status;

    void makeDone(){
        if(status == Status::Done){
            throw std::logic_error("Task already completed");
        }
        status = Status::Done;
    }  

    void makeInProgress(){
        if(status == Status::Done){
            throw std::logic_error("Cannot move Done Task to InProgress");
        }
        status = Status::InProgress;
    }
};

using CommandFunctionType = std::function<void(int, char**)>;

std::unordered_map<std::string, CommandFunctionType> handleCommands();

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "No command line arguments provided.\n";
        std::cout << "Type Help for more information.\n";
        return 1;
    }

    std::unordered_map<std::string, CommandFunctionType> commands = handleCommands();

    std::string command = argv[1];

    if(commands.find(command) != commands.end()){
        commands[command](argc, argv);
    }else{
        std::cout << "Invalid Command: " << command << "\n";
        std::cout << "Type help to view avaliable commands. \n";
    }

    
    return 0;
}

//---handling loading and saving data

Task parseTask(const std::string& line){
    size_t pos1 = line.find("|");
    size_t pos2 = line.find("|", pos1 + 1);
    
    if(pos1 == std::string::npos || pos2 == std::string::npos){
        throw std::runtime_error("Invalid task format");
    }

    int id = std::stoi(line.substr(0, pos1));
    std::string item = line.substr(pos1 + 1, pos2 - pos1 - 1);
    int status = std::stoi(line.substr(pos2 + 1));

    Task task;
    task.id = id;
    task.item = item;
    task.status = static_cast<Status>(status);

    return task;
}

std::string stringTask(const Task& task){
    std::string id = std::to_string(task.id);
    std::string item = task.item;
    std::string status = std::to_string(task.status);
    return id + "|" + item + "|" + status + "\n";
}

void saveTasks(const std::vector<Task>& tasks){
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
        try{
            Task task = parseTask(line);
            tasks.push_back(task);
        }catch(const std::exception& e){
            std::cout << "Loading data fail.\n";
        }
    }
    infile.close();
    return tasks;
}

//---handling commands

void executeHelp()
{
    std::cout << "Available commands:\n";
    std::cout << "  add        - Add a new item\n";
    std::cout << "  delete     - Delete an existing item\n";
    std::cout << "  update     - Update an existing item\n";
    std::cout << "  list       - List all items\n";
    std::cout << "  list -flag - --done, --inprogress, --id, --item"; 
    std::cout << "  done       - Mark an existing item Done\n";
    std::cout << "  inprogress - Make an existing item In Progress\n";
    std::cout << "  help       - Show this help message\n";
}

void executeAdd(std::string item){
    std::vector<Task> tasks = loadTasks();
    int newId = 0;

    for(const Task& task : tasks){
        if(task.id > newId){
            newId = task.id;
        }
    }

    Task new_task;
    new_task.id = newId + 1;
    new_task.item = item;
    new_task.status = Status::Pending;
    tasks.push_back(new_task);
    saveTasks(tasks);

    std::cout << "Task added with ID: " << new_task.id << "\n";
}

void executeDelete(int deleteId){
    std::vector<Task> tasks = loadTasks();
    std::vector<Task> new_tasks;

    bool found = false;
    for(const Task& task: tasks){
        if(task.id != deleteId){
            new_tasks.push_back(task);
            continue;
        }
        found = true;
    }
    saveTasks(new_tasks);
    if(found){
        std::cout << "Task deleted with ID: " << deleteId << "\n";
    }else{
        std::cout << "No task is found at ID: " << deleteId << "\n";
    }
}

std::vector<Task> handleViewListFlags(int argc, char** argv){
    std::vector<Task> tasks = loadTasks();
    if(argc < 3) return tasks;

    std::vector<Task> filtered_tasks;

    bool showPendingOnly = false;
    bool showDoneOnly = false;
    bool showInProgressOnly = false;
    bool sortById = false;
    bool sortByItem = false;

    for(int i = 2; i < argc; i++){
        std::string flag = argv[i];
        if(flag == "--pending"){
            showPendingOnly = true;
            showDoneOnly = false;
            showInProgressOnly = false;
        }else if(flag == "--done"){
            showDoneOnly = true;
            showInProgressOnly = false;
            showPendingOnly = false;
        }else if(flag == "--inprogress"){
            showInProgressOnly = true;
            showDoneOnly = false;
            showPendingOnly = false;
        }else if(flag == "--id"){
            sortById = true;
            sortByItem = false;
        }else if(flag == "--item"){
            sortByItem = true;
            sortById = false;
        }else{
            std::cout << "Invalid flag for list.\n";
            break;
        }
    }

    if(sortById){
        std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b){
            return a.id < b.id;
        });
    }

    if(sortByItem){
        std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b){
            return a.item < b.item;
        });
    }

    if(showPendingOnly || showDoneOnly || showInProgressOnly){
        for(const Task& task : tasks){
            if(showPendingOnly && task.status != Status::Pending) continue;
            if(showDoneOnly && task.status != Status::Done) continue;
            if(showInProgressOnly && task.status != Status::InProgress) continue;
            filtered_tasks.push_back(task);
        }
    }

    return filtered_tasks;

}
void executeViewList(int argc, char* argv[]){
    std::vector<Task>tasks = handleViewListFlags(argc ,argv);


    std::cout << "---View Tasks---" << "\n";
    for(const Task& task: tasks){
        std::cout << task.id << " " << task.item << " " << (task.status == Status :: Done ? "[x]" : "[ ]") << (task.status == Status::InProgress ? " <- In Progress" : "") << "\n";
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
            try{
                task.makeDone();
                found = true;
            }catch(const std::logic_error& e){
                std::cout << e.what() << "\n";
            }
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
            try{
                task.makeInProgress();
                found = true;
            }catch(const std::logic_error& e){
                std::cout << e.what() << "\n";
            }
        }
    }

    saveTasks(tasks);

    if(found){
        std::cout << "Task at ID " << updateId << " marked as In Progress." << "\n";
    }else{
        std::cout << "Id doesn't exist in the list." << "\n";
    }
}

void handleHelp(int argc, char* argv[]){
    executeHelp();
}

void handleList(int argc, char* argv[]){
    executeViewList(argc, argv);
}

void handleAdd(int argc, char* argv[]){
    if(argc != 3){
        std::cout << "Invalid argument format.";
        return;
    }

    try{
        std::string command = argv[2];
        executeAdd(command);
    }catch(const std::invalid_argument& e) {
        std::cout << "Invalid argument type.";
    }
}

void handleDelete(int argc, char* argv[]){
    if(argc != 3){
        std::cout << "Invalid argument format.";
        return;
    }

    try{
        int id = std::stoi(argv[2]);
        executeDelete(id);
    }catch(const std::invalid_argument& e){
        std::cout << "Invalid argument type.";
    }
}

void handleUpdate(int argc, char* argv[]){
    if(argc != 4){
        std::cout << "Invalid argument format.";
        return;
    }
    
    try{
        int id = std::stoi(argv[2]);
        std::string new_item = argv[3];
        executeUpdate(id, new_item);
    }catch(const std::invalid_argument& e){
        std::cout << "Invalid argument type.";
    }
}

void handleDone(int argc, char* argv[]){
    if(argc != 3){
        std::cout << "Invalid argument format.";
        return;
    }

    try{
        int id = std::stoi(argv[2]);
        executeDone(id);
    }catch(const std::invalid_argument& e){
        std::cout << "Invalid argument type.";
    }
}

void handleInProgress(int argc, char* argv[]){
    if(argc != 3){
        std::cout << "Invalid argument format.";
        return;
    }

    try{
        int id = std::stoi(argv[2]);
        executeInProgress(id);
    }catch(const std::invalid_argument& e){
        std::cout << "Invalid argument type.";
    }
}  

std::unordered_map<std::string, CommandFunctionType> handleCommands(){
    std::unordered_map<std::string, CommandFunctionType> commands;
    commands["list"] = handleList;
    commands["add"] = handleAdd;
    commands["delete"] = handleDelete;
    commands["update"] = handleUpdate;
    commands["done"] = handleDone;
    commands["inprogress"] = handleInProgress;
    commands["help"] = handleHelp;
    return commands;
}
