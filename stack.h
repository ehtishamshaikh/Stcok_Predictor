#ifndef STACK_H
#define STACK_H
#include<iostream>
#include<sstream>
using namespace std;


template<class T>
class Stack{
    private:
    int size;
    T *items;
    int top;

    public:
    Stack()
    {
        size=50;
        top=-1;
        items=new T[50];
    }
    Stack(int t)
    {
        size=t;
        top=-1;
        items=new T[size];
    }
    bool is_empty()
    {
        if(top==-1)
        {
            return true;
        }
        else
        {
            return false;
        }
        
    }
    void empty_the_stack()
    {
        top=-1;
    }
    bool is_full()
    {
        if(top==size-1)
        {
            return true;
        }
        else
        {
            return false;
        }
        
    }
    void push(T x)
    {
        if(is_full())
        {
            cout<<"Stack Overflow!"<<endl;
        }
        else
        {
            items[++top]=x;
        }
         
    }
    T pop()
    {
        if(is_empty())
        {
            cout<<"Stack Empty!"<<endl;
        }
        else
        {
            return items[top--];
        }
        
    }
    void display()
    {
        if(is_empty())
        {
            cout<<"Stack Empty!"<<endl;
        }
        else
        {
            for (int i =top ; i >=0 ; i--)
            {
                cout<<items[i]<<" ";
            }
            cout<<endl;
            
        }
        
    }


};
#endif