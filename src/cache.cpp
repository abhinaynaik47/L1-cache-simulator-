#include "cache.hpp"

template <typename T>
Node<T>* cache<T>::findInMap(map<T,Node<T>*> m,sett<T>* s,unsigned int tag){
    Node<T>* firstGot=nullptr;
    for(auto it=m.begin();it!=m.end();it++){
        int ind=it->second->val2;
        if(it->first==tag){
            if(firstGot==nullptr){
                firstGot=it->second;
            }
            if(s->entries->at(ind)->mode!='I'){
                return it->second;
            }
        }
    }
    return firstGot;
}

template <typename T>
unsigned int cache<T>::extractTag(unsigned int address){
    int blockOffset=static_cast<int>(log2(blockSize));
    int noRows=size/blockSize;
    int noSets=noRows/assoc;
    int indexBits=static_cast<int>(log2(noSets));
    return address>>(indexBits+blockOffset);
}
template <typename T>
void cache<T>::updateLRU(unsigned int address){
    pair<bool,pair<int,int>> isMiss=isReadMiss(address);
    if(isMiss.first){
        cerr<<"Invalid miss function jvdh"<<endl;
        exit(1);
    }
    unsigned int tag=this->extractTag(address);
    int setIndex=isMiss.second.first;
    auto nodeSearch=findInMap(cachelines->at(setIndex)->LRU->table,cachelines->at(setIndex),tag);
    if( cachelines->at(setIndex)->LRU->size!=(int)cachelines->at(setIndex)->LRU->table.size()){
        cerr<<"should not fail"<<endl;
        exit(1);
    }
    int ind=nodeSearch->val2;
    cachelines->at(setIndex)->LRU->table.erase(tag);
    cachelines->at(setIndex)->LRU->deleteNode(nodeSearch);
    auto n=cachelines->at(setIndex)->LRU->addNode(address,ind);
    cachelines->at(setIndex)->LRU->table.insert({tag,n});
    if( cachelines->at(setIndex)->LRU->size!=(int)cachelines->at(setIndex)->LRU->table.size()){
        cerr<<"should not"<<endl;
        exit(1);
    }
}

template <typename T>
unsigned int cache<T>::extractIndex(unsigned int address){
    int blockOffset=static_cast<int>(log2(blockSize));
    int noRows=size/blockSize;
    int noSets=noRows/assoc;
    int indexBits=static_cast<int>(log2(noSets));
    address=address>>(blockOffset);
    address=address<<(32-indexBits);
    address=address>>(32-indexBits);
    return address;
}
template <typename T>
unsigned int cache<T>::extractBlockOffset(unsigned int address){
    int blockOffset=static_cast<int>(log2(blockSize));
    address=address<<(32-blockOffset);
    address=address>>(32-blockOffset);
    return address;
}

template <typename T>
pair<bool,pair<int,int>> cache<T>::isReadMiss(unsigned int address){
    unsigned int tag=extractTag(address);
    unsigned int index=extractIndex(address);
    sett<T>* reqSet=cachelines->at(index);
    int rowNum=0;
    for(row* r:*(reqSet->entries)){
        if(r->tag==tag && (r->mode=='M' || r->mode=='E' || r->mode=='S')){
            return make_pair(false,make_pair(index,rowNum));
        }
        rowNum++;
    }
    return make_pair(true,make_pair(index,rowNum));
}
template <typename T>
pair<bool,pair<int,int>> cache<T>::isWriteMiss(unsigned int address){
    unsigned int tag=extractTag(address);
    unsigned int index=extractIndex(address);
    sett<T>* reqSet=cachelines->at(index);
    int rowNum=0;
    for(row* r:*(reqSet->entries)){
        if(r->tag==tag && (r->mode=='M' || r->mode=='E' || r->mode=='S')){
            return make_pair(false,make_pair(index,rowNum));
        }
        rowNum++;
    }
    return make_pair(true,make_pair(index,0));
}
template <typename T>
void cache<T>::printcache(){
    int i=0;
    for (sett<T>* s:*cachelines){
        cout<<"----------"<<i<<"  -----"<<endl;
        for(row* r:*(s->entries)){
            cout<<"tag:"<<r->tag<<"  "<<"isFirst:"<<r->isFirst<<"  "<<"mode:"<<r->mode<<" "<<"valid:"<<r->valid<<endl;
        }
        i++;
    }
}

