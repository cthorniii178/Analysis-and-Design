#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Represents one course in the advising program.
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Removes whitespace from the beginning and end of a string.
string trim(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }

    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

// Converts text to uppercase so course searches are case-insensitive.
string toUpper(string text) {
    transform(text.begin(), text.end(), text.begin(),
        [](unsigned char ch) { return static_cast<char>(toupper(ch)); });
    return text;
}

// Finds a course by course number and returns its position.
// Returns -1 if the course is not found.
int findCourseIndex(const vector<Course>& courses, const string& courseNumber) {
    string target = toUpper(trim(courseNumber));

    for (size_t i = 0; i < courses.size(); ++i) {
        if (toUpper(courses[i].courseNumber) == target) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

// Loads course data from a comma-separated input file.
bool loadCourses(const string& fileName, vector<Course>& courses) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: Could not open file \"" << fileName << "\"." << endl;
        return false;
    }

    vector<Course> loadedCourses;
    string line;

    while (getline(inputFile, line)) {
        if (trim(line).empty()) {
            continue;
        }

        stringstream lineStream(line);
        string field;
        vector<string> fields;

        while (getline(lineStream, field, ',')) {
            fields.push_back(trim(field));
        }

        // Every valid line must contain at least a course number and title.
        if (fields.size() < 2 || fields[0].empty() || fields[1].empty()) {
            cout << "Warning: Skipping invalid course record: " << line << endl;
            continue;
        }

        Course course;
        course.courseNumber = toUpper(fields[0]);
        course.courseTitle = fields[1];

        // Any remaining non-empty fields are prerequisite course numbers.
        for (size_t i = 2; i < fields.size(); ++i) {
            if (!fields[i].empty()) {
                course.prerequisites.push_back(toUpper(fields[i]));
            }
        }

        loadedCourses.push_back(course);
    }

    inputFile.close();

    if (loadedCourses.empty()) {
        cout << "Error: No valid course data was found in the file." << endl;
        return false;
    }

    courses = loadedCourses;
    cout << courses.size() << " courses loaded successfully." << endl;
    return true;
}

// Prints all courses in alphanumeric order.
void printCourseList(vector<Course> courses) {
    sort(courses.begin(), courses.end(),
        [](const Course& a, const Course& b) {
            return a.courseNumber < b.courseNumber;
        });

    cout << endl;
    cout << "Here is a sample schedule:" << endl;

    for (const Course& course : courses) {
        cout << course.courseNumber << ", " << course.courseTitle << endl;
    }
}

// Prints one course and the numbers and titles of its prerequisites.
void printCourseInformation(const vector<Course>& courses) {
    string courseNumber;

    cout << "What course do you want to know about? ";
    getline(cin, courseNumber);

    int courseIndex = findCourseIndex(courses, courseNumber);

    if (courseIndex == -1) {
        cout << "Course \"" << toUpper(trim(courseNumber))
            << "\" was not found." << endl;
        return;
    }

    const Course& course = courses[courseIndex];

    cout << course.courseNumber << ", " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    cout << "Prerequisites: ";

    for (size_t i = 0; i < course.prerequisites.size(); ++i) {
        const string& prerequisiteNumber = course.prerequisites[i];
        int prerequisiteIndex = findCourseIndex(courses, prerequisiteNumber);

        cout << prerequisiteNumber;

        if (prerequisiteIndex != -1) {
            cout << ", " << courses[prerequisiteIndex].courseTitle;
        }

        if (i < course.prerequisites.size() - 1) {
            cout << "; ";
        }
    }

    cout << endl;
}

// Displays the main menu.
void displayMenu() {
    cout << endl;
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
    cout << "What would you like to do? ";
}

int main() {
    vector<Course> courses;
    bool dataLoaded = false;
    string menuInput;

    cout << "Welcome to the course planner." << endl;

    while (true) {
        displayMenu();
        getline(cin, menuInput);

        // Option 1: Load course information from a file.
        if (menuInput == "1") {
            string fileName;
            cout << "Enter the course data file name: ";
            getline(cin, fileName);

            dataLoaded = loadCourses(trim(fileName), courses);
        }

        // Option 2: Print the complete sorted course list.
        else if (menuInput == "2") {
            if (!dataLoaded) {
                cout << "Please load the course data first using option 1." << endl;
            }
            else {
                printCourseList(courses);
            }
        }

        // Option 3: Print information for one course.
        else if (menuInput == "3") {
            if (!dataLoaded) {
                cout << "Please load the course data first using option 1." << endl;
            }
            else {
                printCourseInformation(courses);
            }
        }

        // Option 9: Exit the program.
        else if (menuInput == "9") {
            cout << "Thank you for using the course planner!" << endl;
            break;
        }

        // Handles all invalid menu selections.
        else {
            cout << menuInput << " is not a valid option." << endl;
        }
    }

    return 0;
}