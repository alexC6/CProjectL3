#ifndef __REQUETES_CLIENT_H__
#define __REQUETES_CLIENT_H__

#include "TCP_client.h"
#include "../global/utilitaire.h"
#include "../global/structures.h"

/**
 * @brief  Recupère une requete (et les éventuelles données) au serveur en utilisant TCP
 * @author Guillaume Fourcroy
 * 
 * @param requete requete à récuperer : Elle doit être alouée
 * @return int OK NO_MESSAGES BAD_REQUEST
 */
int recupererRequete(requete requete);

/**
 * @brief Envoie une requete (et les éventuelles données) au serveur en utilisant TCP
 * @author Guillaume Fourcroy
 * 
 * @param requete requete à envoyer qui doit être initilisé et remplie en utilisant initRequete et ajouterArgument
 * @return int OK BAD_REQUEST BAD_SEND BAD_SEND_DATA
 */
int envoyerRequete(requete requete);

/**
 * @brief connection au compte
 * @author Guillaume Fourcroy
 * 
 * @param mail mail du compte
 * @param password mot de passe du compte
 * @return int OK BAD_REQUEST BAD_ARGUMENT
 */

/**
 * @brief Fonction qui gère les requetes de connexion au serveur
 * @author Guillaume FOURCROY
 * 
 * @param mail le mail de l'utilisateur
 * @param password le mot de passe de l'utilisateur
 * @return int OK BAD_REQUEST BAD_ARGUMENT + les erreur de envoyerRequete et recupererRequete
 */
int connectionCompte(char* mail, char* password);

/**
 * @brief Se deconnecte du serveur normalement
 * 
 * @return int OK BAD_REQUEST + les erreur de envoyerRequete et recupererRequete
 */
int demanderDeconnexion();

/**
 * @brief récupuère un annuaire à partir d'un mail
 * 
 * @param mail correspondant à l'utilisateur de l'annuaire 
 * @param tailleAnnuaire le nombre de contact dans l'annuaire
 * @return annuaire L'annuaire recupéré
 */
annuaire recupererAnnuaire(char* mail, size_t* tailleAnnuaire);

/**
 * @brief récupère la liste des mails des personnes ayant partagés leurs annuaires de l'utilisateur
 * 
 * @param tailleListe le nombre de mails
 * @return char** les mails récupérés
 */
char** recupererSharedAnnuaires(size_t* tailleListe);

/**
 * @brief récupère la liste des mails des personnes ayant accès a l'annuaire de l'utilisateur
 * 
 * @param tailleListe le nombre de mails
 * @return char** La liste des annuaires
 */
char** recupererAccesAnnuaires(size_t* tailleListe);


/**
 * @brief ajoute un contact (si le contact existe, il sera ecrasé)
 * 
 * @param c le contact à ajouter
 * @return int erreur
 */
int demandeAjoutContact(contact c);

/**
 * @brief supprime un contact
 * 
 * @param c le contact à ajouter
 * @return int erreur
 */
int demandeSuppressionContact(contact c);

/**
 * @brief Permet à un utilisateur d'avoir accès a l'annuaire du compte connecté
 * 
 * @param mail de l'utilisateur à connecter
 * @return int 
 */
int demandeAjoutPartage(char* mail);

/**
 * @brief Supprime l'accès d'un utilisateur a l'annuaire du compte connecté
 * 
 * @param mail de l'utilisateur à connecter
 * @return int 
 */
int demandeSuppressionPartage(char* mail);

#endif