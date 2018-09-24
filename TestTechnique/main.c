#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <horspool.h>
#include <string.h>
#include <dirent.h>

#include <udp_sender.h>

#define SOFTWARE_ARG_RANK 0
#define FOLDER_ARG_RANK 1
#define IP_ADDR_ARG_RANK 2
#define PORT_ARG_RANK 3
#define NB_ARG 4

#define SIZE_OCCURENCE_TABLE 300000
#define FILE_NAME_SIZE 256

#define SEC_TO_MS   1000.0

#define ONE_MSG_TO_SEND_SIZE  300
#define ALL_MSG_TO_SEND_SIZE (SIZE_OCCURENCE_TABLE*ONE_MSG_TO_SEND_SIZE)


#define handle_error(msg) \
    do { perror(msg); } while (0)
/*
·         A partir d’un répertoire (chemin en argument) contenant N fichiers (texte)
·         Chaque fichier contient une liste de nombres entiers séparés par un retour chariot
·         L'objectif est d'identifier le plus rapidement possible si pour chaque fichier il existe un élément dupliqué
·         Communiquer via UDP (ip et port en argument) « au fil de l'eau » le nom des fichiers qui possèdent au moins
                un élément (nombre) dupliqué, la liste des nombres dupliqués et le temps de traitement du fichier
·         Nous attendons un programme C compilable sous Linux
*/

char * read_file(char * file_name);
int find_occurences(char * file_content,occurence_msg_struct ** table );
int find_occurence(char * file_content,  char * p_needle);

char * make_msg(char * p_file_name,occurence_msg_struct *  table, int  nb_of_occ_msg,double  duration);
void send_msg_to_reader(char * msg, char * ip_addr, char * port);


int main(int argc, char *argv[])
{
    char * ip_addr = NULL;
    char * port = NULL;
    char * folder = NULL;

    // Contrôle d'argument
    if(argc != NB_ARG)
    {
        fprintf(stderr, "Bad usage : TestTechnique <folder> <ip_addr> <port>");
        exit(EXIT_FAILURE);
    }
    else
    {

        folder = argv[FOLDER_ARG_RANK];
        ip_addr = argv[IP_ADDR_ARG_RANK];
        port = argv[PORT_ARG_RANK];
    }

    occurence_msg_struct *table=NULL;

    DIR *d;
    struct dirent *dir;

    // Ouverture du répertoire
    d = opendir(folder);

    // Parcours du répertoire
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            // On ignore les sous dossiers
            if(dir->d_type == DT_DIR)
            {
                continue;
            }

            char * p_file_name = (char *) calloc(FILE_NAME_SIZE,sizeof(char));
            sprintf(p_file_name,"%s%s",folder,dir->d_name);

            // Lancement du chronomètre
            clock_t t1 = clock();

            char * p_file_content = read_file(p_file_name);
            int  nb_of_occ_msg =  find_occurences(p_file_content, &table);

            // Arrêt du chronomètre et mesure du temps écoulé en millisecondes
            clock_t t2 = clock();
            double duration = SEC_TO_MS * (t2-t1)/CLOCKS_PER_SEC;

            // Création du message à envoyer
            char * msg = make_msg(p_file_name,  table, nb_of_occ_msg, duration);

            // Envoi du message
            send_msg_to_reader( msg,ip_addr,port );

            // Libération mémoire
            free(msg);
            free(p_file_name);
        }

        closedir(d);
    }

    return 0;
}

char * read_file( char * file_name  )
{
    char *addr;
    int fd;
    struct stat sb;
    off_t offset, pa_offset;
    size_t length;

    // Vérification des arguments
    if (file_name == NULL)
        handle_error("argument");

    // Ouverture du fichier
    fd = open(file_name, O_RDONLY);
    if (fd == -1)
        handle_error("open");

    if (fstat(fd, &sb) == -1)      /* Pour obtenir la taille du fichier */
        handle_error("fstat");

    offset = 0;
    pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
    /* l'offset pour mmap() doit être aligné sur une page */

    if (offset >= sb.st_size)
    {
        fprintf(stderr, "L'offset dépasse la fin du fichier\n");
        exit(EXIT_FAILURE);
    }


    length = sb.st_size - offset;
    // Mappage du fichier sur le pointeur addr
    addr = mmap(NULL, length + offset - pa_offset, PROT_READ,
                MAP_PRIVATE, fd, pa_offset);
    if (addr == MAP_FAILED)
        handle_error("mmap");


    return addr;
}

