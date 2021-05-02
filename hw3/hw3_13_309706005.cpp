#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
int main()

{



int a=0, b=1, n=a+b;
int i=0;
while(i<=0){
    cout <<"Enter the number of a Fibonacci Sequence:"<<endl;
    cin >> i;
    if (i<=0){
        cout <<"the number is invalid"<<endl;
    }
}

pid_t pid = fork();
if (pid == 0)
{
    cout<<"Child process-start, pid = "<<" "<<getpid()<<endl;
    
    //start fibonacci sequence
    for(int j=1;j<=i;j++){
          if (j==1){
            cout <<"0 ";
        }
        else if(j==2){
            cout <<"1 ";
        }
        else{
             n=a+b;
             cout<<n <<" ";
             a=b;
             b=n;
            
        }
    }
  
    cout<<endl<<"Child process-ends, pid = "<<" "<<getpid()<<endl;
}
    else 
    {
        cout<<"Parent process-start, pid = "<<" "<<getpid()<<endl;
        waitpid(pid, NULL, 0);
        cout<<"Parent process-end, pid = "<<" "<<getpid()<<endl;
    }
    return 0;
}