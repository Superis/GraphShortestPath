class queue
{
	int node;
	queue* next;
public:
	queue(int tnode);
	~queue();
	void enqueue(int tsock);
	int dequeue();
	void change_next(int tsock);
	queue* get_next();
	int get_node();
	int front();
	void print();
};