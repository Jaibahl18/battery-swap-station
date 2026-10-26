#ifndef BATTERY_H
#define BATTERY_H
#include"StationError.h"

class battery{
private:
    int id;
    double soc;
    bool issued;


    inline static int batteryCount = 0;

protected:

    void changeSoc(double delta){
        if(soc + delta > 100) soc = 100;
        else if(soc + delta < 0) soc = 0;
        else soc = soc+delta;
    }




public:

    static int get_battery_count(){
        return batteryCount;
    }

    battery(int idVal, double socVal){
        id = idVal;
        if(socVal > 100 || socVal< 0) throw stationError("SOC should be between 0 - 100");
        else {
            soc = socVal;
            issued = false;
            batteryCount++;
        }
    }
    virtual ~battery(){
        batteryCount--;
    }

    double getSoc() {return soc;}
    int getId() {return id;}
    bool getStatus(){return issued;}
    void issue(){issued = true;}
    void giveBack(){issued = false;}

    virtual void changePerSecond() = 0;
    virtual int getVoltage()=0;
    virtual string getType()=0;

};


#endif // BATTERY_H
