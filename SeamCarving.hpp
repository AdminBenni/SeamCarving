#ifndef SEAM_CARVING_HPP
#define SEAM_CARVING_HPP

#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

// Function which calculates an image's energy map
void calcEnergy(Mat &image, Mat& energy);

// Function which uses dynamic programming to calculate minimum path from given x, y coordinate to the bottom of energy map
int dp(Mat& energy, vector<vector<int>>& mem, vector<vector<pair<int, int>>>& from, int x, int y);

// Function which initiates dp and constructs minimum seam from results, returns seam's starting coordinates
pair<int, int> findSeam(Mat& energy, vector<vector<int>>& mem, vector<vector<pair<int, int>>>& from);

// Function which uses image's energy map with findSeam to find minimum seam and carves it from the image
void carveSeam(Mat& img, Mat& newImg, Mat& energy);

// Function which carves n columns from image and then resizes it back to its original width
void carveColumns(Mat& img, int n, bool disp = true);

// Function which carves n rows from image and then resiees it back to its original height
void carveRows(Mat& img, int n, bool disp = true);

// Function which carves given amount of columns and rows from image
void carveSeams(Mat& img, int cols = 0, int rows = 0, bool disp = true);

// Function which performs seam carving for each frame in frame vector from given starting and ending scale
void carveFrames(vector<Mat>& frames, double x0, double y0, double x1, double y1);

// Function which performs seam carving for each frame in frame vector from given starting and ending scale in parallel (Multithreading)
void carveFramesMulti(vector<Mat>& frames, double x0, double y0, double x1, double y1);

#endif // SEAM_CARVING_HPP
