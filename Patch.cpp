#include "Patch.h"

Patch::Patch()
{
}

Patch::Patch(Mat img_, int x_, int y_, int width_, int height_)

	:img(img_),
	x(x_),
	y(y_),
	width(width_),
	height(height_),
	hasWrinkle(false)
{
}

Patch::Patch(Mat img_, Rect r)
	:img(img_),
	x(r.x),
	y(r.y),
	width(r.width),
	height(r.height),
	hasWrinkle(false)
{

}

int Patch::Distance(Patch p1, vector<vector<int>> coeff)
{
	int ret = 0;

	if (img.rows != p1.img.rows || img.cols != p1.img.cols) { cout << "the comparison cannot be done" << endl; return -1; }
	for (int i = 0; i < img.rows; ++i) {
		for (int j = 0; j < img.cols; ++j) {
			if (coeff[i][j] != 0) {
				ret += SquareDiff(img.at<Vec3b>(i, j), p1.img.at<Vec3b>(i, j));
			}

		}
	}

	return ret;
}

Rect Patch::GetRect()
{
	return Rect(x,y,width,height);
}

void Patch::SetRect(Rect r)
{
	x = r.x;
	y = r.y;
	width = r.width;
	height = r.height;
}

Patch::~Patch()
{
}

