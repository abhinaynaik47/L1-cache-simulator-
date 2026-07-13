#include "bus.hpp"
// ./L1simulate -t app2 -s 5 -E 2 -b 6 -o output_app1.txt
// breakpoint set --file "cache.cpp" --line 12
template <typename T>
void bus<T>::makeInvalidationsfromMem(unsigned int address,int coreNum){
    for(core<T>* cor:*allCores){
        pair<bool,pair<int,int>> isMiss=cor->cach->isReadMiss(address);
        if(!isMiss.first){
            cerr<<"Invalid miss function in read from mem  hkb"<<endl;
            exit(1);
        }
    }
}

template <typename T>
void bus<T>::makeInvalidations(unsigned int address,int coreNum){
    for(core<T>* cor:*allCores){
        if(cor->number!=coreNum){
            pair<bool,pair<int,int>> isMiss=cor->cach->isReadMiss(address);
            if(!isMiss.first){
                cor->cach->changeMode('I',address);
            }
        }
    }
}


template <typename T>
pair<int,pair<int,int>> bus<T>::read(unsigned int address,int coreNum){
    pair<bool,pair<int,int>> isMiss;
    int i=0;
    for(core<T>* cor:*(allCores)){
        if(i!=coreNum){
            isMiss=cor->cach->isReadMiss(address);
            if(!isMiss.first){
                return make_pair(i,isMiss.second);
            }
        }
        i++;
    }
    return make_pair(-1,make_pair(-1,-1));
}
template <typename T>
pair<int,pair<int,int>> bus<T>::readX(unsigned int address,int coreNum){
    int i=0;
    pair<bool,pair<int,int>> isMiss;
    for(core<T>* cor:*allCores){
        if(i!=coreNum){
            isMiss=cor->cach->isReadMiss(address);
            if(!isMiss.first){
                return make_pair(i,isMiss.second);
            }
        }
        i++;
    }
    return make_pair(-1,make_pair(-1,-1));
}
template <typename T>
int bus<T>::takeRequest(){
    int suitableProc=-1;
    int curMinCycle=-1;
    bool isEmpty=true;
    for(core<T>* cor:*allCores){
        if(cor->busStopCycle>0 &&    !cor->isOver ){
            if(isEmpty){
                suitableProc=cor->number;
                curMinCycle=cor->busStopCycle;
                isEmpty=false;
            }
            else if(cor->busStopCycle<curMinCycle){
                suitableProc=cor->number;
                curMinCycle=cor->busStopCycle;
            }
        }
    }
    return suitableProc;
}

template <typename T>
void bus<T>::readFromMem(){
    if(curProcCycle==99){
        allCores->at(curCore)->cach->addOne(address,'E');
        allCores->at(curCore)->dataTraffic++;
        core<T>* cor=allCores->at(curCore);
        cor->canDoNextInstruction=false;
        cor->repeatSame=true;
        cor->busStopCycle=-1;
        isFree=true;
        cor->isReallyRequired=false;
    }
    curProcCycle++;
}

template <typename T>
void bus<T>::writeToMem(){
    if(curProcCycle==1){
        pair<bool,pair<int,int>> dir=allCores->at(curCore)->cach->isReadMiss(address);
        if(dir.first){
            cerr<<"Wrong code for missing checking"<<endl;
            exit(1);
        }
    }
    if(curProcCycle==99){
        // allCores->at(curCore)->instructionExcutionCycles--;
        allCores->at(curCore)->dataTraffic++;
        allCores->at(curCore)->writeBacks++;
        allCores->at(curCore)->canDoNextInstruction=false;
        allCores->at(curCore)->repeatSame=true;
        allCores->at(curCore)->isReallyRequired=false;
        allCores->at(curCore)->busStopCycle=-1;
        if(isReadModify){
            if(allCores->at(curCore)->shouldNotRepeatSame){
                allCores->at(curCore)->repeatSame=false;
                allCores->at(curCore)->isReallyRequired=false;
                allCores->at(curCore)->canDoNextInstruction=true;
            }
            allCores->at(curCore)->shouldNotRepeatSame=false;
            isReadModify=false;
        }
        isFree=true;
    }
    curProcCycle++;
}

