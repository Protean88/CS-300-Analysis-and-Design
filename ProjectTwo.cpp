//=============================================================================
// Name        : ProjectTwo.cpp
// Author      : Jordon Neal
// Course      : CS 300 - Data Structures and Algorithms
// Description : This program helps ABCU advisors by loading course information
// from a CSV file into a binary search tree (the structure recommended back in
// Project One). It then displays all the courses in alphanumeric order and 
// lets you look up any individual course along with its prerequisites.
//=============================================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

//============================================================================
// Course structure - one course object holds the data from one line of the file
//============================================================================
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

//============================================================================
// Helper functions for cleaning up text
//============================================================================

// trims spaces, tabs, and stray carriage returns off both ends of a string
string trim(const string& text) {
    const string whitespace = " \t\r\n";
    size_t start = text.find_first_not_of(whitespace);
    if (start == string::npos) {
        return "";
    }
    size_t end = text.find_last_not_of(whitespace);
    return text.substr(start, end - start + 1);
}

// converts a string to uppercase so "csci400" and "CSCI400" match
string toUpper(string text) {
    transform(text.begin(), text.end(), text.begin(),
              [](unsigned char c) { return static_cast<char>(toupper(c)); });
    return text;
}

// splits one line of the file into its comma-separated fields
vector<string> splitLine(const string& line) {
    vector<string> fields;
    string field;
    stringstream lineStream(line);

    while (getline(lineStream, field, ',')) {
        fields.push_back(trim(field));
    }

    // drop empty trailing fields (like "CSCI100,Intro to CS,,")
    while (!fields.empty() && fields.back().empty()) {
        fields.pop_back();
    }
    return fields;
}

//============================================================================
// Binary Search Tree class
//============================================================================

// each node holds one course plus links to its left and right children
struct Node {
    Course course;
    Node* left;
    Node* right;

    explicit Node(const Course& aCourse) : course(aCourse), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
private:
    Node* root;
    int courseCount;

    void addNode(Node* node, const Course& course);
    void inOrder(Node* node) const;
    void destroyTree(Node* node);

public:
    BinarySearchTree();
    ~BinarySearchTree();

    void Insert(const Course& course);
    void PrintAll() const;
    Course* Search(const string& courseNumber) const;
    void Clear();
    bool IsEmpty() const;
    int Size() const;
};

// start with an empty tree
BinarySearchTree::BinarySearchTree() : root(nullptr), courseCount(0) {}

// free every node when the tree goes away
BinarySearchTree::~BinarySearchTree() {
    destroyTree(root);
}

