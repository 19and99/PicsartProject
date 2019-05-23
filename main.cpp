#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <stdlib.h> 
#include <time.h>  

using namespace std;
using namespace cv;
double WrinklesTime = 0;

int mod(int a) { return a > 0 ? a : -a; }

int squareDiff(Vec3b p1, Vec3b p2) {
	return (p1[0] - p2[0])*(p1[0] - p2[0]) + (p1[1] - p2[1])*(p1[1] - p2[1]) + (p1[2] - p2[2])*(p1[2] - p2[2]);
}
int squareDiffImage(Mat img1, Mat img2, int** coeff) {
	if (img1.rows != img2.rows || img1.cols != img2.cols) { std::cout << "The square difference can't be counted, because the images are not of the same size" << endl; return -1; }
	int ret = 0;
	for (int i = 0; i < img1.rows; ++i) {
		for (int j = 0; j < img1.cols; ++j) {
			ret += coeff[i][j] * squareDiff(img1.at<Vec3b>(i,j), img2.at<Vec3b>(i, j));
		}
	}
	return ret;
}
int min(int a, int b) {
	return a < b ? a : b;
}
int min(int a, int b, int c) {
	return min(a, min(b, c));
}
int max(int a, int b) { return a > b ? a : b; }


void vQuilt(Mat& left, Mat& right) {
	int width = left.cols;
	int height = left.rows;
	if (height != right.rows) { std::cout << "VERTICAL:::the heights are not equal:" << endl; return; }
	if (width != right.cols) { std::cout << "VERTICAL:::the widths are not equal" << endl; return; }

	int** distMatrix = new int*[height];
	for (int i = 0; i < height; ++i) {
		distMatrix[i] = new int[width];
	}
	int** pathMatrix = new int*[height];
	for (int i = 0; i < height; ++i) {
		pathMatrix[i] = new int[width];
	}

	int* path = new int[height];

	
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			distMatrix[i][j] = squareDiff(left.at<Vec3b>(i, j), right.at<Vec3b>(i,j));
		}
	}

	for (int i = 0; i < width; ++i) {
		pathMatrix[0][i] = distMatrix[0][i];
	}
	for (int i = 1; i < height; ++i) {
		pathMatrix[i][0] = distMatrix[i][0] + min(pathMatrix[i - 1][0], pathMatrix[i - 1][1]);
		for (int j = 1; j < width - 1; ++j) {
			pathMatrix[i][j] = distMatrix[i][j] + min(pathMatrix[i - 1][j], pathMatrix[i - 1][j - 1], pathMatrix[i - 1][j + 1]);
		}
		pathMatrix[i][width - 1] = distMatrix[i][width - 1] + min(pathMatrix[i - 1][width - 1], pathMatrix[i - 1][width - 2]);
	}

	int minIndex = 0;
	int min = pathMatrix[height - 1][0];
	for (int i = 1; i < width; ++i) {
		if (pathMatrix[height - 1][i] < min) {
			min = pathMatrix[height - 1][i];
			minIndex = i;
		}
	}
	path[height - 1] = minIndex;
	for (int i = height - 1; i > 0; --i) {
		if (minIndex == 0) {
			if (pathMatrix[i - 1][minIndex] < pathMatrix[i - 1][minIndex + 1]) path[i - 1] = minIndex;
			else {
				++minIndex;
				path[i - 1] = minIndex;
			}
		}
		else if (minIndex == width - 1) {
			if (pathMatrix[i - 1][minIndex] < pathMatrix[i - 1][minIndex - 1]) path[i - 1] = minIndex;
			else {
				--minIndex;
				path[i - 1] = minIndex;
			}
		}

		else if (pathMatrix[i - 1][minIndex] < pathMatrix[i - 1][minIndex - 1] && pathMatrix[i - 1][minIndex] < pathMatrix[i - 1][minIndex + 1]) {
			path[i - 1] = minIndex;
		}
		else if (pathMatrix[i - 1][minIndex - 1] < pathMatrix[i - 1][minIndex + 1]) {
			--minIndex;
			path[i - 1] = minIndex;
		}
		else {
			++minIndex;
			path[i - 1] = minIndex;
		}

	}
	
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
void hQuilt(Mat& top, Mat& bottom) {
	int width = top.cols;
	int height = top.rows;
	if (width != bottom.cols) { std::cout << "HORIZONTAL:::the widths are not equal" << endl; return; }
	if (height != bottom.rows) { std::cout << "HORIZONTAL::the heights are not equal" << endl; return; }
	int** distMatrix = new int*[height];
	for (int i = 0; i < height; ++i) {
		distMatrix[i] = new int[width];
	}
	int** pathMatrix = new int*[height];
	for (int i = 0; i < height; ++i) {
		pathMatrix[i] = new int[width];
	}

	int* path = new int[width];

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			distMatrix[i][j] = squareDiff(top.at<Vec3b>(i, j), bottom.at<Vec3b>(i, j));
		}
	}
	for (int i = 0; i < height; ++i) {
		pathMatrix[i][0] = distMatrix[i][0];
	}
	for (int i = 1; i < width; ++i) {
		pathMatrix[0][i] = distMatrix[0][i] + min(pathMatrix[0][i-1], pathMatrix[1][i-1]);
		for (int j = 1; j < height - 1; ++j) {
			pathMatrix[j][i] = distMatrix[j][i] + min(pathMatrix[j][i-1], pathMatrix[j-1][i-1], pathMatrix[j+1][i-1]);
		}
		pathMatrix[height - 1][i] = distMatrix[height - 1][i] + min(pathMatrix[height - 1][i-1], pathMatrix[height - 2][i-1]);
	}

	int minIndex = 0;
	int min = pathMatrix[0][width - 1];
	for (int i = 1; i < height; ++i) {
		if (pathMatrix[i][width - 1] < min) {
			min = pathMatrix[i][width - 1];
			minIndex = i;
		}
	}
	path[width - 1] = minIndex;
	for (int i = width - 1; i > 0; --i) {

		if (minIndex == 0) {
			if (pathMatrix[minIndex][i - 1] < pathMatrix[minIndex + 1][i - 1]) {
				path[i - 1] = minIndex;
			}
			else {
				++minIndex;
				path[i - 1] = minIndex;
			}
		}

		else if (minIndex == height - 1) {
			if (pathMatrix[minIndex][i - 1] < pathMatrix[minIndex - 1][i - 1]) {
				path[i - 1] = minIndex;
			}
			else {
				--minIndex;
				path[i - 1] = minIndex;
			}
		}

		else if (pathMatrix[minIndex][i-1] < pathMatrix[minIndex - 1][i - 1] && pathMatrix[minIndex][i - 1] < pathMatrix[minIndex + 1][i - 1]) {
			path[i - 1] = minIndex;
		}
		else if (pathMatrix[minIndex - 1][i - 1] <= pathMatrix[minIndex + 1][i - 1]) {
			--minIndex;
			path[i - 1] = minIndex;
		}
		else {
			++minIndex;
			path[i - 1] = minIndex;
		}
	}
	for (int j = 0; j < width; ++j) {
		for (int i = 0; i < path[j]; ++i) {
			bottom.at<Vec3b>(i, j) = top.at<Vec3b>(i, j);
		}
		for (int i = path[j]; i < height; ++i) {
			top.at<Vec3b>(i, j) = bottom.at<Vec3b>(i, j);
		}
	}
}

