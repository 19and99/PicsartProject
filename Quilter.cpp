#include "Quilter.h"



void Quilter::HorQuilt(int overlapSize, bool fromTop)
{
	int width = front.cols;
	int height = overlapSize;	
	Rect tRect(0, 0, width, height), bRect(0, front.rows - height, width, height);
	Mat top = fromTop ? back(tRect) : front(bRect);
	Mat bottom = fromTop ? front(tRect) : back(bRect);

	vector<vector<int>>  distMatrix(width);
	vector<vector<int>> pathMatrix(width);
	vector<int> path(width);
	for (int i = 0; i < width; ++i) {
		distMatrix[i].resize(height);
		pathMatrix[i].resize(height);
	}

	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) {
			int a = SquareDiff(top.at<Vec3b>(j, i), bottom.at<Vec3b>(j, i));
			distMatrix[i][j] = a;
			pathMatrix[i][j] = distMatrix[i][j] + GetMinPrevious(distMatrix, i, j);
		}
	}
	path = GetMinPath(pathMatrix);

	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < path[i]; ++j) {
			bottom.at<Vec3b>(j, i) = top.at<Vec3b>(j, i);
		}
		for (int j = path[i]; j < height; ++j) {
			top.at<Vec3b>(j, i) = bottom.at<Vec3b>(j, i);
		}
		//top.at<Vec3b>(path[i], i) = bottom.at<Vec3b>(path[i], i) = Vec3b(0, 0, 255);
	}
}

void Quilter::VertQuilt(int overlapSize, bool fromLeft)
{
	int width = overlapSize;
	int height = front.rows;
	Rect lRect(0, 0, width, height), rRect(front.cols - width, 0, width, height);
	Mat left = fromLeft ? back(lRect) : front(rRect);
	Mat right = fromLeft ? front(lRect) : back(rRect);

	vector<vector<int>> distMatrix(height);
	vector<vector<int>> pathMatrix(height);
	
	for (int i = 0; i < height; ++i) {
		distMatrix[i].resize(width);
		pathMatrix[i].resize(width);
	}

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			distMatrix[i][j] = SquareDiff(left.at<Vec3b>(i, j), right.at<Vec3b>(i, j));
			pathMatrix[i][j] = distMatrix[i][j] + GetMinPrevious(distMatrix, i,j);
		}
	}
	vector<int> path = GetMinPath(pathMatrix);

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < path[i]; ++j) {
			right.at<Vec3b>(i, j) = left.at<Vec3b>(i, j);
		}
		for (int j = path[i]; j < width; ++j) {
			left.at<Vec3b>(i, j) = right.at<Vec3b>(i, j);
		}
		//left.at<Vec3b>(i, path[i]) = right.at<Vec3b>(i, path[i]) = Vec3b(0, 0, 255);
	}
}

int Quilter::GetMinPrevious(vector<vector<int>>& matrix, int row,int col)
{
	if (row == 0) return matrix[0][col];
	int i1 = Max(col - 1, 0);
	int i3 = Min(col + 1, matrix[0].size() - 1);
	return Min(matrix[row - 1][i1], matrix[row - 1][col], matrix[row - 1][i3]);
}

int Quilter::GetMinPrevIndex(vector<vector<int>>& matrix, int row, int col)
{
	if (row == 0) return col;
	int i1 = Max(col - 1, 0);
	int i3 = Min(col + 1, matrix[0].size() - 1);
	int ret;
	if (matrix[row - 1][i1] <= matrix[row - 1][col]) {
		if (matrix[row - 1][i1] <= matrix[row - 1][i3]) return i1;
		else return i3;
	}
	else {
		if (matrix[row - 1][col] <= matrix[row - 1][i3]) return col;
		else return i3;
	}
}

vector<int> Quilter::GetMinPath(vector<vector<int>>& pathMatrix)
{
	int height = pathMatrix.size();
	int width = pathMatrix[0].size();
	vector<int> path(height);
	int minIndex = 0;
	int min = pathMatrix[height - 1][0];
	for (int i = 0; i < width; ++i) {
		if (min > pathMatrix[height - 1][i]) { min = pathMatrix[height - 1][i]; minIndex = i; }
	}
	path[height - 1] = minIndex;
	for (int i = height - 2; i >= 0; --i) {
		path[i] = GetMinPrevIndex(pathMatrix, i + 1, path[i + 1]);
	}
	return path;
}

Quilter::Quilter()
{
}

Quilter::Quilter(Mat& front_, Mat& back_)
{
	if (front_.rows == back_.rows && front_.cols == back_.cols) {
		front = front_;
		back = back_;
	}
	else std::cout << "The Quilting images are not of the same size" << endl;
}

void Quilter::Quilt(int overlapSize, vector<bool> sides)
{
	if (sides[0]) VertQuilt(overlapSize, true);
	if (sides[1]) HorQuilt(overlapSize, true);
	if (sides[2]) VertQuilt(overlapSize, false);
	if (sides[3]) HorQuilt(overlapSize, false);
}

Quilter::~Quilter()
{
}
