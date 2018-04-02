//
// Created by kwrobel on 09.03.17.
//


#include "../includes/contact.h"

contact *createContact(char *name,char *surname, char *birthday, char *email, char *telnumber, char *address){
    contact *t=malloc(sizeof(contact));
    t->name=strdup(name);
    t->surname=strdup(surname);
    t->birthday=strdup(birthday);
    t->email=strdup(email);
    t->telnumber=strdup(telnumber);
    t->address=strdup(address);
    return t;
}

void printContact(contact *t){
    printf("%s\t\t%s\t\t%s\t\t%s\t\t%s\t\t%s\n",t->name,t->surname,t->birthday,t->email,t->telnumber,t->address);
}

int compareContactWithKey(contact *tcontact, char *key, char by){
    switch(by){
        case 's':
            return strcmp(tcontact->surname,key);
        case 'b':
            return strcmp(tcontact->birthday,key);
        case 'e':
            return strcmp(tcontact->email,key);
        case 't':
            return strcmp(tcontact->telnumber,key);
    }
    return 0;
}

int compare(contact *c1,contact *c2, char by){
    switch(by){
        case 's':
            return strcmp(c1->surname,c2->surname);
        case 'b':
            return strcmp(c1->birthday,c2->birthday);

        case 'e':
            return strcmp(c1->email,c2->email);
        case 't':
            return strcmp(c1->telnumber,c2->telnumber);
    }
    return 0;
}

void deleteContact(contact *ptr){
    free(ptr->address);
    free(ptr->birthday);
    free(ptr->name);
    free(ptr->surname);
    free(ptr->telnumber);
    free(ptr->email);
    free(ptr);
}