#ifndef BATTERYSTATION_H
#define BATTERYSTATION_H
#include"batteryTypes.h"
#include"StationError.h"
#include<vector>

class station{
    vector<battery*> batteryList;
    int capacity;
    int nextId=1;

public:

    station(int cap) : capacity(cap){

        if(cap < 1) throw stationError("Enter atleast 1 station.");
        batteryList = vector<battery*>(capacity, nullptr);
    }

    ~station(){
        for(battery* ptr : batteryList){
            delete ptr;
        }
    }

    void addBattery(char type, double soc){
        for(int i=0 ; i<capacity; i++){
            if(batteryList[i]==nullptr){
                if(type == 'l'){
                    batteryList[i] = new Li_ion(nextId, soc);
                }else{
                    batteryList[i] = new Pb_acid(nextId, soc);
                }
                nextId++;
                return;
            }

        }
        throw stationError("Cannot add battery, capacity full.");
    }

    void removeBattery(int id){
        for(int i=0; i<capacity;i++){
            if(batteryList[i] != nullptr && batteryList[i]->getId()==id){
                delete batteryList[i];
                batteryList[i]=nullptr;
            }
        }
    }

    battery* issueBattery(){
        battery* best = nullptr;
        for(battery* ptr : batteryList){
            if((ptr!=nullptr) && !(ptr->getStatus()) && (best==nullptr || (best->getSoc()<ptr->getSoc()))) best = ptr;
        }
        if(best == nullptr) throw stationError("no battery free, cannot issue.");
        else if(best->getSoc() < 20) throw stationError("please wait,no battery > 20% soc");
        else{
            best->issue();
        }
        return best;
    }
    void returnBattery(int id){
        battery * retBattery= nullptr;
        for(battery* ptr : batteryList){
            if(ptr!= nullptr && ptr->getId()==id) retBattery = ptr;
        }
        if(retBattery==nullptr) throw stationError("no battery with given id");
        if(!(retBattery->getStatus()))throw stationError("battery already at station, cannot return");
        else{
            retBattery->giveBack();
        }
    }

    void changeAll(){
        for(battery* ptr : batteryList){
            if(ptr != nullptr){
                ptr->changePerSecond();
            }
        }
    }

    vector<battery*>& getBatteryList(){ return batteryList; }
    int getCapacity(){ return capacity; }
};


#endif // BATTERYSTATION_H
