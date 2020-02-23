#ifndef Huffman_h
#define Huffman_h

#include <fstream.h>
#include <iostream.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Huffman tree node
typedef struct huffnode
{
    unsigned long int frequency;
    unsigned char symbol;
    huffnode *left, *right;

} PHNODE;

typedef struct huffqueue
{
    PHNODE *node;
} PHQUEUE;

typedef struct huffsymbol
{
    unsigned char symb;
    unsigned long int freq;
} PSYMB;

typedef struct code
{
    unsigned char symb;
    int symb_code[20];
    int length;
} PHCODE;

class Huffman
{
private:
    int j, k, code_buffer[8], symb_count;
    unsigned long int symb_freq[256];
    unsigned int file_size, i;
    char head[4];
    //static int file_size;
    PSYMB *symheader;
    PHQUEUE *pqueue, *temp;
    PHCODE *phcode, *temp_code;
    PHNODE *htree_root, *temp_tree;
    FILE *src, *dest;

    void build_queue();
    void count(char f_name[]);
    void build_tree();
    void store(char f_name[], unsigned int);
    inline void inorder(PHNODE *ptr);
    void build_Code(PHNODE *, int code[], int);
    unsigned char int_binary(int code[], int);
    void read_queue();
    void char_binary(unsigned char);
    void read_code();

public:
    void encode();
    void decode();
};

#endif