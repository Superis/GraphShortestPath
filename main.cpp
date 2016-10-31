/*
 * main.cpp
 *
 *  Created on: Oct 27, 2016
 *      Author: alex
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>

#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "Buffer.h"

using namespace std;

int main(int argc,char **argv) {
	cout << "Program is running " << endl;

	struct timeval  tv1, tv2;
	gettimeofday(&tv1, NULL);

	if (argc != 3) {
		cerr << "Need TWO(not " << argc <<") input files(Graph & Workload,respectively) as arguments" << endl;
		exit(EXIT_FAILURE);
	}

	Buffer *buffer = new Buffer();
	Index *index = new Index();

	/**************		Read from Graph file	 **************/
	char *graphFile = argv[1];
	ifstream myFile;
	myFile.open(graphFile);
	string line; // 11.375753
	if (myFile.is_open()) {
		int source,dest;
		while(getline(myFile, line)) {
			istringstream iss(line);
			if (!(iss >> source >> dest))
				break;
			if (dest == 17)
				cout << "INc" << endl;
			cout << "Read " << source << " and " << dest << endl;
			index->Insert(source,dest,buffer);
			buffer->InsertBuffer(source,dest,index);
		}
	}
	else
		cerr << "Unable to open Graph file" << endl;
	myFile.close();
	buffer->PrintBuffer(index);

	/**************		Read from Workload file	 **************/
	char *workFile = argv[2];
	myFile.open(workFile);
	if (myFile.is_open()) {
		char Command;
		int source,dest;
		while(getline(myFile, line)) {
			istringstream iss(line);
			iss >> Command;
			if (Command == 'Q') {
				iss >> source >> dest;
				cout << "Read command : " << Command << " with " << source << " and " << dest << endl;
			}
			else if (Command == 'A') {
				iss >> source >> dest;
				cout << "Read command : " << Command << " with " << source << " and " << dest << endl;
			}
			else if (Command == 'F') {
				cout << "Read F*" << endl;
			}
		}
	}
	else
		cerr << "Unable to open Workload file" << endl;
	myFile.close();

	gettimeofday(&tv2, NULL);
	printf ("Total time = %f seconds\n",
	         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	         (double) (tv2.tv_sec - tv1.tv_sec));

	delete buffer;
	delete index;

	return EXIT_SUCCESS;
}


