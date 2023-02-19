#include<iostream>
#include<windows.h>
#include<fstream>
#include<sstream>
#include"node.h"
#include"stack.h"
#include"DLL.h"
#include"graphs.h"

using namespace std;


class StockPredictor:public DoubleLinked_list<node>,public Stack<node*>
{

    private:
        Graph graph;
        Stack<Ema> stack;
        float pnl;
        int vol;


        void calculate_ShortEma()
        {
            node *temp=tail;
            //Stack<Ema> stack;
            Ema temp1;
            float x=0.0;
            temp=find_ema(x,temp,temp->short_ema.days);
            temp->short_ema.data=x;
            stack.push(temp->short_ema);
            temp=temp->prev;
            while(temp!=NULL)
            {
                temp1=stack.pop();
                x=2.0/(temp1.days+1);
                temp->short_ema.data=(temp->price*x)+(temp1.data*(1.0-x));
                stack.push(temp->short_ema);
                //cout<<temp->price<<" "<<temp->short_ema.data<<" "<<temp->short_ema.days<<endl;
                temp=temp->prev;
            }
            stack.empty_the_stack();

        }
        void calculate_MediumEma()
        {
            node *temp=tail;
            //Stack<Ema> stack;
            Ema temp1;
            float  x=0.0;
            temp=find_ema(x,temp,temp->medium_ema.days);
            temp->medium_ema.data=x;
            temp->macd.ema_26.data=temp->medium_ema.data;

            stack.push(temp->medium_ema);
            temp=temp->prev;
            while(temp!=NULL)
            {
                temp1=stack.pop();
                x=2.0/(temp1.days+1);
                temp->medium_ema.data=(temp->price*x)+(temp1.data*(1.0-x));
                temp->macd.ema_26.data=temp->medium_ema.data;
                stack.push(temp->medium_ema);
                //cout<<temp->price<<" "<<temp->medium_ema.data<<" "<<temp->medium_ema.days<<endl;
                temp=temp->prev;
            }
            stack.empty_the_stack();

        }

        node* find_ema(float &avg, node* temp,int days)
        {
            for (int i = 0; i < days ; i++, temp=temp->prev)
            {
                avg=avg+temp->price;
            }
            //float x=days;
            avg=avg/days;
            temp=temp->next;
            return temp;
        }
        void calculate_LongEma()
        {
            node *temp=tail;
            //Stack<Ema> stack;
            Ema temp1;
            float x=0.0;
            temp=find_ema(x,temp,temp->long_ema.days);
            temp->long_ema.data=x;

            stack.push(temp->long_ema);
            temp=temp->prev;
            while(temp!=NULL)
            {
                temp1=stack.pop();
                x=2.0/(temp1.days+1);
                temp->long_ema.data=(temp->price*x)+(temp1.data*(1.0-x));
                stack.push(temp->long_ema);
                //cout<<temp->price<<" "<<temp->long_ema.data<<" "<<temp->long_ema.days<<endl;
                temp=temp->prev;
            }
            stack.empty_the_stack();

        }
        
