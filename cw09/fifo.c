//
// Created by kwrobel on 22.05.17.
//

#include <stdlib.h>
#include "fifo.h"

fifo *fifo_init(){
    fifo *f = malloc(sizeof(fifo));
    f->first=NULL;
    f->last=NULL;
    return f;
}

thread_info fifo_pop(fifo *f){
    thread_info res;
    if(!fifo_isEmpty(f)){
        res = f->first->info;
        thread *tmp = f->first;
        f->first=f->first->next;
        free(tmp);
        return res;
    }
    res.id = -1;
    return res;
}

void fifo_push(fifo *f, thread_info info){
    thread *new = malloc(sizeof(thread));
    new->info = info;
    new->next=NULL;
    if(fifo_isEmpty(f)){
        f->first=new;
    }
    else {
        f->last->next = new;
    }
    f->last=new;
}

int fifo_isEmpty(fifo *f){
    return f->first == NULL;
}

void free_fifo(fifo *f){
    while(f->first != NULL) {
        thread *tmp = f->first;
        f->first = tmp->next;
        free(tmp);
    }
    free(f);
}
