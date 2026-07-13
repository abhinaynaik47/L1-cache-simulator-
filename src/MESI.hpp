#pragma once
#include "bus.hpp"
template <typename T>
class process{
    public:
        bus<T>* buss;
        process(int blockBits,int assoc,int indexBits,string trace,int coreN=4){
            buss=new bus<T>(blockBits,assoc,indexBits,trace,coreN);
        }
        // ~process(){
        //     delete buss;
        // }
        void simulate(string outputFile);
};
