#include <stdio.h>
#include <stdlib.h>

#define MAX 1
#define MIN -1

#define INFINITY 9999999

typedef struct moveVal {
  int move;
  int val;
} MoveVal;

MoveVal negamaxDecision(int state, int turn, MoveVal bestMove) {
  MoveVal m;
  m = bestMove;

  /* terminal state */
  if (state == 1) {
    bestMove.val = turn;
    return bestMove;
  }

  bestMove.val = -INFINITY;
  for (int move = 1; move <= 3; move++){
    if (state - move > 0) {

      state = state - move;
      m = negamaxDecision(state, turn, m);
      // printf("%d - %d\n",state,move);
      m.val *= -1;
      if (m.val > bestMove.val) {
        bestMove.val = turn;
        bestMove.move = m.move;
      }
    }
  }
  // printf("%d - %s - %d - %d\n",state,(turn == MAX ? "Max" : "Min"),bestMove.move,bestMove.val);
  return bestMove;
}

void playNim(int state) {
  MoveVal temp;
  MoveVal action;
  int turn = 1;

  action.move = 1;
  action.val = -INFINITY;
  temp = action;
  while (state != 1) {
    action = negamaxDecision(state, turn, temp);
    printf("%d: %s takes %d (valuation: %d)\n", state, 
           (turn == MAX ? "Max" : "Min"), action.move, action.val);
    state = state - action.move;
    turn *= -1;
  }
  printf("1: %s loses\n", (turn == MAX ? "Max" : "Min"));
}

int main(int argc, char *argv[]) {
  if ((argc != 2) || (atoi(argv[1]) < 3)) {
    fprintf(stderr, "Usage: %s <number of sticks>, where ", argv[0]);
    fprintf(stderr, "<number of sticks> must be at least 3!\n");
    return -1;
  }

  playNim(atoi(argv[1]));

  return 0;
}
