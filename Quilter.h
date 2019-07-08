#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include "Utilities.h"

using namespace cv;
using namespace std;
class Quilter
{
private:
	Mat front;
	Mat back;
	/*
	 Quilts the top parts of front and back images with overlap size if fromTop is true
	 and the bottom parts otherwise.
	*/
	void HorQuilt(int overlapSize,bool fromTop);

	/*
	 Quilts the left parts of front and back images with overlap size if fromLeft is true
	 and the right parts otherwise.
	*/
	void VertQuilt(int overlapSize, bool fromLeft);

	/*
	 Returns the minimum of three adjacent numbers from the bottom row
	 of the given matrix of integers.
	*/
	int GetMinPrevious(vector<vector<int>>& matrix, int row,int col);

	/*
	 Returns the index of the minimum of three adjacent numbers from the bottom row
	 of the given matrix of integers.
	*/
	int GetMinPrevIndex(vector<vector<int>>& matrix, int row, int col);

	/*
	 Returns the vactor of indexes which represents 
	 the path with minimum difference given the patch matrix. 
	*/
	vector<int> GetMinPath(vector<vector<int>>& pathMatrix);

public:
	Quilter();
	Quilter(Mat& front_, Mat& back_);

	/*
	Quilts front and back images with overlap size
	and sides vector representing which sides need to be quilted.
	*/
	void Quilt(int overlapSize,vector<bool> sides);

	~Quilter();
};

