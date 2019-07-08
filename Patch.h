#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <time.h>  
#include "Utilities.h"

using namespace cv;
using namespace std;

class Patch
{
public:
	Mat img;
	int x;
	int y;
	int width;
	int height;
	bool hasWrinkle;

	Patch();
	Patch(Mat img_, int x_, int y_, int width_, int height_);
	Patch(Mat img_, Rect r);
	int Distance(Patch p1, vector<vector<int>> ceoff);
	Rect GetRect();
	void SetRect(Rect r);
	~Patch();
};