// post-order delete so children are freed before their parent
void BinarySearchTree::destroyTree(Node* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

// wipes the tree so the file can be reloaded without duplicates
void BinarySearchTree::Clear() {
    destroyTree(root);
    root = nullptr;
    courseCount = 0;
}

bool BinarySearchTree::IsEmpty() const {
    return root == nullptr;
}

int BinarySearchTree::Size() const {
    return courseCount;
}

// adds a course - if the tree is empty it becomes the root,
// otherwise we walk down to find its spot
void BinarySearchTree::Insert(const Course& course) {
    if (root == nullptr) {
        root = new Node(course);
        ++courseCount;
    } else {
        addNode(root, course);
    }
}

// recursive helper: smaller course numbers go left, larger go right
void BinarySearchTree::addNode(Node* node, const Course& course) {
    if (course.courseNumber < node->course.courseNumber) {
        if (node->left == nullptr) {
            node->left = new Node(course);
            ++courseCount;
        } else {
            addNode(node->left, course);
        }
    } else if (course.courseNumber > node->course.courseNumber) {
        if (node->right == nullptr) {
            node->right = new Node(course);
            ++courseCount;
        } else {
            addNode(node->right, course);
        }
    } else {
        // same course number showed up twice - keep the newest data
        node->course = course;
    }
}

// in-order traversal visits nodes smallest to largest,
// which gives us the alphanumeric list without a separate sort
void BinarySearchTree::PrintAll() const {
    inOrder(root);
}

void BinarySearchTree::inOrder(Node* node) const {
    if (node != nullptr) {
        inOrder(node->left);
        cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
        inOrder(node->right);
    }
}

// walks down the tree comparing course numbers - returns nullptr if not found
Course* BinarySearchTree::Search(const string& courseNumber) const {
    Node* current = root;

    while (current != nullptr) {
        if (courseNumber == current->course.courseNumber) {
            return &current->course;
        }
        if (courseNumber < current->course.courseNumber) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return nullptr;
}

//============================================================================
// File loading
//============================================================================

// reads the file, validates every line, and loads the courses into the tree.
// returns true only if the whole file loaded cleanly.
bool loadCourses(const string& fileName, BinarySearchTree& courseTree) {
    ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        cout << "Error: could not open file \"" << fileName << "\"." << endl;
        return false;
    }

    vector<Course> parsedCourses;
    set<string> knownCourseNumbers;
    string line;
    int lineNumber = 0;
    bool formatIsValid = true;

    // first pass: parse every line and check the basic format
    while (getline(inputFile, line)) {
        ++lineNumber;

        // strip a UTF-8 byte order mark if the file was saved from Excel
        if (lineNumber == 1 && line.size() >= 3 &&
            line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }

        line = trim(line);
        if (line.empty()) {
            continue;  // skip blank lines
        }

        vector<string> fields = splitLine(line);

        // every line needs at least a course number and a title
        if (fields.size() < 2 || fields[0].empty() || fields[1].empty()) {
            cout << "Format error on line " << lineNumber
                 << ": each line needs a course number and a title." << endl;
            formatIsValid = false;
            continue;
        }

        Course course;
        course.courseNumber = toUpper(fields[0]);
        course.courseTitle = fields[1];

        // anything past the title is a prerequisite
        for (size_t i = 2; i < fields.size(); ++i) {
            if (!fields[i].empty()) {
                course.prerequisites.push_back(toUpper(fields[i]));
            }
        }

        knownCourseNumbers.insert(course.courseNumber);
        parsedCourses.push_back(course);
    }
    inputFile.close();

    // second pass: every prerequisite has to be a course that exists in the file
    for (const Course& course : parsedCourses) {
        for (const string& prereq : course.prerequisites) {
            if (knownCourseNumbers.find(prereq) == knownCourseNumbers.end()) {
                cout << "Format error: prerequisite " << prereq << " for "
                     << course.courseNumber << " is not listed as a course in the file." << endl;
                formatIsValid = false;
            }
        }
    }

    if (!formatIsValid) {
        cout << "The file was not loaded. Please fix the errors above and try again." << endl;
        return false;
    }

    if (parsedCourses.empty()) {
        cout << "Error: the file \"" << fileName << "\" has no course data." << endl;
        return false;
    }

    // everything checks out - clear any old data and fill the tree
    courseTree.Clear();
    for (const Course& course : parsedCourses) {
        courseTree.Insert(course);
    }

    cout << courseTree.Size() << " courses loaded successfully." << endl;
    return true;
}

//============================================================================
// Menu actions
//============================================================================

// prints one course with its title and each prerequisite's number and title
void printCourseInformation(const BinarySearchTree& courseTree, const string& courseNumber) {
    Course* course = courseTree.Search(courseNumber);

    if (course == nullptr) {
        cout << courseNumber << " was not found. Check the course number and try again." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseTitle << endl;

    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
        return;
    }

    // first line matches the sample output, then list each prereq's title
    cout << "Prerequisites: ";
    for (size_t i = 0; i < course->prerequisites.size(); ++i) {
        cout << course->prerequisites[i];
        if (i < course->prerequisites.size() - 1) {
            cout << ", ";
        }
    }
    cout << endl;

    for (const string& prereqNumber : course->prerequisites) {
        Course* prereq = courseTree.Search(prereqNumber);
        if (prereq != nullptr) {
            cout << "   " << prereq->courseNumber << ", " << prereq->courseTitle << endl;
        }
    }
}

void displayMenu() {
    cout << endl;
    cout << "   1. Load Data Structure." << endl;
    cout << "   2. Print Course List." << endl;
    cout << "   3. Print Course." << endl;
    cout << "   9. Exit" << endl;
    cout << endl;
    cout << "What would you like to do? ";
}

//============================================================================
// main - runs the menu loop until the user picks 9
//============================================================================
int main() {
    BinarySearchTree courseTree;
    bool dataLoaded = false;
    string userInput;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        displayMenu();

        // read the whole line so input like "abc" or "2 3" can't break the loop
        if (!getline(cin, userInput)) {
            break;  // end of input - exit cleanly
        }
        userInput = trim(userInput);

        // make sure the input is a whole number before converting it
        bool isNumber = !userInput.empty() && userInput.size() <= 9 &&
                        all_of(userInput.begin(), userInput.end(),
                               [](unsigned char c) { return isdigit(c) != 0; });
        if (!isNumber) {
            // same wording as the sample output ("8 is not a valid option.")
            cout << (userInput.empty() ? "That" : userInput)
                 << " is not a valid option." << endl;
            choice = 0;
            continue;
        }
        choice = stoi(userInput);

        switch (choice) {
            case 1: {
                cout << "Enter the name of the course data file: ";
                string fileName;
                getline(cin, fileName);
                fileName = trim(fileName);

                if (fileName.empty()) {
                    cout << "No file name entered." << endl;
                    break;
                }
                // only flip the flag on a clean load; a failed reload keeps the old data
                if (loadCourses(fileName, courseTree)) {
                    dataLoaded = true;
                }
                break;
            }

            case 2:
                if (!dataLoaded) {
                    cout << "Please load the data structure first (option 1)." << endl;
                    break;
                }
                // the in-order traversal prints in true alphanumeric order, so
                // CSCI300 comes before CSCI301 (the sample output has those two flipped)
                cout << "Here is a sample schedule:" << endl << endl;
                courseTree.PrintAll();
                break;

            case 3: {
                if (!dataLoaded) {
                    cout << "Please load the data structure first (option 1)." << endl;
                    break;
                }
                cout << "What course do you want to know about? ";
                string courseNumber;
                getline(cin, courseNumber);
                courseNumber = toUpper(trim(courseNumber));

                if (courseNumber.empty()) {
                    cout << "No course number entered." << endl;
                    break;
                }
                printCourseInformation(courseTree, courseNumber);
                break;
            }

            case 9:
                cout << "Thank you for using the course planner!" << endl;
                break;

            default:
                cout << choice << " is not a valid option." << endl;
                break;
        }
    }

    return 0;
}
