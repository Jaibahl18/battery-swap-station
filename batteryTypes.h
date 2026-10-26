#ifndef BATTERYTYPES_H
#define BATTERYTYPES_H

#include"battery.h"

class Li_ion: public battery{
    int voltage = 48;
    string type = "Li-Ion";

public :
    Li_ion(int idVal, double socVal) : battery( idVal,  socVal){}

    void changePerSecond(){
        int currentSoc = getSoc();
        if(!getStatus()){
            if(currentSoc > 80){
                changeSoc(max(0.1, 2.0*(100-currentSoc)/(20)));
            }else{
                changeSoc(2.0);
            }
        }else{
            changeSoc(-1);
        }
    }
    int getVoltage(){return voltage;}
    string getType(){return type;}

};
class Pb_acid: public battery{
    int voltage = 12;
    string type = "Lead Acid";
public :


    Pb_acid(int idVal, double socVal) : battery( idVal,  socVal){}

    void changePerSecond(){
        int currentSoc = getSoc();
        if(!getStatus()){
            if(currentSoc > 70){
                changeSoc(max(0.1, 0.8*(100-currentSoc)/(30)));
            }else{
                changeSoc(0.8);
            }
        }else{
            changeSoc(-1.2);
        }
    }
    int getVoltage(){return voltage;}
    string getType(){return type;}
};

#endif // BATTERYTYPES_H