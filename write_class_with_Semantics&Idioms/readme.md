```cpp
#include <iostream>
#include <vector>
#include <memory>

using namespace std;
/*
-Value-Pass semantics
-Move semantics , Copy semantics
-Unified Assignment Operator
-Perfect forwarding
-NRVO , RVO , RAII
*/
/*
-rule of three if you write one of these , so you have to write all other one
    (destructor , copy constructor , copy assignment operator )
-rule of five 
    (destructor , copy constructor , copy assignment operator ,
                move  constructor , move assignment operator) 
-rule of zero if the resource ownership manged from compiler , no writting  
*/


class MyObject {
private:
    // int* data;
    std::unique_ptr<int[]> data;
public:
    // Constructor
    explicit MyObject(int value) : data(std::make_unique<int[]>(value)) {
        std::cout << "Constructor called\n";
    }


};

class MyString{
public:
    MyString()=default;
    MyString(const char * name , const char * , const char *): m_name(name){
        cout << "MyString constructor: " << m_name << endl;
    }
    MyString(std::string name): m_name(std::move(name)){
        cout << "MyString constructor: " << m_name << endl;
    }

    MyString(const MyString& rhs): m_name(rhs.m_name) {
        cout << "MyString copy constructor: " << m_name << endl;
    }

    MyString(MyString && rhs) noexcept: m_name(std::move(rhs.m_name)) {
        cout << "MyString move constructor: " << m_name << endl;
    }
    ~MyString(){
        cout << "MyString destructor: " << m_name << endl;
    }

private:
    std::string m_name{};
};

class Person{
public:
    Person()=default;
    //member initializer list
    /*
	1-construct const std::string tmp object which hold "ahmed"(resource)--> call constructor
	2-name unv ref  bind with -> const std::string tmp object
	3-mName{name}-->translate copy form tmp object to initalize mName--> (call copy constructor)
	4-call destructor of const std::string tmp object
	so there is an two object:
	(1-tmp object , 2-new copy from tmp object translated to inialize mName)
    */
    // Person(int id , const std::string & name):mId{id} , mName{name}{}
    //Value-Passing semantics
    Person(int id , std::string  name):mId{id} , mName{std::move(name)}{}
    //rhs == ahmed
    Person(const Person & rhs):mId{rhs.mId}, mName{rhs.mName}{
        std::cout << "copy constructor"<< std::endl;
    }
    Person(Person && rhs)noexcept:mId{std::move(rhs.mId)}, mName{std::move(rhs.mName)}{
        rhs.mId=0;
        std::cout << "move constructor"<< std::endl;
    }
    // //Copy Assignment Operator 
    // //ahmed = ahmed;
    // Person& operator=(const Person & rhs){
    //     std::cout << "Copy Assignment Operator"<< std::endl;
    //     if(this == &rhs) return *this;
    //     /*copy-and-swap idiom*/
    //     //tmp object from rhs
    //     Person tmp(rhs);//copy constructor
    //     //exception
    //     swap(*this , tmp);
    //     return *this;
    // }

    // //Move Assignment Operator
    // Person& operator=(Person && rhs)noexcept{
    //     std::cout << "Move Assignment Operator"<< std::endl;
    //     if(this == &rhs) return *this;
    //     /*move-and-swap idiom*/
    //     //tmp object bind rhs resource with it 
    //     Person tmp(std::move(rhs));//move constructor
    //     swap(*this , tmp);
    //     return *this;
    // }
    //if we pass lvalue to rhs (copy) rhs will --->call copy constructor
    //if we pass rvalue to rhs (move) rhs will bind with resource --->move constructor
    Person& operator=(Person rhs){
        std::cout << "Unified Assignment Operator"<< std::endl;
        swap(*this , rhs);//*this = nader , rhs = ahmed
        return *this;
    }

    void printData()const{
        cout<<"Id    :"<<mId<<endl;
        cout<<"Name  :"<<mName<<endl;
    }

    void addFriendByVlaueMove(MyString myFriend){
        mFriends.push_back(std::move(myFriend));
    }
    template<typename ...  U>
    void addFriendByUnvRef(U&& ... myFriend){
        mFriends.emplace_back(std::forward<U>(myFriend)...);
    }

    friend void swap(Person & lhs , Person & rhs)noexcept;
private:
    int mId{};
    std::string mName{};
    std::vector<MyString> mFriends{};
};

void swap(Person & lhs , Person & rhs)noexcept{
    std::swap(lhs.mId , rhs.mId);
    std::swap(lhs.mName , rhs.mName);
}


//NRVO (Named Return Value Optimization) and RVO (Return Value Optimization) 
MyString createObjectFromMyString(){
    MyString tmp("anything");
    return tmp; //copy contructor
}


int main() {
    // std::string name ="Ahmed";
    // Person ahmed(1 , "Ahmed");
    // Person nader(3 , "Nader");
    // Person copyAhmed = ahmed;//call copy constructor
    // Person moveAhmed = std::move(ahmed);//call move constructor
    // ahmed.printData();
    // moveAhmed.printData();
    /*****************************/
    // nader= ahmed;//Copy Assignment Operator 
    // nader= std::move(ahmed);//Move Assignment Operator 
    // ahmed.printData();
    // nader.printData();
    /****************************/
    // MyString returnObject = createObjectFromMyString();
    /********************************/
    Person ahmed(1 , "Ahmed");
    // ahmed.addFriendByVlaueMove("jhon");
    ahmed.addFriendByUnvRef("jhon", "nader" , "sayed");

    cout<<"--------end of program--------"<<endl;

    return 0;
}
```
