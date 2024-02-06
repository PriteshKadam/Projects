#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <calc.h>
#include <math.h>


double remainder(double a, double b)
{
    double mod;
    // Handling negative values
    mod = a;

    // Finding mod by repeated subtraction

    while (mod >= b)
        mod = mod - b;

    return mod;
}

double calculate(char *buf){

  /*************************************************************
   Implement the expression evaluation functionality which
   will be invoked by the server whenever required.
  *************************************************************/

  double operands[20];
  double Operands1[20];
  char op[20];
  char Ops1[20];
  int oprnInd = 0;
  int opIdx = 0;
  double result = 0.0;
  int firstOperandIdx, secondOperandIdx;
  int opIdx1 = 0;

  // implement expression evaluation functionality here

  char* token = strtok(buf, " ");

  while (token != NULL) {
      if (*token == '+' || *token == '-' || *token == '*' || *token == '/' || *token == '%'){
          // operator
          if (oprnInd == 0){
              return result;
          }
          op[opIdx++] = *token;
      }
      else {
          // operand
          operands[oprnInd++] = atof(token);
      }

      if (oprnInd >= 20 || opIdx >= 19)
      {
          return result;
      }

      token = strtok(NULL, " ");
  }
  /*
  printf("Operands: ");
  for (int nextOp = 0; nextOp < oprnInd; nextOp++) {
      printf(" %f ", operands[nextOp]);
  }
  printf("\n Operators: ");
  for (int nextOp = 0; nextOp < opIdx; nextOp++) {
      printf("%c ", op[nextOp]);
  }
  */
  // Solve Div, Mult, Mod first
  Operands1[0] = operands[0];
  firstOperandIdx = 0;
  secondOperandIdx = 1;
  opIdx1 = 0;
  for (int nextOp = 0; nextOp < opIdx; nextOp++)
  {
      switch (op[nextOp]) {
      case '*':
          Operands1[firstOperandIdx] = Operands1[firstOperandIdx] * operands[secondOperandIdx];
          secondOperandIdx++;
          break;
      case '/':
          Operands1[firstOperandIdx] = Operands1[firstOperandIdx] / operands[secondOperandIdx];
          secondOperandIdx++;
          break;
      case '%':
          Operands1[firstOperandIdx] = remainder(Operands1[firstOperandIdx], operands[secondOperandIdx]); //TODO
          secondOperandIdx++;
          break;

      case '+':
      case '-':
          Ops1[opIdx1++] = op[nextOp];
          Operands1[++firstOperandIdx] = operands[secondOperandIdx++];
            break;
      }
  }
  /*
  printf("New Operands: ");
  for (int nextOp = 0; nextOp <= firstOperandIdx; nextOp++){
      printf("%f ", Operands1[nextOp]);
   }
  printf("\nNew Operators: ");
  for (int nextOp = 0; nextOp < opIdx1; nextOp++) {
      printf("%c ", Ops1[nextOp]);
  }
  */

  // Solve Add, sub
  result = Operands1[0];
  secondOperandIdx = 1;
  for (int nextOp = 0; nextOp < opIdx1; nextOp++) {
      switch (Ops1[nextOp]) {
      case '+':
          result = result + Operands1[secondOperandIdx++];
          break;
      case '-':
          result = result - Operands1[secondOperandIdx++];
          break;
      }
  }

  fflush(stdout);
  return(result);
}
