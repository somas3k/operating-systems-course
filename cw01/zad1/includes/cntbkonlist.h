//
// Created by kwrobel on 10.03.17.
//

#include "contact.h"

#ifndef CNTBKONLIST
#define CNTBKONLIST

typedef struct lNode{
    struct contact *contact;
    struct lNode* next;
    struct lNode* prev;
} lNode;

typedef struct contactBookOnList{
    struct lNode *head,*tail;
    unsigned count;
} contactBookOnList;

void push_front(contactBookOnList *list,contact *contact1);
void push_back(contactBookOnList *list,contact *contact);
void remove_node(contactBookOnList *list,lNode *node);
void pop_front(contactBookOnList *list);
void pop_back(contactBookOnList *list);
void partition(contactBookOnList *list,lNode *Lw,lNode *Rw,char by);
void sort(contactBookOnList *list,char by);
contactBookOnList *createContactBookOnList();
void deleteContactBookOnList(contactBookOnList *list);
void addContactToList(contactBookOnList *list,contact *contact);
lNode *search(contactBookOnList *list,char *key,char by);
lNode **searchMore(contactBookOnList *list, char *key,char by);
void deleteContactInList(contactBookOnList *list,char *key,char by);



#endif //CNTBKONLIST
