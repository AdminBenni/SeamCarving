#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include "Resources.hpp"
#include "gfio.hpp"
#include "SeamCarving.hpp"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Args args;
	processArgs(argc, argv, args);
	vector<Mat> frames;
	extractFrames(args.ifile, frames, args.asImage ? 1 : -1, args.loops, args.dupes, args.xResize, args.yResize);

	if(frames.size() < 1)
	{
		cout << "Not enough frames to write" << endl;
		return 3;
	}

	cout << frames[0].cols << " x " << frames[0].rows << " @ " << frames.size() << endl;

	auto start = chrono::system_clock::now();

	if(frames.size() == 1 || args.asImage) // If result needs to saved as an image or single frame format
	{
		if(getExtension(args.ofile) == ".mp4") // Can't save a single frame video
		{
			cout << "Saving single image video is not supported!" << endl;
			return 4;
		}
		// Carves seams for the single image frame
		carveSeams(frames[0], (double)frames[0].cols - (double)frames[0].cols * args.xEndScale, (double)frames[0].rows - (double)frames[0].rows * args.yEndScale);
		if(getExtension(args.ofile) == ".gif") // gif supports single frame images
			saveGif(args.ofile, frames);
		else
			imwrite(args.ofile, frames[0]); // Saves using whatever other format user specified (PNG, JPG, etc)
	}
	else // If result needs to be saved as a video or multi-frame format
	{
		// Carves frames in parallel
		carveFramesMulti(frames, args.xStartScale, args.yStartScale, args.xEndScale, args.yEndScale);
		string ext = getExtension(args.ofile);
		int codec;
		if(ext == ".gif") // gif supports multi-frame images
			saveGif(args.ofile, frames);
		else
		{
			codec = VideoWriter::fourcc('a','v','c','1'); // Saving H264 doesn't work properly with OpenCV, AVC1 used instead
			VideoWriter writer(args.ofile, codec, 10, Size(frames[0].cols, frames[0].rows)); // Creates video encoder using specified video codec
			for(int i = 0; i < frames.size(); i++) // Encodes all frames
				writer.write(frames[i]);
			writer.release();
		}
	}

	auto end = (chrono::system_clock::now() - start);
	cout << "Elapsed time: " << setfill('0') << setw(2) <<
		chrono::duration_cast<chrono::hours>(end).count() << ":" << setw(2) << 
		chrono::duration_cast<chrono::minutes>(end).count() % 60 << ":" << setw(2) <<
		chrono::duration_cast<chrono::seconds>(end).count() % 60 << endl;
	filesystem::permissions(args.ofile, filesystem::perms::all); // So host can read file outside docker image
	return 0;
}
