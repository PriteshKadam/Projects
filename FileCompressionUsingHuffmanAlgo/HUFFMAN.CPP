#include "HUFFMAN.h"

void Huffman::inorder(PHNODE *ptr)
{
    if (ptr != NULL)
    {
        inorder(ptr->left);
        cout << "  " << ptr->frequency;
        inorder(ptr->right);
    }
}

unsigned char Huffman::int_binary(int c[], int length)
{
    unsigned int i, sum = 0, pw;
    for (i = 1; i <= length; i++)
    {
        if (c[i - 1] == 1)
        {
            sum = sum + pow(2, length - i);
        }
    }
    return (unsigned char)sum;
}

void Huffman::char_binary(unsigned char ch)
{
    int binary = ch;
    int code[8], i;
    //cout<<"\nChar is "<<ch<<" "<<binary<<"\n";
    //getch();
    for (i = 7; i >= 0; i--)
    {
        code_buffer[i] = (binary % 2);
        binary = (binary / 2);
        //cout<<code_buffer[i];
    }
    /*cout<<"Code is:"<<ch<<"  \n";

for(i=0;i<8;i++)
cout<<code_buffer[i];*/
}

char name[20];

void Huffman::encode()
{

    int code[20];
    file_size = 0;
    symb_count = 0;
    clrscr();
    cout << "Enter File Name:";
    cin >> name;

    count(name);

    build_queue();
    build_tree();
    delete pqueue;
    getch();

    //inorder(htree_root);
    //getch();
    cout << "\n";
    phcode = new PHCODE[symb_count];
    temp_code = phcode;
    build_Code(htree_root, code, 0);

    store(name, file_size);
}

void Huffman::decode()
{
    clrscr();
    cout << "Enter File Name:";
    cin >> name;
    src = fopen(name, "r");
    if (src == NULL)
    {
        cout << "Error Opening file";
        getch();
        exit(0);
    }
    read_queue();
    build_queue();
    build_tree();
    //inorder(htree_root);
    dest = fopen("dest.txt", "w");
    read_code();
    fclose(dest);
    fclose(src);
}

void Huffman::read_queue()
{
    unsigned char symb, head[4];
    unsigned int freq;
    symb_count = 0;

    fread(head, 4, 1, src);
    head[4] = '\0';
    cout << head;
    if (strcmp(head, "HUFF"))
    {
        cout << "Not an Huffman Encoded File";
        getch();
        exit(0);
    }
    symb = fgetc(src);
    fscanf(src, "%c", &symb_count);
    cout << symb_count;

    symheader = new PSYMB[symb_count];

    fread(symheader, sizeof(PSYMB), symb_count, src);

    return;
}

void Huffman::read_code()
{
    unsigned char symb;
    temp_tree = htree_root;

    fscanf(src, "%c", &symb);

    while (!feof(src))
    {
        char_binary(symb);
        int i = 0;

    label1:
        while (temp_tree->left != NULL && temp_tree->right != NULL)
        {
            if (code_buffer[i] == 0)
                temp_tree = temp_tree->right;
            else
                temp_tree = temp_tree->left;

            if (i == 7)
            {
                goto label;
            }

            i++;
        }
        fprintf(dest, "%c", temp_tree->symbol);
        temp_tree = htree_root;
        goto label1;

    label:
        fscanf(src, "%c", &symb);
    }
}

void Huffman::count(char f_name[])
{
    int j;
    unsigned long int fswap;
    unsigned char symb, sswap;

    src = fopen(f_name, "r");
    if (src == NULL)
    {
        cout << "Error opening file";
        exit(0);
    }

    for (i = 0; i < 256; i++)
    {
        symb_freq[i] = 0;
    }

    while (!feof(src)) //Get symbols and frequencies
    {
        symb = (unsigned char)fgetc(src);
        symb_freq[(int)symb]++;
        file_size++;
        //cout<<(unsigned char)symb<<" ";
        //getch();
    }
    symb_count = 0;
    for (i = 0; i < 256; i++) //Count total no. of symbols
    {
        if (symb_freq[i] > 0)
        {
            symb_count++;
            //cout<<i<<"  "<<(unsigned char)i<<"   "<<symb_freq[i]<<"\n";          //
        }
    }
    //getch();
    //exit(0);
    symheader = new PSYMB[symb_count];
    for (i = 0, j = 0; i < 256; i++)
    {
        if (symb_freq[i] > 0)
        {
            symheader[j].symb = (unsigned char)i;
            symheader[j].freq = symb_freq[i];
            j++;
        }
    }

    /*for(int k=0;k<symb_count;k++)             //
{
cout<<symheader[k].symb<<"  "<<symheader[k].freq;
} */

    for (i = 0; i < symb_count; i++) //sort array
    {
        for (j = 0; j < symb_count - 1; j++)
        {
            if (symheader[j].freq > symheader[j + 1].freq)
            {
                fswap = symheader[j].freq;
                sswap = symheader[j].symb;
                symheader[j].freq = symheader[j + 1].freq;
                symheader[j].symb = symheader[j + 1].symb;
                symheader[j + 1].freq = fswap;
                symheader[j + 1].symb = sswap;
            }
        }
    }

    cout << "\n";
    /*for(i=0;i<symb_count;i++)
{
cout<<symheader[i].symb<<"  "<<symheader[i].freq<<"\n";
}
getch();   */
    fclose(src);
}

