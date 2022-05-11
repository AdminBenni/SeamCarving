#include <thread>
#include <opencv2/imgproc.hpp>
#include "SeamCarving.hpp"
#include "Resources.hpp"
#include "Progress.hpp"

// Function which calculates an image's energy map
void calcEnergy(Mat &image, Mat& energy)
{
	Mat energy_x, energy_y;

	// Gaussian blur helps reducing noice from image for better energy map
	GaussianBlur(image, energy, Size(3,3), 0, 0, BORDER_DEFAULT);

	// Perform convolution using Sobel operator / filter
	Sobel(energy, energy_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT); // X axis
	Sobel(energy, energy_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT); // Y axis

	// Normalize output by converting each pixel to its absolute value
	convertScaleAbs(energy_x, energy_x); // X axis
	convertScaleAbs(energy_y, energy_y); // Y axis

	addWeighted(energy_x, 0.5, energy_y, 0.5, 0, energy); // Adds X and Y axis together

	cvtColor(energy, energy, COLOR_BGRA2GRAY); // Combines energies of all colour channels
}

// Function which uses dynamic programming to calculate minimum path from given x, y coordinate to the bottom of energy map
int dp(Mat& energy, vector<vector<int>>& mem, vector<vector<pair<int, int>>>& from, int x, int y)
{
	if(y >= energy.rows) return mem[x][y] = 0; // If reached bottom, return 0
	if(mem[x][y] != -1) return mem[x][y]; // If min path for coordinate has already been calculated, return calculated value
	int best = dp(energy, mem, from, x, y+1); // Value of min path going straight down
	from[x][y] = {x, y+1}; // Save min path into path
	if(x-1 >= 0)
	{
		best = min(best, dp(energy, mem, from, x-1, y+1)); // Value of min path going left diagonally
		if(mem[x-1][y+1] == best) // If value was better than straight down, save into path
			from[x][y] = {x-1, y+1};
	}
	if(x+1 < energy.cols)
	{
		best = min(best, dp(energy, mem, from, x+1, y+1)); // Value of min path going right diagonally
		if(mem[x+1][y+1] == best) // If value better than both straigt and right, save into path
			from[x][y] = {x+1, y+1};
	}
	return mem[x][y] = best + (int)energy.at<uchar>(y, x); // Return min value + energy cost of visiting this coordinate
}

// Function which initiates dp and constructs minimum seam from results, returns seam's starting coordinates
pair<int, int> findSeam(Mat& energy, vector<vector<int>>& mem, vector<vector<pair<int, int>>>& from)
{
	int best = INT_MAX; // Initilize as worst possible value
	pair<int, int> start;
	for(int i = energy.cols; i--;) // Start dp for each pixel of image's top row
	{
		best = min(best, dp(energy, mem, from, i, 0));
		if(mem[i][0] == best) // Only keep coordinate with smallest energy value
			start = {i,0};
	}
	return start;
}

// Function which uses image's energy map with findSeam to find minimum seam and carves it from the image
void carveSeam(Mat& img, Mat& newImg, Mat& energy)
{
	newImg = Mat(energy.rows, energy.cols-1, img.type());
	vector<vector<int>> mem(energy.cols, vector<int>(energy.rows+1, -1));
	vector<vector<pair<int, int>>> from(energy.cols, vector<pair<int, int>>(energy.rows, {-1, -1}));
	pair<int,int> curr = findSeam(energy, mem, from); // Acquires starting coordinate of seam to carve
	do
	{
		pair<int, int> next = from[curr.first][curr.second]; // Acquires next seam coordinate from current coordinate
		Mat bef = img.row(next.second).colRange(0, next.first); // Stores all pixels in row left of carve coordinate
		Mat aft = img.row(next.second).colRange(next.first+1, img.cols); // Stores all pixels in row right of carve coordinate
		if(!bef.empty() && !aft.empty()) hconcat(bef, aft, newImg.row(next.second)); // If both sides contain pixels, concatinate them to form row in carved image
		else if(!bef.empty()) bef.copyTo(newImg.row(next.second)); // If right side contains no pixels (carving rightmost pixel), add left side to carved image
		else if(!aft.empty()) aft.copyTo(newImg.row(next.second)); // If left side contains no pixels (carving leftmost pixel), add right side to carved image
		curr = next;
	}while(curr.second < energy.rows-1); // While we haven't reached the bottom coordinate
}

