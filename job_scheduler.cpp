#include "job_scheduler.h"

using namespace std;

void StaticQuery(int source,int dest,Index *index,Buffer *buffer
		,SCC *strongCC,int jobID,int *printArray,int *repeat) {
	IndexNode* p = index->GetIndexNode();
	int k = strongCC->IsReachableGrail(index, source, dest);
	if (k == 0)
		printArray[jobID] = -1;
	else if (k == 1) {
		//result << "MAYBE";
		printArray[jobID] =
				strongCC->EstimateShortestPathSCC(buffer,index,source,dest,*repeat);
		(*repeat)++;
	} else if (k == 2) {
		//result << "YES";
		printArray[jobID] = buffer->Query(source, dest, index,
						p[source].componentID,*repeat);
		(*repeat)++;
	}
}

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
		delete j;
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
