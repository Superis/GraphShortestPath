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
#include <cmath>

#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "Buffer.h"

using namespace std;

inline int max(int a,int b) {
	return((a>b)?(a):(b));
}

int main(int argc,char **argv) {
	cout << "Program is running " << endl;

	struct timeval  tv1, tv2;
	gettimeofday(&tv1, NULL);

	if (argc != 3) {
		cerr << "Need TWO(not " << argc <<") input files(Graph & Workload,respectively) as arguments" << endl;
		exit(EXIT_FAILURE);
	}

	/**************		Read from Graph file 	 **************/
	char *graphFile = argv[1];
	ifstream myFile;
	myFile.open(graphFile);
	string line;
	int maxVal = 0;
	double optimalCellValue = 0;
	/*
	 * Find :
	 * max value of file so we don't need to reallocate Index array.
	 * the average of neighbors for the most optimal allocation.
	 */
	if (myFile.is_open()) {
		int source,dest,srcompare = -1,uniqueNodes = 0,linesCounter = 0;
		while(getline(myFile, line)) {
			istringstream iss(line);
			if (!(iss >> source >> dest))
				break;
			maxVal = max(maxVal,max(source,dest));
			if (source != srcompare) {
				uniqueNodes++;
				srcompare = source;
			}
			linesCounter++;
		}
		optimalCellValue = ceil(linesCounter/(float)uniqueNodes);
	}
	else
		cerr << "Unable to open Graph file(1)" << endl;

	cout << "Max value of file = " << maxVal << endl;
	cout << "Optimal Cell Value = " << optimalCellValue << endl;
	/*value = optimalCellValue;
	Redefinition(); // auti i mlkia den douleuei.mallon to define ginetai sto compile opote einai fixed.
	// gia na douleptsei mallon prepei to neightbor na nai pointer.Rwtiste piazza an einai*/


	Buffer *buffer = new Buffer(maxVal);
	Index *index = new Index(maxVal);

	/** Resetting file for second read**/
	myFile.clear();
	myFile.seekg(0,myFile.beg);

	if (myFile.is_open()) {
		int source,dest;
		while(getline(myFile, line)) {
			istringstream iss(line);
			if (!(iss >> source >> dest))
				break;
			//cout << "Read " << source << " and " << dest << endl;
			if (source == 16)
				cout << endl;
			index->Insert(source,dest,buffer);
			buffer->InsertBuffer(source,dest,index);
		}
	}
	else
		cerr << "Unable to open Graph file(2)" << endl;
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
				buffer->Query(source,dest,index);
			}
			else if (Command == 'A') {
				iss >> source >> dest;
				buffer->AddNeighbor(source,dest,index);
			}
			else if (Command == 'F') {
				// kanoume tpt edw?
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


