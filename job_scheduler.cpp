#include "job_scheduler.h"

using namespace std;


pthread_mutex_t mtx;
pthread_cond_t cond;
pthread_mutex_t pmtx;
pthread_cond_t print;


void JobInit(
		Job *job,
		void* (*adressToFunction)(void *),
		int source,int dest,int commandCounter,
		Index *index,Buffer *buffer,void *compPointer,JobScheduler *_js)
{
	job->adressToFunction = StaticQuery; // storing function call

	job->src = source;
	job->dest = dest;
	job->printPos = commandCounter;
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
	int repeat = j->version;
	int threadNum=j->id;
	IndexNode* p = index->GetIndexNode();
	//cout << "\tThread #" << j->id << " started "
	// << "S::" << j->src << "   D::" << j->dest << endl;

	/*pthread_mutex_lock(&mtx);
	pthread_mutex_unlock(&mtx);*/

	int k = strongCC->IsReachableGrail(index, source, dest);
	if (k == 0) {
		//pthread_mutex_lock(&mtx);
		//cout << "Thread #" << j->id << " -" << (*repeat) << "- @ " << j->printPos << endl;
		printArray[j->printPos] = -1;
		//pthread_mutex_unlock(&mtx);
	}
	else if (k == 1) {
		//result << "MAYBE";
		//pthread_mutex_lock(&mtx);
		//cout << "\tThread #" << j->id << " " << (*repeat) << " @ " << j->printPos << endl;
		printArray[j->printPos] =
				strongCC->EstimateShortestPathSCC(buffer,index,source,dest,repeat,threadNum);
		//(*repeat)++;
		//pthread_mutex_unlock(&mtx);
	} else if (k == 2) {
		//result << "YES";
/*
		pthread_mutex_lock(&pmtx);
		//cout << "Thread #" << j->id << " -" << (*repeat) << "- @ " << j->printPos << endl;
		printArray[j->printPos] = -1;
		pthread_mutex_unlock(&pmtx);
	}
	else if (k == 1) {
		//result << "MAYBE";
		pthread_mutex_lock(&pmtx);
		//cout << "\tThread #" << j->id << " " << (*repeat) << " @ " << j->printPos << endl;
		printArray[j->printPos] =
				strongCC->EstimateShortestPathSCC(buffer,index,source,dest,*repeat);
		(*repeat)++;
		pthread_mutex_unlock(&pmtx);
	} else if (k == 2) {
		//result << "YES";
		pthread_mutex_lock(&pmtx);*/
		//cout << "\tThread #" << j->id << " " << (*repeat) << " @ " << j->printPos << endl;
		//pthread_mutex_lock(&mtx);
		printArray[j->printPos] = buffer->Query(source, dest, index,
						p[source].componentID,repeat,threadNum);
		//(*repeat)++;
		//pthread_mutex_unlock(&mtx);
	}
	/*pthread_mutex_lock(&mtx);
	j->js->IncPrintNum();
	pthread_mutex_unlock(&mtx);*/
	delete j;
	return NULL;
}

void* JobScheduler::ExecuteThread(void *job) {
	Job *j = ((Job*) job);
	int threadID = j->id;
	Queue<Job*> *queue = j->js->GetQueue();
	JobScheduler *js = j->js;
	int repeat=0;
	int *running = js->GetRunningThreads();
	(*running)++;// +1 to avoid signalling cond_print
	/*pthread_mutex_lock(&mtx);
	cout << "\tThread #" << threadID << " was created!" << endl;
	pthread_mutex_unlock(&mtx);*/
	delete j;
	while(1) {
		pthread_mutex_lock(&mtx);
		if (!queue->isEmpty()) {
			j = queue->Dequeue();
			/*if (j == NULL) {
				//cout << "NULL::Thread #" << threadID << " exiting?" << endl;
				if (pthread_mutex_unlock(&mtx) != 0)
					Psystem_error("mtx unlock @ JobScheduler::SubmitJob ");
				continue;
			}*/

		//cout << "\tThread #" << threadID << " dequeue" << endl;
		}
		else {
			//pthread_mutex_lock(&mtx);
			if (js->IsFinished()) {
				pthread_mutex_unlock(&mtx);
				break;
			}
			else {
				(*running)--;
				if ((*running) == 0) {
					//pthread_mutex_lock(&pmtx);
					//cout << "\tThread #" << threadID << " signalling for print!" << endl;
					pthread_cond_signal(&print);
					//pthread_mutex_unlock(&pmtx);
				}
				//cout << "\tThread #" << threadID << " is waiting|EMPTY QUEUE! " << *running << endl;
				pthread_cond_wait(&cond,&mtx);
			}
			//cout << "\tThread #" << id << " exiting!" << endl;
			pthread_mutex_unlock(&mtx);
			continue;
		}
		pthread_mutex_unlock(&mtx);
		j->id=threadID;
		j->version=repeat;
		j->adressToFunction((void*) j);
		repeat++;
		//pthread_mutex_lock(&mtx);
		//cout << "\tThread #" << id << " retrieved from queue" << endl;
		//pthread_mutex_unlock(&mtx);
	}
	//pthread_mutex_lock(&mtx);
	//cout << "Thread #" << threadID << " destroyed" << endl;
	//pthread_mutex_unlock(&mtx);
	return NULL;
}

