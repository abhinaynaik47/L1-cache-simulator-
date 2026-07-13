#include "core.hpp"


template <typename T>
void core<T>::read(unsigned int address){
    canDoNextInstruction=false;
    pair<bool,pair<int,int>> isMiss=cach->isReadMiss(address);
    if(!isMiss.first){
        unsigned int tag=cach->extractTag(address);
        auto nodeSearch=cach->findInMap(cach->cachelines->at(isMiss.second.first)->LRU->table,cach->cachelines->at(isMiss.second.first),tag);
        if(nodeSearch==nullptr){
            cerr<<"Invalid readmiss function"<<endl;
            cout<<"hi"<<endl;
            exit(1);
        }
        cach->updateLRU(address);
        canDoNextInstruction=true;
        if(!repeatSame){
            hits++;
        }
        repeatSame=false;
        busStopCycle=-1;
    }
    else{
        if(!repeatSame){
            misses++;
        }
        bool isDirtyToEvict=isdirtyToreplace(address);
        if(isDirtyToEvict){
            unsigned int addre=cach->cachelines->at(isMiss.second.first)->LRU->tail->val;
            auto isMiss=cach->isReadMiss(addre);
            if(isMiss.first){
                cerr<<"isdirty dirty"<<endl;
                exit(1);
            }
            busRequest=make_pair("WriteToMem",addre);
            repeatSame=true;
            if(!isReallyRequired){
                busStopCycle=curCycle+1;
            }
            isReallyRequired=true;
        }
        else{
            repeatSame=false;
            busRequest=make_pair("BusRd",address);
            busStopCycle=curCycle+1;
            isReallyRequired=false;
        }

    }
}
template <typename T>
bool core<T>::isdirtyToreplace(unsigned int address){
    int setIndex=cach->extractIndex(address);
    sett<T>* st=cach->cachelines->at(setIndex);
    if(st->invaldNodes->size==0){
        unsigned int addr=st->LRU->tail->val;
        pair<bool,pair<int,int>> isMiss=cach->isReadMiss(addr);
        if(isMiss.first){
            cerr<<"Invalid miss function"<<endl;
            exit(1);
        }
        char mode=cach->cachelines->at(isMiss.second.first)->entries->at(isMiss.second.second)->mode;
        if(setIndex!=isMiss.second.first){
            cerr<<"invalid coded "<<endl;
            exit(1);
        }
        if(st!=cach->cachelines->at(isMiss.second.first)){
            cerr<<"invalid code "<<endl;
            exit(1);
        }
        if(mode=='M'){
            return true;
        }
    }
    return false;
}
template <typename T>
void core<T>::write(unsigned int address,vector<core<T>*>* allCores){
    canDoNextInstruction=false;
    pair<bool,pair<int,int>> isMiss=cach->isWriteMiss(address);
    if(!isMiss.first){
        if(!repeatSame){
            hits++;
        }
        int setNum= isMiss.second.first;
        int rowNum=isMiss.second.second;
        char mode=this->cach->cachelines->at(setNum)->entries->at(rowNum)->mode;
        if(mode=='I'){
            cerr<<"Error in the write misses checking"<<endl;
            exit(1);
        }
        if(mode=='S' ){
            repeatSame=true;
            if(!isReallyRequired){
                busStopCycle=curCycle+1;
            }
            isReallyRequired=true;
            busRequest=make_pair("Invalidations",address);

        }
        else if(mode=='M'){
            canDoNextInstruction=true;
            cach->cachelines->at(setNum)->entries->at(rowNum)->mode='M';
            cach->updateLRU(address);
            busStopCycle=-1;
            repeatSame=false;
            isReallyRequired=false;
        }
        else if(mode=='E'){
            cach->cachelines->at(setNum)->entries->at(rowNum)->mode='M';
            canDoNextInstruction=true;
            cach->updateLRU(address);
            busStopCycle=-1;
            repeatSame=false;
            isReallyRequired=false;
        }
        else{
            cerr<<"Miss function incorrect"<<endl;
            exit(1);
        }

        // repeatSame=false;
    }
    else{
        if(!repeatSame){
            misses++;
        }
        bool isDirtyToEvict=isdirtyToreplace(address);
        if(isDirtyToEvict){
            unsigned int addre=cach->cachelines->at(isMiss.second.first)->LRU->tail->val;
            busRequest=make_pair("WriteToMem",addre);
            repeatSame=true;
            if(!isReallyRequired){
                busStopCycle=curCycle+1;
            }
            isReallyRequired=true;
        }
        else{
            repeatSame=false;
            busRequest=make_pair("BusRdX",address);
            isReallyRequired=false;
            busStopCycle=curCycle+1;
        }
        isBusNeeded=true;

    }
}


