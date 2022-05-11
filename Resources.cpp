#include <iostream>
#include <filesystem>
#include <opencv2/imgcodecs.hpp>
#include "Resources.hpp"

using namespace cv;

// Function for extracting extensions from filenames (.mp4, .png, etc)
string getExtension(string& filename)
{
	string result = filename.substr(MIN(filename.length(), filename.rfind('.')),-1);
	transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

// Function for mapping program's runtime arguments to the Args structure
void processArgs(int argc, char** argv, Args& args)
{
    if(argc < 3)
	{
		cout << "Not enough arguments!" << endl;
		exit(1);
	}
	args.ifile = string(argv[1]);
	args.ofile = string(argv[2]);
	if(!filesystem::exists(args.ifile))
	{
		cout << "File '" << args.ifile << "' does not exist!" << endl;
		exit(4);
	}
	if(!(args.asImage = haveImageWriter(args.ofile)))
	{
		string ext = getExtension(args.ofile);
		if(ext != ".gif" && ext != ".mp4")
		{
			cout << "Unsupported output format: " << ext << endl;
			cout << "If you are trying to output video, only mp4 and gif are supported for now." << endl;
			exit(2);
		}
	}
	if(argc > 3) args.xEndScale = stod(argv[3]);
	if(argc > 4) args.yEndScale = stod(argv[4]);
	if(argc > 5) args.xStartScale = stod(argv[5]);
	if(argc > 6) args.yStartScale = stod(argv[6]);
	if(argc > 7) args.loops = stod(argv[7]);
	if(argc > 8) args.dupes = stod(argv[8]);
	if(argc > 9) args.xResize = stod(argv[9]);
	if(argc > 10) args.yResize = stod(argv[10]);
	if(args.xEndScale == 0 || args.yEndScale == 0 || args.xStartScale == 0 || args.yStartScale == 0 || args.loops == 0 || args.dupes == 0 || args.xResize == 0 || args.yResize == 0)
	{
		cout << "No option may be 0" << endl;
		exit(5);
	}
}
