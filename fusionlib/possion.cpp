#include"possion.h"
CvScalar POSSION::sub(CvScalar a, CvScalar b) {

	a.val[0] -= b.val[0];
	a.val[1] -= b.val[1];
	a.val[2] -= b.val[2];
	a.val[3] -= b.val[3];

	return a;
}
CvScalar POSSION::add(CvScalar a, CvScalar b) {

	a.val[0] += b.val[0];
	a.val[1] += b.val[1];
	a.val[2] += b.val[2];
	a.val[3] += b.val[3];

	return a;
}
IplImage* POSSION::rotateImage2(IplImage* image, double scale, int degree)
{
	IplImage*img = cvCreateImage(cvSize(int(image->width*scale), int(image->height*scale)), 8, image->nChannels);
	cvResize(image, img);
	double angle = degree  * CV_PI / 180.;
	double a = sin(angle), b = cos(angle);
	int width = img->width, height = img->height; 
	int width_rotate = int(height * fabs(a) + width * fabs(b));
	int height_rotate = int(width * fabs(a) + height * fabs(b));
	IplImage* img_rotate = cvCreateImage(cvSize(width_rotate, height_rotate), img->depth, img->nChannels);
	cvZero(img_rotate);
	int tempLength = sqrt((double)width * width + (double)height *height) + 10;
	int tempX = (tempLength + 1) / 2 - width / 2;
	int tempY = (tempLength + 1) / 2 - height / 2;
	IplImage* temp = cvCreateImage(cvSize(tempLength, tempLength), img->depth, img->nChannels);
	cvZero(temp);  
	cvSetImageROI(temp, cvRect(tempX, tempY, width, height));
	cvCopy(img, temp, NULL);
	cvResetImageROI(temp);
	float m[6];
	int w = temp->width;
	int h = temp->height;
	m[0] = b;
	m[1] = a;
	m[3] = -m[1];
	m[4] = m[0];    
	m[2] = w * 0.5f;
	m[5] = h * 0.5f;
	CvMat M = cvMat(2, 3, CV_32F, m);
	cvGetQuadrangleSubPix(temp, img_rotate, &M);
	cvReleaseImage(&temp);
	cvReleaseImage(&img);
	return img_rotate;
}
IplImage* POSSION::poissoncompute(IplImage* src1, IplImage* src2, IplImage* mask) {
	std::map<unsigned int, unsigned int>	  masked;
	int N = 0; // variable indexer
	for (int y = 1; y < mask->height - 1; y++) {
		for (int x = 1; x < mask->width - 1; x++) {
			unsigned int id = y*mask->widthStep + x;
			if (mask->imageData[id]) { // Masked pixel
				masked[id] = N;
				N++;
			}
		}
	}
	IplImage*src2_temp = cvCreateImage(cvGetSize(src2), 32, 3);
	cvLaplace(src2, src2_temp, 1);
	SuperMatrix    A, L, U;
	SuperMatrix    B;
	NCformat       *Ustore;
	double         *a, *rhs, *u;
	int            *asub, *xa;
	int            *perm_r;
	int            *perm_c;
	int            info, nrhs, row_inc;
	int            m, n, nnz, index;
	superlu_options_t options;
	SuperLUStat_t stat;
	set_default_options(&options);

	m = n = N;
	nnz = N * 5;

	if (!(a = doubleMalloc(nnz))) ABORT("Malloc fails for a[].");
	if (!(asub = intMalloc(nnz))) ABORT("Malloc fails for asub[].");
	if (!(xa = intMalloc(n + 1))) ABORT("Malloc fails for xa[].");


	nrhs = 3;
	index = 0;
	if (!(rhs = doubleMalloc(m * nrhs))) ABORT("Malloc fails for rhs[].");
	row_inc = 0;
	for (int y = 1; y < src1->height - 1; y++) {
		for (int x = 1; x < src1->width - 1; x++) {
			if (mask->imageData[x + y*mask->widthStep]) { // Variable
				unsigned int id = x + y*mask->widthStep;
				xa[row_inc] = index;

				// Right hand side is initialized to zero
				CvScalar bb = cvScalarAll(0);
				bb = add(bb, cvGet2D(src2_temp, y, x));
				if (mask->imageData[(x)+(y - 1)*mask->widthStep]) {
					a[index] = 1.0;
					asub[index] = masked[id - mask->widthStep];
					index++;
				}
				else {
					// Known pixel, update right hand side
					bb = sub(bb, cvGet2D(src1, y - 1, x));
				}


				if (mask->imageData[(x - 1) + (y)*mask->widthStep]) {
					a[index] = 1.0;
					asub[index] = masked[id - 1];
					index++;
				}
				else {
					bb = sub(bb, cvGet2D(src1, y, x - 1));
				}
				a[index] = -4.0;
				asub[index] = masked[id];
				index++;

				if (mask->imageData[(x + 1) + (y)*mask->widthStep]) {
					a[index] = 1.0;
					asub[index] = masked[id + 1];
					index++;
				}
				else {
					bb = sub(bb, cvGet2D(src1, y, x + 1));
				}

				if (mask->imageData[(x)+(y + 1)*mask->widthStep]) {
					a[index] = 1.0;
					asub[index] = masked[id + mask->widthStep];
					index++;
				}
				else {
					bb = sub(bb, cvGet2D(src1, y + 1, x));
				}

				unsigned int i = masked[id];
				// Spread the right hand side so we can solve using TAUCS for
				// 3 channels at once.
				for (int chan = 0; chan<src1->nChannels; chan++) {
					rhs[i + N*chan] = bb.val[chan];
				}
				row_inc++;
			}
		}
	}
	assert(row_inc == N);
	xa[n] = index;

	// Create matrix A in the format expected by SuperLU. 
	dCreate_CompCol_Matrix(&A, m, n, nnz, a, asub, xa, SLU_NC, SLU_D, SLU_GE);
	// Create right-hand side matrix B. 
	dCreate_Dense_Matrix(&B, m, nrhs, rhs, m, SLU_DN, SLU_D, SLU_GE);
	// Set the default input options. 
	set_default_options(&options);
	options.ColPerm = NATURAL;
	options.Trans = TRANS;
	options.ColPerm = COLAMD;
	if (!(perm_r = intMalloc(m))) ABORT("Malloc fails for perm_r[].");
	if (!(perm_c = intMalloc(n))) ABORT("Malloc fails for perm_c[].");

	// Initialize the statistics variables. 
	StatInit(&stat);
	// Solve the linear system. 
	dgssv(&options, &A, perm_c, perm_r, &L, &U, &B, &stat, &info);
	Ustore = (NCformat *)B.Store;
	u = (double*)Ustore->nzval;
	IplImage* result = cvCreateImage(cvGetSize(src1), 8, src1->nChannels);
	cvCopy(src1, result);
	for (int y = 1; y < src1->height; y++) {
		for (int x = 1; x < src1->width; x++) {
			if (mask->imageData[(x)+(y)*mask->widthStep]) {
				unsigned int id = y*mask->widthStep + x;
				unsigned int ii = masked[id];
				CvScalar p;
				for (int chan = 0; chan<src1->nChannels; chan++) {
					p.val[chan] = u[ii + N*chan];
				}
				cvSet2D(result, y, x, p);
			}
		}
	}

	// Clean Up Solver
	SUPERLU_FREE(rhs);
	SUPERLU_FREE(perm_r);
	SUPERLU_FREE(perm_c);
	Destroy_CompCol_Matrix(&A);
	Destroy_SuperMatrix_Store(&B);
	Destroy_SuperNode_Matrix(&L);
	Destroy_CompCol_Matrix(&U);
	return result;
}
