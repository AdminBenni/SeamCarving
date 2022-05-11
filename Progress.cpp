#include <iomanip>
#include <cmath>
#include <cstdlib>
#include "Progress.hpp"
#include "Resources.hpp"

Progress::Progress(string title, int barwidth, ostream& out) : title(title), barwidth(barwidth), out(out) {}

void Progress::display(int at, int mx, int mn)
{
    mx = max(mx, 1); // No zero division thank you
    int mid = forceRange(at, mn, mx, 0, barwidth-1);
    if(title != "")
        out << title << " ";
    out << "[";
    for(int i = 0; i < mid; i++)
        out << "=";
    out << ">";
    for(int i = mid+1; i < barwidth; i++)
        out << " ";
    out << "] ";
    out << setfill('0') << setw(3) << (at-mn)*100/(mx-mn) << "% ";
    out << setw((int)ceil(log10(mx-mn+1))) <<at-mn << "/" << mx-mn << endl;
    out << setfill(' ') << setw(0);
}

void Progress::refresh()
{
    out << "\033[F\33[2K\r";
}
