#include <iostream> 
#include <fstream> //Fonctionnalités de gestion des fichiers  
#include <limits>
#include <string>


using namespace std; //Fonctionnalité pour éviter de écrire std::cout etc ....

// Vérifie si le caractère correspond à un espace ou à un retour à la ligne
bool is_space(char c) {
    return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
}

// Vérifie si le caractère correspond au début d'une nouvelle séquence FASTA
bool is_new_sequence(char c, int num_colonne) {
    return (num_colonne == 0) && (c == '>');
}

int main() { 
	const char* fichier = "./files_/1_fichier_fasta_simple_une_ligne.fasta"; //Path sauvegardé dans une variable et on peut tester sur les files_
	ifstream ifs (fichier); //Flux de fichier en lecture seule
	// Variables pour suivre la position dans le fichier
	int num_ligne = 0;
    int num_colonne = 0;
	
	// Variables liées à la séquence
    string entete;
    int taille = 0;
    bool has_sequence = false;

	if (!ifs.is_open()){// Vérifier si le fichier a été ouvert avec succès
    	cerr << "Erreur : impossible d'ouvrir le fichier '" << fichier << "'." << endl;
    	return 1;
	}
	cout << "Le fichier '" << fichier << "' a été ouvert correctement." << endl;
	while (ifs) {
        int c = ifs.get();
        if (c != -1) { //Vérifier qu'on n'est pas à la fin du fichier
			// Cas d'un début de nouvelle séquence
            if (is_new_sequence((char)c, num_colonne)) {

                // Si une séquence précédente existe, on affiche son résumé
                if (has_sequence) {
                    cout << "Fin de la séquence '" << entete
                         << "' de taille " << taille << endl;
                }

                has_sequence = true;
                taille = 0;

                // On lit le reste de la ligne comme entête
                getline(ifs, entete);

                num_ligne++;
                num_colonne = 0;
            }
			
			// Si on rencontre un saut de ligne
			else if(c == '\n') {
				num_ligne++; // On passe à la ligne suivante
				num_colonne = 0; // On réinitialise la colonne
			} 
			else {
				num_colonne++; // // Sinon, on avance d'une colonne
			// Si on est dans une séquence, on compte les caractères non blancs
                if (has_sequence && !is_space((char)c)) {
                    taille++;
                }
            }
        }
    }
	// Affichage de la dernière séquence lue
    if (has_sequence) {
        cout << "Fin de la séquence '" << entete
             << "' de taille " << taille << endl;
    }
	cout << "Fin de la lecture du fichier." << endl; //Fin du programme
	return 0;
}

