#include "job_scheduler.h"

using namespace std;

pthread_mutex_t mtx;
pthread_mutex_t q;
pthread_cond_t cond;

void JobInit(
		Job *job,
		void* (*adressToFunction)(void *),
		int source,int dest,int commandCounter,int *ver,
		Index *index,Buffer *buffer,void *compPointer,JobScheduler *_js)
{
	job->adressToFunction = StaticQuery; // storing function call

	job->src = source;
	job->dest = dest;
	job->printPos = commandCounter;
	job->version = ver;
	//job->printArr = _js->GetArray();

	job->componentsPointer = compPointer;
	job->index = index;
	job->buffer = buffer;
	job->js = _js;
}

void* StaticQuery(void *job) {
	Job *j = ((Job*) job);
	Index *index = j->index;
	Buffer *buffer = j->buffer;
	int *printArray = j->js->GetArray();
	SCC* strongCC = static_cast<SCC*>(j->componentsPointer);
	int source = j->src;
	int dest = j->dest;
	int* repeat = j->version;
	IndexNode* p = index->GetIndexNode();

	/*pthread_mutex_lock(&mtx);
	cout << "\tThread #" << j->id << " started "
	 << "S::" << j->src << "   D::" << j->dest << endl;
	pthread_mutex_unlock(&mtx);*/

	int k = strongCC->IsReachableGrail(index, source, dest);
	if (k == 0) {
		pthread_mutex_lock(&mtx);
		//cout << "Thread #" << j->id << " -" << (*repeat) << "- @ " << j->printPos << endl;
		printArray[j->printPos] = -1;
		pthread_mutex_unlock(&mtx);
	}
	else if (k == 1) {
		//result << "MAYBE";
		pthread_mutex_lock(&mtx);
		//cout << "\tThread #" << j->id << " " << (*repeat) << " @ " << j->printPos << endl;
		printArray[j->printPos] =
				strongCC->EstimateShortestPathSCC(buffer,index,source,dest,*repeat);
		(*repeat)++;
		pthread_mutex_unlock(&mtx);
	} else if (k == 2) {
		//result << "YES";
		pthread_mutex_lock(&mtx);
		//cout << "\tThread #" << j->id << " " << (*repeat) << " @ " << j->printPos << endl;
		printArray[j->printPos] = buffer->Query(source, dest, index,
						p[source].componentID,*repeat);
		(*repeat)++;
		pthread_mutex_unlock(&mtx);
	}

	delete j;
	return NULL;
}



JobScheduler::JobScheduler(int threadpool) {
	executionThreads = threadpool;
	queue = new Queue<Job*>();
	runningThreads= threadpool;
	lastFinishedThreadID = -1;
	printArray = NULL;
	queue_size = 0;
	runningThreads = 0;
	result.open(OUTPUT_FILE,ios::app); //output file for Queries
	tids = new pthread_t[executionThreads];
	if (pthread_mutex_init(&q, NULL) != 0)
		Psystem_error("mutex init ");
	if (pthread_mutex_init(&mtx, NULL) != 0)
		Psystem_error("mutex init ");
	if (pthread_cond_init(&cond, NULL) != 0)
		Psystem_error("cond init ");

}

JobScheduler::~JobScheduler() {
	delete queue;
	delete[] tids;
	result.close();
	if (pthread_mutex_destroy(&mtx) != 0)
		Psystem_error("mutex destroy ");
	if (pthread_cond_destroy(&cond) != 0)
		Psystem_error("cond destroy ");

}

void JobScheduler::SubmitJob(Job *j) {
	if (pthread_mutex_lock(&mtx) != 0)
		Psystem_error("mtx lock @ JobScheduler::SubmitJob ");
	queue->Enqueue(j);
	if (pthread_mutex_unlock(&mtx) != 0)
		Psystem_error("mtx unlock @ JobScheduler::SubmitJob ");
}

void* JobScheduler::ExecuteThread(void *job) {
	Job *j = ((Job*) job);
	int threadID = j->id;
	Queue<Job*> *queue = j->js->GetQueue();
	j->adressToFunction(job);
	pthread_mutex_lock(&mtx);
	//cout << "\tThread #" << threadID << " finished 1st task" << endl;
	pthread_mutex_unlock(&mtx);
	while(1) {
		if (pthread_mutex_lock(&mtx) != 0)
			Psystem_error("mtx lock @ JobScheduler::SubmitJob ");
		if (!queue->isEmpty()) {
			j = queue->Dequeue();
			if (j == NULL) {
				//cout << "NULL::Thread #" << threadID << " exiting!" << endl;
				if (pthread_mutex_unlock(&mtx) != 0)
					Psystem_error("mtx unlock @ JobScheduler::SubmitJob ");
				break;
			}
			j->id = threadID;
			//j->printArr = j->js->GetArray();
			//cout << "\tThread #" << threadID << " dequeue" << endl;
			if (pthread_mutex_unlock(&mtx) != 0)
				Psystem_error("mtx unlock @ JobScheduler::SubmitJob ");
		}
		else {
			//cout << "\tThread #" << threadID << " exiting!" << endl;
			if (pthread_mutex_unlock(&mtx) != 0)
				Psystem_error("mtx unlock @ JobScheduler::SubmitJob ");
			break;
		}

		pthread_mutex_lock(&mtx);
		//cout << "\tThread #" << threadID << " retrieved from queue" << endl;
		pthread_mutex_unlock(&mtx);

		j->adressToFunction((void*) j);
	}
	//pthread_mutex_lock(&mtx);
	//cout << "Thread #" << threadID << " FINISHED" << endl;
	//pthread_mutex_unlock(&mtx);
	return NULL;
}

void JobScheduler::ExecuteJobs() {
	//cout << "Executing whole queue of jobs." << endl;
	runningThreads = executionThreads;
	queue_size = this->queue->GetSize();
	if (printArray == NULL)
		this->printArray = new int[queue_size];
	else {
		delete[] printArray;
		this->printArray = new int[queue_size];
	}
	// Assign tasks to empty Thread pool
	for( int i = 0; i < executionThreads; i++) {
		pthread_mutex_lock(&mtx);
		Job *j = queue->Dequeue();
		pthread_mutex_unlock(&mtx);
		if (j == NULL) { // den einai parallila ta threads opote
			// ta threads 0-2 kanoun oli tin douleia kai ola ta alla tpt
			pthread_mutex_lock(&mtx);
			runningThreads--;
			//cout << "MAIN::NULL::Thread #" << i << " wont even start!" << endl;
			pthread_mutex_unlock(&mtx);
			continue;
		}
		//j->printArr = this->printArray;
		j->id = i;
		//cout << "Assigning job to thread #" << i << endl;
		if (pthread_create(&tids[i],NULL,this->ExecuteThread,j) != 0)
			Psystem_error("pthread_create ");
	}
	//cout << "Waiting threads to finish" << endl;
	this->WaitAll();
}

void JobScheduler::WaitAll() {
	for( int i = 0; i < runningThreads; i++) {
		void *threadReturn;
        if (pthread_join(tids[i], &threadReturn) < 0) {
        	perror("pthread_join ");
        	exit(EXIT_FAILURE);
        }
       // cout << "Thread #" << tids[i] << " exited with exit code : "
        		//<< (uint8_t*)threadReturn << endl;
	}
	this->PrintResults();
	//cout << "ALL THREADS FINISHED NEXT RIPH" << endl;
}

void JobScheduler::PrintResults() {
	for (int i = 0 ; i < this->queue_size ; i++) {
		result << this->printArray[i] << "\n";
	}
}
