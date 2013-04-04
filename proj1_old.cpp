// proj1.cpp
// Project #1 Spell-Checker
//
// Created by Sean Fay
// smf071000@utdallas.edu
//
// Usage: proj1 arg1 arg2 arg3
// arg1 = # child processes
// arg2 = document file
// arg3 = dictionary file

#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>

using namespace std;

int main(int argc, char * argv[])
{
  int i = 0;
  int pd[2];
  int numProcs = 0;
  int chbegin = 0;
  int chend = 0;
  int div = 0; // var used to decriment within the for loop for forked procs
  string line;

  // Process related variables
  pid_t pid;
  pid_t ppid;
  pid_t chpid;

  // File size variable
  size_t fileSize = 0;

  // Using vectors to store the words from each document
  vector<string> dict; // vector for the dictionary file
  vector<string> doco; // vector for the document file
  vector<string> outp; // vector for the list of mispelled words
  vector<string>::iterator it;

  // Checking proper command line syntax
  if(argc<4) {
      cout << "Invalid syntax, usage: proj1 arg1 arg2 arg3\n";
      cout << "Where:\n";
      cout << "arg1=# of child processes\n";
      cout << "arg2=document file\n";
      cout << "arg3=dictionary file\n";
      exit(-1);
  }

  for(i=0; i < argc; i++) {
      cout << "argv[" << i << "] = " << argv[i] << endl;
  }

  // Setting the # child processes, and the files 
  numProcs = atoi(argv[1]);
  ifstream infile(argv[2]);
  ifstream dictfile(argv[3]);

  // Creating the pipe object that will allow the parent process to
  // pass handlers for the pipe to each of the child processes. Also
  // begin the child process work that will divide up the document
  // read in the dictionary file on each child, and compare it against
  // the document file, etc.
  if(pipe(pd) == -1) {
    cout << "pipe() failed, aborting...\n";
    exit(-1);
  }
  else {
      cout << "pipe() created, good work!\n";
  }
  
  // Opening up the dictionary file and storing it in the dict vector
  while(getline(dictfile,line)) {
      dict.push_back(line);
  }
  cout << "Number of words in dictionary file: " << dict.size() << '\n';

  // Opening up the dictionary file and storing it in the doco vector
  while(getline(infile,line)) {
      doco.push_back(line);
  }
  cout << "Number of words in the document file: " << doco.size() << '\n';

  // Get the file size for the document file
  if(infile.is_open()) {
      infile.seekg(0, ios::end); // move to end of the file
      fileSize = infile.tellg();
  }
  cout << "The size of the document file is: " << fileSize << " bytes in length\n";
  cout << "Each child process will process: " << (doco.size() / numProcs) << " words\n";

  // Creating the fork() code for the # of child procs (numprocs)
  div = numProcs;
  for(i=0; i<numProcs; i++) {
      switch(pid=fork())
      {
          case -1:
          // Here pid is -1, the fork failed
          // Some possible reasons are that you're
          // out of process slots or virtual memory
          cout << "The fork failed!\n";
          break;

          case 0:
          // Pid of zero is the child
          ppid = getppid();
          chpid = getpid();
          cout << "Hi, I'm child " << i << ", my pid is " << chpid
            << ", my parent pid is " << ppid << "\n";
          _exit(0);
       }
  }









  infile.close();
  dictfile.close();
  exit(0);
}
