#include <iostream> 
#include <fstream> //Fonctionnalités de gestion des fichiers  

using namespace std; //Fonctionnalité pour éviter de écrire std::cout etc ....

int main() { 
	const char* fichier = "./files_/1_fichier_fasta_simple_une_ligne.fasta"; //Path sauvegardé dans une variable et on peut tester sur les files_
	ifstream ifs (fichier); //Flux de fichier en lecture seule
	// Variables pour suivre la position dans le fichier
	int num_ligne = 0;
    int num_colonne = 0;
	if (ifs.is_open()){ // Vérifier si le fichier a été ouvert avec succès
		cout << "Le fichier'" << fichier << "' a été ouvert correctement." << endl;
	while (ifs) {
        int c = ifs.get();
        if (c != -1) { //Vérifier qu'on n'est pas à la fin du fichier
			// Si on rencontre un saut de ligne
			if(c == '\n') {
				num_ligne++; // On passe à la ligne suivante
				num_colonne = 0; // On réinitialise la colonne
			} else {
				num_colonne++; // // Sinon, on avance d'une colonne
			}
			// On affiche chaque caractère et sa position
            cout << "Caractère: '" << (char)c << "'"
				<< " Ligne: " << num_ligne
                << " Colonne: " << num_colonne
                << endl;
        }
    }
	cout << "Fin de la lecture du fichier." << endl; //Fin du programme
} else {
	cout << "Erreur : impossible d'ouvrir le fichier '" << fichier << "'." << endl;
	}
	return 0; 
}