JobScheduler::JobScheduler(int threadpool) {
	executionThreads = threadpool;
	queue = new Queue<Job*>();
	lastFinishedThreadID = -1;
	printArray = NULL;
	queue_size = 0;
	runningThreads = new int(threadpool);
	finished = false;
	result.open(OUTPUT_FILE,ios::app); //output file for Queries
	tids = new pthread_t[executionThreads];
	if (pthread_mutex_init(&pmtx, NULL) != 0)
		Psystem_error("mutex init ");
	if (pthread_mutex_init(&mtx, NULL) != 0)
		Psystem_error("mutex init ");
	if (pthread_cond_init(&cond, NULL) != 0)
		Psystem_error("cond init ");
	if (pthread_cond_init(&print, NULL) != 0)
		Psystem_error("cond init ");
	for( int i = 0; i < executionThreads; i++) {
		Job *job = new Job();
		job->js = this;
		job->id = i;
		if (pthread_create(&tids[i],NULL,this->ExecuteThread,job) != 0)
			Psystem_error("pthread_create ");
	}
}

JobScheduler::~JobScheduler() {
	delete queue;
	delete[] tids;
	result.close();
	if ((pthread_mutex_destroy(&mtx)) != 0)
		Psystem_error("mutex destroy mtx");
	if ((pthread_cond_destroy(&cond)) != 0)
		Psystem_error("cond destroy cond");
	if ((pthread_mutex_destroy(&pmtx)) != 0)
		Psystem_error("mutex destroy pmtx");
	if ((pthread_cond_destroy(&print)) != 0)
		Psystem_error("cond destroy print");

}

void JobScheduler::SubmitJob(Job *j) {
	if (pthread_mutex_lock(&mtx) != 0)
		Psystem_error("mtx lock @ JobScheduler::SubmitJob ");
	queue->Enqueue(j);
	if (pthread_mutex_unlock(&mtx) != 0)
		Psystem_error("mtx unlock @ JobScheduler::SubmitJob ");
}
void JobScheduler::ExecuteJobs() {
	//cout << "Executing whole queue of jobs." << endl;
	(*runningThreads) = executionThreads;
//>>>>>>> 1d98db289583e732e63cb5019f61fe595fc1562b
	queue_size = this->queue->GetSize();
	if (printArray == NULL)
		this->printArray = new int[queue_size];
	else {
		delete[] printArray;
		this->printArray = new int[queue_size];
	}
	//found_empty=0;
	// Assign tasks to empty Thread pool
	pthread_mutex_lock(&mtx);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mtx);
	this->WaitAll();
}


void JobScheduler::WaitAll() {
	pthread_mutex_lock(&mtx);
	pthread_cond_wait(&print,&mtx);
	pthread_mutex_unlock(&mtx);

	this->PrintResults();
}

void JobScheduler::DestroyAll() {
	finished = true;
	cout << "Destroying all threads" << endl;
	pthread_mutex_lock(&mtx);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&mtx);
	for( int i = 0; i < executionThreads; i++) {
		void *threadReturn;
        if (pthread_join(tids[i], &threadReturn) < 0) {
        	perror("pthread_join ");
        	exit(EXIT_FAILURE);
        }
        cout << "Thread #" << i << " ended." << endl;
	}
	//this->PrintResults();
	//cout << "ALL THREADS FINISHED NEXT RIPH" << endl;
}

void JobScheduler::PrintResults() {
	for (int i = 0 ; i < this->queue_size ; i++) {
		result << this->printArray[i] << "\n";
	}
}