template <typename T>
pair<char,unsigned int> core<T>::parseInstruction(int curInstruction,int curCycleNo){
    char buffer[100];
    if(!isOver){
        if(fgets(buffer,sizeof(buffer),fptr)){
            char* typ =strtok(buffer," ");
            char* strAdd=strtok(nullptr," ");
            unsigned int address=strtoul(strAdd,nullptr,16);
            char typReq=typ[0];
            numInstructions++;
            instructionExcutionCycles++;
            return make_pair(typReq,address);
        }
        else{
            isOver=true;
            totalExcutionCycles=curCycleNo;
        }
    }
    return make_pair('A',0);
}

template <typename T>
void core<T>::sendData(){
    isDataReadyToSend=true;
}

template <typename T>
void core<T>::kick(vector<core<T>*>* allCores){
    if(this->cach->cachelines->at(3)->LRU->size!=(int)this->cach->cachelines->at(3)->LRU->table.size()){
        cout<<"got it"<<endl;
        exit(1);
    }
    if(busStopCycle>0 && canDoNextInstruction && busRequest.first=="BusRd"){
        cout<<"wrong"<<endl;
        exit(1);
    }
    if(isOver){
        exit(1);
        return;
    }
    if(repeatSame){
        if(curInstructionData.first=='W'){
            write(curInstructionData.second,allCores);
            instructionExcutionCycles++;

        }
        else if(curInstructionData.first=='R'){
            read(curInstructionData.second);
            instructionExcutionCycles++;
        }
        else{
            cerr<<"parse error"<<endl;
            exit(1);
        }
    }
    else{
        if(canDoNextInstruction){
            repeatSame=false;
            busStopCycle=-1;
            isReallyRequired=false;
            pair<char,unsigned int> imp=parseInstruction(curInstruction,curCycle);
            curInstructionData=imp;
            if(imp.first=='W'){
                write(imp.second,allCores);
                writes++;
            }
            else if(imp.first=='R'){
                read(imp.second);
                reads++;
            }
            else{
                isOver=true;
            }
        }
        else{
            if(!currentBusUs){
                idleCycles++;
            }
            else{
                instructionExcutionCycles++;
            }
        }
    }
    curCycle++;
}

template <typename T>
void core<T>::printInfo(){
    cout<<"Core "<<number<<" Statistics:"<<endl;
    cout<<"Total Instructions: "<<numInstructions<<endl;
    cout<<"Total Reads: "<<reads<<endl;
    cout<<"Total Writes: "<<writes<<endl;
    cout<<"Total Execution Cycles: "<<totalExcutionCycles<<endl;
    cout<<"Instruction Execution Cycles: "<<instructionExcutionCycles<<endl;
    cout<<"Total Idle Cycles: "<<idleCycles<<endl;
    cout<<"Cache Misses: "<<misses<<endl;
    cout<<"Cache Hits: "<<hits<<endl;
    cout<<"Cache miss rate: ";
    if(numInstructions>0){
        cout<<((float)(misses)/(numInstructions))*100<<endl;
    }
    else{
        cout<<0<<endl;
    }
    cout<<"Cache Evictions: "<<cach->evictions<<endl;
    cout<<"Writebacks: "<<writeBacks<<endl;
    cout<<"Bus Invalidations: "<<busInvalidations<<endl;
    cout<<"Data Traffic: "<<(dataTraffic) * (this->cach->blockSize)<<endl;
    cout<<endl;
}
template <typename T>
void core<T>::writeInfo(string output_file){
    ofstream file(output_file, ios::app);
    if(file.is_open()){
    file<<"Core "<<number<<" Statistics:"<<endl;
    file<<"Total Instructions: "<<numInstructions<<endl;
    file<<"Total Reads: "<<reads<<endl;
    file<<"Total Writes: "<<writes<<endl;
    file<<"Total Execution Cycles: "<<totalExcutionCycles<<endl;
    file<<"Instruction Execution Cycles: "<<instructionExcutionCycles<<endl;
    file<<"Total Idle Cycles: "<<idleCycles<<endl;
    file<<"Cache Misses: "<<misses<<endl;
    file<<"Cache Hits: "<<hits<<endl;
    file<<"Cache miss rate: ";
    if(numInstructions>0){
        file<<((float)(misses)/(numInstructions))*100<<endl;
    }
    else{
        file<<0<<endl;
    }
    file<<"Cache Evictions: "<<cach->evictions<<endl;
    file<<"Writebacks: "<<writeBacks<<endl;
    file<<"Bus Invalidations: "<<busInvalidations<<endl;
    file<<"Data Traffic: "<<(dataTraffic) * (this->cach->blockSize)<<endl;
    file<<endl;
    file.close();
    }
    else {
        cerr << "Invalid File" << endl;
        exit(1);
    }
}
template class core<unsigned int>;