void patchReplace(Mat& image, Mat& patch, Point LT, int height,int width) {
	for (int i = LT.y; i < LT.y + height; ++i) {
		for (int j = LT.x; j < LT.x + width; ++j) {
			image.at<Vec3b>(i, j) = patch.at<Vec3b>(i - LT.y, j - LT.x);
		}
	}
}

bool containsWrinkle(Point LT,int size, Mat wrinkles) {
	clock_t begin = clock();

	if (LT.x >= wrinkles.cols || LT.y >= wrinkles.rows) return false;

	int left = LT.x >= 0 ? LT.x : 0;
	int right = left + size - 1 < wrinkles.cols ? left + size - 1 : wrinkles.cols - 1;
	int top = LT.y >= 0 ? LT.y : 0;
	int bottom = top + size - 1 < wrinkles.rows ? top + size - 1 : wrinkles.rows - 1;
	
	for (int i = left; i <= right; ++i) {
		for (int j = top; j <= bottom; ++j) {
			if (wrinkles.at<Vec3b>(j, i) == Vec3b(255, 255, 255)) {
				clock_t end = clock();
				WrinklesTime += double(end - begin);
				return true;
			}
		}
	}

	clock_t end = clock();
	WrinklesTime += double(end - begin);
	return false;
}

Mat searchSource(Mat& image,Mat mask,Mat wrinkles,Point LT,int length,double ratio) {
	if (image.rows != mask.rows || image.cols != mask.cols) cout << "image and mask are not of the same size" << endl;
	if (image.rows != wrinkles.rows || image.cols != wrinkles.cols) cout << "image and wrinkles are not of the same size" << endl;
	Mat ret;
	int sourceLength = length * ratio;
	Point sourceLT;
	int height = image.rows;
	int width = image.cols;

	int searchDistance = max(max(LT.y,LT.x),max(height - (LT.y + length) + 1, width - (LT.x + length) + 1));
	bool found = false;

	Point ltCenter(LT.x - (sourceLength - length) / 2, LT.y + (length - sourceLength)/2);
	for (int d = 0; d <= searchDistance; ++d) {
		for (int shift = 0; shift <= d; ++shift) {
			sourceLT.x = LT.x - (sourceLength - length) / 2 + shift;
			sourceLT.y = LT.y - sourceLength - d;
			if (sourceLT.x >= 0 && sourceLT.y >= 0 && sourceLT.x + sourceLength < width && sourceLT.y + sourceLength < height) {
				if (!containsWrinkle(sourceLT, sourceLength, wrinkles) && !containsWrinkle(sourceLT, sourceLength, mask)) {
					found = true;
					break;
				}
			}

			sourceLT.y += length + sourceLength + d;
			sourceLT.x -= 2 * shift;
			if (sourceLT.x >= 0 && sourceLT.y >= 0 && sourceLT.x + sourceLength < width && sourceLT.y + sourceLength < height) {
				if (!containsWrinkle(sourceLT, sourceLength, wrinkles) && !containsWrinkle(sourceLT, sourceLength, mask)) {
					found = true;
					break;
				}
			}
			sourceLT.x = sourceLT.y - ltCenter.y + ltCenter.x;
			sourceLT.y = ltCenter.y + shift;
			if (sourceLT.x >= 0 && sourceLT.y >= 0 && sourceLT.x + sourceLength < width && sourceLT.y + sourceLength < height) {
				if (!containsWrinkle(sourceLT, sourceLength, wrinkles) && !containsWrinkle(sourceLT, sourceLength, mask)) {
					found = true;
					break;
				}
			}

			sourceLT.x -= length + sourceLength + d;
			sourceLT.y -= 2 * shift;
			if (sourceLT.x >= 0 && sourceLT.y >= 0 && sourceLT.x + sourceLength < width && sourceLT.y + sourceLength < height) {
				if (!containsWrinkle(sourceLT, sourceLength, wrinkles) && !containsWrinkle(sourceLT, sourceLength, mask)) {
					found = true;
					break;
				}
			}
		}
		if (found) break;
	}
	if (!found) { std::cout << "Couldn't find source from image" << endl; return ret; }
	Rect r(sourceLT.x, sourceLT.y, sourceLength, sourceLength);
	image(r).copyTo(ret);
	return ret;
}

