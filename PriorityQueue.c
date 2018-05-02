
#include <stdio.h>
#include <stdlib.h>
#include <PriorityQueue.h>

Position* createPosition(int x, int y){
	Position* position = malloc(sizeof(Position));
	position->x = x;
	position->y = y;
	return position;
	
}

Queue* createPriorityQueue(int maxS){
	Queue* queue = malloc(sizeof(PriorityQueue));
	queue->front = 0; 
	queue->size = 0;
	queue-> maxSize = maxS;
	queue->back = 0;
	
	return queue;
}

int isFull(Queue* q){
	if(q->size == q->maxSize){
		return 1;
	}
	return 0;
}

int isEmpty(Queue* q){
	if(q->size == 0){
		return 1;
	else{
		return 0;
	}
	
}

void enqueue(Queue* q, Position p){
	printf("Running1\n");
	if(isFull(q) == 0){
		printf("Running2\n");
		q->back = (q->back+1) % q->maxSize;
		q->array[q->back].x = p.x;
		q->array[q->back].y = p.y;
		q->size++;
	}
}


Position dequeue(Queue* q){
	if(isEmpty(q) == 0){
		Position returnPosition = q->array[q->front]; 
		q->front == (q->front+1) % q->maxSize;
		q->size--;
		return returnPosition;
	}
		
		
}

void printQueue(Queue* q){
	int relativePosition = 0;
	while(relativePosition != q->size){
		printf("(%d,%d)\n", q->array[(q->front+relativePosition)%q->maxSize));
	}
}
