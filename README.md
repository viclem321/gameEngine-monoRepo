Moteur de jeu en ligne 3D basé sur Opengl (pour les graphismes) et l'api native windows (pour le multi-joueur).
Le language utilisé est C++.
Il est tout de même possible de lancer une démonstration en compilant et exécutant le serveur sur une machine, et le client sur 1 ou plusieurs machines qui ont accès au réseau. 
Pour une exécution, il est nécessaire de modifier :
  - sur le projet serveur, dans le fichier include/reseau/udpServer.h, modifier les adresse IP du serveur (dans la fonction InitUdp()) et le port sur lequel fonctionne le serveur (NPORT)
  - sur le projet client, dans le fichier include/includeSauvage/beforeLoop.cpp, modifier l'adresse et port du serveur (dans la section "Connexion au serveur")

Note pour ceux qui voudraient mettre leur nez dans ce projet:
  C'est un projet annexe qui n'a jamais été terminé. Le code est chaotique et souvent mal organisé, beaucoup de données sont écritent en dur, et il existe un certains nombre de bugs/d'erreurs non répertoriés.
