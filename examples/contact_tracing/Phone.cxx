#include <Phone.hxx>

#include <GeneralState.hxx>

#include <math.h>
#include <algorithm>
#include <random>

namespace Examples 
{

Phone::Phone(const int& threshold1, const int& threshold2, const bool& hasApplicattion, const int& signalRadius) 
: _threshold1(_threshold1), _threshold2(threshold2), _hasApplication(hasApplicattion), _signalRadius(signalRadius), _variance(5),
    _maxDistance(100) {
    _startOffset = Engine::GeneralState::statistics().getUniformDistValue(0,300);
    _buckets = std::vector<int>(3);
    _bucketsSick = std::vector<int>(3);
}

Phone::~Phone() {}

bool Phone::hasApp() {
    return _hasApplication;
}

std::vector<int> Phone::getBuckets() {
    return _buckets;
}

std::vector<int> Phone::getBucketsPositive() {
    return _bucketsSick;
}

int Phone::getStartOffset() {
    return _startOffset;
}

int Phone::checkDetection(const int& sick, const double& distance, const int& dt) {
    int d_ref = 9;
	int s_ref = 70;
	int s_base = 40;
    double s_center = s_base + std::log(distance)*(s_ref-s_base)/std::log(d_ref);
    double noise = 2 * _variance * std::min((s_center-s_base)/20,1.0);
    std::default_random_engine generator;
    std::normal_distribution<double> normal(s_center,noise);
    double signal = normal(generator);
    int bucket = -1;
    int detect = 0;
    if (signal < _threshold1) {
        bucket = 0;
        detect = 1;
    }
    else if (signal >= _threshold1 and signal < _threshold2) {
        bucket = 1;
        detect = 1;
    }
    else if (signal >= _threshold2 and signal < _maxDistance) {
        bucket = 2;
    }
    if (bucket > -1) {
        _buckets[bucket] = _buckets[bucket] + dt;
        if (sick) {
            _bucketsSick[bucket] = _bucketsSick[bucket] + dt;
        }
    }
    return detect;
}

}