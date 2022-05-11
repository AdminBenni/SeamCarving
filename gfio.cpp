#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "gfio.hpp"
#include "Progress.hpp"
#include "egif/GifEncoder.h"

// Function which splits images and videos into a vector of frames (Single frame for images)
void extractFrames(string& filename, vector<Mat>& frames, int count, int loops, int dupes, double xResize, double yResize)
{
	VideoCapture cap(filename); // VideoCapture also works for images, it only returns one frame
	count = count == -1 ? INT_MAX : count; // Specifies maximum frames to read from capture, -1 for all frames
	if(loops < 1 || dupes < 1)
	{
		cout << "Decimal loops and dupes has not been implemented yet!" << endl;
		loops = 1, dupes = 1;
	}
	if(!cap.isOpened())
	{
		cout << "Error opening file for frame extraction!" << endl;
		exit(1);
	}
	int size = MAX(1, (int)cap.get(CAP_PROP_FRAME_COUNT)); // Getting the frame count can be quite inconsistent for some file formats
	frames.reserve(dupes * loops * size); // Reserves minimum needed size of image to minimize data copying
	do
	{
		if((xResize != 1 || yResize != 1) && frames.size() > 0) // If user specifies a resize, resize image according to given cale
			resize(frames[frames.size()-1],
				frames[frames.size()-1],
				Size((int)(xResize * frames[frames.size()-1].cols),
					(int)(yResize * frames[frames.size()-1].rows)),
				0,0,INTER_CUBIC);
		if(dupes > 1 && frames.size() > 0) // If user specified dupes > 1, duplicate frame in frame vector dupes times
			for(int i = dupes; i--;)
				frames.push_back(frames[frames.size()-1]);
		frames.push_back(Mat()); // Push empty frame into frame counter
	}while(cap.read(frames[frames.size()-1]) && count--); // Reads frame from capture into last frame of frame vector, breaks loop when reading frame fails or max frame count reached
	frames.pop_back(); // Removes last frame from frame vector since it is empty
	cap.release();
	size = frames.size();
	int trueSize = round(loops * (double)frames.size());
	frames.resize(trueSize); // Resizes frame vector to final required size (either truncates unwanted frames or creates more empty frames)
	for(int i = size; i < trueSize; i++) // Duplicates frames to end of frame vector loops times
		frames[i % size].copyTo(frames[i]);
}

// Function which saves frame vector as a gif (unfortunately quite slow)
void saveGif(string& filename, vector<Mat>& frames)
{
	cout << "Saving Gif (This may be slow)." << endl;
	Progress progress("Frames: ");
	GifEncoder encoder;
	encoder.open(filename, frames[0].cols, frames[0].rows, 1, false, 0, MAX(frames[0].cols, frames[0].rows) * 3);
	for(int i = 0; i < frames.size(); i++)
	{
		progress.display(i, frames.size()-1);
		// Pushes frame to gif using pointer to frame's data as unsigned byte pointer so GifEncoder can read it
		encoder.push(GifEncoder::PIXEL_FORMAT_BGR, (uint8_t*)frames[i].data, frames[i].cols, frames[i].rows, 1);
		progress.refresh();
	}
	encoder.close();
	progress.refresh();
}
