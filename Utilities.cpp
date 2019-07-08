#include "Utilities.h"

int SquareDiff(Vec3b p1, Vec3b p2)
{
	int a = (p1[0] + p1[1] + p1[2]) / 3;
	int b = (p2[0] + p2[1] + p2[2]) / 3;
	return (a - b) * (a - b);
}

int Distance(Mat a, Mat b, vector<vector<int>> coeff)
{
	int ret = 0;
	if (a.rows != b.rows || a.cols != b.cols) { cout << "the distance cannot be computed" << endl; return -1; }
	for (int i = 0; i < a.rows; ++i) {
		for (int j = 0; j < a.cols; ++j) {
			if (coeff[i][j] != 0) {
				ret += coeff[i][j] * SquareDiff(a.at<Vec3b>(i, j), b.at<Vec3b>(i, j));
			}

		}
	}

	return ret;
}

vector<Point2d> centersFromPoint(Point2d center, int distSquare)
{
	clock_t b = clock();

	vector<Point2d> ret;

	for (double i = 0; i <= distSquare; i += 0.5) {
		double r = sqrt(distSquare - i * i);
		if ((r - (int)r == 0.5) || (r - (int)r) == 0) {
			ret.push_back(Point2d(center.x + i, center.y + r));
			ret.push_back(Point2d(center.x - i, center.y + r));
			ret.push_back(Point2d(center.x + i, center.y - r));
			ret.push_back(Point2d(center.x - i, center.y - r));
		}
	}
	return ret;
}