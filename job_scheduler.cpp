/*
 * job_scheduler.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: alex
 */

#include "job_scheduler.h"

using namespace std;

JobScheduler::JobScheduler(int threadpool) {
	executionThreads = threadpool;
	queue = new Queue<Job*>();
	tids = new pthread_t[executionThreads];
	pthread_mutex_init(&mtx, NULL);
	pthread_cond_init(&cond, NULL);

}

JobScheduler::~JobScheduler() {
	delete queue;
	delete[] tids;
}

void JobScheduler::SubmitJob(Job *j) {
	queue->Enqueue(j);
	//executionThreads++;
}

void JobScheduler::ExecuteJobs() {
	for( int i = 0; i < executionThreads; i++) {
		Job *j = queue->Dequeue();
		//pthread_create(&tids[i],NULL,process_request,&bank);
	}

}

void JobScheduler::WaitAll() {
	//while (executionThreads > 0) sleep(1);
	for( int i = 0; i < executionThreads; i++) {
		void *threadReturn;
        if (pthread_join(tids[i], &threadReturn) < 0) {
        	perror("pthread_join ");
        	exit(EXIT_FAILURE);
        }
        cout << "Thread #" << tids[i] << " exited with exit code : "
        		<< (uint8_t*)threadReturn << endl;
	}
	//executionThreads = 0;
}
