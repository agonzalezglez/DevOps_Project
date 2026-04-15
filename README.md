# Projet Mapping

## Introduction

Dépôt git associé à l’UE de Développement opérationnel avancé (Master Bioinformatique, 2025-2026).

---

## Fonctionnalités principales

Le projet permet de :

- lire des fichiers FASTA et FASTQ,
- représenter des séquences biologiques,
- normaliser les séquences (A, C, G, T, N),
- construire une table des suffixes sur une référence,
- rechercher des motifs dans la référence,
- mapper des reads à l’aide d’une approche basée sur les k-mers,
- générer des résultats avec position, brin, score, mismatches et CIGAR.

---

## Structure du projet

```text
hau803i-2025-2026/
├── include/        # Fichiers d'en-tête
├── src/            # Implémentations C++
├── ressources/     # Données de test (FASTA, FASTQ)
├── docs/           # Documentation (Doxygen)
├── output/         # Résultats générés
├── Makefile
├── README.md
└── .gitignore

## Compilation

Pour compiler les programmes, il suffit
de taper:

```sh
make
```

## Utilisation

Pour utiliser le programme `mapping`, il suffit de taper par exemple:
```sh
./bin/mapping < reference > < reads > < k >
```
Par exemple 
```sh
./bin/mapping ressources/genom_ref/reference_coupe.fna ressources/reads/run1.fastq output/test_fastq.tsv 9 4
```

Pour utiliser le programme `table_suffixes`, il suffit de taper:
```sh
./table_suffixes <word> [query1 query2 ...]
```

## Nettoyage

Pour supprimer les fichiers issus de la compilation et les backups, il suffit de taper:

```sh
make clean
```
