#include "Filter.h"

Filter::Filter()
{
}


void Filter::Inpaint(int patchLength, int overlapLength, double sourceRatio)
{
	vector<vector<Rect>> patches = GetPatches(patchLength);
	vector<vector<bool>> patchWrinkles = GetPatchWrinkles(patches);
	for (int i = 0; i < patches.size(); ++i) {
		for (int j = 0; j < patches[i].size(); ++j) {
			if (patchWrinkles[i][j])
			{
				std::cout << "Row " << i <<"/"<< patches.size()<< " Column " << j << std::endl;
				vector<bool> sides = GetSidesVector(patchWrinkles,i,j);
				Rect bigPatchR = GetBigPatchRect(patches[i][j],overlapLength,sides);
				Mat bigPatch = img(bigPatchR);
				vector<vector<int>> coeff = GetCoeffMatrix(bigPatchR,overlapLength,sides);
				Mat inpaintPatch = GetPatch(bigPatchR, sourceRatio, coeff);
				Quilter q(inpaintPatch,bigPatch);
				q.Quilt(overlapLength, sides);
				DrawPatch(inpaintPatch,Point(bigPatchR.x,bigPatchR.y));
				patchWrinkles[i][j] = false;
			}
		}
	}
}


vector<vector<Rect>> Filter::GetPatches(int patchLength)
{
	vector<vector<Rect>> ret;
	int rowCount = cols / patchLength;
	int colCount = rows / patchLength;
	for (int i = 0; i < colCount; ++i) {
		vector<Rect> v;
		for (int j = 0; j < rowCount; ++j) {
			Rect r(j * patchLength, i * patchLength,patchLength,patchLength);
			v.push_back(r);
		}
		ret.push_back(v);
	}
	return ret;
}


vector<vector<int>> Filter::GetCoeffMatrix(Rect r, int offset,vector<bool>& sides)
{
	vector<vector<int>> coeff(r.height);
	for (int i = 0; i < r.height; ++i) {
		for (int j = 0; j < r.width; ++j) {
			if ((sides[2]*j >= r.width - offset) || (j < sides[0]*offset) || (sides[3]*i >= r.height - offset) || (i < sides[1]*offset)) coeff[i].push_back(1);
			else coeff[i].push_back(0);
		}
	}
	return coeff;
}


vector<bool> Filter::GetSidesVector(vector<vector<bool>>& patchWrinkles, int row, int col)
{
	vector<bool> sides;
	bool hasRight = !(col == patchWrinkles[row].size() - 1 || patchWrinkles[row][col + 1]);
	bool hasTop = !(row == 0 || patchWrinkles[row - 1][col]);
	bool hasLeft = !(col == 0 || patchWrinkles[row][col - 1]);
	bool hasBottom = !(row == patchWrinkles.size() - 1 || patchWrinkles[row + 1][col]);
	sides.push_back(hasLeft);
	sides.push_back(hasTop);
	sides.push_back(hasRight);
	sides.push_back(hasBottom);
	return sides;
}


Rect Filter::GetBigPatchRect(Rect patch, int offset, vector<bool> sides)
{
	Rect ret(patch.x - sides[0] * offset,
		patch.y - sides[1] * offset,
		patch.width + (sides[2] + sides[0]) * offset,
		patch.height + (sides[3] + sides[1]) * offset);
	return ret;
}


Mat Filter::GetPatch(Rect p, double ratio, vector<vector<int>>& coeff)
{
	Mat ret;
	Mat base = img(p);
	Rect searchRect;
	searchRect.x = Max(p.x - ratio * p.width, 0);
	searchRect.y = Max(p.y - ratio * p.height, 0);
	searchRect.width = Min((2 * ratio + 1) * p.width,cols - searchRect.x);
	searchRect.height = Min((2 * ratio + 1) * p.height,rows - searchRect.y);

	Rect minRect(0, 0,p.width,p.height);
	unsigned int minDist = 0xffffffff;
	for (int i = 0; i <= searchRect.width - p.width; i+=2) {
		for (int j = 0; j <= searchRect.height - p.height; j+=2) {
			Rect temp(searchRect.x + i, searchRect.y + j, p.width, p.height);
			if (!ContainsWrinkle(temp)) {
				unsigned int tempDist = Distance(img(temp), base, coeff);
				if (tempDist < minDist) {
					minRect = temp;
					minDist = tempDist;
				}
			}
		}
	}
	if (minDist == 0xffffffff) {
		searchRect.x = Max(p.x - 2*ratio * p.width, 0);
		searchRect.y = Max(p.y - 2*ratio * p.height, 0);
		searchRect.width = Min((4 * ratio + 1) * p.width, cols - searchRect.x);
		searchRect.height = Min((4 * ratio + 1) * p.height, rows - searchRect.y);
		for (int i = 0; i <= searchRect.width - p.width; i += 2) {
			for (int j = 0; j <= searchRect.height - p.height; j += 2) {
				Rect temp(searchRect.x + i, searchRect.y + j, p.width, p.height);
				if (!ContainsWrinkle(temp)) {
					unsigned int tempDist = Distance(img(temp), base, coeff);
					if (tempDist < minDist) {
						minRect = temp;
						minDist = tempDist;
					}
				}
			}
		}
	}
	img(minRect).copyTo(ret);
	return ret;
}


void Filter::DrawPatch(Mat patch,Point lt)
{
	for (int i = lt.y; i < lt.y + patch.rows; ++i) {
		for (int j = lt.x; j < lt.x + patch.cols; ++j) {
			img.at<Vec3b>(i, j) = patch.at<Vec3b>(i - lt.y, j - lt.x);
		}
	}
}


bool Filter::ContainsWrinkle(Rect r)
{
	int x1 = r.x, x2 = r.x + r.width - 1, y1 = r.y, y2 = r.y + r.height - 1;
	if (x1 < 0 || x2 >= wrinkles.cols || y1 < 0 || y2 >= wrinkles.rows) return true;
	for (int i = y1; i <= y2; ++i) {
		for (int j = x1; j <= x2; ++j) {
			if (wrinkles.at<Vec3b>(i, j) == Vec3b(255, 255, 255)) return true;
		}
	}
	return false;
}


vector<vector<bool>> Filter::GetPatchWrinkles(vector<vector<Rect>>& patches)
{
	vector<vector<bool>> ret(patches.size());
	for (int i = 0; i < ret.size(); ++i) {
		ret[i].resize(patches[0].size());
		for (int j = 0; j < ret[0].size(); ++j) {
			ret[i][j] = ContainsWrinkle(patches[i][j]);
		}
	}
	return ret;
}

Filter::~Filter()
{
}


