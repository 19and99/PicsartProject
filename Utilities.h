#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <time.h>  
using namespace std;
using namespace cv;

vector<Point2d> centersFromPoint(Point2d center, int distSquare);

int SquareDiff(Vec3b p1, Vec3b p2);

inline int Min(int a, int b) { return a > b ? b : a; }

inline int Min(int a, int b, int c) { return min(a, b) < c ? min(a, b) : c; }

inline int Max(int a, int b) { return a > b ? a : b; }

int Distance(Mat a, Mat b, vector<vector<int>> coeff);