Mat searchPatch(Mat& image,Mat& source, Point patchLT, int patchWidth, int patchHeight, double tollerance,int** coeff) {
	if (source.rows < patchHeight || source.cols < patchWidth) std::cout << "the patch is bigger than the source" << endl;
	Rect patchRect(patchLT.x, patchLT.y, patchWidth, patchHeight);
	Mat patch = image(patchRect);

	int distHeight = source.rows - patchHeight + 1;
	int distWidth = source.cols - patchWidth + 1;
	int** distMatrix = new int*[distHeight];
	for (int i = 0; i < distHeight; ++i) {
		distMatrix[i] = new int[distWidth];
	}

	for (int i = 0; i < distHeight; ++i) {
		for (int j = 0; j < distWidth; ++j) {
			Rect sourceRect(j, i, patchWidth, patchHeight);
			distMatrix[i][j] = squareDiffImage(patch,source(sourceRect),coeff);
			if (distMatrix[i][j] == 0) distMatrix[i][j] = 1;
		}
	}
	int minDist = distMatrix[0][0];
	for (int i = 0; i < distHeight; ++i) {
		for (int j = 0; j < distWidth; ++j) {
			if (minDist > distMatrix[i][j]) minDist = distMatrix[i][j];
		}
	}
	double t = (1 + tollerance) * minDist;
	vector<Point> LTs;
	for (int i = 0; i < distHeight; ++i) {
		for (int j = 0; j < distWidth; ++j) {
			if (distMatrix[i][j] < t) { 
				Point p(j, i);
				LTs.push_back(p);
			}
		}
	}
	srand(time(NULL));
	int index = rand()%(LTs.size());
	Point randomLT = LTs[index];
	Rect retRect(randomLT.x,randomLT.y,patchWidth,patchHeight);
	Mat ret = source(retRect);
	return ret;
}

