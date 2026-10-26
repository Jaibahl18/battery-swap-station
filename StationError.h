#ifndef STATIONERROR_H
#define STATIONERROR_H

#include<stdexcept>
#include<string>
using namespace std;

class stationError : public runtime_error{

public :
    stationError(string error_msg) : runtime_error(error_msg){}
};



#endif // STATIONERROR_H
