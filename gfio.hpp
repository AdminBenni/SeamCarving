#ifndef GFIO_HPP
#define GFIO_HPP

#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

// Function which splits images and videos into a vector of frames (Single frame for images)
void extractFrames(string& filename, vector<Mat>& frames, int count = -1, int loops = 1, int dupes = 1, double xResize = 1, double yResize = 1);

// Function which saves frame vector as a gif (unfortunately quite slow)
void saveGif(string& filename, vector<Mat>& frames);

#endif // GFIO_HPP
