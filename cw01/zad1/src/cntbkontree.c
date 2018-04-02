#include <stdlib.h>
#include <stdbool.h>
#include "../includes/contact.h"
#include "../includes/cntbkontree.h"

//
// Created by kwrobel on 09.03.17.
//


void addContactToTree(contactBookOnTree *tree, contact *tcontact){
    tNode *t1;
    tNode *t2;
    t1=malloc(sizeof(tNode));
    t1->left=t1->right=NULL;
    t1->contact=tcontact;
    t2=tree->root;

    if(!(t2)){
        tree->root=t1;
    }
    else{
        while(true){
            if(strcmp(tcontact->surname,t2->contact->surname)<0){
                if(!(t2)->left){
                    (t2)->left=t1;
                    break;
                }
                else (t2)=(t2)->left;
            }
            else{
                if(!(t2)->right){
                    (t2)->right=t1;
                    break;
                }
                else (t2)=(t2)->right;
            }
        }
    }
    t1->up=(t2);
    tree->count++;

}

tNode *findInBST(contactBookOnTree *tree,char *key,char by){
    tNode *tmp=tree->root;
    while(tmp && compareContactWithKey(tmp->contact,key,by)!=0){
        tmp = !(compareContactWithKey(tmp->contact,key,by) < 0) ? tmp->left : tmp->right;
    }
    return tmp;
}

void addNodeToTree(contactBookOnTree *tree,tNode *node,char by){
    tNode *tmp=tree->root;
    if(!tmp){
        tree->root=node;
    }
    else{
        while(true){
            if(compare(node->contact,tmp->contact,by)<0){
                if(!tmp->left){
                    tmp->left=node;
                    break;
                }
                else tmp=tmp->left;
            }
            else{
                if(!tmp->right){
                    tmp->right=node;
                    break;
                }
                else tmp=tmp->right;
            }
        }
    }
    node->up=tmp;
    tree->count++;
}

void rebuildTree(contactBookOnTree *tree,tNode *toInsert, char by){
    if(!toInsert) return;
    tNode *right = toInsert->right;
    tNode *left = toInsert->left;
    toInsert->left=NULL;
    toInsert->right=NULL;

    rebuildTree(tree,left,by);
    if(!tree->root){
        tree->root=toInsert;
    }
    else{
        addNodeToTree(tree,toInsert,by);
    }
    rebuildTree(tree,right,by);
}

void rebuildBy(contactBookOnTree *tree, char by){
    tNode *root=tree->root;
    tree->root=NULL;
    tree->count=0;
    rebuildTree(tree,root,by);
}

void deleteTree(tNode *node){
    if(node){
        deleteTree(node->left);
        deleteTree(node->right);
        free(node);
    }
}

void deleteContactBookOnTree(contactBookOnTree *tree){
    deleteTree(tree->root);
    tree->root=NULL;
    tree->count=0;

}

tNode *minNode(tNode *node){
    if(node) while(node->left) node=node->left;
    return node;
}

tNode *succNode(tNode *node){
    tNode *tmp;

    if(node){
        if(node->right) return minNode(node->right);
        else{
            tmp=node->up;
            while(tmp && (node==tmp->right)){
                node=tmp;
                tmp=tmp->up;
            }
        return tmp;
        }
    }
    return node;
}

tNode * minValueNode(tNode* node)
{
    tNode* current = node;

    /* loop down to find the leftmost leaf */
    while (current->left != NULL)
        current = current->left;

    return current;
}

/* Given a binary search tree and a key, this function deletes the key
   and returns the new root */
tNode* _deleteNode(tNode *root, contact *contact, char by){
    // base case
    if (root == NULL) return root;

    // If the key to be deleted is smaller than the root's key,
    // then it lies in left subtree
    //if (key < root->key)
    if(compare(contact,root->contact,by)<0)
        root->left = _deleteNode(root->left, contact, by);

        // If the key to be deleted is greater than the root's key,
        // then it lies in right subtree
        //else if (key > root->key)
    else if(compare(contact,root->contact,by)>0)
        root->right = _deleteNode(root->right, contact, by);

        // if key is same as root's key, then This is the node
        // to be deleted
    else
    {
        // node with only one child or no child
        if (root->left == NULL)
        {
            tNode *tmp = root->right;
            free(root);
            return tmp;
        }
        else if (root->right == NULL)
        {
            tNode *tmp = root->left;
            free(root);
            return tmp;
        }

        // node with two children: Get the inorder successor (smallest
        // in the right subtree)
        tNode* tmp = minValueNode(root->right);

        // Copy the inorder successor's content to this node
        root->contact = tmp->contact;

        // Delete the inorder successor
        root->right = _deleteNode(root->right, tmp->contact, by);
    }
    return root;
}

void deleteNode(contactBookOnTree *tree, contact *contact, char by) {
    tree->root = _deleteNode(tree->root,contact,by);
}


contactBookOnTree *createContactBookOnTree(){
    contactBookOnTree *tree = malloc(sizeof(contactBookOnTree));
    tree->root=NULL;
    tree->count=0;
    return tree;
}

void printBook(tNode *node){
    if(node){
        printBook(node->left);
        printContact(node->contact);
        printBook(node->right);
    }
}