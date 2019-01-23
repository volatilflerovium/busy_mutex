#include<stack>
#include<iostream>
#include<sstream>
#include<thread>
#include<ctime>
#include<unistd.h>

using namespace std;

//g++ -std=c++11 -pthread stack_mutex.cpp -o stack_mutex
//####################################################################

string num_to_string(int x){
	ostringstream oss;
	oss<<x;
	return oss.str();
}

//####################################################################

class Stack_wrapper
{
	private:
		stack<string>* stk;
		bool available;
		unsigned short id;

	public:
		Stack_wrapper(stack<string>* stk_);
		//Stack_wrapper(const Stack_wrapper& STKW);
		//Stack_wrapper& operator=(const Stack_wrapper& STKW);
		Stack_wrapper()=delete;
		void locking(const unsigned short id_);
		void unlocking(unsigned short id_);
		int size();
		void push(string str);
		const int wait();
		const int signal();
		bool exit;
};

//====================================================================

Stack_wrapper::Stack_wrapper(stack<string>* stk_)
				:stk(stk_), available(true), exit(false), id(0){}

//====================================================================

void Stack_wrapper::locking(const unsigned short id_){
	std::this_thread::sleep_for(std::chrono::microseconds(2));
	while(1){
		if(available){
			available=false;
			id=id_;
			//cout<<"ID: "<<id_<<endl;
		}
		std::this_thread::sleep_for(std::chrono::microseconds(2));
		if(id==id_){
			//cout<<"ID: "<<id_<<"<<---------------"<<endl;
			break;
		}
		else{
			//cout<<"Thread: "<<id_<<" waiting...."<<endl;
			//cout<<"Another thread got first "<<id<<" + "<<id_<<" = = = =  = = = = = = = = = = = = ="<<endl;
		}
	}
}

//====================================================================

void Stack_wrapper::unlocking(unsigned short id_){
	if(id==id_){
		available=true;
	}
	else{
		exit=true;
		cout<<"Corrupted?+ + + + + + + +++++++++++++++++++++++++++++++++++++++++++\n\n\n\n";
	}
}

//====================================================================

void Stack_wrapper::push(string str){
	stk->push(str);
}

//====================================================================

int Stack_wrapper::size(){
	return stk->size();
}


//####################################################################
//####################################################################
//####################################################################
//####################################################################


void do_something(Stack_wrapper* stkw, unsigned short id){
	int size;
	int tid=id;
	while(true){
		stkw->locking(tid);

		stkw->push("Thread: "+num_to_string(id));
		cout<<id<<": "<<stkw->size()<<endl;
		
		size=stkw->size();
		stkw->unlocking(tid);
		if(size>500000 || stkw->exit){
			cout<<"EXIT!!!"<<endl;
			if(stkw->exit){
				cout<<"Corrupted data?"<<endl;
			}
			break;
		}
	}
}


//####################################################################
//####################################################################
//####################################################################

int main(){
	srand(time(0));
	int x=rand()%3;

	stack<string>* stk=new stack<string>;
	stk->push("Hello");
	cout<<stk->size()<<endl;
	
	Stack_wrapper Stk(stk);
	
	std::thread t1(do_something, &Stk, 23);
	std::thread t2(do_something, &Stk, 24);
	std::thread t3(do_something, &Stk, 25);
	std::thread t4(do_something, &Stk, 26);
	t1.join();
	t2.join();
	t3.join();
	t4.join();

	delete stk;

	//########################################

	return 0;	

}