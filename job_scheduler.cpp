/*
 * job_scheduler.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: alex
 */

#include "job_scheduler.h"

using namespace std;

JobScheduler::JobScheduler() {
	executionThreads = NUMBER_OF_THREADS;
	queue = new Queue<Job*>();
	tids = new pthread_t[executionThreads];
	for( int i = 0; i < executionThreads; i++) {
		//pthread_create(&warray[index],NULL,process_request,&bank);
		//tids[i] = std::thread();
	}

	pthread_mutex_init(&mtx, NULL);
	pthread_cond_init(&cond, NULL);

}

JobScheduler::~JobScheduler() {
	delete queue;
	delete[] tids;
}

