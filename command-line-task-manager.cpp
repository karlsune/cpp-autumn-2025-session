#include <iostream> // for input&output operations
#include <vector> // for using the std::vector to store tasks
#include <string> // for using std::string class for text
#include <limits> // for handling input errors

// Task structure, grouping related variables together

struct Task {
    std::string description;
    bool isCompleted;
};

// Function prototypes, tells the compiler about our functions, defined later
void addTask(std::vector<Task>& tasks);
void viewTasks(const std::vector<Task>& tasks);
void markTaskCompleted(std::vector<Task>& tasks);
void deleteTask(std::vector<Task>& tasks);
void clearInputBuffer();

//main function, entry point of program

int main() {
    //std::vector is a dynamic array that holds all task objs.
    std::vector<Task> task_list;
    // variable to store usrs menu choice
    int choice;

    //do-while loop
    do {
        // display menu options to user
        std::cout << "\n--- Command-Line Task Manager ---\n";
        std::cout << "1. Add a new task\n";
        std::cout << "2. View all tasks\n";
        std::cout << "3. Mark a task as completed\n";
        std::cout << "4. Delete a task\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice: ";

        //read user choice from charstream
        std::cin >> choice;

        //call clearInputBuffer to handle bad input & avoid looping
        clearInputBuffer();

        //switch&case to handle menu option selection

        switch(choice) {
            case 1:
                addTask(task_list);
                break;
            case 2:
                viewTasks(task_list);
                break;
            case 3:
                markTaskCompleted(task_list);
                break;
            case 4:
                deleteTask(task_list);
                break;
            case 5:
                std::cout << "Exiting the program. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid choice, please try again.\n";
                break;
        }
    } while (choice != 5); //loop continue until user choose 5

    return 0;
}

// function defenitions

//add task to the vector function
void addTask(std::vector<Task>& tasks) {
    std::string description; //var to hold desc string
    std::cout << "Enter the task defenition: ";
    std::getline(std::cin, description); //get entire line & assign to desc

    //create new task obj
    Task newTask;
    newTask.description = description;
    newTask.isCompleted = false;

    tasks.push_back(newTask); //add new task to end of vector

    std::cout << "Task successfully added!\n";
}

// display all tasks function
void viewTasks(const std::vector<Task>& tasks) {
    // Check if the vector is empty.
    if (tasks.empty()) {
        std::cout << "No tasks to display.\n";
        return; // Exit the function early.
    }

    std::cout << "\n--- Your Tasks ---\n";
    // A 'for' loop to iterate over each task in the vector.
    // The 'size_t' type is an unsigned integer used for sizes and indices.
    for (size_t i = 0; i < tasks.size(); ++i) {
        // 'const auto& task' is a C++11 feature called a range-based for loop.
        // It's a cleaner way to loop through a container.
        const Task& task = tasks[i];
        // 'i + 1' is used to show a user-friendly 1-based index.
        std::cout << i + 1 << ". " << (task.isCompleted ? "[X] " : "[ ] ")
        << task.description << "\n";
    }
}

// Function to mark a task as completed.
void markTaskCompleted(std::vector<Task>& tasks) {
    viewTasks(tasks); // First, show the user the list of tasks.

    // Check if there are any tasks to mark.
    if (tasks.empty()) {
        return;
    }

    int task_index; // Variable to hold the user's task number choice.
    std::cout << "Enter the number of the task to mark as completed: ";
    std::cin >> task_index;

    // Check for valid input and prevent out-of-bounds access.
    // 'task_index - 1' is used because vectors are 0-indexed.
    if (task_index > 0 && task_index <= tasks.size()) {
        tasks[task_index - 1].isCompleted = true;
        std::cout << "Task marked as completed.\n";
    } else {
        std::cout << "Invalid task number. Please try again.\n";
    }
    clearInputBuffer();
}

// Function to delete a task.
void deleteTask(std::vector<Task>& tasks) {
    viewTasks(tasks); // Show the user the list of tasks.

    if (tasks.empty()) {
        return;
    }

    int task_index;
    std::cout << "Enter the number of the task to delete: ";
    std::cin >> task_index;

    // Check for valid input.
    if (task_index > 0 && task_index <= tasks.size()) {
        // The 'erase' function removes an element at a specific position.
        // 'tasks.begin() + (task_index - 1)' gives us an iterator to the element.
        tasks.erase(tasks.begin() + (task_index - 1));
        std::cout << "Task deleted successfully.\n";
    } else {
        std::cout << "Invalid task number. Please try again.\n";
    }
    clearInputBuffer();
}

// Helper function to clear the input buffer.

void clearInputBuffer() {
    // std::cin.ignore discards characters from the input buffer.
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


























