#pragma once 
#include "LRU.hpp"
#include <cmath>
class row{
    public:
        unsigned int tag:31;
        unsigned int valid:1;
        int data;
        unsigned int isFirst:1;
        char mode;
        bool isEmpty;
        row():tag(0),valid(0),data(0),isFirst(0),mode('I'),isEmpty(true){}
};
template <typename T>
class sett{
    public:
        DLL<T>* LRU;
        DLL<T>* invaldNodes;
        vector<row*>* entries;
        int size=0;
        sett(int assoc):LRU(nullptr){
            invaldNodes=new DLL<T>();
            LRU=new DLL<T>();
            entries=new vector<row*>();
            for(int i=0;i<assoc;i++){
                entries->push_back(new row());
            }
            for(int i=0;i<assoc;i++){
                invaldNodes->addNode(0,i);
            }
            // invaldNodes->print();
            // cout<<invaldNodes->size<<endl;
            size=0;
        }
        ~sett(){
            delete entries;
            delete LRU;
            delete invaldNodes;
        }
};
template <typename T>
class cache{
    public:
        int blockSize;
        int size;
        int assoc;
        int evictions;
        vector<sett<T>*>* cachelines;
        cache(int blockBits,int assoc,int indexBits):blockSize(1<<blockBits),size((1<<blockBits)*assoc*(1<<indexBits)),assoc(assoc){
            int numSets=(1<<indexBits);
            cachelines=new vector<sett<T>*>();
            for(int i=0;i<numSets;i++){
                cachelines->push_back(new sett<T>(assoc));
            }
            evictions=0;
        }
        ~cache(){
            delete cachelines;
        }
        pair<bool,pair<int,int>> isReadMiss(unsigned int address);
        pair<bool,pair<int,int>> isWriteMiss(unsigned  int address);
        void printcache();
        int evictOne(int index);
        void addOne(unsigned int address,char mode);
        void updateLRU(unsigned int address);
        unsigned int extractTag(unsigned int address);
        unsigned int extractIndex(unsigned int address);
        unsigned int extractBlockOffset(unsigned int address);
        Node<T>* findInMap(map<T,Node<T>*> m,sett<T>* s,unsigned int tag);
        void changeMode(char newMode,unsigned int address);
};
