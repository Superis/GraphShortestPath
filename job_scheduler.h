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

#include "buffer.h"
#include "components.h"
#include "template_queue.h"

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
	*version, // repeat for STATIC || version for DYNAMIC
	*printArr; // pointer to dynamic Array for results
	void *componentsPointer; // points to SCC or CC
	Index *index;
	Buffer *buffer;
	JobScheduler *js;
};

void JobInit(Job *job,
		void* (*adressToFunction)(void *job),
		int source,int dest,int *version,Index *index,Buffer *buffer);

void *StaticQuery(void *j);
void *DynamicQuery(void *j);
void *EdgeAddition(void *j);


class JobScheduler {
	int executionThreads; // poolsize
	Queue<Job*>* queue; // job/task queue
	pthread_t *tids; // array of threads_id
	pthread_mutex_t mtx;
	pthread_cond_t cond;

	int lastFinishedThreadID;
	int queue_size;

	int *printArray;


public:
	JobScheduler(int threadpool);
	~JobScheduler();

	pthread_mutex_t* GetMtx() { return &(this->mtx); };
	pthread_cond_t *GetCond() { return &(this->cond); };
	void SetLastThread(int id) { this->lastFinishedThreadID = id; };

	void SubmitJob(Job *j);
	void ExecuteJobs();
	void WaitAll(); // waits all submitted tasks to finish
	void PrintResults(std::ofstream&);
};

#endif /* JOB_SCHEDULER_H_ */
