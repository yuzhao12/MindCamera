#pragma once
#include <vector>
#include <opencv/cxcore.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define SETUP_SCALES 		scales.push_back(7);	scales.push_back(11);	scales.push_back(15);

#define POISSON_SPEEDUP_DEGRADE (1)
#define RESIZE_IMAGES_PAD 100
//#define IS_IMAGE
//#define SKIP_GRADIENT_FIELD_INTERPOLATION
//#define SOFTASSIGN
//#define NOHOGZONE
#define SOFTASSIGN_SIGMA (0.1)
#define DO_COSINE
enum GFHOGType{
	Image,
	Sketch
};

class GFHOG : public std::vector<std::vector<double> >
{
public:
	GFHOG(void);
	~GFHOG(void);
	void Compute(IplImage* src,GFHOGType t, IplImage* mask = NULL);
	void ComputeImage(IplImage* src,IplImage* mask = NULL);
	void ComputeSketch(IplImage* src,IplImage* mask = NULL);
	void ComputeEdge(IplImage* edge,IplImage* mask = NULL);
	void ComputeGradient(IplImage* Edge8bit,IplImage* mask);

	IplImage* Gradient() { return _gradient; };
	static IplImage* ResizeToFaster(IplImage* image,int maxdim);
private:
	IplImage* _gradient;
	void gradientField(IplImage* inpmask32,IplImage* filtermask32);
	void histogramOfGradients(IplImage* edge,IplImage* gradient);
	IplImage* poissoncompute(IplImage* src, IplImage* mask);
	CvScalar sub(CvScalar a, CvScalar b);
};
