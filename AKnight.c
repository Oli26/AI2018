
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define N 500   /* N times N chessboard */
 
 
 
////////// THIS SHOULD BE PriorityQueue.h


typedef struct PriorityQueue Queue;
typedef struct Position Position;
typedef struct Link Link;

struct Link{
	Position* position;
	Link* link;
	int value;
};

struct PriorityQueue{
	Link* anchor;
	int size, maxSize, heuristic;
	int finalX, finalY;
};


struct Position{
	int x;
	int y;
	int turn;
};


////////////////////////////////////////////



////////// THIS SHOULD BE  PriorityQueue.c 

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
	
		return 	xDiff + yDiff;
		
		
	}else if(heuristic == 2){
		
		return xDiff+yDiff + p->turn;
	}
}

void enqueue(Queue* q, Position* p){
	if(isEmpty(q) == 1){
		q->anchor->link = malloc(sizeof(Link));
		q->anchor->link->position = createPosition(p->x, p->y, 0);
		q->anchor->link->value = valuePosition(q->heuristic, p, q->finalX, q->finalY);
		q->size++;
		return;
	}
	if(isFull(q) == 0){
		
		int valueNew = valuePosition(q->heuristic, p, q->finalX, q->finalY);
		
		
		// printf("Enqueuing position (%d,%d) with turn = %d\n", p->x, p->y, p->turn);
		
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

//////////////////////////////////////////////////////////////




int actions[8][2] = {  /* knight moves */
  {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1,-2}, {1,2}, {2, -1}, {2, 1}
};
int costShortestPath[N][N];
unsigned long statesVisited = 0;

int isValidLocation(int x, int y) {
  return (0<=x && x < N && 0<= y && y < N);
}


void attemptedEnqueue(Queue* openQueue, Queue* closedQueue, int newX, int newY, int nextTurn){
	int result1 = contains(openQueue, newX, newY);
	int result2 = contains(closedQueue, newX, newY);
	
	if(result1 == -1 && result2 == -1){
		enqueue(openQueue, createPosition(newX, newY, nextTurn));
	}else{
		if(result1 != -1 && result2 != -1){
			if(result2 > nextTurn && result1 > nextTurn){
				enqueue(openQueue, createPosition(newX, newY, nextTurn));
				printQueue(openQueue);
			}
		}
		if(result1 != -1){
			if(result1 > nextTurn){
				enqueue(openQueue, createPosition(newX, newY, nextTurn));
			}
		}
		if(result2 != -1){
			if(result2 > nextTurn){
				enqueue(openQueue, createPosition(newX, newY, nextTurn));
			}
		}
	}
}	


int knightA(int X, int Y, int finalX, int finalY, int h){

	int limit = 10000;
	int heuristicType = h;
	Queue* q = createPriorityQueue(limit,heuristicType,finalX,finalY);
	Queue* closedQueue = createPriorityQueue(limit,heuristicType, finalX, finalY);
	 
	
	Position* initalPosition = createPosition(0,0,0);
	enqueue(q,initalPosition);
	
	
	while(!isEmpty(q)){
		statesVisited++;
		Position* dequeuedPosition = dequeue(q);
		enqueue(closedQueue, dequeuedPosition);
		int nextTurn = dequeuedPosition->turn+1;
		for(int i=0;i<8;i++){
			int newX = dequeuedPosition->x + actions[i][0];
			int newY = dequeuedPosition->y + actions[i][1];
			
			if(newX == finalX && newY == finalY){
						printf("FOUND! (%d,%d)\n", newX, newY);
						return nextTurn;
			}
			
			if(isValidLocation(newX, newY)){
				attemptedEnqueue(q, closedQueue, newX, newY, nextTurn);
			}
			
			
		}
	}
}

int main(int argc, char *argv[]) {
	int x0,y0, x1,y1;
	do {
		printf("Start location (x,y) = "); fflush(stdout);
		scanf("%d %d", &x0, &y0);
	} while (!isValidLocation(x0,y0));
	do {
		printf("Goal location (x,y)  = "); fflush(stdout);
		scanf("%d %d", &x1, &y1);
	} while (!isValidLocation(x1,y1));
	
	int h;
	printf("Chose heuristic 1 or 2\n");
	scanf("%d", &h);

	printf("Length shortest path: %d\n", knightA(x0,y0, x1,y1, h));
	printf("#visited states: %lu\n", statesVisited);
	

	return 0;
}
