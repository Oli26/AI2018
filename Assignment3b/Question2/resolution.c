#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Assumption: the propositional symbols are a, b, .., z.
 * So, each propositional formula contains at most 26 
 * different variables. Sets of variables can therefore 
 * be represented by 26 bits, hence an integer (bitstring 
 * of 32 bits) suffices.
 */

#define FALSE 0
#define TRUE  1

typedef unsigned int bitset;

typedef struct clause {
  bitset positive;   /* bitset of positive symbols in clause */
  bitset negative;   /* bitset of negative symbols in clause */
} clause;

typedef struct clauseSet {
  int size;      /* number of clauses in set                    */
  int allocated; /* maximum number of clauses (allocated space) */
  clause *clauses;
} clauseSet;

/******** ADT for clauses ***************************************/

int isEmptyClause(clause c) {
  /* returns TRUE if and only if the clauses c is empty */
  return ((c.positive == 0) && (c.negative == 0) ? TRUE : FALSE);
}

void makeEmptyClause(clause *c) {
  /* makes c the empty clause (i.e. false) */
  c->positive = 0;
  c->negative = 0;
}

int expectLetter(char c) {
  /* helper routine for parsing clause strings in makeClause() */
  if ((c < 'a') || (c > 'z')) {
    fprintf(stderr, "Syntax error: expected a letter ('a'..'z')\n");
    exit(EXIT_FAILURE);
  }
  return c - 'a';
}

void makeClause(clause *c, char *cstr) {
  /* Converts the string cstr into a clause.
   * For example, the clause {a,b,~c} is represented 
   * by the string "a,b,~c".
   */
  int n, idx = 0;
  int pow2[] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,
                32768,65536,131072,262144,524288,1048576,2097152,4194304,
                8388608,16777216,33554432};
  makeEmptyClause(c);
  while (cstr[idx] != '\0') {
    if (cstr[idx] == '~') {
      /* negative literal */
      idx++;
      n = expectLetter(cstr[idx++]);
      c->negative |= pow2[n];
    } else {
      /* positive literal */
      n = expectLetter(cstr[idx++]);
      c->positive |= pow2[n];
    }
    if (cstr[idx] == ',') {
      idx++;
      if (cstr[idx] == '\0') {
        fprintf(stderr, "Syntax error: truncated clause\n");
	exit(EXIT_FAILURE);
      }
    }
  }
}

int areEqualClauses(clause a, clause b) {
  /* returns TRUE if and only if the clauses a and b are the same */
  if ((a.positive == b.positive) && (a.negative == b.negative)) {
    return TRUE;
  }
  return FALSE;
}

void printClause(clause c) {
  /* prints clause c on standard output */
  int i, mask, trueflag, comma=0;
  printf("[");
  trueflag = 0;
  for (i=0,mask=1; i < 26; i++,mask*=2) {
    int cnt = 0;
    if (c.positive & mask) {
      if (comma) putchar(',');
      putchar('a'+i);
      comma = 1;
      cnt++;
    }
    if (c.negative & mask) {
      if (comma) putchar(',');
      printf("~%c", 'a'+i);
      comma = 1;
      cnt++;
    }
    trueflag |= (cnt == 2);
  }
  printf("]");
  if (trueflag) {
    printf("=TRUE");
  } else {
    if (isEmptyClause(c)) {
      printf("=FALSE");
    }
  }
}

void printlnClause(clause c) {
  /* prints clause c followed by a newline on standard output */
  printClause(c);
  putchar('\n');
}

/******** ADT for clauseSets ************************************/

void freeClauseSet(clauseSet set) {
  /* releases the memory allocated for set */
  free(set.clauses);
}

void makeEmptyClauseSet(clauseSet *set) {
  /* makes an empty clause set */
  set->size = 0;
  set->allocated = 0;
  set->clauses = NULL;
}

int isEmptyClauseSet(clauseSet s) {
  /* returns TRUE if and only if s is and empty set of clauses */
  return (s.size == 0 ? TRUE : FALSE);
}

int findIndexOfClause(clause c, clauseSet s) {
  /* returns index of clause c in set s, or -1 if c is not found */
  int i = 0;
  while (i < s.size && !areEqualClauses(s.clauses[i], c)) {
    i++;
  }
  return (i < s.size ? i : -1);
}

int isElementOfClauseSet(clause c, clauseSet s) {
  /* returns TRUE if and only if c is in set s */
  return (findIndexOfClause(c, s) == -1  ? FALSE : TRUE);
}

int containsEmptyClause(clauseSet s) {
  /* returns TRUE if and only if the set s contains the empty clause */
  clause empty;
  makeEmptyClause(&empty);
  return isElementOfClauseSet(empty, s);
}

int isClauseSubset(clauseSet a, clauseSet b) {
  /* returns TRUE if and only if a is a subset of b */
  int i;
  if (b.size < a.size) {
    return FALSE;
  }
  for (i=0; i < a.size; i++) {
    if (!isElementOfClauseSet(a.clauses[i], b)) {
      return FALSE;
    }
  }
  return TRUE;
}

void insertInClauseSet(clause c, clauseSet *s) {
  /* inserts clause s in set s */
  if (isElementOfClauseSet(c, *s)) {
    return;   /* clause was already in set */
  }
  if (s->size == s->allocated) {
    /* reallocation needed. */
    s->allocated += 128;
    s->clauses = realloc(s->clauses, s->allocated*sizeof(clause));
  }
  s->clauses[s->size++] = c;
}

void unionOfClauseSets(clauseSet *a, clauseSet b) {
  /* implements: a = union(a,b) */
  int i;
  for (i=0; i < b.size; i++) {
    insertInClauseSet(b.clauses[i], a);
  }
}

