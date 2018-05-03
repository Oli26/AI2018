
#include <stdio.h>
#include <stdlib.h>
#include <PriorityQueue.h>

Position* createPosition(int setX, int setY, int setTurn){
	Position* position = malloc(sizeof(Position));
	position->x = setX;
	position->y = setY;
	position->turn = setTurn;
	return position;
	
}

Queue* createPriorityQueue(int maxS, int h, int fX, int fY){
	Queue* queue = malloc(sizeof(Queue));
	queue->anchor = malloc(sizeof(Link*));
	queue->size = 0;
	queue-> maxSize = maxS;
	queue->heuristic = h;
	queue->finalX = fX;
	queue->finalY = fY;
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
	}else{
		return 0;
	}
	
}

int valuePosition(int heuristic, Position* p, int fX, int fY){
		
	int xDiff;
	if(p->x > fX){
		xDiff = p->x-fX;
	}else{
		xDiff = fX-p->x;
	}
	int yDiff;
	if(p->y > fY){
		yDiff = p->y-fY;
	}else{
		yDiff = fY-p->y;
	}
	
	
	if(heuristic == 1){
	
		
		float squaredDistance = (xDiff*xDiff) + (yDiff*yDiff);
		double absDistance = sqrt(squaredDistance);
		return 	absDistance/sqrt(5.0) + p->turn;
		
		
	}else if(heuristic == 2){
		
		return xDiff+yDiff + p->turn;
	}
}

void enqueue(Queue* q, Position* p){
	if(isEmpty(q) == 1){
		//printf("Enqueuing position (%d,%d) to empty queue\n", p->x, p->y);
		q->anchor->link = malloc(sizeof(Link));
		q->anchor->link->position = createPosition(p->x, p->y, 0);
		q->anchor->link->value = valuePosition(q->heuristic, p, q->finalX, q->finalY);
		q->size++;
		return;
	}
	if(isFull(q) == 0){
		
		int valueNew = valuePosition(q->heuristic, p, q->finalX, q->finalY);
		
		
		printf("Enqueuing position (%d,%d) with turn = %d\n", p->x, p->y, p->turn);
		
		// Iterate through list until ready to be placed.
		Link* currentLink = q->anchor;
		
		if(currentLink->link->link == NULL){
			if(currentLink->link->value < valueNew){
				currentLink = currentLink->link;	
			}
		}else{
			while(currentLink->link->link != NULL && currentLink->link->value < valueNew){			
				currentLink = currentLink->link;
			}
			if(currentLink->link-> value < valueNew){
				currentLink = currentLink->link;
			}
		}
		
		
		
		// Save end of the queue.
		Link* tempLink = currentLink->link;
		// Create the new node.
		Link* newLink = malloc(sizeof(Link));
		newLink->position = createPosition(p->x, p->y, p->turn);
		newLink->value = valueNew;
		
		// Place the new node.
		currentLink->link = newLink;
		// Place back the end of the queue.
		currentLink->link->link = tempLink;
		q->size++;

	}
}


Position* dequeue(Queue* q){
	if(isEmpty(q) == 0){
		Position* returnPosition = q->anchor->link->position;
		q->anchor->link = q->anchor->link->link; 
		q->size--;
		return returnPosition;
	}
		
		
}

void printQueue(Queue* q){
	Link* currentLink = q->anchor->link;
	while(currentLink->link != NULL){
		printf("Position = (%d,%d), value = %d\n", currentLink->position->x ,currentLink->position->y, currentLink->value);
		currentLink = currentLink->link;
	}
	printf("Position = (%d,%d), value = %d\n", currentLink->position->x ,currentLink->position->y, currentLink->value);
}


int contains(Queue* q, int testX, int testY){
	Link* currentLink = q->anchor->link;
	while(currentLink != NULL){
		if(currentLink->position->x == testX && currentLink->position->y == testY){
			return currentLink->position->turn;
		}
		currentLink=currentLink->link;
	}
	return -1;
	
}


int usable(Queue* q, int testX, int testY, int turn){
	int value = contains(q, testX, testY);
	if(value == -1){
		return 1;
	}else{
		if(turn < value){
			return 1;
		}else{
			return 0;
		}
	}
}