template <typename T>
void bus<T>::CacheToCacheTransfer(){
    if(curProcCycle==1){
        pair<bool,pair<int,int>> dir=allCores->at(curCore2)->cach->isReadMiss(address);
        if(dir.first){
            cerr<<"Incorrect miss detection in cache to cache"<<endl;
        }
    }
    if(curProcCycle==((0.5 * (allCores->at(curCore)->cach->blockSize)))-1){
        isFree=true;
        allCores->at(curCore)->busStopCycle=-1;
        allCores->at(curCore)->cach->addOne(address,'S');
        allCores->at(curCore)->dataTraffic++;
        // allCores->at(curCore)->instructionExcutionCycles--;
        allCores->at(curCore2)->dataTraffic++;
        allCores->at(curCore)->repeatSame=true;
        allCores->at(curCore)->canDoNextInstruction=false;
        if(isWriteToMem){
            isFree=false;
            isReadModify=true;
            curProc="WriteToMem";
            address=this->address;
            curCore=this->curCore2;
            curProcCycle=-1;
            allCores->at(curCore)->canDoNextInstruction=false;
            allCores->at(curCore)->repeatSame=false;
            allCores->at(curCore)->shouldNotRepeatSame=true;
            transactions++;
            isWriteToMem=false;
        }
        }
    curProcCycle++;
}

template <typename T>
void bus<T>::readFromMemAndWrite(){
    if(curProcCycle==99){
        isFree=true;
        allCores->at(curCore)->cach->addOne(address,'M');
        allCores->at(curCore)->canDoNextInstruction=false;
        allCores->at(curCore)->busStopCycle=-1;
        allCores->at(curCore)->repeatSame=true;
        allCores->at(curCore)->dataTraffic++;
        allCores->at(curCore)->isReallyRequired=false;

    }
    curProcCycle++;
}

template <typename T>
void bus<T>::RWITM(){
    if(curProcCycle==99){
        allCores->at(curCore)->cach->addOne(address,'M');
        // allCores->at(curCore)->instructionExcutionCycles--;
        allCores->at(curCore)->canDoNextInstruction=false;
        allCores->at(curCore)->busStopCycle=-1;
        allCores->at(curCore)->repeatSame=true;
        allCores->at(curCore)->isReallyRequired=false;
        allCores->at(curCore)->dataTraffic++;
        isFree=true;
    }
    curProcCycle++;
}

template <typename T>
void bus<T>::kick(){
    if(isFree){
        int coreNum=takeRequest();
        if(coreNum<0){
            return;
        }
        else{
            curProcCycle=1;
            isFree=false;
            string signal=allCores->at(coreNum)->busRequest.first;
            unsigned int  addr=allCores->at(coreNum)->busRequest.second;
            if(signal=="Invalidations"){
                curCore=coreNum;
                address=addr;
                makeInvalidations(address,curCore);
                allCores->at(curCore)->cach->changeMode('E',address);
                allCores->at(curCore)->repeatSame=true;
                allCores->at(curCore)->isReallyRequired=false;
                allCores->at(curCore)->busInvalidations++;
                this->transactions++;
                isFree=true;
            }
            else if(signal=="WriteToMem"){
                curProc="WriteToMem";
                curCore=coreNum;
                curProcCycle=1;
                address=addr;
                allCores->at(curCore)->cach->changeMode('E',address);
                allCores->at(curCore)->currentBusUs=true;
                transactions++;

            }
            else if(signal=="BusRd"){
                pair<int,pair<int,int>> isPresentOthercaches=read(addr,coreNum);
                if(isPresentOthercaches.first<0){
                    curProc="ReadMem";
                    curProcCycle=1;
                    curCore=coreNum;
                    address=addr;
                    transactions++;
                    allCores->at(curCore)->currentBusUs=true;
                    allCores->at(curCore)->busStopCycle=-1;
                }
                else{
                    int setIndex=isPresentOthercaches.second.first;
                    int rowIndex=isPresentOthercaches.second.second;
                    char mode=allCores->at(isPresentOthercaches.first)->cach->cachelines->at(setIndex)->entries->at(rowIndex)->mode;
                    curProc="CacheToCacheTransfer";
                    curProcCycle=1;
                    curCore=coreNum;
                    curCore2=isPresentOthercaches.first;
                    this->address=addr;
                    if(mode=='M'){
                        isWriteToMem=true;
                    }
                    else{
                        isWriteToMem=false;
                    }
                    transactions++;
                    allCores->at(curCore2)->cach->changeMode('S',address);
                    allCores->at(curCore)->currentBusUs=true;

                }
            }
            else if(signal=="BusRdX"){
                pair<int,pair<int,int>> isMiss=readX(addr,coreNum);
                if(isMiss.first<0){
                    curProc="ReadMemX";
                    curCore=coreNum;
                    curProcCycle=1;
                    address=addr;
                    transactions++;
                    makeInvalidationsfromMem(address,curCore);
                    allCores->at(curCore)->currentBusUs=true;
                }
                else{
                    int setIndex=isMiss.second.first;
                    int rowIndex=isMiss.second.second;
                    char mode=allCores->at(isMiss.first)->cach->cachelines->at(setIndex)->entries->at(rowIndex)->mode;
                    pair<bool,pair<int,int>> path=allCores->at(isMiss.first)->cach->isReadMiss(addr);
                    if(path.first){
                        cerr<<"RWITM wrong"<<endl;
                        exit(1);
                    }
                    if(mode=='E' || mode=='S'){
                        curProc="RWITM";
                        curCore=coreNum;
    
                        curCore2=isMiss.first; //  ------NEED TO CHEK IF REQUIRED OR NOT ------
                        curProcCycle=1;
                        address=addr;
                        transactions++;
                        makeInvalidations(address,curCore);
                        allCores->at(curCore)->busInvalidations++;
                        allCores->at(curCore)->currentBusUs=true;
                    }
                    else if(mode=='M'){
                        curProc="WriteToMem";
                        transactions++;
                        curCore=isMiss.first;
                        curProcCycle=1;
                        address=addr;
                        allCores->at(curCore)->cach->changeMode('E',address);
                        allCores->at(curCore)->currentBusUs=true;
                    }
                    else{
                        cerr<<"Error in the missing checking function wrie miss "<<endl;
                        exit(1);
                    }
            }

                }
                    }
            }

    else{
        for(core<T>* cor:*this->allCores){
            if(cor->cach->cachelines->at(0)->LRU->size!=(int)cor->cach->cachelines->at(0)->LRU->table.size()){
                cout<<"gotch it"<<endl;
                exit(1);
            }
        }
        if(curProc=="ReadMem"){
            // cout<<"ReadMemmmmmmmmm"<<endl;
            readFromMem();
        }
        else if(curProc=="ReadMemX"){
            // cout<<"ReadMemX"<<endl;
            readFromMemAndWrite();
        }
        else if(curProc=="WriteToMem"){
            // cout<<"WriteToMem"<<endl;
            writeToMem();
        }
        else if(curProc=="RWITM"){
            // cout<<"RWITM"<<endl;
            RWITM();
        }
        else if(curProc=="CacheToCacheTransfer"){
            // cout<<"RCacheToCacheTransferWITM"<<endl;
            CacheToCacheTransfer();
        }
        else{
            cerr<<"Invalid operation on bus"<<endl;
            exit(1);
        }
        // allCores->at(curCore)->instructionExcutionCycles++;
        if(isFree){
            allCores->at(curCore)->currentBusUs=false;
            allCores->at(curCore)->isReallyRequired=false;
            // allCores->at(curCore)->instructionExcutionCycles++;
        }
        if(curProcCycle>110 && curProcCycle >((0.5 * (allCores->at(curCore)->cach->blockSize)))+5){
            cerr<<"infinite recursion"<<endl;
            exit(1);
        }
    }
}

