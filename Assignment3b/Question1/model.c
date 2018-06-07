#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXKBSIZE          1024
#define MAXIDENTIFIERS     1024
#define MAXIDENTNAMELENGTH   30

#define FALSE      0
#define TRUE       1

#define CONSTANT   0
#define IDENTIFIER 1
#define EQUIV      2
#define IMPLIES    3
#define AND        4
#define OR         5
#define NEG        6

typedef struct Expression {
  int operator;
  int atom;          /* only used if operator is CONSTANT or IDENTIFIER     */
  struct Expression *operand1; /* only used if expression is not an atom */
  struct Expression *operand2; /* only used if operator is binary        */
} *Expression;

int linenr = 1;
int colnr = 0;
int curchar;

int kbSize, inferSize;
Expression kb[MAXKBSIZE], infer[MAXKBSIZE];
int model[MAXIDENTIFIERS];
int inferred;

int cntidents = 0;
char identifiers[MAXIDENTIFIERS][MAXIDENTNAMELENGTH+1];

Expression makeConstantExpression(int value) {
  Expression e = malloc(sizeof(struct Expression));
  e->operator = CONSTANT;
  e->atom = value;
  e->operand1 = e->operand2 = NULL;  /* not used */
  return e;
}

Expression makeIdentifier(char *ident) {
  int i = 0;
  Expression e;
  if (strcmp("false", ident)==0) return makeConstantExpression(FALSE);
  if (strcmp("true", ident)==0) return makeConstantExpression(TRUE);

  /* is this a new identifier? */
  while ((i < cntidents) && (strcmp(identifiers[i], ident))) i++;
  if (i == cntidents) {
    /* new identifier, insert in symbol table */
    strncpy(identifiers[i], ident, MAXIDENTNAMELENGTH);
    identifiers[i][MAXIDENTNAMELENGTH-1]='\0';
    cntidents++; 
  }
  e = malloc(sizeof(struct Expression));
  e->operator = IDENTIFIER;
  e->atom = i;
  e->operand1 = e->operand2 = NULL;  /* not used */
  return e;
}

Expression makeNegation(Expression arg) {
  Expression e = malloc(sizeof(struct Expression));
  e->operator = NEG;
  e->atom = 99;  /* not used */
  e->operand1 = arg;
  e->operand2 = NULL;  /* not used */
  return e;
}

Expression makeBinaryExpr(int op, Expression arg1, Expression arg2) {
  Expression e = malloc(sizeof(struct Expression));
  e->operator = op;
  e->atom = 99;  /* not used */
  e->operand1 = arg1;
  e->operand2 = arg2;
  return e;
}

void printExpression(Expression e) {
  if (e->operator == CONSTANT) {
    printf ("%s", (e->atom==TRUE ? "true" :"false")); 
    return;
  }
  if (e->operator == IDENTIFIER) {
    printf("%s", identifiers[e->atom]);
    return;
  }
  if (e->operator == NEG) {
    printf ("!");
    printExpression(e->operand1);
    return;
  }
 
  printf("(");
  printExpression(e->operand1);
 
  switch(e->operator) {
  case EQUIV:
    printf (" <=> ");
    break;
  case IMPLIES:
    printf (" => ");
    break;
  case AND:
    printf ("*");
    break;
  case OR:
    printf (" + ");
    break;
  }
  printExpression(e->operand2);
  printf(")");
}

void showExpSet(char *name, int size, Expression expset[]) {
  int i;
  printf("%s = [\n", name);
  for (i=0; i + 1 < size; i++) {
    printf("  ");
    printExpression(expset[i]);
    printf(" ;\n");
  }
  printf("  ");
  printExpression(expset[i]);
  printf("\n]\n");
}

int evaluateExpression(Expression e) {
  switch(e->operator) {
  case CONSTANT:
    return (e->atom==TRUE ? 1 : 0);
  case IDENTIFIER:
    return model[e->atom];
  case EQUIV:
    return (evaluateExpression(e->operand1) == evaluateExpression(e->operand2));
  case IMPLIES:
    return ((!evaluateExpression(e->operand1)) || evaluateExpression(e->operand2));
  case AND:
    return (evaluateExpression(e->operand1) && evaluateExpression(e->operand2));
  case OR:
    return (evaluateExpression(e->operand1) || evaluateExpression(e->operand2));
  case NEG:
    return (!evaluateExpression(e->operand1));
  }
  /* we should never get here */
  printf("Houston, we've got a problem\n");
  return 0;
}

int evaluateExpressionSet(int size, Expression expset[]) {
  int i;
  for (i=0; i < size; i++) {
    if (evaluateExpression(expset[i]) == 0) {
      return 0;
    }
  }
  return 1;
}

int nextchar(int skipspaces) {
  do {
    curchar = getchar();
    colnr++;
    if (curchar == '\n') {
      linenr++;
      colnr = 0;
    }
  } while ((skipspaces) && 
	   ((curchar == ' ') || (curchar == '\t') || (curchar == '\n')));
  curchar = tolower(curchar);
  return curchar;
}

void showLocation() {
  printf("line %d (column %d): ", linenr, colnr);
}

void matchFailure(char *str) {
  showLocation();
  printf("parsing failed, expected '%s' ", str);
  printf("[failed on character '");
  if (curchar > ' ') putchar(curchar);
  else if (curchar == EOF) printf("EOF");
  else printf("chr(%d)", (int)(curchar));
  printf("'].\n");
  exit(EXIT_FAILURE);
}

