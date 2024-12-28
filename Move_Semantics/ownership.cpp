#include<iostream>
#include<memory>

using namespace std;
class Allocate
{
private:
    /* data */
    int *ptr ;
public:
    Allocate(/* args */)
    {
        ptr = new int[5];
        cout<<"Allocate"<<endl;
    }

    ~Allocate()
    {
        delete [] ptr;
        cout<<"deallocate"<<endl;

    }
};



int main(){
    cout<<"first outer"<<endl;
    try{
        {
            // Allocate obj;
            auto ptr = make_unique<int[]>(5);
        // auto ptr = new int[5];
        // cout<<"Allocate 5 locations"<<endl;

        throw "exception";

        // delete [] ptr;
        // cout<<"delete 5 locations"<<endl;
        }
    }catch(...){

    }

    cout<<"second outer"<<endl;

    return 0;
}