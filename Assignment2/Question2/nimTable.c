#include <stdio.h>
#include <stdlib.h>

#define MAX 0
#define MIN 1

#define INFINITY 9999999

typedef struct moveVal {
  int move;
  int val;
} MoveVal;

MoveVal *tTable;

MoveVal negamaxDecision(int state, int turn, MoveVal bestMove) {
  MoveVal m;
  int move,moveValue;
  m = bestMove;

  /* terminal state */
  if (state == 1) {
    bestMove.val = -1;
    return bestMove;
  }

  if (tTable[state].val != 0) {
    return tTable[state];
  }

  bestMove.val = -INFINITY;
  for (move = 1; move <= 3; move++){
    if (state - move > 0) {
      moveValue = -1 * negamaxDecision(state - move, turn, bestMove).val;

      if (moveValue > bestMove.val) {
        bestMove.val = moveValue;
        bestMove.move = move;
      }
    }
  }
  tTable[state] = bestMove;
  return bestMove;
}

void playNim(int state) {
  MoveVal temp;
  int action;
  int turn = 0;

  temp.val = -INFINITY;
  while (state != 1) {
    action = negamaxDecision(state, turn, temp).move;
    printf("%d: %s takes %d\n", state, 
           (turn == MAX ? "Max" : "Min"), action);
    state = state - action;
    turn = 1 - turn;
  }
  printf("1: %s loses\n", (turn == MAX ? "Max" : "Min"));
}

int main(int argc, char *argv[]) {
  if ((argc != 2) || (atoi(argv[1]) < 3)) {
    fprintf(stderr, "Usage: %s <number of sticks>, where ", argv[0]);
    fprintf(stderr, "<number of sticks> must be at least 3!\n");
    return -1;
  }
  
  tTable = calloc((atoi(argv[1])+1),sizeof(MoveVal));
  playNim(atoi(argv[1]));

  return 0;
}
