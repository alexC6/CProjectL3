/**
 * @file fichier.h
 * @author Alexandre Coulais - Guillaume Fourcroy
 * @brief Entêtes des fonctions de gestion des fichiers \
 * @version 2021-02-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef _FICHIERS_
#define _FICHIERS_

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "../../inc/global/structures.h"
#include "../../inc/global/error.h"
#include "../../inc/global/color.h"

// Variable d'erreur
extern int filesError;

/**
 * @brief Verification de l'existence et de l'accessibilite d'un fichier
 * @author Alexandre Coulais
 * 
 * @param filename  Nom du fichier dont on souhaite verifier l'acces
 * @return int      Retourne 0 si le fichier existe, 2 s'il a ete cree, 1 en cas d'erreur
 */
int checkFileAccess(char *fileName);

/**
 * @brief Obtenir l'ID d'un compte a partir de son mail
 * @author Alexandre Coulais
 * 
 * @param mail Adresse mail du compte dont on veut l'ID
 * @return int Retourne l'ID du compte correspondant au mail, -1 sinon
 */
int getID(char *mail);

/**
 * @brief Recupere l'ID du dernier compte ou contact cree dans un fichier et l'incremente \
 *        \n Si le fichier n'existe pas, il est cree
 * @author Alexandre Coulais
 * 
 * @param type Designe un ID de compte ou de contact
 * @return int Retourne le dernier ID incremente si ca s'est bien passe, -1 sinon
 */
int getNewID(char *type);

/**
 * @brief Calcule le nombre de caracteres dans un fichier
 * @author Alexandre Coulais
 * 
 * @param fileName Nom du fichier dont on souhaite obtenir le nombre de caracteres
 * @return ssize_t Retourne le nombre de caractères si ca s'est bien passe, -1 sinon
 */
ssize_t calculTailleFichier(char *fileName);

/**
 * @brief Verification de l'existence et de l'accessibilite des dossier necessaires \
 *        \n au fonctionnement du serveur
 * @author Alexandre Coulais
 * 
 * @return int Retourne 0 si ca s'est bien passe, 1 sinon
 */
int checkDirectoriesAccess();

/**
 * @brief Charge l'ensemble des comptes en memoire et retourne le pointeur sur le tableau
 * @author Alexandre Coulais
 * 
 * @param number    Variable ou stocker le nombre de comptes
 * @return compte   Retourne la liste des comptes, NULL si ca s'est mal passe ou qu'il n'existe aucun compte
 */
compte loadAccount();

/**
 * @brief Creation d'un nouveau compte utilisateur
 * @author Alexandre Coulais
 * 
 * @param pCompte   Pointeur sur le compte cree a ajoute dans le fichier
 * @return int      Retourne 0 si ca s'est bien passe, 1 sinon
 */
int addAccount(compte pCompte);

/**
 * @brief Retourne un pointeur sur le compte contenant le mail passe en parametre
 * @author Alexandre Coulais
 * 
 * @param mail      Mail du compte souhaite
 * @return compte   Retourne le pointeur sur le compte dans la zone memoire globale \
 * \n NULL si ca s'est mal passe
 */
compte getAccount(char *mail);

/**
 * @brief Sauvegarde l'ensemble des comptes dans le fichier
 * @author Alexandre Coulais
 * 
 * @return int Retourne 0 si ca s'est bien passe, 1 sinon
 */
int saveAccount(void);

/**
 * @brief Supprime un compte utilisateur
 * @author Alexandre Coulais
 * 
 * @param mail Mail du compte utilisateur a supprimer
 * @return int Retourne 0 si ca s'est bien passe, 1 sinon
 */
int deleteAccount(char *mail);

/**
 * @brief Retourne la liste des mails de tous les comptes existants
 * @author Alexandre Coulais
 * 
 * @param nbMails   Pointeur sur une variable pour obtenir le nombre de comptes existants
 * @return char**   Retourne le pointeur sur la liste des mails, NULL si ca s'est mal passe
 */
char **getMailList(size_t *nbMails);

/**
 * @brief Retourne la liste des info des comptes exsitants
 * 
 * @param nbMails Pointeur sur une variable pour obtenir le nombre de comptes existants
 * @return char** Retourne le pointeur sur la liste des informations, NULL si ca s'est mal passe
 */
char **getInfoComptes();

/**
 * @brief Chargement de l'annuaire correspondant au compte du mail en parametre
 * @author Alexandre Coulais
 * 
 * @param number    Variable ou stocker le nombre de contacts
 * @param mail      Mail du compte dont on souhaite l'annuaire
 * @return int      Retourne le pointeur sur l'annuaire, NULL si ca s'est mal passe
 */
annuaire getAnnuaire(char *mail, size_t *number);

