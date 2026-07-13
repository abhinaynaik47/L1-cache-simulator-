#pragma once
#include <iostream>
#include <map>
#include <exception>
#include <string>
#include <vector>
using namespace std;
class MyException:exception{
    string msg;
    public:
        MyException(const string& msg):msg(msg){}
        const char* what() const noexcept override;
};
template <typename T>
struct Node{
    T val;
    int val2;
    Node<T>* next;
    Node<T>* prev;
    Node(T val,int val2):val(val),val2(val2),next(nullptr),prev(nullptr){}
    Node(T val,Node<T>* next,Node<T>* prev,int val2):val(val),val2(val2),next(next),prev(prev){}
    Node():val(0),val2(0),next(nullptr),prev(nullptr){}
};
template <typename T>
class DLL{
    public:
        Node<T>* head;
        Node<T>* tail;
        map<T,Node<T>*> table;
        int size;
        DLL():head(nullptr),tail(nullptr),size(0){}
        ~DLL(){
            Node<T>* temp=this->head;
            while(temp!=nullptr){
                Node<T>* nextNode=temp->next;
                temp->next=nullptr;
                if(nextNode!=nullptr){
                    nextNode->prev=nullptr;
                }
                delete temp;
                temp=nextNode;
            }
            head=nullptr;
            tail=nullptr;
        
        }
        Node<T>* addNode(T val,int val2);
        T deleteNode(Node<T>* node);
        T deleteNodeEnd();
        void print();
};