// Function which carves n columns from image and then resizes it back to its original width
void carveColumns(Mat& img, int n, bool disp)
{
	Mat swapImg, energy; // We use swapImg as an intermediate value to save results
	int r = img.cols;
	Progress progress("Seams: ");
	for(int i = 0; i < n; i++)
	{
		if(disp) progress.display(i, n-1); // We do not want to display per seam progress for parallel operation
		if(i%2)
		{
			calcEnergy(swapImg, energy);
			carveSeam(swapImg, img, energy); // Carve swapImg and save results into img
		}
		else
		{
			calcEnergy(img, energy);
			carveSeam(img, swapImg, energy); // Carve img and save results into swapImg
		}
		if(disp) progress.refresh();
	}
	if(n%2) resize(swapImg, img, Size(r, img.rows), 0, 0, INTER_CUBIC); // If last result was in swapImg, save resize results into img
	else resize(img, img, Size(r, img.rows), 0, 0, INTER_CUBIC); // If last result was in img, save resize results into img
}

// Function which carves n rows from image and then resiees it back to its original height
void carveRows(Mat& img, int n, bool disp)
{
	// Though the method used here isn't optimal, it is easy to implement and I will change it once I continue outside of school
	rotate(img, img, ROTATE_90_CLOCKWISE); // Rotate image by 90 degrees so rows become columns
	carveColumns(img, n, disp); // We can now reuse carveColumns
	rotate(img, img, ROTATE_90_COUNTERCLOCKWISE); // Rotate image back
}

// Function which carves given amount of columns and rows from image
void carveSeams(Mat& img, int cols, int rows, bool disp)
{
	carveColumns(img, cols, disp);
	carveRows(img, rows, disp);
}

// Function which performs seam carving for each frame in frame vector from given starting and ending scale
void carveFrames(vector<Mat>& frames, double x0, double y0, double x1, double y1)
{
	Progress progress("Frames:");
	double cols = frames[0].cols, rows = frames[0].rows;
	for(int i = 0; i < frames.size(); i++)
	{
		progress.display(i, frames.size()-1);
		// Uses forceRange to create values between startScale and endScale
		carveSeams(frames[i], round(cols-cols*forceRange<double>(i,0,frames.size()-1,x0,x1)), round(rows-rows*forceRange<double>(i,0,frames.size()-1,y0,y1)));
		progress.refresh();
	}
}

// Function which performs seam carving for each frame in frame vector from given starting and ending scale in parallel (Multithreading)
void carveFramesMulti(vector<Mat>& frames, double x0, double y0, double x1, double y1)
{
	Progress progress("Frames:");
	double cols = frames[0].cols, rows = frames[0].rows;
	const auto threadCount = MAX(2, thread::hardware_concurrency()); // Will always use at least 2 threads, otherwise as many as hardware allows
	vector<thread> threads(threadCount); // Thread buffer
	int i = 0;
	for(; i < frames.size(); i++) // Loops through frames and creates at most threadCount frames at a time to process each frame
	{
		if(i != 0 && i % threadCount == 0) // If threadbuffer is full of working threads
		{
			for(int j = 0; j < threadCount; j++) // Loops though thread buffer and join all threads
			{
				progress.display(i-threadCount+j, frames.size()-1);
				threads[j].join();
				progress.refresh();
			}
		}
		threads[i%threadCount] = thread( // Add thread to next unused index in thread buffer
			carveSeams,
			ref(frames[i]),
			round(cols-cols*forceRange<double>(i,0,frames.size()-1,x0,x1)),
			round(rows-rows*forceRange<double>(i,0,frames.size()-1,y0,y1)),
			false
		);
	}
	for(int j = 0; j < ((i-1) % threadCount)+1; j++) // Join straggling working threads
	{
		progress.display(i-(((i-1) % threadCount)+1)+j, frames.size()-1);
		threads[j].join();
		progress.refresh();
	}
}
