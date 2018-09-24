#ifndef HORSPOOL_H_INCLUDED
#define HORSPOOL_H_INCLUDED

typedef struct
{
    int occurence;
    const char * p_string;
} occurence_msg_struct;



void preBmBc(char * x,int m, int bmBc[]);
int horspool(char *haystack, int haystack_length, char * needle, int needle_length);

#endif // HORSPOOL_H_INCLUDED
