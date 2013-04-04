//
//  proj1.cpp
//  OS_Project1
//
//  Created by Sean Fay.
//  Email: smf071000@utdallas.edu
//  Usage: proj1 arg1 arg2 arg3
//  arg1 = # of child processes
//  arg2 = document file (document.txt)
//  arg3 = dictionary file (dictionary.txt)
//

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

    // for(i=0; i < argc; i++) {
        // cout << "argv[" << i << "] = " << argv[i] << endl;
    // }

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
    else {
        cout << "pipe() created, good work!\n";
    }

    // Opening up the dictionary file and storing it in the dict vector
	while(getline(dictfile,line)) {
		dict.push_back(line);
	}
    cout << "Size of the dictionary vector is: " << dict.size() << "\n";

	// Opening up the document file and storing it in the doco string
    string doco = "";
    while(getline(infile,line)) {
        doco += line;
    }

    // How much each chld process will process (# indexes / child process)
    // cout << "Each child process will process: " << (int)(doco.size() / numProcs) << " words\n";

	int length = doco.length() / numProcs;

    // Creating the fork() code for the chld processes
    for(i=0; i < numProcs; i++) {
		// Creating chDoc as the chopped up document
		string chDoc = doco.substr(0, length);
		doco = doco.substr(length, doco.length());
        switch(pid=fork()) {
            case -1:
                // Here pid is -1, the fork failed
                cout << "The fork() failed!\n";
                break;

            case 0:
                // pid of zero is the child
                ppid = getppid();
                chpid = getpid();
				stringstream ss(chDoc); // streaming the very long string
				string word;

				while((ss>>word) > 0) { // every word from chDoc  assigned to "word"
					bool found=false;
					
					//cout << "Word to be checked is: " << word << endl;

                    if(binary_search(dict.begin(), dict.end(), word)) {
                        found = true;
                    }

					if(!found) {
                        cout << "Incorrectly spelled word: " << word << endl;
						outp.push_back(word);
						cout << "Size of output vector is: " << outp.size() << endl;
					}
				}

                int length = outp.size();
                if(write(pdlength[1], &length, sizeof(int)) == -1) {
                    error_exit("write() of pdlength failed");
                }
                _exit(0);
        }
    }
    int z;
    cout << "Done forking..." << endl;
    string stringout="";

    for(int i = 0; i < numProcs; i++) {
 //   	cout << "Parent reading " << i << endl;
	if(read(pdlength[0], &z, sizeof(int)) == -1) {
	    error_exit("parent read() of length failed");
	}
	
	char temp1[z];

	string temp = "";
//	cout << "Reading " << z << endl;
	if(read(pd[0], &temp1, z) == -1) {
	    error_exit("parent read() of pd failed");
	}

	temp = temp1; 
	temp = temp.substr(0,z);

	stringout += temp;
	

    for(int i=0; i < numProcs; ++i) {
   	    waitpid(-1, NULL, 0);
    }
    string out;

    stringstream allWords (stringout);
    while((allWords >> out) > 0) {
        outp.push_back(out);
    }

    cout << "\nReport: " << endl;
    cout << "There were " << outp.size() << " incorrect words found.\n" << endl;

    infile.close();
    dictfile.close();
    return 0;
}
}

void error_exit(char *s)
{
    cerr << "\nERROR: " << s << " - bye!\n";
    exit(1);
}
