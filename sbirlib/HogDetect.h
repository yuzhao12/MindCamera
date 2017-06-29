#pragma once
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#define ALREADY_GRADIENT

#define SQRT2 (1.414213562373)
#define TWOPI (6.283185307180)


typedef struct _hogparams_struct {

	int winsize;
	int superwinsize;
	int hogchannels;

} HOGPARAMS;

class HogDetect {

public:
	HogDetect(IplImage* img);
	HogDetect(IplImage* img,IplImage* avoid);
	~HogDetect();

	void Construct();
	void GetHOG(int x, int y, float* histo, HOGPARAMS* hg);
	bool pointInValidRange(int x, int y, HOGPARAMS* hg);
	static int CalcNumberDenseSamples(HOGPARAMS* hogparams);

protected:
	IplImage* _sourceImage;
	IplImage* _convx;
	IplImage* _convy;
	IplImage* _gradientAngle;
	IplImage* _gradientMag;
	IplImage* _avoidImage;

	IplImage* HorizDiff(IplImage* img);
	IplImage* ImageFlipXY(IplImage* in);
	int quantize_radian(float ang, int quant);
	void GetLocalHOG(int x, int y, HOGPARAMS* hogparams, float* localhisto);

};

