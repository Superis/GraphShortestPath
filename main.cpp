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

#include "buffer.h" // Part 1
#include "components.h" // Part 2
#include "job_scheduler.h" // Part 3

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

void CreateGraph(Buffer *buffer,Index *index,ifstream &myFile,string specifier) {
	int source, dest;
	string line;
	if (specifier == "STATIC") {
		while (getline(myFile, line)) {
			istringstream iss(line);
			if (!(iss >> source >> dest))
				break;
			index->Insert(source, dest, buffer);
			buffer->InsertBuffer(source, dest, index);
		}
	} else if (specifier == "DYNAMIC") {
		while (getline(myFile, line)) {
			istringstream iss(line);
			if (!(iss >> source >> dest))
				break;
			index->Insert(source, dest, buffer);
			buffer->InsertBuffer(source, dest, index,0);
		}
	} else {
		cerr << "Wrong specifier @ workload file(neither 'STATIC' nor 'DYNAMIC'."
				"Check spelling.Program exiting." << endl;
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	cout << "Program is running " << endl;
	if (argc != 3) {
		cerr << "Need TWO(not " << argc << ") input files(Graph & Workload,respectively) as arguments" << endl;
		exit(EXIT_FAILURE);
	}

	/*
	 * Read workload file 1st line to see the kind of graph,
	 * so to avoid initializing edgeProperty.
	 */
	ifstream workload;
	char *workFile = argv[2];
	workload.open(workFile);
	string specifier;
	if (workload.is_open()) {
		getline(workload,specifier); // DYNAMIC || STATIC
	} else {
		cerr << "Workload-file couldn't be opened!Aborting graph creation & exitting" << endl;
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
		CreateGraph(buffer,index,myFile,specifier);
	}
	else
		cerr << "Unable to open Graph file(2)" << endl;
	myFile.close();

	cout << "Index & Graph were created." << endl;

	//buffer->PrintBuffer(index); // insert_unitest

	/**************		Read from Workload file	 **************/
	ofstream result("results.txt"); //output file for Queries

	// Creating a threadpool equal to the system cores
	int currentSystemCores = sysconf(_SC_NPROCESSORS_ONLN);
	JobScheduler *js = new JobScheduler(currentSystemCores);

	if (specifier == "STATIC") {
		cout << "Graph is labeled as STATIC.Perfoming Tarjan algorithm." << endl;
		cout << maxVal << endl;
		int estimatedComponentsAmount = maxVal / 5;
		if (estimatedComponentsAmount == 0)
			estimatedComponentsAmount = 50;
		SCC strongCC(estimatedComponentsAmount);
		strongCC.EstimateSCC(buffer,index,maxVal);

		//result << strongCC.GetCompCount();

		if (workload.is_open()) {
			int repeat = 0;
			char command;
			int source, dest;
			IndexNode* p = index->GetIndexNode();
			strongCC.BuildHypergraph(index, buffer);
			strongCC.BuildGrailIndex();
			while (getline(workload, line)) {
				istringstream iss(line);
				iss >> command;
				if (command == 'Q') {
					iss >> source >> dest;
					int k = strongCC.IsReachableGrail(index, source, dest);
					if (k == 0)
						result << "-1" << endl;
					else if (k == 1) {
						//result << "MAYBE";
						result
								<< strongCC.EstimateShortestPathSCC(buffer,
										index, source, dest, repeat) << endl;
						repeat++;
					} else if (k == 2) {
						//result << "YES";
						result
								<< buffer->Query(source, dest, index,
										p[source].componentID, repeat) << endl;
						repeat++;
					}
				} else if (command == 'A') {
					cout << "Found additions on static graph.Exiting" << endl;
					break;
				} else if (command == 'F') {
					continue;
				}
			}
		}
	} else if (specifier == "DYNAMIC") {
		int version = 0;
		if (workload.is_open()) {
			Job *job = new Job();
			char command,lastCommand;
			int source, dest,commandCounter = 0;
			while (getline(workload, line)) {
				istringstream iss(line);
				iss >> command;
				if (command == 'Q') {
					iss >> source >> dest;
					job->src = source;
					job->dest = dest;
					job->command = 'Q';
					job->id = commandCounter++;
					buffer->Query(source,dest,index,'p',1);
				}
				else if (command == 'A') {
					if (lastCommand == 'Q')
						version++;
					iss >> source >> dest;
					job->src = source;
					job->dest = dest;
					job->command = 'A';
					job->id = commandCounter++;
					index->CheckCap(source,dest); // Checking if reallocation is needed for Index
					index->Insert(source, dest, buffer);
					buffer->AddNeighbor(source, dest, index);
				}
				else if (command == 'F') {
					commandCounter = 0;
					continue; //sto epomeno skelos pou tha asxolithoume me tis ripes ergasiwn

				}
				lastCommand = command;
			}
		}
	}
	else
		cerr << "Unable to open Workload file" << endl;

	workload.close();
	result.close();

	delete js;
	delete buffer;
	delete index;

	cout << "Program terminated!" << endl;
	return 0;
}