/**
 * @brief Sauvegarde sur le fichier de l'annuaire tous les contacts charges en memoire
 * @author Alexandre Coulais
 * 
 * @param pAnnuaire Annuaire dont on souhaite faire une sauvegarde
 * @param mail      Mail du compte correspondant a l'annuaire
 * @return int      Retourne 0 si ca s'est bien passe, 1 sinon
 */
int saveAnnuaire(annuaire pAnnuaire, char *mail);

/**
 * @brief Ajout d'un contact à l'annuaire passe en parametre
 * @author Alexandre Coulais
 * 
 * @param pContact  Pointeur sur le contact qu'on souhaite ajouter a l'annuaire
 * @param mail      Mail du compte correspondant a l'annuaire dans lequel ajouter le contact
 * @return int      Retourne 0 si ca s'est bien passe, 1 sinon
 */
int addContact(contact pContact, char *mail);

/**
 * @brief Supprime un contact a partir de l'adresse mail passee en parametre
 * @author Alexandre Coulais
 * 
 * @param contactMail   Mail du contact a supprimer
 * @param userMail      Mail du compte correspondant a l'annuaire contenant le contact a supprimer
 * @return int          Retourne 0 si ca s'est bien passe, 1 sinon
 */
int deleteContact(char *contactMail, char *userMail);

/**
 * @brief Supprime une autorisation d'acces à l'annuaire de l'utilisateur connecte
 * @author Alexandre Coulais
 * 
 * @param userMail  Mail de l'utilisateur sur lequel on souhaite supprimer l'acces au compte de otherMail
 * @param otherMail Mail de l'utilisateur dont on souhaite supprimer l'acces
 * @return int      Retourne 0 si ca s'est bien passe, 1 sinon
 */
int deleteShared(char *userMail, char *otherMail);

/**
 * @brief Ajoute une autorisation d'acces à l'annuaire de l'utilisateur connecte
 * @author Alexandre Coulais
 * 
 * @param userMail  Mail de l'utilisateur sur lequel on souhaite ajouter l'acces au compte de otherMail
 * @param otherMail Mail de l'utilisateur dont on souhaite ajouter l'acces
 * @return int      Retourne 0 si ca s'est bien passe, 1 sinon
 */
int addShared(char *userMail, char *otherMail);

/**
 * @brief Obtenir la liste des ID de comptes qui ont partagés leur annuaire avec l'utilisateur
 * @author Guillaume FOURCROY
 * 
 * @param number    Variable ou stocker le nombre de comptes dont l'utilisateur a accès
 * @param mail      Mail de l'utilisateur dont on souhaite obtenir les informations
 * @return int*     Retourne la liste des id des compte des annuaires \
 * \n auxquels a acces l'utilisateur et NULL si ca s'est mal passe
 */
int *getView(char *mail, size_t *number);

/**
 * @brief Obtenir la liste des ID de comptes qui ont accès a l'annuaire de l'utilisateur
 * @author Alexandre Coulais
 * 
 * @param number    Variable ou stocker le nombre de comptes qui ont acces a l'annuaire
 * @param mail      Mail de l'utilisateur dont on souhaite obtenir les informations
 * @return int*     Retourne la liste des id des comptes \
 * \n auxquels l'utilisateur à partagé son annuaire et NULL si ca s'est mal passe
 */
int *getShared(char *mail, size_t *number);

/**
 * @brief Libere la memoire allouee pour la liste des contacts et l'annuaire charge \
 * \n en variable globale
 * @author Alexandre Coulais
 */
void unload(void);

/**
 * @brief Verifie si l'utilisateur a la permission de lire/modifier
 * @author Guillaume Fourcroy
 * 
 * @param pCompte compte de l'utilisateur connecte
 * @param mail mail de l'utilisateur du compte a modifier
 * @return int 0 si il n'y a aucune permission \
 * \n 1 si il peut le lire 2 si il peut le modifier
 */
int gotPermission(compte pCompte, char* mail);

/**
 * @brief Recupère la dernière erreur de fichier et la reset
 * 
 * @return int l'erreur de fichier
 */
int getfileError();

/**
 * @brief A partir d'une liste d'ID de compte, renvoie une liste de mails
 * 
 * @param listeID La liste d'ID de comptes
 * @param tailleListe la taille de la liste de compte
 * @return char** La liste de mails de comptes
 */
char** listIDToListMail(int* listeID, int tailleListe);

/**
 * @brief Récupère l'indice de l'emplacement du contact dans l'annuaire
 * 
 * @param contact Le contact à vérifier
 * @param annuaire L'annuaire ou il faut récupérer l'indice
 * @param tailleAnnuaire La taille de l'annuaire
 * @return size_t -1 si il n'existe pas, l'indice sinon
 */
size_t getIndiceContactAnnuaire(contact contact, annuaire annuaire, size_t tailleAnnuaire);

#endif