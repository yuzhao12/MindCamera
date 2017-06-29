#include"BOWHistogram.h"
#include<stdio.h>
using namespace std;
BOWHistogram::BOWHistogram( const Ptr<DescriptorMatcher>& _dmatcher ) :
 dmatcher(_dmatcher)
{}

BOWHistogram::~BOWHistogram()
{}

void BOWHistogram::setVocabulary( const Mat& _vocabulary )
{
    dmatcher->clear();
    vocabulary = _vocabulary;
    dmatcher->add( vector<Mat>(1, vocabulary) );
}

const Mat& BOWHistogram::getVocabulary() const
{
    return vocabulary;
}

void BOWHistogram::compute(const Mat &descriptors,Mat& des)
{
	des.release();
	if( descriptors.empty() )
        return;
	// calculate descriptors
		int clusterCount = vocabulary.rows;
		des=Mat( 1, clusterCount, CV_32FC1, Scalar::all(0.0) );
		vector<DMatch> matches;
		dmatcher->match( descriptors, matches );
        // Compute image descriptor
		float *dptr = (float*)des.data;
		for( size_t i = 0; i < matches.size(); i++ )
		{
			int queryIdx = matches[i].queryIdx;
			int trainIdx = matches[i].trainIdx; // cluster index
			CV_Assert( queryIdx == (int)i );
			dptr[trainIdx] = dptr[trainIdx] + 1.f;
		}
		// Normalize image descriptor.
	   des /= descriptors.rows;
}
int BOWHistogram::descriptorSize() const
{
    return vocabulary.empty() ? 0 : vocabulary.rows;
}

int BOWHistogram::descriptorType() const
{
    return CV_32FC1;
}