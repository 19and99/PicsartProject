#pragma once
#include "Patch.h"
#include "Quilter.h"

using namespace cv;
using namespace std;

class Filter
{
private:
	Mat img;
	Mat wrinkles;
	int rows;
	int cols;
	/*
	 Returns the grid of image in form of Rect objects
	 with corresponding patch length.
	*/
	vector<vector<Rect>> GetPatches(int patchLength);

	/*
	 Returns coefficent matrix consisting of ones and zeros
	 which represents the overlapping region of two patches.
	*/
	vector<vector<int>> GetCoeffMatrix(Rect r, int offset, vector<bool>& sides);

	/*
	 Returns a vector of bools that tells which sides need to be quilted.
	 sides[0] is for left,sides[1] - top, sides[2] - right, sides[3] - bottom.
	*/
	vector<bool> GetSidesVector(vector<vector<bool>>& patchWrinkles, int row, int col);

	/*
	 Returns the bigger patch for the original one
	 that contains overlapping regions.
	*/
	Rect GetBigPatchRect(Rect patch, int offset, vector<bool> sides);

	/*
	 Returns Mat object that contains the image of the patch
	 that has the minimum euclidian distance from the given rectangle region of image
	 with coefficents of coeff matrix.
	*/
	Mat GetPatch(Rect p, double ratio, vector<vector<int>>& coeff);

	/*
	 Draws the patch image with left top point lt.
	*/
	void DrawPatch(Mat p, Point lt);

	/*
	 Returns true if the r region of mask contains white point
	 and false otherwise.
	*/
	bool ContainsWrinkle(Rect r);

	/*
	 Returns bool matrix.
	 the ret[i][j] is true, if the rect of the grid
	 with i and j coordinates contains wrinkle
	 and false otherwise.
	*/
	vector<vector<bool>> GetPatchWrinkles(vector<vector<Rect>>& patches);

public:
	Filter();
	Filter(Mat img_, Mat wrinkles_)
		:img(img_),
		wrinkles(wrinkles_),
		rows(img_.rows),
		cols(img_.cols)
	{
	}
	~Filter();

	/*
	 Inpaint the masked region of image.
	*/
	void Inpaint(int patchLength, int overlapLength, double sourceRatio);
	
};

