#pragma once
#include "cache.hpp"
#include <fstream>
#include <cstring>
#include <sstream>
#include <cstdlib>
template <typename T>
class core{
    public:
        int number;
        FILE* fptr;
        int hits;
        int misses;
        int numInstructions;
        int reads;
        int writes;
        int totalExcutionCycles;
        int idleCycles;
        int evictions;
        int writeBacks;
        int busInvalidations;
        int dataTraffic;
        string fileName;
        cache<T>* cach;
        int curInstruction;
        bool canDoNextInstruction;
        pair<char,unsigned int > curInstructionData;
        int busStopCycle;
        bool repeatSame;
        bool isInvalidation;
        int curCycle;
        bool isOver;
        bool isDataRequested;
        int dataSendCurCycle;
        int instructionExcutionCycles;
        unsigned int sendAddress;
        bool isDataReadyToSend;
        bool isBusNeeded;
        bool shouldNotRepeatSame;
        bool currentBusUs;
        bool isReallyRequired;
        pair<string,unsigned int> busRequest;
        core(int blockBits,int assoc,int indexBits,string file,int number){
            fileName=file;
            curInstruction=0;
            isOver=false;
            canDoNextInstruction=true;
            busStopCycle=-1;
            hits=misses=reads=writes=totalExcutionCycles=idleCycles=evictions=writeBacks=busInvalidations=0;
            cach=new cache<T>(blockBits,assoc,indexBits);
            this->number=number;
            // cout<<"file is "<<file<<endl;
            instructionExcutionCycles=0;
            shouldNotRepeatSame=false;
            currentBusUs=false;
            fptr=fopen(fileName.c_str(),"r");
            if(!fptr){
                cerr<<"file invalid"<<endl;
                exit(1);
            }
            repeatSame=false;
            isReallyRequired=false;
            dataTraffic=0;
            busRequest=make_pair("",0);
            // isDataRequested=false;
            // dataSendCurCycle=0;
            // isBusNeeded=false;
            // sendAddress=0;
            // isDataReadyToSend=false;
            curCycle=0;
            isInvalidation=false;
        }
        ~core(){
            delete cach;
        }
        pair<char,unsigned int> parseInstruction(int curInstruction,int curCycleNo);
        void read(unsigned int address);
        void write(unsigned int address,vector<core<T>*>* allCores);
        // void invalidate(unsigned int address,vector<core<T>*>* allCores);
        void printInfo();
        void sendData();
        bool isdirtyToreplace(unsigned int address);
        void kick(vector<core<T>*>* allCores);
        void writeInfo(string output_file);
};
