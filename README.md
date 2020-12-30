# FilePipeline

#Utilisation  
Il y a 2 exécutables permettant d'éffectuer les actions suivantes:  

-download.exe:  
	-permet de se connecter à un uploader et téléchargé ses fichiers  
	-télécharge les fichier dans le répertoire download présent dans le répertoire de l'exécutable  

-upload.exe:  
	-permet de partager des fichiers  
	-partage les fichiers présent dans le répertoire upload présent dans le répertoire de l'exécutable  
	-nécessite l'ouverture d'un port  

#Pour que la compilation fonctionne ajouter la dépendance suivante:  
- Ajouter SFML dans lib/ comme lib/SFML  
	https://www.sfml-dev.org/files/SFML-2.5.1-sources.zip  
- Pour les utilisateurs de Linux, le script lib_setup.sh permet de faire ça automatiquement  

