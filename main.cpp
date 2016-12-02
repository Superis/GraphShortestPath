/*
* main.cpp
*
*  Created on: Oct 27, 2016
*      Author:
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"

using namespace std;

inline int max(int a, int b) {
	return((a>b) ? (a) : (b));
}

int main(int argc, char **argv) {
	cout << "Program is running " << endl;

	//struct timeval  tv1, tv2;
	//gettimeofday(&tv1, NULL);

	if (argc != 3) {
		cerr << "Need TWO(not " << argc << ") input files(Graph & Workload,respectively) as arguments" << endl;
		exit(EXIT_FAILURE);
	}

	/**************		Read from Graph file 	 **************/
	char *graphFile = argv[1];
	ifstream myFile;
	myFile.open(graphFile);
	string line;
	int maxVal = 0;
	/*
	* Find :
	* max value of file so we don't need to reallocate Index array.
	* the average of neighbors for the most optimal allocation.
	*/
	if (myFile.is_open()) {
		int source, dest;
		while (getline(myFile, line)) {
			istringstream iss(line);
			if (!(iss >> source >> dest))
				break;
			maxVal = max(maxVal, max(source, dest));
			/*if (source != srcompare) {
				uniqueNodes++;
				srcompare = source;
			}
			linesCounter++;*/
		}
		//optimalCellValue = ceil(linesCounter / (float)uniqueNodes);
	}
	else
		cerr << "Unable to open Graph file(1)" << endl;

	Buffer *buffer = new Buffer(maxVal + 1);
	Index *index = new Index(maxVal + 1);

	/** Resetting file for second read**/
	myFile.clear();
	myFile.seekg(0, myFile.beg);

	if (myFile.is_open()) {
		int source, dest;
		while (getline(myFile, line)) {
			istringstream iss(line);
			if (!(iss >> source >> dest))
				break;
			//cout << "Read " << source << " and " << dest << endl;
			//if (source == 16)
			//cout << endl;
			index->Insert(source, dest, buffer);
			//cout<<"index inserted"<<endl;
			buffer->InsertBuffer(source, dest, index);
			//cout<<"buff inserted"<<endl;
		}
	}
	else
		cerr << "Unable to open Graph file(2)" << endl;

	myFile.close();
	buffer->PrintBuffer(index);

	ofstream result("results.txt");

	/**************		Read from Workload file	 **************/
	char *workFile = argv[2];
	myFile.open(workFile);
	if (myFile.is_open()) {
		char Command;
		int source, dest;
		while (getline(myFile, line)) {
			istringstream iss(line);
			iss >> Command;
			if (Command == 'Q') {
				iss >> source >> dest;
				//buffer->Query(source,dest,index);
				result << buffer->Query(source, dest, index) << endl;
			}
			else if (Command == 'A') {
				iss >> source >> dest;
				index->Insert(source, dest, buffer);
				buffer->AddNeighbor(source, dest, index);
			}
			else if (Command == 'F') {
				continue; //sto epomeno skelos pou tha asxolithoume me tis ripes ergasiwn

			}
		}
	}
	else
		cerr << "Unable to open Workload file" << endl;
	myFile.close();
	result.close();

	delete buffer;
	delete index;
	return 0;
}
