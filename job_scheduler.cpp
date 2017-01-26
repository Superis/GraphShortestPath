#include "job_scheduler.h"

using namespace std;

void JobInit(
		Job *job,
		void* (*adressToFunction)(void *),
		int source,int dest,int *ver,Index *index,Buffer *buffer) {
	job->src = source;
	job->dest = dest;
	job->version = ver;
	job->index = index;
	job->buffer = buffer;
	job->adressToFunction = StaticQuery; // storing function call
}

void* StaticQuery(void *job) {
	Job *j = ((Job*) job);
	cout << "S::" << j->src << "   D::" << j->dest << endl;
	Index *index = j->index;
	Buffer *buffer = j->buffer;
	int *printArray = j->printArr;
	SCC* strongCC = (SCC*) j->componentsPointer;
	int source = j->src;
	int dest = j->dest;
	int jobID = j->id;
	int* repeat = j->version;
	IndexNode* p = index->GetIndexNode();
	pthread_mutex_t *mtx = j->js->GetMtx();
	pthread_cond_t *cond = j->js->GetCond();
	int k = strongCC->IsReachableGrail(index, source, dest);
	if (k == 0)
		printArray[jobID] = -1;
	else if (k == 1) {
		//result << "MAYBE";
		pthread_mutex_lock(mtx);
		printArray[jobID] =
				strongCC->EstimateShortestPathSCC(buffer,index,source,dest,*repeat);
		(*repeat)++;
		pthread_mutex_unlock(mtx);
	} else if (k == 2) {
		//result << "YES";
		pthread_mutex_lock(mtx);
		printArray[jobID] = buffer->Query(source, dest, index,
						p[source].componentID,*repeat);
		(*repeat)++;
		pthread_mutex_unlock(mtx);
	}
	/*
	 * Set var to last thread ID and signal cond_var.
	 * Awakes ExecuteJobs to assign new task to finished thread.
	 */
	if (pthread_mutex_lock(mtx) != 0)
		Psystem_error("mtx lock @ JobScheduler::SubmitJob ");
	j->js->SetLastThread(jobID);
	pthread_cond_signal(cond);
	pthread_mutex_unlock(mtx);

	delete j;
	return NULL;
}

JobScheduler::JobScheduler(int threadpool) {
	executionThreads = threadpool;
	queue = new Queue<Job*>();
	lastFinishedThreadID = -1;
	printArray = NULL;
	queue_size = 0;
	tids = new pthread_t[executionThreads];
	if (pthread_mutex_init(&mtx, NULL) != 0)
		Psystem_error("mutex init ");

	if (pthread_cond_init(&cond, NULL) != 0)
		Psystem_error("cond init ");

}

JobScheduler::~JobScheduler() {
	delete queue;
	delete[] tids;
}

void JobScheduler::SubmitJob(Job *j) {
	queue->Enqueue(j);
}

void JobScheduler::ExecuteJobs() {
	cout << "Executing whole queue of jobs." << endl;
	queue_size = this->queue->GetSize();
	this->printArray = new int[queue_size];
	// Assign tasks to empty Thread pool
	for( int i = 0; i < executionThreads; i++) {
		Job *j = queue->Dequeue();
		j->id = i;
		cout << "Assigning job to thread #" << i << endl;
		if (pthread_create(&tids[i],NULL,j->adressToFunction,j) != 0)
			Psystem_error("pthread_create ");
	}
	while(!queue->isEmpty()) {
		/*
		 * While queue is not empty wait for a thread to finish
		 * and assign it a new job.
		 */
		pthread_mutex_lock(&mtx);
		pthread_cond_wait(&cond,&mtx);
		if (lastFinishedThreadID < 0 || lastFinishedThreadID >= executionThreads)
			cout << "\tWRONG THREAD ID::ExecuteJobs" << endl;
		Job *j = queue->Dequeue();
		cout << "Thread #" << lastFinishedThreadID << " finished!Another task is being assigned" << endl;
		if (pthread_create(&tids[lastFinishedThreadID],NULL,j->adressToFunction,&j) != 0)
		pthread_mutex_unlock(&mtx);
	}
	cout << "Waiting threads to finish" << endl;
	this->WaitAll();
}

void JobScheduler::WaitAll() {
	for( int i = 0; i < executionThreads; i++) {
		void *threadReturn;
        if (pthread_join(tids[i], &threadReturn) < 0) {
        	perror("pthread_join ");
        	exit(EXIT_FAILURE);
        }
        cout << "Thread #" << tids[i] << " exited with exit code : "
        		<< (uint8_t*)threadReturn << endl;
	}
}

void JobScheduler::PrintResults(std::ofstream& res) {
	for (int i = 0 ; i < this->queue_size ; i++) {
		cout << this->printArray[i] << endl;
	}
	delete[] printArray;
}
