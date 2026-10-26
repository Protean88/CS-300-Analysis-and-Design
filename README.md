# CS 300: Data Structures and Algorithms

**Jordon Neal** | Southern New Hampshire University

## About This Repository

This repository holds my work from CS 300, built around an advising program for ABCU's Computer Science department. Advisors needed a way to load the course catalog, print every course in order, and look up a single course with its prerequisites.

**Artifacts:**
- `Neal_CS300_Project_One.docx`: Project One runtime and memory analysis comparing a vector, a hash table, and a binary search tree for storing the course data, with my recommendation.
- `ProjectTwo.cpp`: Project Two working C++ program that loads the course file into a binary search tree, prints all Computer Science courses in alphanumeric order, and looks up individual courses with their prerequisites.

---

## Reflection

### What was the problem you were solving in the projects for this course?

ABCU's advisors needed a tool that could read the course catalog from a file, check it for formatting errors, and answer two basic questions: what courses are offered (in order), and what does a specific course require before you can take it. Project One was figuring out which data structure should hold that data. Project Two was actually building the program around the structure I picked.

### How did you approach the problem? Consider why data structures are important to understand.

I started by writing pseudocode for the same program three different ways, using a vector, a hash table, and a binary search tree. Then I ran a line-by-line runtime analysis on each one for loading the file and creating the course objects. On paper all three hit O(n²) in the worst case, but the typical case told a different story. The vector stays slow because every search walks the whole list. The hash table is the fastest at lookups, but it doesn't keep anything in order, so printing a sorted list means sorting it every time. The BST gives fast average lookups and prints in order for free with an in-order traversal, which is exactly what the advisors asked for. That's why I went with the BST.

This is why data structures matter. The program does the same job no matter which one you pick, but the choice decides how fast it runs, how much memory it uses, and how much extra code you have to write. Picking the right one up front saves you from fighting it later.

### How did you overcome any roadblocks you encountered while going through the activities or project?

A lot of my roadblocks were in Visual Studio, not the code itself. More than once my file opened as "Miscellaneous Files" instead of being part of a project, which caused build and linker errors. The fix was setting up an Empty Project, adding the .cpp file through Solution Explorer, and making sure the CSV file sat in the same folder as the built program so it could actually find it.

On the code side, the hash table assignment's starter hint for removing an item would have broken the table if I'd followed it as written. Working through why it was wrong and handling the linked list removal cases properly taught me more than the assignment itself. For the final project, I tested the program with bad data on purpose, like missing fields and prerequisites that didn't exist, and checked my output against the rubric. That caught a sorting issue in my sample output before I turned it in.

### How has your work on this project expanded your approach to designing software and developing programs?

Before this course I'd usually just start coding and figure it out as I went. Now I plan first. Writing pseudocode and doing a runtime analysis before touching real code made Project Two way easier, because the hard decisions were already made. I also think more about how a program will be used, not just whether it works. The advisors cared about sorted output and quick lookups, and that drove the whole design. I use the same thinking at work when I build tools for my team: figure out what people actually need to do with the data, then pick the setup that makes that easy.

### How has your work on this project evolved the way you write programs that are maintainable, readable, and adaptable?

I break programs into small functions that each do one thing, like trimming text, splitting a line, loading the file, validating prerequisites, and printing a course. That makes each piece easier to read, test, and fix without touching everything else. I use clear names, comment the "why" instead of restating the code, and validate input so bad data gets a helpful message instead of a crash. Because the course data is kept separate from the menu and printing logic, the program could be adapted to a different data structure or a bigger catalog without rewriting the whole thing.
