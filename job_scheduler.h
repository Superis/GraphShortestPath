/*
 * job_sheduler.h
 *
 *  Created on: Jan 17, 2017
 *      Author: alex
 */

#ifndef JOB_SCHEDULER_H_
#define JOB_SCHEDULER_H_

//#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <fstream>

#include "buffer.h"
#include "components.h"
#include "template_queue.h"

#define OUTPUT_FILE "results.txt"

// perror_function for threads.
inline void Psystem_error(const char *message) {
	perror(message);
	exit(1);
}

inline void Puser_error(const char *message, char *detail) {
	fprintf(stderr, "%s: %s\n", message, detail);
	exit(1);
}


class JobScheduler;

struct Job {
	void* (*adressToFunction)(void *job);
	int src,dest,id,
	version, // repeat for STATIC || version for DYNAMIC
	*printArr,printPos; // pointer to dynamic Array for results
	void *componentsPointer; // points to SCC or CC
	Index *index;
	Buffer *buffer;
	JobScheduler *js;
};

void JobInit(Job *job,
		void* (*adressToFunction)(void *job),
		int source,int dest,int ccounter,
		Index *index,Buffer *buffer,void *compPoint,JobScheduler *js);

void *StaticQuery(void *job);
void *DynamicQuery(void *job);
void *EdgeAddition(void *job);


class JobScheduler {
	int executionThreads; // poolsize
	Queue<Job*>* queue; // job/task queue
	pthread_t *tids; // array of threads_id
	int lastFinishedThreadID;
	int queue_size;
	bool Finish;
	std::ofstream result;
	int *printArray;
	int *runningThreads;
	bool finished;
public:
	JobScheduler(int threadpool);
	~JobScheduler();
	void setfinish(){ Finish=true;};
	//pthread_mutex_t GetMtx() { return this->mtx; };
	//pthread_cond_t GetCond() { return this->cond; };
	void CreateThreads();
	void SetLastThread(int id) { this->lastFinishedThreadID = id; };
	Queue<Job*>* GetQueue() { return this->queue; };
	int* GetArray() { return this->printArray; };
	int *GetRunningThreads() {return runningThreads;};
	bool IsFinished() {return finished; };
	static void* ExecuteThread(void *job);
	void SubmitJob(Job *j);
	void ExecuteJobs();
	void WaitAll(); // waits all submitted tasks to finish
	void DestroyAll();
	void PrintResults();
	//int GetPrintNum() { return print_ready; };
	//void IncPrintNum() { print_ready++; };
};

#endif /* JOB_SCHEDULER_H_ */
