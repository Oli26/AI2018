#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

 
typedef struct PriorityQueue Queue;
typedef struct Position Position;


struct PriorityQueue{
	Position* array;
	int front, back, size, maxSize;
};


struct Position{
	int x;
	int y;
};

int isFull(Queue* q);
int isEmpty(Queue* q);
void enqueue(Queue* q, Position p);
Position dequeue(Queue* q);
Queue createPriorityQueue(int maxS);
Position* createPosition(int x, int y);
#endif


