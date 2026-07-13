#pragma once
#include "core.cpp"
template <typename T>
class bus{
    public:
        int numCore;
        string traceFile;
        vector<core<T>*>* allCores;
        bool isFree;
        string curSignal;
        int curProcCycle;
        string curProc;
        int curCore;
        int curCore2;
        bool isWriteToMem;
        unsigned int address;
        bool isRWITMBlocking;
        int blockBits;
        int assoc;
        int indexBits;
        int transactions;
        int traffic;
        bool isReadModify;
        int dataTraffic;
        bus(int blockBits,int assoc,int indexBits,string trace,int coreN=4){
            this->blockBits=blockBits;
            this->assoc=assoc;
            this->indexBits=indexBits;
            numCore=coreN;
            traceFile=trace;
            allCores=new vector<core<T>*>();
            for(int i=0;i<coreN;i++){
                string fileName=trace + "_proc"+to_string(i)+".trace";
                allCores->push_back(new core<T>(blockBits,assoc,indexBits,fileName,i));
            }
            isFree=true;
            curProcCycle=0;
            curProc="";
            curCore=-1;
            curCore2=-1;
            isWriteToMem=false;
            address=0;
            isRWITMBlocking=false;
            transactions=0;
            dataTraffic=0;
            isReadModify=false;
        }
        ~bus(){
            delete allCores;
        }
        pair<int,pair<int,int>> read(unsigned int address,int coreNum);
        pair<int,pair<int,int>> readX(unsigned int address,int coreNum);
        void writeToMem();
        void readFromMem();
        int takeRequest();
        void makeInvalidations(unsigned int address,int coreNum);
        void kick();
        void RWITM();
        void CacheToCacheTransfer();
        void readFromMemAndWrite();
        void makeInvalidationsfromMem(unsigned int address,int coreNum);
        void printSimulationParameters();
        void writeSimulationParameters(string output_file);
        // bool isdirtyToreplace(int coreNumber,unsigned int address);
    };
