#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#define numBlocks 20
#define chromLength numBlocks
#define popSize 10

int blocks[numBlocks];
bool generation[popSize][chromLength];

int max(int a, int b) {
	return (a > b ? a : b);
}

void readBlocks() {
	for(int i = 0; i < numBlocks; i++) {
		scanf("%d", blocks + i);
	}
}

// create a random chromosome
void rndChrom(bool *chromosome) {
	for(int i = 0; i < chromLength; i++) {
		chromosome[i] = (rand() > (RAND_MAX / 2));
	}
}

// creates random population (generation)
void rndGen() {
	for(int i = 0; i < popSize; i++) {
		rndChrom(generation[i]);
	}
}

void mutate(bool *chromosome) {
	chromosome[(rand() % 20)] = !(chromosome[(rand() % 20)]);
}

// functions for biologically accurate crossover {
void crossSwap(bool *chromosomeA, bool *chromosomeB, int c1, int c2) {
	int tmp;
	bool swap;

	if(c2 < c1){
		tmp = c2;
		c2 = c1;
		c1 = tmp;
	}

	for(int i = c1; i < c2; i++) {
		swap = chromosomeB[i];
		chromosomeB[i] = chromosomeA[i];
		chromosomeA[i] = swap;
	}
}

void cross(bool *chromosomeA, bool *chromosomeB) {
	int i;

	// Random number of cuts
	int cutNum = (rand() % 20);

	cutNum = (cutNum > 0 ? cutNum : 1);

	int *cuts = malloc(cutNum*sizeof(int));

	// Fill in random cutting locations
	for(i = 0; i < cutNum; i++) {
		cuts[i] = (rand() % 20);
	}

	if(cutNum == 1) {
		crossSwap(chromosomeA, chromosomeB, cuts[0], chromLength - 1);
	} else {
		for(i = 0; i < cutNum - 1; i++) {
			crossSwap(chromosomeA, chromosomeB, cuts[i], cuts[i + 1]);
		}
	}
}
// }

// copy chromosome a into b
void copyChromosome(bool *a, bool *b) {
	for(int i = 0; i < chromLength; i++) {
		b[i] = a[i];
	}
}

int heightOfTower(bool *chromosome, bool b) {
	int height = 0;
	for(int i = 0; i < numBlocks; i++) {
		if(b) {
			if(chromosome[i]) {
				height += blocks[i];
			}
		} else {
			if(!(chromosome[i])) {
				height += blocks[i];
			}
		}
	}
	return height;
}

int getFitness(bool *chromosome) {
	int h = heightOfTower(chromosome, true) - heightOfTower(chromosome, false);
	return abs(h);
}

int bestFitness() {
	int min = 0;
	for(int i = 1; i < popSize; i++) {
		if(getFitness(generation[i]) < getFitness(generation[min])) {
			min = i;
		}
	}
	return min;
}

float averageFitness() {
	int sum = 0;
	for(int i = 0; i < popSize; i++) {
		sum += getFitness(generation[i]);
	}
	return sum/popSize;
}

// printing functions
void printChrom(bool *chromosome) {
	for(int i = 0; i < chromLength; i++) {
		printf("%d ", chromosome[i]);
	}
	printf("\n");
}

void printBlocks() {
	for(int i = 0; i < numBlocks; i++) {
		printf("%d ", blocks[i]);
	}
	printf("\n");
}

// print partitioned towers using given char
void printTowers(bool *chromosome, char c) {
	// get maximum number of lines to print
	int h1 = heightOfTower(chromosome, true), h2 = heightOfTower(chromosome, false);
	int h = max(h1, h2);

	// inner width of towers (excluding walls)
	int width = 8;

	// space between towers
	int space = 5;

	int h1b = 0, h2b = 0;

	// get blocks for each tower in seperate arrays
	int *trues = malloc(numBlocks*sizeof(int)), *falses = malloc(numBlocks*sizeof(int));;

	int idxT = 0, idxF = 0;
	for(int i = 0; i < chromLength; i++) {
		if(chromosome[i]) {
			trues[idxT] = blocks[i];
			idxT++;
		} else {
			falses[idxF] = blocks[i];
			idxF++;
		}
	}

	// first block is shorter due to extra space needed by block seperations
	// add one to compensate
	int countA = trues[0] + 1, countB = falses[0] + 1;

	idxT = 1;
	idxF = 1;

	// from top to bottom...
	for(int i = h; i >= 0; i--) {

		// only print tower if at the proper height
		h1b = (h1 >= i);
		h2b = (h2 >= i);

		// get next block height (number of lines to count till seperator)
		if(countA <= 0) {
			countA = trues[idxT];
			idxT++;
		}
		if(countB <= 0) {
			countB = falses[idxF];
			idxF++;
		}


		// left wall of first tower
		if(h1b) {
			printf("%c", c);
		} else {
			printf(" ");
		}

		for(int j = 0; j < width; j++) {
			// print seperator if at end of block
			if((countA <= 1 && h1b) || i == h1 || i == 0) {
				printf("%c", c);
			} else {
				printf(" ");
			}
		}

		// right wall of first tower
		if(h1b) {
			printf("%c", c);
		} else {
			printf(" ");
		}

		// space between towers
		for(int j = 0; j < space; j++) {
			printf(" ");
		}

		// left wall of second tower
		if(h2b) {
			printf("%c", c);
		} else {
			printf(" ");
		}

		for(int j = 0; j < width; j++) {
			// print seperator if at end of block
			if((countB <= 1 && h2b) || i == h2 || i == 0) {
				printf("%c", c);
			} else {
				printf(" ");
			}
		}

		// right wall of second tower
		if(h2b) {
			printf("%c", c);
		} else {
			printf(" ");
		}

		// count lines
		if(h1b) {
			countA--;
		}
		if(h2b) {
			countB--;
		}
		printf("\n");
	}

	free(trues);
	free(falses);
}

void simulate(int iterations) {
	for(int i = 0; i < iterations; i++) {
		int average = averageFitness();
		int *dead = malloc(popSize*sizeof(int)), *alive = malloc(popSize*sizeof(int));
		int deadIdx = 0, aliveIdx = 0;

		// seperate population into dead and alive based on average fitness
		// (selection)
		for(int j = 0; j < popSize; j++) {
			if(getFitness(generation[j]) >= average) {
				dead[deadIdx] = j;
				deadIdx++;
			} else {
				alive[aliveIdx] = j;
				aliveIdx++;
			}
		}

		// replace dead chromosomes with copies of living ones
		// (reproduction)
		int numAlive = aliveIdx + 1;
		for(int j = 0; j <= deadIdx; j++) {
			if(numAlive != 0) {
				aliveIdx = j % numAlive;
				copyChromosome(generation[alive[aliveIdx]], generation[dead[j]]);
			}
		}

		// mutate randomly
		for(int j = 0; j < popSize; j++) {
			if(rand() > (RAND_MAX / popSize)) {
				mutate(generation[j]);
			}
		}

		// crossover with neighbors randomly
		for(int j = 0; j < popSize - 1; j++) {
			if(rand() > (RAND_MAX / popSize)) {
				cross(generation[j], generation[j + 1]);
			}
		}

		free(dead);
		free(alive);
	}

	printTowers(generation[bestFitness()], '#');
	printf("Difference: %d\n", getFitness(generation[bestFitness()]));
}

int main(int argc, char *argv[]) {
	// use milliseconds and seconds for higher frequency seeding
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand(t1.tv_usec * t1.tv_sec);

	// get blocks from input
	readBlocks();

	// initial generation
	rndGen();

	// iterate population and print best found partitioning
	simulate(1000);

	return 0;
}
