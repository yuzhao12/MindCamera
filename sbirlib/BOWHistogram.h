#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include<opencv2/features2d/features2d.hpp>
using  namespace cv;
class  BOWHistogram
{
public:
	BOWHistogram(const Ptr<DescriptorMatcher>& dmatcher );
    virtual ~BOWHistogram();

    void setVocabulary( const Mat& vocabulary );
    const Mat& getVocabulary() const;
 
	void compute(const Mat &descriptors,Mat& des);
    // compute() is not constant because DescriptorMatcher::match is not constant

    int descriptorSize() const;
    int descriptorType() const;

protected:
    Mat vocabulary;
    Ptr<DescriptorMatcher> dmatcher;
};