        void calculate_macd()
        {
            node *temp=tail;
            //Stack<Ema> stack;
            float x=0.0;
            Ema temp1;

            //Calculating macd 12 ema
            temp=find_ema(x,temp,12);
            temp->macd.ema_12.data=x;
            stack.push(temp->macd.ema_12);
            temp=temp->prev;

            while(temp!=NULL)
            {
                temp1=stack.pop();
                x=2.0/(temp1.days+1);
                temp->macd.ema_12.data=(temp->price*x)+(temp1.data*(1.0-x));
                stack.push(temp->macd.ema_12);
                //cout<<temp->price<<" "<<temp->short_ema.data<<" "<<temp->short_ema.days<<endl;
                temp=temp->prev;
            }
            stack.empty_the_stack();

            //Calculating macd line
            temp=tail;
            while(temp!=NULL)
            {
                if(temp->macd.ema_12.data!=0 && temp->macd.ema_26.data!=0)
                {
                    temp->macd.macd_line.data=temp->macd.ema_12.data - temp->macd.ema_26.data;
                }
                temp=temp->prev;
            }

            //Calculating macd signal line
            temp=tail;
            float avg=0.0;
            for (int i = 0; i < 9 ;  temp=temp->prev)
            {
                avg=avg+temp->macd.macd_line.data;
                if(temp->macd.macd_line.data!=0)
                {
                    i++;
                }
            }
            //float x=days;
            avg=avg/9;
            temp=temp->next;
            temp->macd.signal_line.data=avg;
            stack.push(temp->macd.signal_line);
            temp=temp->prev;
            x=2.0/(10.0);
            while (temp!=NULL)
            {
                temp1=stack.pop();
                temp->macd.signal_line.data=(temp->macd.macd_line.data*x)+((temp1.data*(1-x)));
                stack.push(temp->macd.signal_line);
                temp=temp->prev;
            }
            stack.empty_the_stack();

            //Calculating macd histogram
            temp=tail;
            while(temp!=NULL)
            {
                if(temp->macd.macd_line.data!=0 && temp->macd.signal_line.data!=0)
                {
                    temp->macd.Histogram=temp->macd.macd_line.data - temp->macd.signal_line.data;
                }
                temp=temp->prev;
            }

        }
        void generate_signal_Sema_Lema(int x,int y)
        {
            node *temp=tail;
            node *tmp;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc|ios::out);
            //out<<"Date\tBUY\tSEll\tProfit/loss @1000 shares"<<endl;
            while(temp!=NULL)
            {
                if(temp->price < temp->short_ema.data)
                {
                    if(!is_empty())
                    {
                        tmp=pop();
                        out<<temp->date<<"\t\t";
                        out<<temp->price<<"\t\t";
                        float sprice=0.0;
                        sprice=tmp->price;
                        sprice=temp->price - tmp->price;
                        pnl=pnl+sprice;
                        out<<sprice*vol<<"       "<<endl;
                        if(sprice<0)
                        {
                            graph.matrix[x][y]-=0.10*sprice;
                        }
                        else if(sprice>0)
                        {
                            graph.matrix[x][y]+=0.10*sprice;
                        }

                    }
                }
                else if(temp->short_ema.data > temp->long_ema.data )
                {
                    if(is_empty())
                    {
                        push(temp);
                        out<<temp->date<<"\t";
                        out<<temp->price<<"\t"<<endl;
                    }
                    
                }
                
                temp=temp->prev;
            }
            empty_the_stack();
            //out<<endl<<"Net P/L=\t\t\t\t\t"<<pnl*vol<<endl;
            out.close();
            //pnl=0.0;
        }
        void generate_signal_Sema_Macd(int x,int y)
        {
            node *temp=tail;
            node *tmp;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc|ios::out);
            //cout<<"Date\t\tBUY\tSEll\tProfit/loss @1000 shares"<<endl;
            while(temp!=NULL)
            {
                if( (temp->price > temp->short_ema.data) && (temp->macd.macd_line.data > temp->macd.signal_line.data)  )
                {
                    if(is_empty())
                    {
                        push(temp);
                        out<<temp->date<<"\t";
                        out<<temp->price<<"\t"<<endl;
                    }
                    
                }
                else if((temp->price < temp->short_ema.data) && (temp->macd.macd_line.data < temp->macd.signal_line.data) )
                {
                    if(!is_empty())
                    {
                        tmp=pop();
                        out<<temp->date<<"\t\t";
                        out<<temp->price<<"\t\t";
                        float sprice=0.0;
                        sprice=tmp->price;
                        sprice=temp->price - tmp->price;
                        pnl=pnl+sprice;
                        out<<sprice*vol<<"       "<<endl;
                        if(sprice<0)
                        {
                            graph.matrix[x][y]-=0.10*sprice;
                        }
                        else if(sprice>0)
                        {
                            graph.matrix[x][y]+=0.10*sprice;
                        }

                    }
                }
                temp=temp->prev;
            }
            empty_the_stack();
            out.close();
            //cout<<endl<<"Net P/L="<<pnl*vol<<endl;
            //pnl=0.0;
        }
        void generate_signal_Lema_Mema(int x,int y)
        {
            node *temp=tail;
            node *tmp;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc|ios::out);
            //cout<<"Date\t\tBUY\tSEll\tProfit/loss @1000 shares"<<endl;
            while(temp!=NULL)
            {
                if(temp->price < temp->medium_ema.data)
                {
                    if(!is_empty())
                    {
                        tmp=pop();
                        out<<temp->date<<"\t\t";
                        out<<temp->price<<"\t\t";
                        float sprice=0.0;
                        sprice=tmp->price;
                        sprice=temp->price - tmp->price;
                        pnl=pnl+sprice;
                        out<<sprice*vol<<"       "<<endl;
                        if(sprice<0)
                        {
                            graph.matrix[x][y]-=0.10*sprice;
                        }
                        else if(sprice>0)
                        {
                            graph.matrix[x][y]+=0.10*sprice;
                        }

                    }
                }
                else if(temp->medium_ema.data > temp->long_ema.data )
                {
                    if(is_empty())
                    {
                        push(temp);
                        out<<temp->date<<"\t";
                        out<<temp->price<<"\t"<<endl;
                    }
                    
                }
                
                temp=temp->prev;
            }
            empty_the_stack();
            out.close();
            //cout<<endl<<"Net P/L="<<pnl*vol<<endl;
            //pnl=0.0;
        }
        void generate_signal_Macd_Lema(int x,int y)
        {
            node *temp=tail;
            node *tmp;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc|ios::out);
            //cout<<"Date\t\tBUY\tSEll\tProfit/loss @1000 shares"<<endl;
            while(temp!=NULL)
            {
                if( (temp->price > temp->long_ema.data) && (temp->macd.macd_line.data > temp->macd.signal_line.data)  )
                {
                    if(is_empty())
                    {
                        push(temp);
                        out<<temp->date<<"\t";
                        out<<temp->price<<"\t"<<endl;
                    }
                    
                }
                else if((temp->price < temp->long_ema.data) && (temp->macd.macd_line.data < temp->macd.signal_line.data) )
                {
                    if(!is_empty())
                    {
                        tmp=pop();
                        out<<temp->date<<"\t\t";
                        out<<temp->price<<"\t\t";
                        float sprice=0.0;
                        sprice=tmp->price;
                        sprice=temp->price - tmp->price;
                        pnl=pnl+sprice;
                        out<<sprice*vol<<"       "<<endl;
                        if(sprice<0)
                        {
                            graph.matrix[x][y]-=0.10*sprice;
                        }
                        else if(sprice>0)
                        {
                            graph.matrix[x][y]+=0.10*sprice;
                        }

                    }
                }
                temp=temp->prev;
            }
            empty_the_stack();
            out.close();
            //cout<<endl<<"Net P/L="<<pnl*vol<<endl;
            //pnl=0.0;
        }
        void generate_signal_Macd_Mema(int x,int y)
        {
           node *temp=tail;
            node *tmp;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc|ios::out);
            //cout<<"Date\t\tBUY\tSEll\tProfit/loss @1000 shares"<<endl;
            while(temp!=NULL)
            {
                if( (temp->price > temp->medium_ema.data) && (temp->macd.macd_line.data > temp->macd.signal_line.data) )
                {
                    if(is_empty())
                    {
                        push(temp);
                        out<<temp->date<<"\t";
                        out<<temp->price<<"\t"<<endl;
                    }
                    
                }
                else if((temp->price < temp->medium_ema.data) && (temp->macd.macd_line.data < temp->macd.signal_line.data) )
                {
                    if(!is_empty())
                    {
                        tmp=pop();
                        out<<temp->date<<"\t\t";
                        out<<temp->price<<"\t\t";
                        float sprice=0.0;
                        sprice=tmp->price;
                        sprice=temp->price - tmp->price;
                        pnl=pnl+sprice;
                        out<<sprice*vol<<"       "<<endl;
                        if(sprice<0)
                        {
                            graph.matrix[x][y]-=0.10*sprice;
                        }
                        else if(sprice>0)
                        {
                            graph.matrix[x][y]+=0.10*sprice;
                        }

                    }
                }
                temp=temp->prev;
            }
            empty_the_stack();
            out.close();
            //cout<<endl<<"Net P/L="<<pnl*vol<<endl;
            //pnl=0.0; 
        }
        void generate_signal_Mema_Sema(int x,int y)
        {

            node *temp=tail;
            node *tmp;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc|ios::out);
            //cout<<"Date\t\tBUY\tSEll\tProfit/loss @1000 shares"<<endl;
            while(temp!=NULL)
            {
                if( temp->price < temp->short_ema.data )
                {
                	
                    if(!is_empty())
                    {
                        tmp=pop();
                        out<<temp->date<<"\t\t";
                        out<<temp->price<<"\t\t";
                        float sprice=0.0;
                        sprice=tmp->price;
                        sprice=temp->price - tmp->price;
                        pnl=pnl+sprice;
                        out<<sprice*vol<<"       "<<endl;
                        if(sprice<0)
                        {
                            graph.matrix[x][y]-=0.10*sprice;
                        }
                        else if(sprice>0)
                        {
                            graph.matrix[x][y]+=0.10*sprice;
                        }

                    }
                }
                else if(temp->short_ema.data > temp->medium_ema.data )
                {
                	
                    if(is_empty())
                    {
                        push(temp);
                        out<<temp->date<<"\t";
                        out<<temp->price<<"\t"<<endl;
                    }
                    
                }
                
                temp=temp->prev;
            }

            empty_the_stack();
            out.close();
            //cout<<endl<<"Net P/L="<<pnl*vol<<endl;
            //pnl=0.0;
        }
        void generate_signal_Sema(int x, int y)
        {
            node *temp=tail;
            node *tmp;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc|ios::out);
            //cout<<"Date       BUY         SEll        Profit/loss @1000 shares"<<endl;
            while(temp!=NULL)
            {
                if(temp->price > temp->short_ema.data)
                {
                    if(is_empty())
                    {
                        push(temp);
                        out<<temp->date<<"\t";
                        out<<temp->price<<"\t"<<endl;
                    }
                    
                }
                else if(temp->price < temp->short_ema.data)
                {
                    if(!is_empty())
                    {
                        tmp=pop();
                        out<<temp->date<<"\t\t";
                        out<<temp->price<<"\t\t";
                        float sprice=0.0;
                        sprice=tmp->price;
                        sprice=temp->price - tmp->price;
                        pnl=pnl+sprice;
                        out<<sprice*vol<<" "<<endl;
                        if(sprice<0)
                        {
                            graph.matrix[x][y]-=0.10*sprice;
                        }
                        else if(sprice>0)
                        {
                            graph.matrix[x][y]+=0.10*sprice;
                        }

                    }
                }
                temp=temp->prev;
            }
            empty_the_stack();
            out.close();
            //cout<<endl<<"Net P/L="<<pnl*vol<<endl;
            //pnl=0;
        }
        void generate_signal_Mema(int x, int y)
        {
            node *temp=tail;
            node *tmp;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc|ios::out);
            //cout<<"Date       BUY         SEll        Profit/loss"<<endl;
            while(temp!=NULL)
            {
                if(temp->price > temp->medium_ema.data)
                {
                    if(is_empty())
                    {
                        push(temp);
                        out<<temp->date<<"\t";
                        out<<temp->price<<"\t"<<endl;
                    }
                    
                }
                else if(temp->price < temp->medium_ema.data)
                {
                    if(!is_empty())
                    {
                        tmp=pop();
                        out<<temp->date<<"\t\t";
                        out<<temp->price<<"\t\t";
                        float sprice=0.0;
                        sprice=tmp->price;
                        sprice=temp->price - tmp->price;
                        pnl=pnl+sprice;
                        out<<sprice*vol<<"       "<<endl;
                        if(sprice<0)
                        {
                            graph.matrix[x][y]-=0.10*sprice;
                        }
                        else if(sprice>0)
                        {
                            graph.matrix[x][y]+=0.10*sprice;
                        }
                    }
                }
                temp=temp->prev;
            }
            //cout<<"Net P/L="<<pnl<<endl;
            empty_the_stack();
            out.close();
            //pnl=0;
        }
        void generate_signal_Lema(int x, int y)
        {
            node *temp=tail;
            node *tmp;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc|ios::out);
            //cout<<"Date       BUY         SEll        Profit/loss"<<endl;
            while(temp!=NULL)
            {
                if(temp->price > temp->long_ema.data)
                {
                    if(is_empty())
                    {
                        push(temp);
                        out<<temp->date<<"\t";
                        out<<temp->price<<"\t"<<endl;
                    }
                    
                }
                else if(temp->price < temp->long_ema.data)
                {
                    if(!is_empty())
                    {
                        tmp=pop();
                        out<<temp->date<<"\t\t";
                        out<<temp->price<<"\t\t";
                        float sprice=0.0;
                        sprice=tmp->price;
                        sprice=temp->price - tmp->price;
                        pnl=pnl+sprice;
                        out<<sprice*vol<<"       "<<endl;
                        if(sprice<0)
                        {
                            graph.matrix[x][y]-=0.10*sprice;
                        }
                        else if(sprice>0)
                        {
                            graph.matrix[x][y]+=0.10*sprice;
                        }

                    }
                }
                temp=temp->prev;
            }
            //out<<"Net P/L="<<pnl<<endl;
            empty_the_stack();
            out.close();
            //pnl=0;
        }
        void generate_signal_macd(int x, int y)
        {
            node *temp=tail;
            node *tmp;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc|ios::out);
            //cout<<"Date       BUY         SEll        Profit/loss @1000 shares"<<endl;
            while(temp!=NULL)
            {
                if( (temp->macd.macd_line.data > temp->macd.signal_line.data) && (temp->macd.Histogram > 0) )
                {
                    if(is_empty())
                    {
                        push(temp);
                        out<<temp->date<<"\t";
                        out<<temp->price<<"\t"<<endl;
                    }
                    
                }
                else if( (temp->macd.macd_line.data < temp->macd.signal_line.data) && (temp->macd.Histogram < 0) )
                {
                    if(!is_empty())
                    {
                        tmp=pop();
                        out<<temp->date<<"\t\t";
                        out<<temp->price<<"\t\t";
                        float sprice=0.0;
                        sprice=tmp->price;
                        sprice=temp->price - tmp->price;
                        pnl=pnl+sprice;
                        out<<sprice*vol<<" "<<endl;
                        if(sprice<0)
                        {
                            graph.matrix[x][y]-=0.10*sprice;
                        }
                        else if(sprice>0)
                        {
                            graph.matrix[x][y]+=0.10*sprice;
                        }

                    }
                }
                temp=temp->prev;
            }
            empty_the_stack();
            //cout<<endl<<"Net P/L="<<pnl*vol<<endl;
            //out.close();
        }

    public:
        StockPredictor():graph(4)
        {
            //root=NULL;
            vol=1000;
            pnl=0.0;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc);
            out.close();
            
        }
        void reset_training_model()
        {
            graph.reset_graph();
        }
        void print_graph()
        {
            graph.print_matrix();
        }
        void get_max_weighted_edge(int &x, int &y)
        {
            int max=graph.matrix[0][0];
            int cnt=graph.size;
            for(int i=0; i<cnt; i++)
            {
                for (int j = 0; j < cnt; j++)
                {
                    if(max<graph.matrix[i][j])
                    {
                        max=graph.matrix[i][j];
                        x=i;
                        y=j;
                    }
                }
                
            }

        }
        void model_training()
        {
            int x=graph.size;
            graph.reset_graph();
            for (int i = 0; i < x; i++)
            {
                for (int j = 0; j < x; j++)
                {
                    ofstream out;
                    out.open("MyLedger.xls",ios::trunc);
                    out.close();
                    call_indicator(i,j);
                    // read_ledger();
                    // cout<<endl;
                    print_graph();
                    cout<<endl;
                    out.open("MyLedger.xls",ios::trunc);
                    out.close();
                    pnl=0.0;
                    system("pause");
                    //print_graph();
                }
                
            }
            graph.write_graph_int_file();
            
        }
        void call_indicator(int x,int y)
        {
            if(x==0 && y==1)
            {
                generate_signal_Sema_Lema(x,y);
            }
            else if(x==0 && y==3)
            {
                generate_signal_Sema_Macd(x,y);
            }
            else if(x==1 && y==2)
            {
                generate_signal_Lema_Mema(x,y);
            }
            else if(x==2 && y==0 )
            {
                generate_signal_Mema_Sema(x,y);
            }
            else if(x==3 && y==1)
            {
                generate_signal_Macd_Lema(x,y);
            }
            else if(x==3 && y==2)
            {
                generate_signal_Macd_Mema(x,y);
            }
            else if(x==0 && y==0)
            {
                generate_signal_Sema(x,y);
            }
            else if(x==1 && y==1)
            {
                generate_signal_Lema(x,y);
            }
            else if(x==2 && y==2)
            {
                generate_signal_Mema(x,y);
            }
            else if(x==3 && y==3)
            {
                generate_signal_macd(x,y);
            }
            
        }
        void Read_from_file(string tmp)
        {
            tmp=tmp+".txt";
            ifstream in;
            in.open(tmp,ios::in);
            if(in.is_open())
            {
                float price=0.0;
                string date="\0";
                while(!in.eof())
                {
                    node* temp=new node;
                    in>>date;
                    in>>price;
                    temp->date=date;
                    temp->price=price;
                    add_node(temp);
                    //cout<<temp;
                    
                }
                
            }
            else
            {
                cout<<"File didnot open!"<<endl;
            }
            tail=tail->prev;
            tail->next=NULL;
            in.close();
            calculate_ShortEma();
            calculate_MediumEma();
            calculate_LongEma();
            calculate_macd();
        }
        void prediction_best_result()
        {
            int x=0,y=0;
            graph.read_graph_from_file();
            pnl=0.0;
            ofstream out;
            out.open("MyLedger.xls",ios::trunc);
            out.close();
            get_max_weighted_edge(x,y);
            call_indicator(x,y);
            read_ledger();
            
            
        }
        void read_ledger()
        {
            ifstream in;
            string str;
            in.open("MyLedger.xls",ios::in);
            cout<<"Date\t\tBUY\tSEll\tProfit/loss @1000 shares"<<endl;
            if(in.is_open())
            {
                while(!in.eof())
                {
                    getline(in,str);
                    cout<<str<<endl;
                }
                cout<<endl<<"Net P/L=\t\t\t\t"<<pnl*1000<<endl;
            }
            else
            {
                cout<<"File Didnot open!"<<endl;
            }
            in.close();
        }

        void traverse_data()
        {
            node* temp=head;
            while (temp!=NULL)
            {
                cout<<temp;
                temp=temp->next;
            }
            
        }
        void traverse_data_end()
        {
            node* temp=tail;
            while (temp!=NULL)
            {
                cout<<temp;
                temp=temp->prev;
            }
        }
        
        
        
        

        
        
};

