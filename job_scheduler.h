/*
 * job_sheduler.h
 *
 *  Created on: Jan 17, 2017
 *      Author: alex
 */

#ifndef JOB_SCHEDULER_H_
#define JOB_SCHEDULER_H_

#include <thread>
#include <pthread.h>

#include "template_queue.h"

#define NUMBER_OF_THREADS 8

struct Job {

};

class JobScheduler {
	int executionThreads;
	Queue<Job*>* queue;
	//std::thread *tids;
	pthread_t *tids;

	pthread_mutex_t mtx;
	pthread_cond_t cond;

public:
	JobScheduler();
	virtual ~JobScheduler();
};

#endif /* JOB_SCHEDULER_H_ */
