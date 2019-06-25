#include <bits/stdc++.h>
using namespace std;

typedef struct process{									//data structure for a process with page table info
	int pid;
	string name;
	int mem;
	int vm;
	vector<int> pt;
	map<int,int> addr;
}process;

int main(int argc,char* argv[]){
	int po=1;
    map<int,process> pid;								//map to store pid with prcoess
    int m,v,p,m1,v1;
    m=stoi(argv[1]);									//main memory size
    v=stoi(argv[2]);									//virtual memory size
    p=stoi(argv[3]);									//page size
    m1=m;
    v1=v;
    vector<bool> mm(m*1024/p,0);						//vector to show availabilty of page frames
    vector<bool> vm(v*1024/p,0);						//vector to show availabilty of page frames
    vector<int> it(m*1024,INT_MIN);						//vector to show value present in the given mem address
    string s;							
    list<int> order;									//to store the order of exectuion
    cout<<"<Command> ";
    while(getline(cin,s)){
        stringstream ss(s);
        string str;
        ss>>str;
        if(str=="exit"){
            break;
        }
        else if(str=="load"){
        	string fn;
            while(ss>>fn){
            	ifstream f;
            	f.open(fn);
            	int size;
            	f>>size;
            	if(m1>=size){							//file gets assigned to main memory
            		cout<<fn<<" is loaded in main memory and is assigned process id "<<po<<endl;
            		order.push_back(po);
            		m1-=size;
					process pr;
					pr.pid=po;
					pr.mem=size;
					pr.vm=1;
					pr.name=fn;
					int i=0,j=0;
					vector<int> pt(size*1024/p);
					while(i<size*1024/p){				//page frames alloction
						if(!mm[j]){
							pt[i++]=j;
							mm[j]=true;
						}
						j++;
					}
					pr.pt=pt;
					pid[po++]=pr;			            		
            	}
            	else if(v1>=size){						//file gets assigned to virtual memory
            		cout<<fn<<" is loaded in virtual memory and is assigned process id "<<po<<endl;
            		order.push_back(po);
            		v1-=size;
					process pr;
					pr.pid=po;
					pr.mem=size;
					pr.vm=0;
					pr.name=fn;
					int i=0,j=0;
					vector<int> pt(size*1024/p);
					while(i<size*1024/p){				//page frames alloctaion
						if(!vm[j]){
							pt[i++]=j;
							vm[j]=true;
						}
						j++;
					}
					pr.pt=pt;
					pid[po++]=pr;
            		
            	}
            	else{
            		cout<<fn<<" could not be loaded"<<endl;
            	}
            }
           
        }
        else if(str=="run"){
            int id;
            ss>>id;
            if(pid.find(id)==pid.end()){
            	cout<<"Wrong pid"<<endl;
            }
            else{
		        process pr=pid[id];
		        if(!pr.vm){								//process in main memory swapin to main memory
		        	while(m1<pr.mem && !order.empty()){
				    	int id1=order.front();
				    	order.erase(order.begin());
				    	process pr1=pid[id1];
				    	for(int i=0;i<pr1.pt.size();i++){
				    		mm[pr1.pt[i]]=false;
				   		}
				    	m1+=pr1.mem;
				    	if(v1>=pr1.mem){
				    		v1-=pr.mem;
				    		pr1.vm=0;
				    		int i=0,j=0;
				    		while(i<pr1.mem*1024/p){
								if(!vm[j]){
									pr1.pt[i++]=j;
									vm[j]=true;
								}
								j++;
							}
							pid[id1]=pr1;
				    	}
				    	else{
				    		cout<<"Virtual Memory is FULL to swapout id "<<id1<<endl;
				    		pid.erase(pid.find(id1));
				    		list<int> temp;
							while(!order.empty()){						//removing process from order
								int t;
								t=order.front();
								order.erase(order.begin());
								if(t!=id){
									temp.push_front(t);
								}
						  	}
						  	order=temp;
				    	}	
		        	}
				    if(m1<pr.mem && order.empty()){
				    	cout<<"Even after all swap outs space in main memory is not suffient"<<endl;
				    	break;
				    }
				    else{
				    	for(int i=0;i<pr.pt.size();i++){
				    		vm[pr.pt[i]]=false;
				    	}
				    	v1+=pr.mem;
						pr.vm=1;
						m1-=pr.mem;
						int i=0,j=0;
						while(i<pr.mem*1024/p){
							if(!mm[j]){
								pr.pt[i++]=j;
								mm[j]=true;
							}
							j++;
						}
						pid[id]=pr;
					}
		        }
		        list<int> temp;
		        while(!order.empty()){
		        	int t;
		        	t=order.front();
		        	order.erase(order.begin());
		        	if(t!=id){
		        		temp.push_front(t);
		        	}
		        }
		        temp.push_front(id);
		        order=temp;
		        ifstream f;
		        f.open(pr.name);
		        int size;
		        f>>size;
		        string op;
		        while(f>>op){
		        	if(op=="add"){
		        		int x,y,z;
		        		f>>x>>y>>z;
		        		if(x>=size*1024 || y>=size*1024 || z>=size*1024){
		        			cout<<"Wrong logical address"<<endl;
		        		}
		        		else{
				    		pr.addr[z]=pr.addr[x]+pr.addr[y];
				    		int paget=z/p;
				    		int pagef=pr.pt[paget];
				    		it[pagef*p+z%p]=pr.addr[z];
		        		}
		        	}
		        	else if(op=="sub"){
		        		int x,y,z;
		        		f>>x>>y>>z;
		        		if(x>=size*1024 || y>=size*1024 || z>=size*1024){
		        			cout<<"Wrong logical address"<<endl;
		        		}
		        		else{
				    		pr.addr[z]=pr.addr[x]-pr.addr[y];
				    		int paget=z/p;
				    		int pagef=pr.pt[paget];
				    		it[pagef*p+z%p]=pr.addr[z];
				    	}
		        	}
		        	else if(op=="print"){
		        		int x;
		        		f>>x;
		        		if(x>=size*1024){
		        			cout<<"Wrong logical address"<<endl;
		        		}
		        		if(pr.addr.find(x)==pr.addr.end()){
		        			cout<<"Nothing is present in that adress"<<endl;	
		        		}
		        		else{
		        			cout<<pr.addr[x]<<endl;
		        		}
		        	}
		        	else if(op=="load"){
		        		int a,y;
		        		f>>a>>y;
		        		if(y>=size*1024){
		        			cout<<"Wrong logical address"<<endl;
		        			continue;
		        		}
		        		pr.addr[y]=a;
		        		int paget=y/p;
		        		int pagef=pr.pt[paget];
		        		it[pagef*p+y%p]=a;
		        	}
		        }
            }
        }
        else if(str=="kill"){
            int id;
            ss>>id;
            if(pid.find(id)==pid.end()){
            	cout<<"Wrong pid"<<endl;
            	continue;
            }
            process pr=pid[id];
            for(int i=0;i<pr.pt.size();i++){			//freeing page frames
            	if(pr.vm){
            		mm[pr.pt[i]]=false;
            	}
            	else{
            		vm[pr.pt[i]]=false;
            	}
            }
            if(pr.vm){
            	m1+=pr.mem;
            }
            else{
            	v1+=pr.mem;
            }
            pid.erase(pid.find(id));
            list<int> temp;
		    while(!order.empty()){						//removing process from order
		        int t;
		        t=order.front();
		        order.erase(order.begin());
		        if(t!=id){
		        	temp.push_front(t);
		        }
		  	}
		  	order=temp;
            cout<<"Process with id: "<<id<<" has been removed"<<endl;
        }
        else if(str=="listpr"){							//printing process ids
            cout<<"Main Memory pids"<<endl;
            for(auto i=pid.begin();i!=pid.end();i++){
            	if((i->second).vm==1){
            		cout<<(i->second).pid<<endl;
            	}
            }
            cout<<"Virtual Memory pids"<<endl;
            for(auto i=pid.begin();i!=pid.end();i++){
            	if((i->second).vm==0){
            		cout<<(i->second).pid<<endl;
            	}
            }
        }
        else if(str=="pte"){							//printing page tables
            int id;
            string fn;
            ss>>id>>fn;
            if(pid.find(id)==pid.end()){
            	cout<<"Wrong pid"<<endl;
            }
            else{
		        process pr=pid[id];
		        ofstream f;
		        f.open(fn,std::ios_base::app);
		        f<<"Page table of file with pid: "<<id<<" which is in ";
		        if(pr.vm){
		        	f<<"Main Memory"<<endl;
		        }
		        else{
		        	f<<"Virtual Memory"<<endl;
		        }
		        f<<"Page number "<<"Page frame"<<endl;
		        for(int i=0;i<pr.pt.size();i++){
		        	f<<i<<"\t"<<pr.pt[i]<<endl;
		        }
            }
        }
        else if(str=="pteall"){
            string fn;
            ss>>fn;
            ofstream f;
            f.open(fn,std::ios_base::app);
            for(auto i=pid.begin();i!=pid.end();i++){
            	process pr=pid[i->first];
            	f<<"Page table of file with pid: "<<i->first<<" which is in ";
		        if(pr.vm){
		        	f<<"Main Memory"<<endl;
		        }
		        else{
		        	f<<"Virtual Memory"<<endl;
		        }
           	    f<<"Page number "<<"Page frame"<<endl;
            	for(int j=0;j<pr.pt.size();j++){
            		f<<j<<"\t"<<pr.pt[j]<<endl;
            	}	
            }
        }
        else if(str=="swapout"){
       		int id;
       		ss>>id;
       		if(pid.find(id)==pid.end()){
            	cout<<"Wrong pid"<<endl;
            }
            else{
		        process pr=pid[id];
		        for(int i=0;i<pr.pt.size();i++){
		        	mm[pr.pt[i]]=false;
		        }
		        m1+=pr.mem;
		        if(v1>=pr.mem){								//swapping to virtual memory if space is presnt
		        	v1-=pr.mem;
		        	pr.vm=0;
		        	int i=0,j=0;
		        	while(i<pr.mem*1024/p){
						if(!vm[j]){
							pr.pt[i++]=j;
							vm[j]=true;
						}
						j++;
					}
					pid[id]=pr;
		        }
		        else{										//killing file as virtual memory is full
		        	cout<<"Virtual Memory is FULL to swapout id "<<id<<endl;
		        	pid.erase(pid.find(id));
		        	list<int> temp;
					while(!order.empty()){						//removing process from order
						int t;
						t=order.front();
						order.erase(order.begin());
						if(t!=id){
							temp.push_front(t);
						}
				  	}
				  	order=temp;
		        }
            }
        }
        else if(str=="swapin"){
            int id;
            ss>>id;
            if(pid.find(id)==pid.end()){
            	cout<<"Wrong pid"<<endl;
            }
            else{
		        process pr=pid[id];
		        while(m1<pr.mem && !order.empty()){			//swapping out files till space is available for file to swapin
		        	int id1=order.front();
		        	order.erase(order.begin());
		        	process pr1=pid[id1];
		        	for(int i=0;i<pr1.pt.size();i++){
		        		mm[pr1.pt[i]]=false;
		       		}
		        	m1+=pr1.mem;
		        	if(v1>=pr1.mem){
		        		v1-=pr.mem;
		        		pr1.vm=0;
		        		int i=0,j=0;
		        		while(i<pr1.mem*1024/p){			//page fram allocation
							if(!vm[j]){
								pr1.pt[i++]=j;
								vm[j]=true;
							}
							j++;
						}
						pid[id1]=pr1;
		        	}
		        	else{
		        		cout<<"Virtual Memory is FULL to swapout id "<<id1<<endl;
		        		pid.erase(pid.find(id1));
		        		list<int> temp;
						while(!order.empty()){						//removing process from order
							int t;
							t=order.front();
							order.erase(order.begin());
							if(t!=id){
								temp.push_front(t);
							}
					  	}
					  	order=temp;
		        	}	
            	}
		        if(m1<pr.mem && order.empty()){
		        	cout<<"Even after all swap outs space in main memory is not suffient"<<endl;
		        }
		        else{
		        	for(int i=0;i<pr.pt.size();i++){
		        		vm[pr.pt[i]]=false;
		        	}
		        	v1+=pr.mem;
				    pr.vm=1;
				    m1-=pr.mem;
				    int i=0,j=0;
				    while(i<pr.mem*1024/p){
						if(!mm[j]){
							pr.pt[i++]=j;
							mm[j]=true;
						}
						j++;
					}
					pid[id]=pr;
				}
			}
        }
        else if(str=="print"){
            int st,n;
            ss>>st>>n;
            for(int i=st;i<st+n;i++){
            	if(it[i]!=INT_MIN){
            		cout<<"Value of "<<i<<": "<<it[i]<<endl;
            	}
            	else{
            		cout<<"Value of "<<i<<": Nothing is present"<<endl;
            	}
            }
        }
        cout<<"<Command> ";
    }
}
//load file1.txt file2.txt file3.txt file4.txt
