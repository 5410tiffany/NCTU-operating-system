
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

    int n;
    int customer;

int main()
{
    string filename("input.txt");

   
    ifstream input_file(filename);

    input_file >> n;
    input_file >> customer;
    cout << customer << " " << endl;
    
    int Bshop[customer];
    
    for (int i=0; i< customer-1; i++){
        input_file >> Bshop[i];
    }
    
    cout << endl;
    input_file.close();

    return 0;
}