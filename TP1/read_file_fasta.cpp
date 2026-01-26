// 1st C++ Program
#include <iostream>
#include <fstream> //Fonctionnalités de gestion des fichiers  

using namespace std;

int main() {
	ifstream ifs ("/root/.bashrc");
	// Vérifier si le fichier a été ouvert avec succès
	if (ifs.is_open()){
		cout << "Le fichier a été ouvert correctement" << endl;
	} else {
		cout << "Oh non, vous avez eu un erreur" << endl;
	}

	cout << "Terminé" << endl; //Fin du programme
	return 0; 
}

