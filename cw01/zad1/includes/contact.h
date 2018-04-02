//
// Created by kwrobel on 09.03.17.
//

#ifndef CONTACT
#define CONTACT

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct contact{
    char *name;
    char *surname;
    char *birthday;
    char *email;
    char *telnumber;
    char *address;
} contact;

contact *createContact(char *name,char *surname, char *birthday, char *email, char *telnumber, char *address);

void printContact(contact *t);

int compareContactWithKey(contact *tcontact, char *key, char by);

int compare(contact *c1,contact *c2, char by);

void deleteContact(contact *ptr);

#endif
