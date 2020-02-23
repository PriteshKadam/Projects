#include "HUFFMAN.h"

void main()
{
    flushall();
    clrscr();
    Huffman h;
    int ch;

    cout << "Enter ur choice:\n"
         << "1:Encode\n2:Decode";
    cin >> ch;
    switch (ch)
    {
    case 1:
        h.encode();
        break;

    case 2:
        h.decode();
        break;
    }
}
