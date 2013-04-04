
//  proj1.cpp
//  OS_Project1
//
//  Created by Sean Fay.
//  Email: smf071000@utdallas.edu
//  Usage: proj1 arg1 arg2 arg3
//  arg1 = # of child processes
//  arg2 = document file (document.txt)
//  arg3 = dictionary file (dictionary.txt)


#include <iostream>
#include <sys/types.h>
#include <fstream>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <sstream>

using namespace std;
void error_exit(char *s);

int main (int argc, const char * argv[])
{
    int i = 0;
    int pd[2];
    int pdlength[2];
    int numProcs = 0;
    string line;
    
    // Child Process related variables
    pid_t pid;
    pid_t ppid;
    pid_t chpid;
    
    // Using vectors<string> to store the content of the files
    vector<string> dict; // vector for the dictionary file
    vector<string> outp; // vector for the misspelled words
    
    // Checking proper command line syntax
    if(argc<4) {
        cout << "Invalid syntax, usage: proj1 arg1 arg2 arg3\n";
        cout << "Where:\n";
        cout << "arg1=# of child processes\n";
        cout << "arg2=document file\n";
        cout << "arg3=dictionary file\n";
        exit(-1);
    }
    
    // Setting the # of child processes, and the files
    numProcs = atoi(argv[1]);
    ifstream infile(argv[2]);
    ifstream dictfile(argv[3]);
    
    // Creating the pipe object that will allow the parent process
    // to pass handlers for the pipe to each of the child processes.
    if(pipe(pd) == -1|| pipe(pdlength) == -1) {
        cout << "pipe() failed, aborting...\n";
        exit(-1);
    }
    
    // Opening up the dictionary file and storing it in the dict vector
    while(getline(dictfile,line)) {
        dict.push_back(line);
    }
    string doco = "";
    // Opening up the document file and storing it in the doco vector
    while(getline(infile,line)) {
        doco += line;
    }
    
    int length = doco.length() / numProcs;
    
    // Creating the fork() code for the chld processes
    for(i=0; i < numProcs; i++) {
		string chDoc = doco.substr(0, length); // smaller portion of the document.txt file for each child
		doco = doco.substr(length, doco.length()); // re-assign doco with newly shortened length
        
        switch(pid=fork()) {
                
            case -1:
                // Here pid is -1, the fork failed
                cout << "The fork() failed!" << endl;
                break;
                
            case 0:
                //cout << "The fork() worked! Child spawned." << endl;
				stringstream ss(chDoc);
				string word, output;
				
				while((ss>>word) > 0) {
					bool found=false;
                    
                    if(binary_search(dict.begin(), dict.end(), word)) {
                        found = true;
                    }
                    
					if(!found) {
                        output += " " + word;
					}
				}
                
                int length = output.length();
                if(write(pdlength[1], &length, sizeof(int)) == -1) {
                    error_exit("write() of pdlength failed");
                }
				char tempOutput[length];
				for(int i = 0; i < length; i++) {
					tempOutput[i] = output[i];
                }
                
                //cout << "Child writing" << output << endl;
                if(write(pd[1], &tempOutput, length) == -1) {
                    error_exit("write() failed");
                }
                
                _exit(0);
                
        }
    }
    int tempLength = 0;
    //cout << "Done forking"<< endl;
    string stringout = "";
    for(int i = 0; i < numProcs; i++) {

        if(read(pdlength[0], &tempLength, sizeof(int)) == -1) {
            error_exit("parent read() of length failed");
        }
        
        char tempString[tempLength];
        
        string temp = ""; // making a new temporary string 

        if(read(pd[0], &tempString, tempLength) == -1) {
            error_exit("parent read() of pd failed");
        }

        temp = tempString; // take the char[] array >> temp
        temp = temp.substr(0, tempLength); // cut it down to just the size we need
        
        stringout += temp;
    }
    
    for(int i=0; i < numProcs; ++i) {
   	    waitpid(-1, NULL, 0);
    } 

    string out;
    
    stringstream allWords (stringout);
    while((allWords >> out) > 0) {
        outp.push_back(out);
    }
    
    // Reporting on the incorrectly spelled words
    cout << "\n\nReport: " << endl;
    cout << "There were " << outp.size() << " incorrect words found.\n" << endl;
    for(int i = 0; i < outp.size(); i++) {
        cout << "Incorrectly spelled word " << i << " is: " << outp[i] << endl;
    }
    
    // Closing files and ending the program
    infile.close();
    dictfile.close();
    return 0;
}

void error_exit(char *s)
{
    cerr << "\nERROR: " << s << " - bye!\n";
    exit(1);
}