void crossClauses(clause a, clause b, clauseSet *rsv) {
  /* returns in rsv the set of clauses that are produced by 
   * resolving the positive literals of a with the negative literals
   * of b. Note that rsv must be an empty set, before 
   * calling this function.
   */
  int crossing = a.positive & b.negative;
  int mask = 1;
  while (crossing != 0) {
    if (crossing & mask) {
      clause c;
      c.positive = (a.positive | b.positive) & (~mask);
      c.negative = (a.negative | b.negative) & (~mask);
      insertInClauseSet(c, rsv);
    }
    crossing &= ~mask;
    mask *= 2;
  }
}

void printClauseSet(clauseSet s) {
  /* prints set of clauses s on standard output */  
  int i;
  printf("{");
  if (s.size > 0) {
    printClause(s.clauses[0]);
    for (i=1; i < s.size; i++) {
      printf(", ");
      printClause(s.clauses[i]);
    }
  }
  printf("}");
}

void printlnClauseSet(clauseSet s) {
  /* prints set of clauses s followed by a newline on standard output */  
  printClauseSet(s);
  putchar('\n');
}

/******** Main program ***************************************/

void resolveClauses(clause a, clause b, clauseSet *rsv) {
  /* returns the resolvents of the clauses a and b in the set rsv */
  makeEmptyClauseSet(rsv);
  crossClauses(a, b, rsv);
  crossClauses(b, a, rsv);
}

void resolution(clauseSet *kb) {
  /* Extends the kb with rules that can be inferred by resolution.
   * The function returns, as soon as it inferred the empty 
   * clause (i.e. false). The function also returns, if all possible
   * resolvents have been computed.
   */
  while (!containsEmptyClause(*kb)) {
    int i, j;
    clauseSet inferred;
    makeEmptyClauseSet(&inferred);
    for (i=0; i < kb->size; i++) {
      for (j=i+1; j < kb->size; j++) {
        clauseSet resolvents;
	resolveClauses(kb->clauses[i], kb->clauses[j], &resolvents);
	unionOfClauseSets(&inferred, resolvents);
	freeClauseSet(resolvents);
      }
    }
    if (isClauseSubset(inferred, *kb)) {
      break; /* No new clauses found */
    }
    unionOfClauseSets(kb, inferred);
    freeClauseSet(inferred);
  }
}
void init(clauseSet *s, char* input){
	makeEmptyClauseSet(s);
	int i;
	char* clauseHolder;
	for(i = 0; i < strlen(input); i++){
		
		clauseHolder = malloc(1*sizeof(char));
		int index=0;
		// pass end bracket or first bracket
		i++;
		// pass comma or second 
		i++;
		if(i > strlen(input)){
			return;
		}
		while(input[i] != ']'){

			clauseHolder[index] = input[i];
			index++;
			i++;
			clauseHolder = realloc(clauseHolder, (index+1)*sizeof(char));
		}
		
		clause c;
		makeClause(&c, clauseHolder);
		insertInClauseSet(c, s);
	}
}

void findParents(int childIndex, int *a, int *b, clauseSet s){
	int i,j;
	clauseSet children;
	for(i = 0; i<childIndex; i++){
		for(j = 0; j<childIndex; j++){
			resolveClauses(s.clauses[i], s.clauses[j], &children);
			if(isElementOfClauseSet(s.clauses[childIndex], children)){
				*a = i;
				*b = j;
				return;
			} 
		}
	}
}
void printStack(int idx, int *a, int *b, clauseSet s, clauseSet startSet){
	idx = *a;
	int tempa = 0;
	int tempb = 0;
	int print = 0;
	if(!isElementOfClauseSet(s.clauses[idx], startSet)){
		print = 1;
		findParents(idx, a, b, s);
		tempa = *a;
		tempb = *b;
		printStack(idx, a, b, s, startSet);
	}
	idx = *b;
	if(!isElementOfClauseSet(s.clauses[idx], startSet)){
		print = 1;
		findParents(idx, a, b, s);
		tempa = *a;
		tempb = *b;
		printStack(idx, a, b, s, startSet);
	}
	if(print){	
		printClause(s.clauses[idx]);
		printf(" is infered from ");
		printClause(s.clauses[tempa]);
		printf(" , ");
		printClause(s.clauses[tempb]);
		printf("\n");		
	}
}

void recursivePrintProof(int idx, clauseSet s, clauseSet startSet) {
  int a;
  int b;
  findParents(idx, &a, &b, s);
  int lasta = a;
  int lastb = b;
  int lastidx = idx;
  printStack(idx, &a, &b, s, startSet);
  printClause(s.clauses[lastidx]);
  printf(" is infered from ");
  printClause(s.clauses[lasta]);
  printf(" , ");
  printClause(s.clauses[lastb]);
  printf("\n");
  
}

void printProof(clauseSet s, clauseSet startSet) {
  int idx;
  clause empty;
  makeEmptyClause(&empty);
  idx = findIndexOfClause(empty, s);
  recursivePrintProof(idx, s, startSet);
}
	       
int main(int argc, char *argv[]) {
  clauseSet kb;
  clauseSet startSet;
  init(&kb, argv[1]);
  init(&startSet, argv[1]);
  printf("KB=");
  printlnClauseSet(kb);
  resolution(&kb);
  printf("KB after resolution=");
  printlnClauseSet(kb);
  if (containsEmptyClause(kb)) {
    printf("Resolution proof completed.\n");
    printf("\nProof:\n");
    printProof(kb, startSet);
  } else {
    printf("Resolution proof failed.\n");
  }
  freeClauseSet(kb);
  return EXIT_SUCCESS;
}