int main()
{
    
    char ch='\0';
    string str;
    
    while(1)
    {
        StockPredictor s1;
        system("cls");
        cout<<"\t\t\tSTOCK PREDICTOR"<<endl;
        cout<<"\t\t\t---------------"<<endl<<endl;

        cout<<"1:Enter the stock symbol:"<<endl;
        cout<<"2:Exit:"<<endl<<endl;
        cin>>str;
        if(str=="2")
        {
            exit(1);
        }
        else if(str=="1")
        {
            cout<<"Symbol:";
            cin>>str;
            
            s1.Read_from_file(str);
        }
        else
        {
            cout<<"Invalid Option!"<<endl;
            break;
        }
        
        while(1)
        {
            system("cls");
            cout<<"\t\t\tSTOCK PREDICTOR"<<endl;
            cout<<"\t\t\t---------------"<<endl<<endl;
            cout<<"1:Get prediction result:"<<endl;
            cout<<"2:Train the model:"<<endl;
            cout<<"3:Print my trading ledger:"<<endl;
            cout<<"4:Select another symbol:"<<endl;
            cout<<"5:Exit:"<<endl<<endl;
            cin>>ch;
            if(ch=='4')
            {
                s1.reset_training_model();
                break;
            }
            else if(ch=='5')
            {
                exit(1);
            }
            else if(ch=='1')
            {
                s1.prediction_best_result();
                system("pause");
            }
            else if(ch=='2')
            {
                s1.model_training();
                system("pause");

            }
            else if(ch=='3')
            {
                s1.read_ledger();
                system("pause");
            }
            else
            {
                cout<<"Invalid Symbol!"<<endl;
                system("pause");
            }
            
        }
        
    }

}