void filter(Mat& image, Mat mask, Mat& wrinkles, int smallPatchLength, double sourceRatio, double overlapRatio, double tollerance) {
	if (image.rows != wrinkles.rows || image.cols != wrinkles.cols) { std::cout << "image and mask should be of the same size" << endl; return; }
	int height = image.rows;
	int width = image.cols;
	
	Mat wrinklesInitial;
	wrinkles.copyTo(wrinklesInitial);

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (wrinkles.at<Vec3b>(i, j) == Vec3b(255,255,255))
			{
				image.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
			}
		}
	}
	

	Point lt(0,0);
	Point rb(width - 1,height - 1);

	int regWidth = rb.x - lt.x + 1;
	int regHeight = rb.y - lt.y + 1;
	int patchInRows = regWidth / smallPatchLength;
	int patchInCols = regHeight / smallPatchLength;

	double QuiltTime = 0;
	double SourceSearchTime = 0;
	double PatchSearchTime = 0;

	for (int i = 0; i < patchInCols; ++i) {
		for (int j = 0; j < patchInRows; ++j) {
			Point smallPatchLT(lt.x + j * smallPatchLength, lt.y + i * smallPatchLength);
			if (containsWrinkle(smallPatchLT, smallPatchLength, wrinkles)) {
				bool hasRight = !containsWrinkle(Point(smallPatchLT.x + smallPatchLength, smallPatchLT.y),smallPatchLength,wrinkles);
				bool hasBottom = !containsWrinkle(Point(smallPatchLT.x, smallPatchLT.y + smallPatchLength), smallPatchLength, wrinkles);
				bool hasTop = true;
				bool hasLeft = true;

				int lengthDiff = overlapRatio * smallPatchLength;

				if (smallPatchLT.x + smallPatchLength + lengthDiff >= image.cols) hasRight = false;
				if (smallPatchLT.y + smallPatchLength + lengthDiff >= image.rows) hasBottom = false;
				if (smallPatchLT.x - lengthDiff < 0) hasLeft = false;
				if (smallPatchLT.y - lengthDiff < 0) hasTop = false;


				int bigPatchHeight = smallPatchLength + (1 + hasBottom)*lengthDiff;
				int bigPatchWidth = smallPatchLength + (1 + hasRight)*lengthDiff;

				Point bigPatchLT(smallPatchLT.x - hasLeft * lengthDiff,smallPatchLT.y - hasTop * lengthDiff);


				Rect rectLeft(bigPatchLT.x, bigPatchLT.y,lengthDiff,bigPatchHeight);
				Rect rectTop(bigPatchLT.x, bigPatchLT.y, bigPatchWidth, lengthDiff);
				Rect rectRight(smallPatchLT.x + smallPatchLength,bigPatchLT.y,lengthDiff, bigPatchHeight);
				Rect rectBottom(bigPatchLT.x,smallPatchLT.y + smallPatchLength, bigPatchWidth, lengthDiff);


				int** coeff = new int*[bigPatchHeight];
				for (int i = 0; i < bigPatchHeight; ++i) {
					coeff[i] = new int[bigPatchWidth];
					for (int j = 0; j < bigPatchWidth; ++j) {
						if (i < lengthDiff || j < lengthDiff || (hasRight && j >= (bigPatchWidth - lengthDiff)) || (hasBottom && i >= (bigPatchHeight - lengthDiff)))
							coeff[i][j] = 1;
						else
							coeff[i][j] = 0;
					}
				}
				clock_t sBegin = clock();
				Mat source = searchSource(image, mask, wrinklesInitial, smallPatchLT, smallPatchLength, sourceRatio);
				clock_t sEnd = clock();
				SourceSearchTime += (double)(sEnd - sBegin);

				clock_t pBegin = clock();
				Mat patch = searchPatch(image, source,bigPatchLT,bigPatchWidth,bigPatchHeight,tollerance,coeff);
				clock_t pEnd = clock();
				PatchSearchTime += (double)(pEnd - pBegin);

				clock_t qBegin = clock();

				if (hasLeft) {
					Mat overlayLeft = image(rectLeft);
					Rect patchL(rectLeft.x - bigPatchLT.x, rectLeft.y - bigPatchLT.y, rectLeft.width, rectLeft.height);
					Mat patchLeft = patch(patchL);
					vQuilt(overlayLeft, patchLeft);
				}
				if (hasTop) {
					Mat overlayTop = image(rectTop);
					Rect patchT(rectTop.x - bigPatchLT.x, rectTop.y - bigPatchLT.y, rectTop.width, rectTop.height);
					Mat patchTop = patch(patchT);
					hQuilt(overlayTop, patchTop);
				}
				if (hasRight) {
					Mat overlayRight = image(rectRight);
					Rect patchR(rectRight.x - bigPatchLT.x, rectRight.y - bigPatchLT.y, rectRight.width, rectRight.height);
					Mat patchRight = patch(patchR);
					vQuilt(patchRight, overlayRight);
				}
				if (hasBottom) {
					Mat overlayBottom = image(rectBottom);
					Rect patchB(rectBottom.x - bigPatchLT.x, rectBottom.y - bigPatchLT.y, rectBottom.width, rectBottom.height);
					Mat patchBottom = patch(patchB);
					hQuilt(patchBottom, overlayBottom);
				}

				clock_t qEnd = clock();
				QuiltTime += (double)(qEnd - qBegin);

				patchReplace(image, patch, bigPatchLT, bigPatchHeight,bigPatchWidth);
				
				for (int i = smallPatchLT.y; i < smallPatchLT.y + smallPatchLength; ++i) {
					for (int j = smallPatchLT.x; j < smallPatchLT.x + smallPatchLength; ++j) {
						wrinkles.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
					}
				}
				
			
			}
			int percent = ((i * patchInRows + j) * 100.0) / (patchInCols * patchInRows);
			std::cout << "row:" << i << "  column" << j << endl;
		}
	}

	std::cout << "Quilting time:  " << QuiltTime << std::endl;
	std::cout << "Patch Searching time:  " << PatchSearchTime << std::endl;
	std::cout << "Source Searching time:  " << SourceSearchTime << std::endl;
}




