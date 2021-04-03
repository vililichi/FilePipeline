# FilePipeline

#Utilisation  
Il y a 2 exécutables permettant d'éffectuer les actions suivantes:  

-client.exe:  
	-permet de se connecter à un serveur et d'y envoyer ces commandes
		-upload
		-download
		-list
		-remove
		-rename

-serveur.exe:  
	-permet d'héberger un serveur
	-recoit des commande de download et d'upload du client
	-permet une gestion des clé d'accès et des utilisateurs
	-nécessite l'ouverture d'un port  

#Pour que la compilation fonctionne ajouter la dépendance suivante:  
- Ajouter SFML dans lib/ comme lib/SFML  
	https://www.sfml-dev.org/files/SFML-2.5.1-sources.zip  
- Pour les utilisateurs de Linux, le script lib_setup.sh permet de faire ça automatiquement  

