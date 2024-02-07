/*************************************************************
Author : Pritesh Kadam

OS Assignment 1 - Client-Server Calculator.

This file contains the calculation code.

 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <calc.h>
#include <math.h>
#include <stdbool.h>

extern bool g_debug ;
#define DEBUG_LOG(...) if(g_debug){printf("Client : ");printf(__VA_ARGS__);}

// Assuming a & b will be +ve values only
double remainder(double a, double b)
{
    double mod;
    mod = a;

    while (mod >= b)
        mod = mod - b;

    return mod;
}

// Only numeric operands, and +, -, *, /, % operators accepted.
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

  // Separate operands and operators.
  char* token = strtok(buf, " ");
  while (token != NULL) {
      if (*token == '+' || *token == '-' || *token == '*' || *token == '/' || *token == '%'){
          if (oprnInd == 0){
              return result;
          }
          op[opIdx++] = *token;
      }
      else {
          // operands
          operands[oprnInd++] = atof(token);
      }

      if (oprnInd >= 20 || opIdx >= 19)
      {
          return result;
      }

      token = strtok(NULL, " ");
  }

  // Solve Div, Mult, Mod first . Keep remaining part as it is.
  // Store remaining part of operands in Operands1 & 
  // remaining operators in Ops1 in sequence.
  // Algo : 
  // Consider two operands at a time.
  // save first operand in Operands1 first index (firstoperand).
  // iterate operands in sequence from 2nd index(secondoperand).
  // if operator is *, / , % 
  //     calculate : firstoperand (OP) secondoperand
  //     replace firstoperand with the result. 
  //     go to next operand & next operator
  // else:
  //    save secondoperand , save operator ( here secondoperand becomes firstoperand , next operand becomes secondoperand)
  //    go to next operand & next operator
  // 
  // At the end we will have all Div, Mult, Mod solved and + - remaning.
  //

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

  // Solve Add, sub sequentially
  // 
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

  return(result);
}
