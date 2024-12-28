#include<iostream>


using namespace std;

void printString(const string & str){
    cout<<str<<endl;
}

int main(){
    int a = 10;
    int & ref=a;

    string s = "welcome";
    printString(s);//lvalue
    printString(std::move(s));//rvalue 

return 0;
}