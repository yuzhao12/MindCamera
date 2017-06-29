#include<Python.h>
#include "stdlib.h"
#include "stdio.h"
#include "GFHOG.h"
#include"dirent.h"
#include <iostream>
#include<opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <fstream>
#include"BOWHistogram.h"
using namespace cv;
using namespace std;
#ifdef IS_IMAGE
int type=0;
#else
int type=1;
#endif
#define image_num 10000
	/********************************************************************
	* read k-means vocabulary
	*********************************************************************/
extern "C"
{
	Mat vocabulary;
	Mat tfIdfOfBase;
	vector<vector<int>> invertedList(5000);
	Mat idfMat;
 	vector<vector<string>> imageNameList;
	bool ReadVocabulary(const char* vocFileName)
	{
		cout << "Reading vocabulary...";
		FileStorage fs(vocFileName, FileStorage::READ);
		if (fs.isOpened())
		{
			fs["vocabulary"] >> vocabulary;
			fs.release();
			cout << "Read over. " << endl << endl;
			return true;
		}
		return false;
	}
	bool ReadTfidf(const char*filename)
	{
		cout << "Reading tf_idf..." << endl;
		FileStorage fs(filename, FileStorage::READ);
		if (fs.isOpened())
		{
			fs["wordFreq"] >> tfIdfOfBase;
			fs.release();
			cout << "Read over " << endl;
			return true;
		}
		return false;
	}
	bool ReadInvertedList(const char*filename)
	{
		cout << "Reading indexfile..." << endl;
		ifstream in;
		int value;
		in.open(filename);
		for (int i = 0; i < invertedList.size(); i++) {
			in >> value;
			while (value != -1) {

				invertedList[i].push_back(value);
				in >> value;
			}
		}
		in.close();
		cout << "Read over" << endl;
		return true;
	}
	bool ReadFileName(const char*filename)
	{
		cout << "Reading filename..." << endl;
		ifstream in;
		string line;
		in.open(filename);
		if(in)
			{
		  		while(getline(in, line))
				{
					istringstream fin(line);
					string tmp;
					vector<string> strtmp;
					while (getline(fin, tmp, ','))
					{
						strtmp.push_back(tmp);
					}
					imageNameList.push_back(strtmp);
				}
			}	
		in.close();
		cout << "Read over" << endl;
		return true;
	}
	bool ReadIdfMat(const char*filename)
	{
		cout << "Reading IDF..." << endl;
		FileStorage fs(filename, FileStorage::READ);
		if (fs.isOpened())
		{
			fs["IDF"] >> idfMat;
			fs.release();
			cout << "Read over " << endl;
			return true;
		}
		return false;
	}

	/**************************************
	*vector<vector<double>> to  Mat
	**************************************/
	void Vector2Mat(vector<vector<double>>src, Mat dst)
	{
		assert(dst.rows == src.size());
		for (int i = 0; i < src.size(); i++)
		{
			for (int j = 0; j < src[0].size(); j++)
			{
				dst.at<float>(i, j) = src[i][j];

			}
		}
	}

	/*******************************
	 *extract the decrip
	 ******************************/
	Mat Des_Extractor(IplImage*img)
	{
		int setSize = 200;
		if (max(img->width, img->height) > setSize) {
			// Change in size
			CvSize s;
			if (img->width > img->height) {
				float r = (float)setSize / img->width;
				s.width = setSize;
				s.height = (float)(img->height) * r;
			}
			else {
				float r = (float)setSize / img->height;
				s.height = setSize;
				s.width = (float)(img->width) * r;
			}
			IplImage* resize = cvCreateImage(s, 8, 3);
			cvResize(img, resize);
			cvReleaseImage(&img);
			img = resize;
		}
		IplImage*mask = cvCreateImage(cvGetSize(img), 8, 1);
		cvZero(mask);
		cvNot(mask, mask);

		GFHOG descriptor;
		descriptor.Compute(img, (GFHOGType)type, mask);
		Mat des(descriptor.size(), descriptor[0].size(), CV_32F);
		Vector2Mat(descriptor, des);
		cvReleaseImage(&mask);
		cvReleaseImage(&img);
		return des;
	}
	
//////////For windows
/*	void getImageNameList(string imageDir, string patternFilter, vector<string>& imageNameList)
	{
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = nullptr;
		LPCTSTR lpszText;
		string imagePath;
		string separator = "\\";
		imagePath = imageDir + separator + patternFilter;
		lpszText = imagePath.c_str(); 
		hFind = FindFirstFile(lpszText, &FindFileData);
		while (hFind != INVALID_HANDLE_VALUE)
		{
			imageNameList.push_back((string)(FindFileData.cFileName));
			if (!FindNextFile(hFind, &FindFileData))
			{
				FindClose(hFind);
				hFind = INVALID_HANDLE_VALUE;
			}
		}
	}
*/
////////For linux
void getImageNameList(const char *path,const char*filetype,vector<string>& table)  
{     
    FILE *fp;  
    DIR *dp ;  
    struct dirent *dirp ;                          
    if( (dp = opendir( path )) == NULL )  
    {  
         perror("opendir");  
    } 
                                                         
    while( ( dirp = readdir( dp ) ) != NULL)  
    {  
                                                         
       if(strcmp(dirp->d_name,".")==0  || strcmp(dirp->d_name,"..")==0)  
       continue;  
       int size = strlen(dirp->d_name);  
       if(size<5)     
            continue;  
       if(strcmp( ( dirp->d_name + (size - 4) ) , filetype) != 0)  
            continue;        
           string out=dirp->d_name;
           table.push_back(out);
          cout<<out<<endl;
    }  
    closedir(dp);           
} 

	
 string Doreview(const char*inputfile)
	{	
		BOWHistogram bowDE(new FlannBasedMatcher());
		bowDE.setVocabulary(vocabulary);

		Mat img1 = imread(inputfile, -1);
		IplImage*img = &IplImage(img1);
		for (int y = 0; y<img->height; y++)
		{
			for (int x = 0; x < img->width; x++)
			{
				CvScalar color_tmp = cvGet2D(img, y, x);
				if (color_tmp.val[3] == 0)
				{
					color_tmp.val[0] = 255;
					color_tmp.val[1] = 255;
					color_tmp.val[2] = 255;
					cvSet2D(img, y, x, color_tmp);
				}

			}

		}
		IplImage*img2 = cvCreateImage(cvGetSize(img), 8, 3);
		cvCvtColor(img, img2, CV_BGRA2BGR);
		Mat des = Des_Extractor(img2);
		Mat descriptors;
		bowDE.compute(des, descriptors);

		Mat tfIdfOfImg(1, 5000, CV_32F);
       for (int i = 0; i < 5000; i++) {
			//tfIdfOfImg.at<float>(0,i) = idfMat.at<float>(0,i)*descriptors.at<float>(0,i);
			tfIdfOfImg.at<float>(0, i) = descriptors.at<float>(0, i);
		}

		cout << "Start searching..." << endl;
		double similarity[image_num] = { 0 };
		int flag[image_num] = { 0 };
		int indices[image_num] = { 0 };
		for (int i = 0; i < tfIdfOfImg.cols; i++) {

			if (tfIdfOfImg.at<float>(0, i) != 0) {
				vector<int> imgsId = invertedList[i];
				for (int j = 0; j < imgsId.size(); j++) {
					int id = imgsId[j];
					if (flag[id] == 0) {
						flag[id] = 1;
						double vectorMultiply = 0.00;
						double vector1Modulo = 0.00;
						double vector2Modulo = 0.00;

						for (int t = 0; t < 5000; t++) {

							vector1Modulo += 0.1 * tfIdfOfBase.ptr<float>(id)[t] * tfIdfOfBase.ptr<float>(id)[t];
							vector2Modulo += 0.1 * tfIdfOfImg.at<float>(0, t) * tfIdfOfImg.at<float>(0, t);
							vectorMultiply += 0.1 * tfIdfOfBase.ptr<float>(id)[t] * tfIdfOfImg.at<float>(0, t)*idfMat.at<float>(0, t);

						}

						vector1Modulo = sqrt(vector1Modulo);
						vector2Modulo = sqrt(vector2Modulo);
						similarity[id] = vectorMultiply / (vector1Modulo*vector2Modulo);
					}
				}

			}
		}
		//sort
		for (size_t i = 0; i < image_num; i++)
		{
			indices[i] = i;
		}

		int tmp_index; double tmp;

		for (int i = 0; i < image_num; i++)
		{
			for (int j = i + 1; j < image_num; j++)
			{
				if (similarity[i] < similarity[j])
				{
					tmp = similarity[i];
					similarity[i] = similarity[j];
					similarity[j] = tmp;

					tmp_index = indices[i];
					indices[i] = indices[j];
					indices[j] = tmp_index;
				}
			}
		}


		vector<int> feedback;
		vector<string> class_name;
		for (int i = 0; i<10; i++)
		{
			int j=0;
			for (j = 0; j < class_name.size(); j++)
			{
				if (class_name[j] == imageNameList[indices[i]][1])
				{
					feedback[j] += 10 - i;
					break;
				}
			}
			if (j == class_name.size())
			{
				class_name.push_back(imageNameList[indices[i]][1]);
				feedback.push_back(10 - i);
			}
			
		}
		for (int i = 0; i < class_name.size(); i++)
		{
			//cout << class_name[i] << ": " << feedback[i] << endl;
			if (class_name[i] == " ")
				feedback[i] = 1;
		}
		/*********
		*feedback
		*
		**********/
 		for (int i = 0; i<30; i++)
		{
			int j;
			for (j = 0; j < class_name.size(); j++)
			{
				if (class_name[j] == imageNameList[indices[i]][1])
				{
					similarity[i] = std::pow(similarity[i], 1.0 / feedback[j]);
					break;
				}
			}

		}

		/****************
		*resort
		*
		******************/
		for (int i = 0; i < 30; i++)
		{
			for (int j = i + 1; j < 30; j++)
			{
				if (similarity[i] < similarity[j])
				{
					tmp = similarity[i];
					similarity[i] = similarity[j];
					similarity[j] = tmp;

					tmp_index = indices[i];
					indices[i] = indices[j];
					indices[j] = tmp_index;
				}
			}
		}

		/***************
		*return top 12
		*
		*****************/
		string imagename;
		for (int i = 0; i < 12; i++)
		{
			imagename = imagename + imageNameList[indices[i]][0];
			if (i != 11)
			{
				imagename += " ";
			}
		}
		return imagename;
	}
/********
*Python 
***/
static PyObject* wrap_Doreview(PyObject* self, PyObject* args)
{
	const char *result;
	const char *inputfile;
	PyObject*str;
	if (!PyArg_ParseTuple(args, "s", &inputfile))
		return NULL;
	result = Doreview(inputfile).c_str();
	return Py_BuildValue("s",result);
}
static PyObject* wrap_ReadVocabulary(PyObject* self, PyObject* args)
{
	int result = 0;
	const char*inputfile;
	if (!PyArg_ParseTuple(args, "s", &inputfile))
		return NULL;
	if (!ReadVocabulary(inputfile))
		return Py_BuildValue("i", result);

	return Py_BuildValue("i", result+1);
}
static PyObject* wrap_ReadTfidf(PyObject* self, PyObject* args)
{
	int result = 0;
	const char*inputfile;
	if (!PyArg_ParseTuple(args, "s", &inputfile))
		return NULL;
	if (!ReadTfidf(inputfile))
		return Py_BuildValue("i", result);

	return Py_BuildValue("i", result + 1);
}
static PyObject* wrap_ReadInvertedList(PyObject* self, PyObject* args)
{
	int result = 0;
	const char*inputfile;
	if (!PyArg_ParseTuple(args, "s", &inputfile))
		return NULL;
	if (!ReadInvertedList(inputfile))
		return Py_BuildValue("i", result);

	return Py_BuildValue("i", result + 1);
}
static PyObject* wrap_ReadFileName(PyObject* self, PyObject* args)
{
	int result = 0;
	const char*inputfile;
	if (!PyArg_ParseTuple(args, "s", &inputfile))
		return NULL;
	if (!ReadFileName(inputfile))
		return Py_BuildValue("i", result);

	return Py_BuildValue("i", result + 1);
}
static PyObject* wrap_ReadIdfMat(PyObject* self, PyObject* args)
{
	int result = 0;
	const char*inputfile;
	if (!PyArg_ParseTuple(args, "s", &inputfile))
		return NULL;
	if (!ReadIdfMat(inputfile))
		return Py_BuildValue("i", result);

	return Py_BuildValue("i", result + 1);
}

static PyMethodDef DoreviewMethods[] = {
	{"wrap_Doreview", wrap_Doreview, METH_VARARGS, "review"},
	{"wrap_ReadVocabulary", wrap_ReadVocabulary, METH_VARARGS, "ReadVocabulary" },
	{"wrap_ReadTfidf", wrap_ReadTfidf, METH_VARARGS, "ReadTfidf" },
	{"wrap_ReadInvertedList", wrap_ReadInvertedList, METH_VARARGS, "ReadInvertedList" },
	{"wrap_ReadFileName", wrap_ReadFileName, METH_VARARGS, "ReadFileName" },
	{"wrap_ReadIdfMat", wrap_ReadIdfMat, METH_VARARGS, "ReadIdfMat" },
	{ NULL,NULL,0,NULL }
};


PyMODINIT_FUNC initreview(){
	PyObject* m;
	m = Py_InitModule("review", DoreviewMethods);
}
}
