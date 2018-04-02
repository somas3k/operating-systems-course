//
// Created by kwrobel on 05.05.17.
//

#include "FIFO.h"

void initFIFO(int *tab, int seats){
    tab[0] = seats+4;
    int N = tab[0];
    tab[N-1] = 0;
    tab[N-2] = 2;
}

int isEmptyFIFO(int *tab){
    int N = tab[0];
    int qcnt = tab[N-1];
    if(qcnt==0) return 1;
    return 0;
}

int isFullFIFO(int *tab){
    int N = tab[0];
    int qcnt = tab[N-1];
    if(qcnt==N-4) return 1;
    return 0;
}

int pushFIFO(int pid, int *tab){
    int i;
    int N = tab[0];
    if(isFullFIFO(tab)==0){
        i=tab[N-2]+tab[N-1];
        tab[N-1]++;
        if(i>N-3) i -= N-4;
        tab[i]=pid;
        return 0;
    }
    return -1;
}

int popFIFO(int *tab){
    int N = tab[0];
    if(isEmptyFIFO(tab)==0){
        int result = tab[tab[N-2]];
        tab[N-1]--;
        tab[N-2]++;
        if(tab[N-2]==N-2) tab[N-2] = 2;
        return result;
    }
    return -1;
}

int getChair(int *tab){
    int chair = tab[1];
    tab[1]=-1;
    return chair;
}

void seatOnChair(int *tab,int pid) {
    tab[1] = pid;
}