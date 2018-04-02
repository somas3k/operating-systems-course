#include <stdlib.h>
#include <stdbool.h>
#include "../includes/contact.h"
#include "../includes/cntbkonlist.h"

//
// Created by kwrobel on 09.03.17.
//

int count;

void push_front(contactBookOnList *list,contact *contact1){
    lNode *p=malloc(sizeof(lNode));
    p->contact=contact1;
    p->prev=NULL;
    p->next=list->head;
    list->head=p;
    list->count++;
    if (p->next) p->next->prev = p;
    else list->tail = p;
}

void push_back(contactBookOnList *list,contact *contact) {
    lNode *p = malloc(sizeof(lNode));
    p->contact = contact;
    p->next = NULL;
    p->prev = list->tail;
    list->tail = p;
    list->count++;
    if (p->prev) p->prev->next = p;
    else list->head = p;
}

void remove_node(contactBookOnList *list,lNode *node){
    list->count--;
    if(node->prev) node->prev->next = node->next;
    else list->head=node->next;
    if(node->next) node->next->prev = node->prev;
    else list->tail=node->prev;
    free(node);
}

void pop_front(contactBookOnList *list){
    if(list->count) remove_node(list,list->head);
}

void pop_back(contactBookOnList *list){
    if(list->count) remove_node(list,list->tail);
}

void partition(contactBookOnList *list,lNode *Lw,lNode *Rw,char by){
    lNode *p,*i,*j;
    p=Lw->next;
    i=p->next;
    if(i != Rw){
        do{
            j=i;
            i=i->next;
            if(compare(j->contact,p->contact,by)<0){
                j->prev->next = j->next;
                j->next->prev=j->prev;
                j->next=p;
                j->prev=p->prev;
                p->prev=j;
                j->prev->next=j;
            }
        } while(i != Rw);

        if(Lw->next!=p) partition(list,Lw,p,by);
        if(p->next!=Rw) partition(list,p,Rw,by);
    }
}

void sort(contactBookOnList *list,char by){
    if(list->count>1){
        contact *t = malloc(sizeof(contact));
        push_front(list,t);
        t=malloc(sizeof(contact));
        push_back(list,t);
        partition(list,list->head,list->tail,by);
        pop_back(list);
        pop_front(list);
    }
}

contactBookOnList *createContactBookOnList(){
    contactBookOnList *tmp = malloc(sizeof(contactBookOnList));
    tmp->count=0;
    tmp->head=NULL;
    tmp->tail=NULL;
    return tmp;
}

void deleteContactBookOnList(contactBookOnList *list){
    lNode *head=list->head;
    lNode *tmp = list->head->next;
    while(head->next!=NULL){
        free(head);
        head=tmp;
        tmp=tmp->next;
    }
    free(head);
    free(list);
}

void addContactToList(contactBookOnList *list,contact *contact) {
    lNode *tmp = malloc(sizeof(lNode));
    tmp->contact = contact;
    tmp->next = NULL;
    tmp->prev = list->tail;
    list->tail = tmp;
    list->count++;
    if (tmp->prev) tmp->prev->next = tmp;
    else list->head = tmp;
}

lNode *search(contactBookOnList *list,char *key,char by) {
    lNode *tmp;
    tmp = list->head;
    while (tmp!= NULL){
        if(!compareContactWithKey(tmp->contact,key,by))
            return tmp;
        tmp=tmp->next;
    }
    return tmp;
}

lNode **searchMore(contactBookOnList *list, char *key,char by){
    lNode **out_table;
    count=0;
    lNode *tmp=search(list,key,by);
    if(tmp==NULL)
        return NULL;
    out_table=(lNode**)malloc(sizeof(lNode*));
    out_table[0]=tmp;
    count++;
    tmp=tmp->next;
    while(tmp!=NULL && compareContactWithKey(tmp->contact,key,by)==0 ){
        out_table=realloc(out_table,(count+1)*sizeof(lNode*));
        out_table[count]=tmp;
        tmp=tmp->next;
        count++;

    }

    return out_table;
}

void deleteContactInList(contactBookOnList *list,char *key,char by){
    lNode **tab;
    tab=searchMore(list,key,by);
    if(count==0) return;
    if(count==1) remove_node(list,tab[0]);
    else{
        printf("Found more than one contact.\nWhich contact do you want to delete? (type contact number)\n");
        int i=0;
        for(i;i<count;i++){
            printf("%d ",i);
            printContact(tab[i]->contact);
        }
        int num;
        while(true) {
            num = getchar();
            num = num - 48;
            if(num>=0&&num<count) break;
            printf("Entered wrong number. Try again\n");
        }
        remove_node(list,tab[num]);
    }


}