void Huffman::build_queue()
{
    pqueue = new PHQUEUE[symb_count]; //Memory allocation for queue

    temp = pqueue;
    for (i = 0; i < symb_count; i++)
    {
        temp->node = new PHNODE;
        temp->node->symbol = (unsigned char)symheader[i].symb;
        temp->node->frequency = symheader[i].freq;
        temp->node->left = NULL;
        temp->node->right = NULL;
        temp++;
    }

    temp = pqueue;

    for (i = 0; i < symb_count; i++) //
    {
        cout << temp[i].node->symbol << "  " << temp[i].node->frequency << "\n";
    }
    getch();
    flushall();
}

//building tree
void Huffman::build_tree()
{
    PHNODE *node1, *node2, *new_node, *swap;
    PHQUEUE *temp;

    temp = pqueue;

    for (i = 0; i < symb_count - 1; i++)
    {
        node1 = temp[i].node;
        node2 = temp[i + 1].node;
        new_node = new PHNODE;
        new_node->frequency = node1->frequency + node2->frequency;
        new_node->left = node1;
        new_node->right = node2;
        temp[i].node = NULL;
        temp[i + 1].node = new_node;

        //Sort queue

        for (int k = i + 1; k < symb_count - 1; k++)
        {
            if (temp[k].node->frequency > temp[k + 1].node->frequency)
            {
                swap = temp[k].node;
                temp[k].node = temp[k + 1].node;
                temp[k + 1].node = swap;
            }
        }
    }
    htree_root = new_node;
}

void Huffman::build_Code(PHNODE *tree, int code[], int n)
{
    int i;
    if ((tree->left == NULL) && (tree->right == NULL))
    {
        cout << "code:" << tree->symbol;
        temp_code->symb = tree->symbol;
        for (i = 0; i < n; i++)
        {
            cout << code[i];
            temp_code->symb_code[i] = code[i];
        }
        temp_code->length = n;
        cout << "   Length: " << temp_code->length;
        temp_code++;
        cout << "\n";
    }
    else
    {
        code[n] = 1;
        n++;
        build_Code(tree->left, code, n);
        code[n - 1] = 0;
        build_Code(tree->right, code, n);
    }
    //getch();
}

void Huffman::store(char f_name[], unsigned int file_size)
{
    //FILE *pdest=NULL;
    unsigned char symb, bin_equ;
    int bin_code[8];
    static unsigned int buffer_size = 0;

    char head[] = "HUFF";
    dest = fopen("dest.h", "w");
    if (dest == NULL)
    {
        cout << "Error Writing File";
        exit(0);
    }

    fprintf(dest, "%s %c", head, symb_count);
    //fprintf(dest," ");
    /*for(i=0;i<symb_count;i++)
{
fprintf(dest," %c %u",symheader[i].symb,symheader[i].freq);
} */
    fwrite(symheader, sizeof(PSYMB), symb_count, dest);

    delete symheader;

    //fprintf(dest,"CODE:");
    //getch();
    src = fopen(name, "r");
    if (src == NULL)
    {
        cout << "Error Opening file::";
        getch();
        exit(0);
    }
    temp_code = phcode;
    k = 0;
    while (!feof(src))
    {
        symb = fgetc(src);

        while (1)
        {
            if (temp_code->symb == symb)
                break;
            else
                temp_code++;
        }

        for (j = 0; j < temp_code->length; j++)
        {
        l1:
            code_buffer[k] = temp_code->symb_code[j];
            if (k == 7)
            {
                bin_equ = int_binary(code_buffer, 8);
                //printf("%d ",bin_equ);
                if (bin_equ == 26)
                {
                    if (j < temp_code->length)
                    {
                        j = 0;
                        k = 0;
                        goto l1;
                    }
                    else
                        goto l2;
                }
                fprintf(dest, "%c", bin_equ);
            l2:
                k = 0;
            }
            else
                k++;
        }
        temp_code = phcode;
    }
    fclose(dest);
}