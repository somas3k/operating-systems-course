//
// Created by kwrobel on 05.05.17.
//

#ifndef LAB7_FIFO_H
#define LAB7_FIFO_H

void initFIFO(int *tab, int seats);
int isEmptyFIFO(int *tab);
int isFullFIFO(int *tab);
int pushFIFO(int pid, int *tab);
int popFIFO(int *tab);
int getChair(int *tab);
void seatOnChair(int *tab,int pid);

#endif //LAB7_FIFO_H
