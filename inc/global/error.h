/**
 * @file error.h
 * @author Guillaume FOURCROY
 * @brief Fichier qui liste les erreurs
 * @version 0.1
 * @date 2021-01-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _ERROR_
#define _ERROR_

/**
 * @brief Pas d'erreur !
 */
#define OK 0
/**
 * @brief Erreur : la requete est mal formé
 */
#define BAD_REQUEST 1
/**
 * @brief Erreur : Il n'y pas de requete recue
 */
#define NO_MESSAGES 2
/**
 * @brief Ereur : les données de la requete sont manquantes
 */
#define DATA_MISSING 3
/**
 * @brief Erreur : Requete mal envoyé
 */
#define BAD_SEND 4
/**
 * @brief Erreur : Données de la requete mal envoyé
 */
#define BAD_SEND_DATA 5
/**
 * @brief Erreur : Mauvaise initialisation
 */
#define BAD_INITIALISATION 6
/**
 * @brief Erreur : Mauvais argument sur la requete ou argument introuvable
 */
#define BAD_ARGUMENT 7

/**
 * @brief Erreur : accès interdit
 * 
 */
#define ACCESS_DENIED 8

/**
 * @brief Erreur : Accès aux fichiers impossible
 * 
 */
#define BAD_ACCESS_FILES 9

/**
 * @brief Erreur : Annulation
 * 
 */
#define ANNULATION 10

/**
 * @brief Erreur : Deconnexion du client
 * 
 */
#define DISCONECTION 10

/**
 * @brief Erreur : les données reçues sont de mauvaise taille
 * 
 */
#define BAD_SIZE_DATA 11

// Valeurs d'erreurs pour filesError

/**
 * @brief Pas d'erreur
 */
#define NO_ERROR        0

/**
 * @brief Erreur d'allocation memoire
 */
#define MEM_ALLOC       1

/**
 * @brief Erreur d'acces a un fichier
 */
#define FILE_ACCESS     2

/**
 * @brief Erreur de lecture dans un fichier
 */
#define FILE_READ       3

/**
 * @brief Erreur d'ecriture dans un fichier
 */
#define FILE_WRITE      4

/**
 * @brief Le compte demande n'existe pas
 */
#define NO_ACCOUNT      5

/**
 * @brief Le compte existe deja
 */
#define EXIST_ACCOUNT   6

/**
 * @brief Le contact demande n'existe pas
 */
#define NO_CONTACT      7

/**
 * @brief Le contact existe deja
 */
#define EXIST_CONTACT   8

/**
 * @brief Erreur d'obtention d'ID
 */
#define GET_ID          9

#endif