void match(char *str) {
  int i;
  if (curchar != str[0]) matchFailure(str);
  for (i=1; str[i] != '\0'; i++) {
    if (nextchar(FALSE) != str[i]) matchFailure(str);
  }
  nextchar(TRUE);
}

void matchIdentifier(char *ident) {
  match(ident);
  if (isalnum(curchar)) matchFailure(ident);
}

Expression parseEquivalence();  /* forward decl. (mutual recursion) */

Expression parseAtom() {
  char id[MAXIDENTNAMELENGTH];
  int i=0;
  
  /* identifier/true/false */
  if (!isalpha(curchar)) {
    showLocation();
    printf("parse error, expected false, true, identifier or (expression)\n");
    exit(EXIT_FAILURE);
  }
  
  while ((i < MAXIDENTNAMELENGTH) && (isalnum(curchar))) {
    id[i++] = curchar;
    nextchar(FALSE);
  }
  id[i] = '\0';
  if (i > MAXIDENTNAMELENGTH) {
    printf("Error: identifier (%s..) too long ", id);
    printf("(maximum length is %d characters)\n", MAXIDENTNAMELENGTH);
    exit(EXIT_FAILURE); 
  }
  id[i] = '\0';
  if ((curchar == ' ') || (curchar == '\t') || (curchar == '\n')) {
    nextchar(TRUE);
  }
  return makeIdentifier(id);
}

Expression parseTerm() {
  if (curchar == '!') {
    Expression e;
    match("!");
    e = parseTerm();
    return makeNegation(e);
  }
  if (curchar == '(') {
    Expression e;
    match("(");
    e = parseEquivalence();
    match(")");
    return e;
  }
  return parseAtom();
}

Expression parseConjunction() {
  Expression e0;
  e0 = parseTerm();
  if (curchar == '*') {
    Expression e1;
    match("*");
    e1 = parseTerm();
    return makeBinaryExpr(AND, e0, e1);
  }
  return e0;
}

Expression parseDisjunction() {
  Expression e0;
  e0 = parseConjunction();
  if (curchar == '+') {
    Expression e1;
    match("+");
    e1 = parseConjunction();
    return makeBinaryExpr(OR, e0, e1);
  }
  return e0;
}

Expression parseImplication() {
  Expression e0;
  e0 = parseDisjunction();
  if (curchar == '=') {
    Expression e1;
    match("=>");
    e1 = parseImplication();
    return makeBinaryExpr(IMPLIES, e0, e1);
  }
  return e0;
}

Expression parseEquivalence() {
  Expression e0;
  e0 = parseImplication();
  if (curchar == '<') {
    Expression e1;
    match("<=>");
    e1 = parseEquivalence();
    return makeBinaryExpr(EQUIV, e0, e1);
  }
  return e0;
}

int parseSentences(Expression expset[]) {
  int numberOfSentences = 0;
  Expression e;
  e = parseEquivalence();
  expset[numberOfSentences] = e;
  numberOfSentences++;
  while (curchar == ';') {
    match(";");
    e = parseEquivalence();
    expset[numberOfSentences] = e;
    numberOfSentences++;
  }
  return numberOfSentences;
}

int parseSentenceSet(char *setname, Expression expset[]) {
  int numberOfSentences;
  matchIdentifier(setname);
  match("=");
  match("[");
  numberOfSentences = parseSentences(expset);  
  match("]");
  return numberOfSentences;
}

void parseInput() {
  nextchar(TRUE);
  kbSize = parseSentenceSet("kb", kb);
  inferSize = parseSentenceSet("infer", infer);
}

void showBoolean(int val) {
  if (val == 0) printf ("false");
  else printf("true");
}

void showModel(int modelSize) {
  int i;
  printf("[");
  for (i=0; i < modelSize; i++) {
    if (i > 0) printf(",");
    printf("%s=", identifiers[i]);
    showBoolean(model[i]);
  }
  printf("]\n");
}

void evaluateRandomModel(int modelSize) {
  int i;
  /* make a random assignment/model */
  for (i=0; i < modelSize; i++) {
    model[i] = random()%2;  /* 0 or 1 (False or True) */
  }

  /* print model */
  printf("Random chosen model: ");
  showModel(modelSize);
  
  /* evaluate KB */
  printf("  KB evaluates to ");
  showBoolean(evaluateExpressionSet(kbSize, kb));
  printf("\n");

  /* evaluate INFER */
  printf("  INFER evaluates to ");
  showBoolean(evaluateExpressionSet(inferSize, infer));
  printf("\n");
}

/*** You should not need to change any code above this line ****/

int checkAllModels(int modelSize) {
  /* return 1 if KB entails INFER, otherwise 0 */
  inferred = 1;
  printf("THE FUNCTION checkAllModels IS NOT IMPLEMENTED YET\n");
  printf("PLEASE IMPLEMENT IT YOURSELF!\n");
  printf("THIS FUNCTION CURRENTLY ALWAYS RETURNS 1.\n\n");
  return inferred;
}


int main(int argc, char *argv[]) {
  parseInput();

  showExpSet("KB", kbSize, kb);
  printf("\n");
  showExpSet("INFER", inferSize, infer);
  printf("\n");

  evaluateRandomModel(cntidents);

  printf("\n");
  if (checkAllModels(cntidents)) {
    printf("KB entails INFER\n");
  } else {
    printf("KB does not entail INFER\n");
  }

  return 0;
}