int find_occurences(char * file_content,occurence_msg_struct ** table )
{
    // Structure représentant les occurrences un numéro
    occurence_msg_struct occ_msg;
    int index_table =0;

    char * p_parse_file = file_content;
    char * p_end_needle = strchr(p_parse_file,'\n');
    char * results      = (char *) calloc( strlen(file_content), sizeof(char));

    // Création d'un tableau de "occurence_msg_struct" dynamique
    occurence_msg_struct * p_tmp = (occurence_msg_struct*) malloc( sizeof(occ_msg) * (index_table+1));

    // Allocation de la zone mémoire nécessaire au stockage du nombre à rechercher
    char * p_needle = (char *) calloc(p_end_needle - p_parse_file, sizeof(char));

    while(p_end_needle!=NULL && index_table < SIZE_OCCURENCE_TABLE)
    {
        // Nombre à rechercher dans le fichier
        memcpy(p_needle,p_parse_file,p_end_needle - p_parse_file);

        // L’occurrence du nombre à t'elle déjà été recherchée
        if (0 == find_occurence( results,p_needle) )
        {

            // Recherche de l’occurrence
            int occurence = find_occurence( p_parse_file,p_needle);

            char * p_string = (char *) calloc(strlen(p_needle),sizeof(char));
            memcpy(p_string,p_needle,strlen(p_needle));
            occ_msg.p_string = p_string;
            occ_msg.occurence = occurence;

            occurence_msg_struct * p_tmp2 = (occurence_msg_struct*) realloc(p_tmp, sizeof(occ_msg) * (index_table+1));

            if(NULL == p_tmp2 )
            {
                break;
            }

            p_tmp = p_tmp2;
            p_tmp[index_table] = occ_msg;
            index_table++;

            memcpy(results + strlen(results),p_needle,strlen(p_needle));

        }

        p_parse_file = p_end_needle +1;
        p_end_needle = strchr(p_parse_file,'\n');

    }


    free(results);
    *table=p_tmp;

    return index_table;
}

int find_occurence(char * file_content,  char * p_needle)
{

    int haystack_length = strlen(file_content);
    int needle_length = strlen(p_needle);
    // Recherche par l'algorithme de horspool
    int occurence = horspool(file_content, haystack_length, p_needle, needle_length);

    return occurence;
}

char * make_msg(char * p_file_name,occurence_msg_struct *  table, int nb_of_occ_msg,double  duration)
{
    char * msg = (char * ) calloc (ALL_MSG_TO_SEND_SIZE, sizeof(char));
    char * p_temp = (char * ) calloc (ONE_MSG_TO_SEND_SIZE, sizeof(char));

    if((p_file_name == NULL) || (table == NULL) )
    {
        return NULL;
    }

    // Création du message
    sprintf(msg,"Filename = %s Treatment duration : %.6f ms \n",p_file_name, duration );

    for(int j = 0; j < nb_of_occ_msg; j++)
    {
        if(table[j].occurence >1)
        {
            sprintf(p_temp,"Value %s repeated %d times \n",table[j].p_string, table[j].occurence);
            strcat(msg, p_temp);
        }
    }

    free(p_temp);
    return msg;
}

void send_msg_to_reader(char * msg, char * ip_addr, char * port)
{
    // Envoi du message
    int rtn = send_msg(msg,ip_addr, port);
    if(rtn != EXIT_SUCCESS)
    {
        perror("Error sending msg:");
    }
    return;
}
