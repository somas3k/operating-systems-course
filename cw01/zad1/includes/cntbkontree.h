//
// Created by kwrobel on 13.03.17.
//

#ifndef CNTBKONTREE_H
#define CNTBKONTREE_H

#include "contact.h"

typedef struct tNode{
    struct contact *contact;
    struct tNode* left;
    struct tNode* right;
    struct tNode* up;
} tNode;

typedef struct contactBookOnTree{
    tNode *root;
    int count;
} contactBookOnTree;

void addContactToTree(contactBookOnTree *tree, contact *tcontact);

tNode *findInBST(contactBookOnTree *tree,char *key,char by);

void addNodeToTree(contactBookOnTree *tree,tNode *node,char by);

void rebuildTree(contactBookOnTree *tree,tNode *toInsert, char by);

void rebuildBy(contactBookOnTree *tree, char by);

void deleteTree(tNode *node);

void deleteContactBookOnTree(contactBookOnTree *tree);

tNode *minNode(tNode *node);

tNode *succNode(tNode *node);

void deleteNode(contactBookOnTree *tree, contact *contact, char by);

contactBookOnTree *createContactBookOnTree();

void printBook(tNode *node);

#endif
