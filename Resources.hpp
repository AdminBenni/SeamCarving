#ifndef RESOURCES_HPP
#define RESOURCES_HPP

using namespace std;

// Struct for storing program's input arguments
struct Args
{
    string ifile;
    string ofile;
    bool asImage;
    double xEndScale = 1;
    double yEndScale = 1;
    double xStartScale = 1;
    double yStartScale = 1;
    double loops = 1;
    double dupes = 1;
    double xResize = 1;
    double yResize =1;
};

// https://www.arduino.cc/reference/en/language/functions/math/map/
template<class T>
T forceRange(T x, T in_min, T in_max, T out_min, T out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Function for extracting extensions from filenames (.mp4, .png, etc)
string getExtension(string& filename);

// Function for mapping program's runtime arguments to the Args structure
void processArgs(int argc, char** argv, Args& args);

#endif // RESOURCES_HPP
