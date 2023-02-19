#ifndef NODE_H
#define NODE_H
#include<iostream>
#include"DLL.h"
using namespace std;

struct Ema
{
    float data;
    int days;
    
    Ema()
    {
        data=0.0;
        days=0;
    }
    Ema(int x)
    {
        data=0.0;
        days=x;
    }
    void set_ema_day(int x)
    {
        days=x;
    }

};

struct Macd
{
    struct Ema ema_12;
    struct Ema ema_26;
    struct Ema macd_line;
    struct Ema signal_line;
    float Histogram;
    Macd(int x, int y):ema_12(x),ema_26(y)
    {
        Histogram=0.0;
    }

};

class node
{
    public:
    float price;
    string date;
    node* next;
    node* prev;
    Ema short_ema;
    Ema medium_ema;
    Ema long_ema;
    Macd macd;

    node():short_ema(9),medium_ema(26),long_ema(50),macd(12,26)
    {
        price=0.0;
        date="\0";
        next=NULL;
        prev=NULL;
    }
    friend istream&operator>>(istream& input,node &n)
    {
        input>>n.date;
        input>>n.price;  
    }
    friend ostream&operator<<(ostream &out,node *n)
    {
        out<<n->date<<" ";
        out<<n->price<<" ";
        out<<n->short_ema.data<<" ";
        out<<n->medium_ema.data<<" ";
        out<<n->long_ema.data<<" ";
        // out<<n->macd.ema_12.data<<" ";
        // out<<n->macd.ema_26.data<<" ";
        // out<<n->macd.signal_line.data<<" ";
        // out<<n->macd.macd_line.data<<" ";
        // out<<n->macd.Histogram<<endl;
        out<<endl;
       
        
    }

};

#endif