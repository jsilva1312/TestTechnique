#include <horspool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <memory.h>

#define SIZE 256

void preBmBc(char * x,int m, int bmBc[])
{
    int i;
    if(x == NULL) return;

    for (i = 0; i < SIZE; ++i ) bmBc[i] = m;
    for (i = 0; i  < m-1; ++i) bmBc[(int)x[i]] = m-i-1;


}


int horspool(char *haystack, int haystack_length, char * needle, int needle_length)
{

    int bmBc[SIZE];
    // Création du tableau de saut d'index
    preBmBc(needle,needle_length,bmBc);

    int nboccurence=0;
    int i = needle_length-1;
    int k =0;

    // Recherche de l'aiguille (le numéro) dans "la meule de foin" (le fichier)
    while(i < haystack_length)
    {
        while( (k<needle_length) && ( haystack[i-k] == needle[needle_length - 1 - k ] ) )
        {
            //  Comparaison caracteres par caracteres en commencant par la droite
            // Si les 2 caracteres comparés sont indentiques , on compare le caractere à gauche
            k++;
        }
        if(k==needle_length)
        {
            // Si N caractéres sont identiques à la suite , avec N, la taille de l'aiguille, on
            // considere que l'aiguille a été trouvé et on incrémente le compteur d'occurence
            nboccurence++;
            i+=1;
            k=0;
        }
        else
        {
            // En cas de caractères differents , on déplace l'index vers la droite de M cases,avec M
            // la valeur du saut à effectuer
            i += bmBc[(int)haystack[i]];
            k=0;
        }
    }

    return nboccurence;
}
