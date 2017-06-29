#pragma once

#ifndef POSSION_H_
#define POSSION_H_

#include<opencv2/opencv.hpp>
#include<slu_ddefs.h>
class POSSION
{
public:
	static IplImage*poissoncompute(IplImage* src1, IplImage* src2, IplImage* mask);
	static IplImage* rotateImage2(IplImage* image, double scale = 1, int degree = 0);
	static CvScalar sub(CvScalar a, CvScalar b);
	static CvScalar add(CvScalar a, CvScalar b);
};
#endif