template <typename T>
void bus<T>::printSimulationParameters(){
    cout<<"Simulation Parameters: "<<endl;
    cout<<"Set Index Bits: "<<this->indexBits<<endl;
    cout<<"Associativity: "<<this->assoc<<endl;
    cout<<"Block Bits: "<<this->blockBits<<endl;
    cout<<"Block Size (Bytes): "<<(1<<(this->blockBits))<<endl;
    cout<<"Number of Sets: "<<(1<<(this->indexBits))<<endl;
    int siz=(1<<this->blockBits)*this->assoc*(1<<this->indexBits);
    cout<<"Cache Size (KB per core): "<<((float)siz/1024)<<endl;
    cout<<"MESI PROTOCOL: Enabled"<<endl;
    cout<<"Write Policy: Write back, Write allocate"<<endl;
    cout<<"Replacement Policy: LRU"<<endl;
    cout<<"Bus: Cenetral Snooping Bus"<<endl;
    cout<<endl;
}

template <typename T>
void bus<T>::writeSimulationParameters(string output_file){
    ofstream file(output_file);
    file<<"Simulation Parameters: "<<endl;
    file<<"Set Index Bits: "<<this->indexBits<<endl;
    file<<"Associativity: "<<this->assoc<<endl;
    file<<"Block Bits: "<<this->blockBits<<endl;
    file<<"Block Size (Bytes): "<<(1<<(this->blockBits))<<endl;
    file<<"Number of Sets: "<<(1<<(this->indexBits))<<endl;
    int siz=(1<<this->blockBits)*this->assoc*(1<<this->indexBits);
    file<<"Cache Size (KB per core): "<<((float)siz/1024)<<endl;
    file<<"MESI PROTOCOL: Enabled"<<endl;
    file<<"Write Policy: Write back, Write allocate"<<endl;
    file<<"Replacement Policy: LRU"<<endl;
    file<<"Bus: Cenetral Snooping Bus"<<endl;
    file<<endl;
    file.close();
}
template class bus<unsigned int>;

