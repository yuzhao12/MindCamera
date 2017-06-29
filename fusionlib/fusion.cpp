// fusion.cpp : 定义 DLL 应用程序的导出函数。
//
//#include<Python.h>
#include<opencv2/opencv.hpp>
#include<stdio.h>
#include"possion.h"
#include <time.h>
#include <iostream>
using namespace std;
using namespace cv;
typedef unsigned int uint;

/**
*	Verifies the validity of the mask. Checks that:
*  - The binary mask does not translate outside the target image.
*	- There is no mask on the boundary.
*	@return 'false' if verfication failed, 'true' if verification
*          passed (maybe with warnings).
*/
extern "C" {
	bool verifyMask(IplImage*SM, uint tw, uint th, uint ox, uint oy)
	{
		uint w = SM->width;
		uint h = SM->height;

		bool reportedBadColor = false;

		// Verify binary mask
		for (uint y = 0; y < h; y++) {
			for (uint x = 0; x < w; x++) {
				uint id = x + y*SM->widthStep;
				if (SM->imageData[id] != 0 && ((x + ox) >= tw || (y + oy) >= th)) {
					cout << "Error: Source mask translates outside target image" << endl;
					return false;
				}
			}
		}

		// Verify no mask on the boundary
		for (uint x = 0; x < w; x++) {
			if (SM->imageData[x] != 0 || SM->imageData[x + (h - 1)*SM->widthStep] != 0) {
				cout << "Warning: Mask must not be set on the image boundary" << endl;
				return true;
			}
		}
		for (uint y = 0; y < h; y++) {
			if (SM->imageData[y*SM->widthStep] != 0 || SM->imageData[w - 1 + y*SM->widthStep] != 0) {
				cout << "Warning: Mask must not be set on the image boundary" << endl;
				return true;
			}
		}

		return true;
	}


	/**
	*	Clone the masked area of the source image onto the target image by using
	*  the gradients in the source image as a guidance field.
	*	See class slides at http://www.cs.tau.ac.il/~tommer/adv-graphics/ex1.htm
	*	or "Poisson Image Editing" paper.
	*/
	int doClone(const char* simageFname,
		const char* smaskFname,
		const char* timageFname,
		const char* oimageFname,
		int ox,
		int oy,
		float ratio,
		int rotation) {
		// Load input images
		IplImage* SI;
		IplImage*TI;
		IplImage*SM;


		SI = cvLoadImage(simageFname, 1);
		TI = cvLoadImage(timageFname, 1);
		SM = cvLoadImage(smaskFname, 0);

		// Verify input
		if (SI->height != SM->height || SI->width != SM->width) {
			cout << "Error: Input image and input mask must have the same dimensions" << endl;
			return -1;
		}
	

		IplImage*SI_tmp=POSSION::rotateImage2(SI, ratio,rotation);
		IplImage*SM_tmp=POSSION::rotateImage2(SM, ratio, rotation);
		cvReleaseImage(&SI);
		cvReleaseImage(&SM);
		SM=SM_tmp;
		SI=SI_tmp;
		if (!verifyMask(SM, TI->widthStep, TI->height, ox, oy)) {
			// Error text printed by verifyMask
			return -1;
		}
		// Translate the source image and the source mask
		IplImage* TSI = cvCreateImage(cvGetSize(TI), 8, 3); // TSI = Translated Source Image
		IplImage* TSM = cvCreateImage(cvGetSize(TI), 8, 1);    // TSM = Translated Source Mask
		cvZero(TSM);
		for (uint y = 0; y < TSI->height; y++) {
			for (uint x = 0; x < TSI->width; x++) {
				uint id = x + y*TSI->widthStep;
				if (x - ox >= 0 && x - ox<SI->width&&y - oy >= 0 && y - oy<SI->height) {
					cvSet2D(TSI, y, x, cvGet2D(SI, y - oy, x - ox));
					cvSet2D(TSM, y, x, cvGet2D(SM, y - oy, x - ox));
				}
			}
		}

		// Run poisson solver on the divergence field on the translated source image
		// (TSI)
		IplImage* O = POSSION::poissoncompute(TI, TSI, TSM);
		//POSSION::solve(imagelib::convert(TI), TSM,imagelib::divergents(imagelib::gradient(imagelib::convert(TSI))), O);
		/*
		// The above code is equal to just calculating the laplacian
		Solver::solve(imagelib::convert(TI), TSM,
		imagelib::laplacian(imagelib::convert(TSI)), O);
		*/

		cvSaveImage(oimageFname, O);
		cvReleaseImage(&TSI);
		cvReleaseImage(&SI);
		cvReleaseImage(&TSM);
		cvReleaseImage(&SM);
		cvReleaseImage(&TI);
		cvReleaseImage(&O);

		return 0;
	}

	 int doCopy(const char* simageFname,
		const char* smaskFname,
		const char* timageFname,
		const char* oimageFname,
		int ox,
		int oy,
		float ratio,
		int rotation) {
		// Load input images
		IplImage* SI;
		IplImage*TI;
		IplImage*SM;


		SI = cvLoadImage(simageFname, 1);
		TI = cvLoadImage(timageFname, 1);
		SM = cvLoadImage(smaskFname, 0);

		// Verify input
		if (SI->height != SM->height || SI->width != SM->width) {
			cout << "Error: Input image and input mask must have the same dimensions" << endl;
			return -1;
		}
		if (!verifyMask(SM, TI->widthStep, TI->height, ox, oy)) {
			// Error text printed by verifyMask
			return -1;
		}
	
		IplImage*SI_tmp=POSSION::rotateImage2(SI,ratio, rotation);
		IplImage*SM_tmp=POSSION::rotateImage2(SM,ratio, rotation);
		cvReleaseImage(&SI);
		cvReleaseImage(&SM);
		SM=SM_tmp;
		SI=SI_tmp;
		// Translate the source image and the source mask
		IplImage* TSI = cvCreateImage(cvGetSize(TI), 8, 3); // TSI = Translated Source Image
		IplImage* TSM = cvCreateImage(cvGetSize(TI), 8, 1);    // TSM = Translated Source Mask
		cvZero(TSM);
		for (uint y = 0; y < TSI->height; y++) {
			for (uint x = 0; x < TSI->width; x++) {
				uint id = x + y*TSI->widthStep;
				if (x - ox >= 0 && x - ox<SI->width&&y - oy >= 0 && y - oy<SI->height) {
					cvSet2D(TSI, y, x, cvGet2D(SI, y - oy, x - ox));
					cvSet2D(TSM, y, x, cvGet2D(SM, y - oy, x - ox));
				}
			}
		}

		// Run poisson solver on the divergence field on the translated source image
		// (TSI)
		IplImage* O = cvCreateImage(cvGetSize(TI), 8, 3);
		cvCopy(TI, O);
		cvCopy(TSI, O, TSM);
		cvSaveImage(oimageFname, O);
		cvSaveImage(oimageFname, O);

		cvReleaseImage(&TSI);
		cvReleaseImage(&SI);
		cvReleaseImage(&TSM);
		cvReleaseImage(&SM);
		cvReleaseImage(&TI);
		cvReleaseImage(&O);
		return 0;
	}
}

