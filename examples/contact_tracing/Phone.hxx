#ifndef __Phone_hxx__
#define __Phone_hxx__

#include <vector>

namespace Examples 
{

class Phone {

    private:

        int _threshold1;
        int _threshold2;
        bool _hasApplication;
        int _signalRadius; 
        int _variance;
        std::vector<int> _buckets;
        std::vector<int> _bucketsSick;
        int _maxDistance;
        int _startOffset;

    public:

    Phone(const int& threshold1, const int& threshold2, const bool& hasApplicattion, const int& signalRadius);

    ~Phone();

    bool hasApp();

    std::vector<int> getBuckets();

    std::vector<int> getBucketsPositive();

    int getStartOffset();

    int getSignalRadius();

    int checkDetection(const int& sick, const double& distance, const int& dt);

};

}

#endif