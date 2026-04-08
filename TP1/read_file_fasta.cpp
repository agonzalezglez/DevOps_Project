#include <iostream> 
#include <fstream> //Fonctionnalités de gestion des fichiers  

using namespace std; //Fonctionnalité pour éviter de écrire std::cout etc ....

int main() { 
	const char* fichier = "./files_fasta/1_fichier_fasta_simple_une_ligne.fasta"; //Path sauvegardé dans une variable et on peut tester sur les files_fasta
	ifstream ifs (fichier); //Flux de fichier en lecture seule
	// Vérifier si le fichier a été ouvert avec succès
	if (ifs.is_open()){
		cout << "Le fichier'" << fichier << "' a été ouvert correctement." << endl;
	} else {
		cout << "Erreur : impossible d'ouvrir le fichier '" << fichier << "'." << endl;
	}
	cout << "Terminé" << endl; //Fin du programme
	return 0; 
}

