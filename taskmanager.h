#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

struct TaskManager{
    std::vector <std::string> file_lines;
    std::string file_path;

    TaskManager(const std::string& path);
    ~TaskManager();

    void addTask(std::string_view task);
    std::vector <std::string> findTasks(std::string_view keyword) const;
    void deleteTasks(std::string_view keyword);
    void saveTasks();
    void exportTasks() const;
    void backup() const;
};

#endif // TASKMANAGER_H
