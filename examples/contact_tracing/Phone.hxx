#ifndef __Phone_hxx__
#define __Phone_hxx__

namespace Examples 
{

class Phone {

    private:

        int _threshold1;
        int _threshold2;
        bool _hasApplication;
        int _signalRadius; 

    public:

    Phone(const int& threshold1, const int& threshold2, const bool& hasApplicattion, const int& signalRadius);

    ~Phone();


};

}

#endif