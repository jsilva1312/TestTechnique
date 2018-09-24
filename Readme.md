# Test à faire

         A partir d’un répertoire (chemin en argument) contenant N fichiers (texte)
         Chaque fichier contient une liste de nombres entiers séparés par un retour chariot
         L'objectif est d'identifier le plus rapidement possible si pour chaque fichier il existe un élément dupliqué
         Communiquer via UDP (ip et port en argument) « au fil de l'eau » le nom des fichiers qui possèdent au moins
         un élément (nombre) dupliqué, la liste des nombres dupliqués et le temps de traitement du fichier
         Nous attendons un programme C compilable sous Linux



# Composition du livrable

 Le livrable se compose de 2 exécutables. Le premier UDP_Reader et en attente des trames UDP et permet de vérifier le bon 
 fonctionnement du second, TestTechnique, qui va transmettre les messages 


# Installation

Parcourir les dossiers TestTechnique et UDP_Reader et lancer le make

cd TestTechnique/
make 
cd ../UDP_reader/
make

# Lancement des exécutables

 Étape 1 : lancer l’exécutable UDP_reader (qui se trouve dans UDP_reader/bin/Release )
 	    Avec la syntaxe suivante "./udp_reader <port>"

Exemple
./UDP_reader/bin/Release/udp_reader 1312

 Étape 2: lance l’exécutable TestTechnique (present dans le répétoire TestTechnique/bin/Release)
	    Avec la syntaxe suivante "./TestTechnique <folder> <ip_addr> <port>"

Exemple
./TestTechnique/bin/Release/TestTechnique ~/MyFolder 127.0.0.1 1312

 Etape 3 : attendre la fin de l’exécution de TestTechnique

