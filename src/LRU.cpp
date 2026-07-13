#include "LRU.hpp"
const char* MyException::what()const noexcept{
    return msg.c_str();
}

template <typename T>
Node<T>* DLL<T>::addNode(T val, int val2) {
    Node<T>* newNode = new Node(val, val2);
    newNode->next = head;
    newNode->prev = nullptr;

    if (head != nullptr) {
        head->prev = newNode;
    }

    head = newNode;

    if (size == 0) {
        tail = newNode;
    }

    size++;

    // table.insert({val, newNode}); // Uncomment if you're using a lookup table

    return newNode;
}

template <typename T>
T DLL<T>::deleteNode(Node<T>* node){
    Node<T>* prev = node->prev;
    Node<T>* next = node->next;

    if (head == node) {
        if (tail == node) {
            head = tail = nullptr;
        } else {
            head = next;
            next->prev = nullptr;
        }
    } else if (tail == node) {
        tail = prev;
        prev->next = nullptr;
    } else {
        prev->next = next;
        next->prev = prev;
    }

    size--;
    T val2 = node->val2;  // or val2, but must match T
    delete node;
    return val2;
}

template <typename T>
T DLL<T>::deleteNodeEnd(){
    if(size==0){
        throw MyException("The DLL is empty cannot delete tail");
    }
    return deleteNode(tail);
}
template <typename T>
void DLL<T>::print(){
    Node<T>* temp=head;
    while(temp!=nullptr){
        cout<<temp->val<<"<->";
        temp=temp->next;
    }
    cout<<endl;
    
}
template class DLL<unsigned int>;
// int main(){
//     DLL<int> r;
//     DLL<int>* t;
//     for(int i=0;i<10;i++){
//         r.addNode(i);
//         r.print();
//     }
//     for(int i=0;i<1;i++){
//         r.deleteNode(r.table.find(1)->second);
//         r.print();
//     }
//     return 0;
// }