template <typename T>
void cache<T>::changeMode(char newMode,unsigned int address){
    auto tag=extractTag(address);
    pair<bool,pair<int,int>> dir=isReadMiss(address);
    if(dir.first){
        cerr<<"Wrond code for missing cache to cache transfer"<<endl;
        exit(1);
    }
    if(cachelines->at(dir.second.first)->LRU->size!=(int)cachelines->at(dir.second.first)->LRU->table.size()){
        cerr<<"Invalid function"<<endl;
        cout<<"hello"<<endl;
        exit(1);
    }
        if(newMode=='I'){
            auto nod=findInMap(cachelines->at(dir.second.first)->LRU->table,cachelines->at(dir.second.first),tag);
            cachelines->at(dir.second.first)->LRU->table.erase(tag);
            auto x=cachelines->at(dir.second.first)->LRU->deleteNode(nod);
            auto b=cachelines->at(dir.second.first)->invaldNodes->addNode(address,x);
            cachelines->at(dir.second.first)->invaldNodes->table.insert({tag,b});
        }
        if(cachelines->at(dir.second.first)->LRU->size!=(int)cachelines->at(dir.second.first)->LRU->table.size()){
            cerr<<"Invalid function"<<endl;
            cout<<"hi"<<endl;
            exit(1);
        }
        bool isFirst=true;
        for(row* r:*cachelines->at(dir.second.first)->entries){
            if(r->tag==tag && r->mode!='I' ){
                if(isFirst){
                    r->mode=newMode;
                    isFirst=false;
                }
                else{
                    cerr<<"error flagged"<<endl;
                    exit(1);
                }
            }
        }
        if(cachelines->at(dir.second.first)->LRU->size!=(int)cachelines->at(dir.second.first)->LRU->table.size()){
            cerr<<"Invalid function"<<endl;
            exit(1);
        }
}

template <typename T>
int cache<T>::evictOne(int index){
    sett<T>* s=cachelines->at(index);
    int ind;
    if(s->invaldNodes->size>0){
        ind=s->invaldNodes->tail->val2;
        auto tag=extractTag(s->invaldNodes->tail->val);
        s->invaldNodes->table.erase(tag);
        s->invaldNodes->deleteNodeEnd();
    }
    else{
        ind=s->LRU->tail->val2;
        auto tag=extractTag(s->LRU->tail->val);
        s->LRU->table.erase(tag);
        s->LRU->deleteNodeEnd();
    }
    return ind;
}

template <typename T>
void cache<T>::addOne(unsigned int address,char mode){
    unsigned int tag=extractTag(address);
    auto isMisss=isReadMiss(address);
    if(!isMisss.first){
        cout<<"another adding wrong"<<endl;
        exit(1);
    }
    unsigned int index=extractIndex(address);
    sett<T>* s=cachelines->at(index);

    auto ind=evictOne(index);

    if(s->LRU->size != (int)s->LRU->table.size()){
        cerr<<"in addNode"<<endl;
        exit(1);
    }
    if(s->entries->at(ind)->mode!='I'){
        evictions++;
    }
    Node<T>* n=s->LRU->addNode(address,ind);
    s->entries->at(ind)->isEmpty=false;
    s->LRU->table.insert({tag,n});
    row* r=cachelines->at(index)->entries->at(ind);
    r->tag=tag;
    r->valid=1;
    r->data=0;
    r->isFirst=0;
    r->mode=mode;
}

template class cache<unsigned int>;