void test(int smallPatchLength, double overlapRatio, double tollerance) {
	Mat image = imread("filling.jpg");
	Mat wrinkles = imread("wrinkles.jpg");


	Mat source = imread("texture.jpg");

	Point lt(0, 0);
	Point rb(image.cols - 1, image.rows - 1);

	int regWidth = rb.x - lt.x + 1;
	int regHeight = rb.y - lt.y + 1;
	int patchInRows = regWidth / smallPatchLength;
	int patchInCols = regHeight / smallPatchLength;

	bool** patchStatus = new bool*[patchInCols];
	for (int i = 0; i < patchInCols; ++i) {

	}

	for (int i = 0; i < patchInCols; ++i) {
		for (int j = 0; j < patchInRows; ++j) {
			Point smallPatchLT(lt.x + j * smallPatchLength, lt.y + i * smallPatchLength);
		
			bool hasTop = i > 0 ? true : false;
			bool hasLeft = j > 0 ? true : false;

			int lengthDiff = overlapRatio * smallPatchLength;
			int bigPatchHeight = smallPatchLength + hasTop * lengthDiff;
			int bigPatchWidth = smallPatchLength + hasLeft * lengthDiff;

			Point bigPatchLT(smallPatchLT.x - hasLeft * lengthDiff, smallPatchLT.y - hasTop * lengthDiff);
			Rect rectLeft(bigPatchLT.x, bigPatchLT.y, lengthDiff, bigPatchHeight);
			Rect rectTop(bigPatchLT.x, bigPatchLT.y, bigPatchWidth, lengthDiff);
			

			int** coeff = new int*[bigPatchHeight];
			for (int i = 0; i < bigPatchHeight; ++i) {
				coeff[i] = new int[bigPatchWidth];
				for (int j = 0; j < bigPatchWidth; ++j) {
					if ((hasTop && i < lengthDiff )|| (hasLeft && j < lengthDiff ))
						coeff[i][j] = 1;
					else
						coeff[i][j] = 0;
				}
			}
			Mat patch;
			if (hasTop || hasLeft) {
				patch = searchPatch(image, source, bigPatchLT, bigPatchWidth, bigPatchHeight, tollerance, coeff);
				if (hasTop) {
					Mat overlayTop = image(rectTop);
					Rect patchR(rectTop.x - bigPatchLT.x, rectTop.y - bigPatchLT.y, rectTop.width, rectTop.height);
					Mat patchTop = patch(patchR);
					hQuilt(overlayTop, patchTop);
				}
				if (hasLeft) {
					Mat overlayLeft = image(rectLeft);
					Rect patchB(rectLeft.x - bigPatchLT.x, rectLeft.y - bigPatchLT.y, rectLeft.width, rectLeft.height);
					Mat patchLeft = patch(patchB);
					vQuilt(overlayLeft, patchLeft);
				}


			}
			else {
				Rect patchRect(0, 0, bigPatchWidth, bigPatchHeight);
				patch = source(patchRect);
			}

			patchReplace(image, patch, bigPatchLT, bigPatchHeight, bigPatchWidth);
			Rect smallPatchRect(smallPatchLT.x, smallPatchLT.y, smallPatchLength, smallPatchLength);
			//rectangle(image, smallPatchRect, Scalar(0, 0, 255));
			
		int percent = ((i * patchInRows + j) * 100.0) / (patchInCols * patchInRows);
		std::cout <<"row:"<< i + 1 << "patch" << j + 1<< endl;
		}
	}

	imshow("asd", image);
}

int main() {
	Mat image = imread("forehead.jpg");
	Mat wrinkles = imread("mask.jpg");
	Mat hairMask = imread("hairMask.jpg");

	clock_t begin = clock();
	if (image.rows != wrinkles.rows || image.cols != wrinkles.cols) {
		std::cout << "image and mask should be of the same size" << endl; 
	}
	else filter(image,hairMask, wrinkles,15,1.7,0.33,0.6);

	//test(20,0.2,0.2);
	clock_t end = clock();
	std::cout << "Wrinkles Time:  " << WrinklesTime << endl;
	std::cout << "Overall Time:  " << (double)(end - begin) << endl;
	

	imshow("quilted overlay",image);
	imwrite("removed.jpg", image);
	waitKey(0);
	return 0;
}