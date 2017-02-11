#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <chrono>

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

			auto start_time = std::chrono::high_resolution_clock::now();
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
		cout << "Graph is " << specifier << endl;
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
			}ψδ ./
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
	int currentSystemCores = 8;
	JobScheduler *js = new JobScheduler(currentSystemCores);
	index->InitializeVisited(currentSystemCores);
	auto current_time = std::chrono::high_resolution_clock::now();

	std::cout << "Function was running for " <<
	std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() << "\"" << std::endl;
	//buffer->PrintBuffer(index); // insert_unitest

	/**************		Read from Workload file	 **************/
	//ofstream result("results.txt"); //output file for Queries

	// Creating a threadpool equal to the system cores

	cout << "Job Scheduler created" << endl;
	int source, dest,commandCounter = - 1;
	//int version = new int(); // = 0
	char command;
	if (specifier == "STATIC") {
		cout << "Graph is labeled as STATIC.\nPerfoming Tarjan algorithm." << endl;
		//cout << "Maximum value of graph : " << maxVal << endl;
		int estimatedComponentsAmount = maxVal / 5 ;
		if (estimatedComponentsAmount == 0)
			estimatedComponentsAmount = 50;
		SCC* strongCC = new SCC(estimatedComponentsAmount);
		strongCC->EstimateSCC(buffer,index,maxVal);

		cout << "Building Hyper Graph & Grail Index." << endl;
		strongCC->BuildHypergraph(index, buffer);
		strongCC->BuildGrailIndex();

		//result << strongCC.GetCompCount();
		if (workload.is_open()) {

			//js->CreateThreads();
			while (getline(workload, line)) {
				istringstream iss(line);
				iss >> command;
				if (command == 'Q') {
					Job *job = new Job();
					commandCounter++;
					iss >> source >> dest;
					JobInit(job,StaticQuery,source,dest,commandCounter,
							index,buffer,(void *)strongCC,js);
					js->SubmitJob(job);
				} else if (command == 'F') {
					commandCounter = -1;
					js->ExecuteJobs(); // Execute whole queue of jobs and wait till finished.
					continue;
				} else if (command == 'A') {
					cout << "Found additions on static graph.Exiting" << endl;
					break;
				}
			}

		}
	} else if (specifier == "DYNAMIC") {
		if (workload.is_open()) {
			char lastCommand;
			while (getline(workload, line)) {
				Job *job = new Job();
				istringstream iss(line);
				iss >> command;
				if (command == 'Q') {
					iss >> source >> dest;
					//JobInit();
					buffer->Query(source,dest,index,'p',1,0);
				}
				else if (command == 'A') {
					if (lastCommand == 'Q')
						//version++;
					iss >> source >> dest;
					//JobInit();
					index->CheckCap(source,dest); // Checking if reallocation is needed for Index
					index->Insert(source, dest, buffer);
					buffer->AddNeighbor(source, dest, index);
				}
				else if (command == 'F') {
					js->ExecuteJobs();
					//js->PrintResults(result);
				}
				lastCommand = command;
			}
		}
		else
			cout << "Wrong specifier for WORKLOAD file." << endl;
	}
	else
		cerr << "Unable to open Workload file" << endl;
	workload.close();
	//result.close();

	js->DestroyAll();
	delete js;
	//delete version;
	delete buffer;
	delete index;

	cout << "Program terminated!" << endl
			<< "Results can be found @ " << OUTPUT_FILE << endl;
	return 0;
}
