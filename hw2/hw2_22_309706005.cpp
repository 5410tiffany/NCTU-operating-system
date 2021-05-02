#define _POSIX_SOURCE
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ptrace.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/reg.h>
#include <bits/stdc++.h>

using namespace std;
int main(int argc,char **argv)
{
	pid_t child_pid; //define child_pid
	int p_status;
	long syscall;

	child_pid = fork();

	if(child_pid == 0) // child process
	{
		ptrace(PTRACE_TRACEME, child_pid, NULL, NULL); //ptrace:trace system calls
		kill(getpid(), SIGSTOP);
        
        
        FILE *fptr1, *fptr2;
        char filename[100], c, dfilename[100];
		
        // input copied file
        cout << "Please input the source file name: " << endl;
        cin >> filename;
        fptr1 = fopen(filename, "r");
        if (fptr1 == NULL)
        {
        cout << "Failed to open file " << filename <<endl;
        }
        cout << "Please input the destination file name: "<<endl;
        cin >> dfilename;
        
        
        // generate empty output file
        fstream file;
        file.open(dfilename,ios::out);
        file.close();
        
        fptr2 = fopen(dfilename, "w");
        c = fgetc(fptr1);
        while (c != EOF)
        {
        fputc(c, fptr2);
        c = fgetc(fptr1);
        }
        
        cout<<dfilename<<" has generated";
        fclose(fptr1);
        fclose(fptr2);
        
        
		exit(0);
	}
	else if(child_pid > 0) // parent process
		while(waitpid(child_pid, &p_status, 0))
			if(WIFEXITED(p_status))
				break;
			else
			{
				syscall = ptrace(PTRACE_PEEKUSER, child_pid, sizeof(long)*ORIG_RAX, NULL);
				printf("system call: %ld\n", syscall);
				ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
			}
	else
		printf("fork error\n");


	return 0;
}
