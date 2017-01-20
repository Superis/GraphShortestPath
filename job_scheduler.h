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
#include "template_queue.h"

// finding the cores of the system
//#define EFFECTIVE_THREAD_NUMBER sysconf(_SC_NPROCESSORS_ONLN);

struct Job {
	int (*adressToFunction)(int src,int dest,Index *index);
	char command;
	int src,dest,
	version,id;
};

void StaticQuery(int src,int dest,Index *index,Buffer *buffer);
void DynamicQuery(int src,int dest,Index *index,Buffer *buffer);
void EdgeAddition(int src,int dest,Index *index,Buffer *buffer);

class JobScheduler {
	int executionThreads;
	Queue<Job*>* queue;
	pthread_t *tids;

	pthread_mutex_t mtx;
	pthread_cond_t cond;

public:
	JobScheduler(int threadpool);
	~JobScheduler();

	void SubmitJob(Job *j);
	void ExecuteJobs();
	void WaitAll(); // waits all submitted tasks to finish
};

#endif /* JOB_SCHEDULER_H_ */
