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
#include "components.h"

using namespace std;

inline int max(int a, int b) {
	return((a>b) ? (a) : (b));
}

// perror_function for threads.
void Psystem_error(char *message) {
	perror(message);
	exit(1);
}

void Puser_error(char *message, char *detail) {
	fprintf(stderr, "%s: %s\n", message, detail);
	exit(1);
}

int main(int argc, char **argv) {
	cout << "Program is running " << endl;
	if (argc != 3) {
		cerr << "Need TWO(not " << argc << ") input files(Graph & Workload,respectively) as arguments" << endl;
		exit(EXIT_FAILURE);
	}

	/**************		Read from Graph file 	 **************/
	char *graphFile = argv[1];
	ifstream myFile;
	myFile.open(graphFile);
	string line,specifier;
	int maxVal = 0;
	/*
	* Find :
	* max value of file so we don't need to reallocate Index array.
	* the average of neighbors for the most optimal allocation.
	*/
	if (myFile.is_open()) {
		int source, dest;
		//getline(myFile, specifier); // DYNAMIC || STATIC
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
		//getline(myFile, specifier);
		while (getline(myFile, line)) {
			istringstream iss(line);
			if (!(iss >> source >> dest))
				break;
			index->Insert(source, dest, buffer);
			buffer->InsertBuffer(source, dest, index);
		}
	}
	else
		cerr << "Unable to open Graph file(2)" << endl;
	myFile.close();

	cout << "Index & Graph were created." << endl;
	//buffer->PrintBuffer(index); // insert_unitest
	/* get neighbors unit test
	int x =index->NeighboursNum(17,'o',buffer);
	do {
		index->GetIndexNode()[17].recursive_level++;
		cout << index->GetIndexNode()[17].recursive_level << " :: "<< index->GetNeighbor(17,buffer) << endl;

	} while(index->GetIndexNode()[17].recursive_level < x);
	cout << index->GetIndexNode()[17].recursive_level << " & " << x << endl;
	cout << "Waiting char" << endl;
	getchar();*/
	int estimatedComponentsAmount = maxVal / 5;
	if (estimatedComponentsAmount == 0)
		estimatedComponentsAmount = 50;
	SCC strongCC(estimatedComponentsAmount);
	strongCC.EstimateSCC(buffer,index,maxVal);
	//strongCC.Print();
	//cout << "Waiting char" << endl;
	//getchar();
	//return 1;

	ofstream result("results.txt"); //output file for Queries
	//result << strongCC.GetCompCount();
	/**************		Read from Workload file	 **************/
	char *workFile = argv[2];
	myFile.open(workFile);
	if (myFile.is_open()) {
		char Command;
		int source, dest;
		IndexNode* p=index->GetIndexNode();
		strongCC.BuildHypergraph(index,buffer);
		strongCC.BuildGrailIndex();
		while (getline(myFile, line)) {
			istringstream iss(line);
			iss >> Command;
			if (Command == 'Q') {
				iss >> source >> dest;
				//buffer->Query(source,dest,index);
				int k=strongCC.IsReachableGrail(index,source,dest);
				if (k==0)
					result << "-1" << endl;
				else if (k==1){
					//result << "MAYBE";
					result << strongCC.EstimateShortestPathSCC(buffer,index,source,dest) << endl;
				}
				else if (k==2){
					//result << "YES";
					result << buffer->Query(source,dest,index,'S',p[source].componentID) << endl;
				}
			}
			else if (Command == 'A') {
				iss >> source >> dest;
				index->CheckCap(source,dest); // Checking if reallocation is needed for Index
				index->Insert(source, dest, buffer);
				buffer->AddNeighbor(source, dest, index);
			}
			else //if (Command == 'F') {
				continue; //sto epomeno skelos pou tha asxolithoume me tis ripes ergasiwn

			//}

		}
	}
	else
		cerr << "Unable to open Workload file" << endl;
	myFile.close();
	result.close();

	delete buffer;
	delete index;
	cout << "Program terminated successfully!" << endl;
	return 0;
}
