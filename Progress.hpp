#ifndef PROGRESS_HPP
#define PROGRESS_HPP

#include <iostream>

using namespace std;

// Class for displaying progress of counters and loops (similar to Python's tqdm package)
class Progress
{
public:
    Progress(string title = "", int barwidth = 70, ostream& out = cout);

    void display(int at, int max, int min = 0);

    void refresh();

private:
    string title;
	int barwidth;
	ostream& out;
};

#endif // PROGRESS_HPP
