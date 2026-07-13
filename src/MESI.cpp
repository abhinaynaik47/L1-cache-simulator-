#include "MESI.hpp"
#include <chrono>
#include <thread>


template <typename T>
void process<T>::simulate(string outputFile){
    bool areAllDone=false;
    // int i=0;
    while(!areAllDone){
        // cout<<i<<endl;
        // i++;
        // if(i==122218){
        //     cout<<"hello"<<endl;
        // }
        // cout<<"i is "<<i<<endl;
        areAllDone=true;
        buss->kick();
        for(core<T>* cor:*(buss->allCores)){
            if(!(cor->isOver)){
                cor->kick(buss->allCores);
                areAllDone=false;
            }
        }
        // if(i>98){
        // std::cout << "Sleeping for 2 seconds...\n";
        // std::this_thread::sleep_for(std::chrono::seconds(2));
        // }
    }
    buss->printSimulationParameters();
    buss->writeSimulationParameters(outputFile);
    int maxCycle=0;
    for(core<T>* cor:*(buss->allCores)){
        cor->printInfo();
        maxCycle=max(maxCycle,cor->totalExcutionCycles);
        cor->writeInfo(outputFile);
    }
    ofstream file(outputFile,ios::app);
    cout<<"Overall Bus Summary: "<<endl;
    file<<"Overall Bus Summary: "<<endl;
    cout<<"Last Cycle: "<<maxCycle<<endl;
    file<<"Last Cycle: "<<maxCycle<<endl;
    cout<<"Total Bus Transactions: "<<buss->transactions<<endl;
    file<<"Total Bus Transactions: "<<buss->transactions<<endl;
    int totaltraffic=0;
    for(core<T>* cor:*(buss->allCores)){
        totaltraffic+=cor->dataTraffic * cor->cach->blockSize;
    }
    cout<<"Total Bus Traffic(in Bytes): "<<totaltraffic<<endl;
    file<<"Total Bus Traffic(in Bytes): "<<totaltraffic;
    file.close();
}
// template class bus<unsigned int>;
// template class cache<unsigned int>;
// template class core<unsigned int>;
template class process<unsigned int>;

int main(int argc,char* argv[]){
    string file="";
    int indexBits=-1;
    int associativity=-1;
    int blockBits=-1;
    string outputFile="";
    for(int i=1;i<argc;i++){
        string arg=argv[i];
        if(arg=="-t"){
            file=argv[++i];
        }
        else if(arg=="-s"){
            indexBits=atoi(argv[++i]);
        }
        else if(arg=="-E"){
            associativity=atoi(argv[++i]);
        }
        else if(arg=="-b"){
            blockBits=atoi(argv[++i]);
        }
        else if(arg=="-o"){
            outputFile=argv[++i];
        }
        else if(arg=="-h"){
            cout<<"Input format is follws"<<endl;
            cout<<"-t <tracefile>: name of parallel application (e.g. app1) whose 4 traces are to be used in simulation"<<endl;
            cout<<"-s <s>: number of set index bits"<<endl;
            cout<<"-E <E>: associativity (number of cache lines per set)"<<endl;
            cout<<"-b <b>: number of block bits"<<endl;
            cout<<"-o: <outfilename> logs output in file for plotting etc."<<endl;
            cout<<"-h: prints this help"<<endl;
        }
        else{
            cerr<<"invalid input"<<endl;
            exit(1);
        }
    }
    if(file=="" || indexBits<0 || associativity <0 || blockBits<0 ){
        cerr<<"Invalid commmand line inputs"<<endl;
        exit(0);
    }
    // cout<<"file is "<<file<<endl;
    // cout<<"number of set bits are  "<<indexBits<<endl;
    // cout<<"associativity is "<<associativity<<endl;
    // cout<<"number of block bits are "<<blockBits<<endl;
    // cout<<"output file is "<<outputFile<<endl;
    process<unsigned int>* p=new process<unsigned int>(blockBits,associativity,indexBits,file,4);
    p->simulate(outputFile);
    cout<<"----------------------"<<endl;
    delete p;
    return 0;
}
