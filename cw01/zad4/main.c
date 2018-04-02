#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "contact.h"
#include "cntbkonlist.h"
#include "cntbkontree.h"
#include <time.h>
#include <sys/times.h>
#include <sys/resource.h>


typedef FILE* file;



int main() {

    file fp;
    if((fp=fopen("contacts.txt","r"))==NULL){
        printf("Nie mozna otworzyc pliku\n");
        exit(1);
    }

    char buffor[200];
    contact *tab[1000];
    char *name,*surname,*birthday,*email,*telnumber,*address;
    contact *tcontact;

    int i=0;


    while(fgets(buffor,sizeof buffor,fp)){
        name=strtok(buffor,",");
        surname=strtok(NULL,",");
        birthday=strtok(NULL,",");
        email=strtok(NULL,",");
        telnumber=strtok(NULL,",");
        address=strtok(NULL,"\n");

        tcontact=createContact(name,surname,birthday,email,telnumber,address);
        tab[i]=tcontact;
        i++;
    }

    fclose(fp);



    contactBookOnList *bookOnList = createContactBookOnList();

    struct rusage rusage;
    clock_t start,end;
    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    double u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    double s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;

    for(i=0;i<1000;i++){
        addContactToList(bookOnList,tab[i]);
    }

    getrusage(RUSAGE_SELF, &rusage);
    end=clock();
    double u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    double s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("creating contact book on list:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    deleteContactBookOnList(bookOnList);

    lNode *tmp;
    tmp=tmp->next;

    bookOnList=createContactBookOnList();

    for(i=0;i<1000;i++){
        getrusage(RUSAGE_SELF,&rusage);
        start=clock();
        u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec/(double)1e6 ;
        s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec/(double)1e6 ;
        addContactToList(bookOnList,tab[i]);
        getrusage(RUSAGE_SELF, &rusage);
        end=clock();
        u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec/ (double)1e6 ;
        s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec /(double)1e6;
        printf("adding %d element: ",i+1);
        printf("Real: %.7f System: %.7f User: %.7f\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    }
    printf("\n");
    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    deleteContactInList(bookOnList,"troberts0@example.com",'e');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("deleting in optimistic case:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    deleteContactInList(bookOnList,"jfordrr@jugem.jp",'e');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("deleting in worst case:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);



    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    search(bookOnList,"jgutierrez1@acquirethisname.com",'e');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("searching in optimistic case\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);


    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    search(bookOnList,"tgonzalezrq@state.gov",'e');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("searching in worst case:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    sort(bookOnList,'s');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("sorting by surname:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    sort(bookOnList,'e');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("sorting by email:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    sort(bookOnList,'t');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("sorting by phone number:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    sort(bookOnList,'b');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("sorting by birthday:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);
    deleteContactBookOnList(bookOnList);




    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    contactBookOnTree *bookOnTree=createContactBookOnTree();
    for(i=0;i<1000;i++){
        addContactToTree(bookOnTree,tab[i]);
    }
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("creating contact book on tree:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);
    deleteContactBookOnTree(bookOnTree);

    bookOnTree=createContactBookOnTree();

    for(i=0;i<1000;i++){
        getrusage(RUSAGE_SELF,&rusage);
        start=clock();
        u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
        s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
        addContactToTree(bookOnTree,tab[i]);
        getrusage(RUSAGE_SELF,&rusage);
        end=clock();
        u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
        s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
        printf("adding %d element: ",i+1);
        printf("Real: %.7f System: %.7f User: %.7f\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);
    }
    printf("\n");

    rebuildBy(bookOnTree,'e');

    rebuildBy(bookOnTree,'e');

    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    tNode *tmp1=findInBST(bookOnTree,"aallen8e@hexun.com",'e');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("searching in optimistic case:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    deleteNode(bookOnTree,tmp1->contact,'e');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("deleting in optimistic case:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    tmp1=findInBST(bookOnTree,"wwilliamsp7@flickr.com",'e');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("searching in worst case:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    deleteNode(bookOnTree,tmp1->contact,'e');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("deleting in worst case:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    rebuildBy(bookOnTree,'s');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("rebuilding from email to surname:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    rebuildBy(bookOnTree,'b');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("rebuilding from surname to birthday:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);


    getrusage(RUSAGE_SELF,&rusage);
    start=clock();
    u_s = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_s = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    rebuildBy(bookOnTree,'t');
    getrusage(RUSAGE_SELF,&rusage);
    end=clock();
    u_e = (double)rusage.ru_utime.tv_sec + (double)rusage.ru_utime.tv_usec / (double)1e6;
    s_e = (double)rusage.ru_stime.tv_sec + (double)rusage.ru_stime.tv_usec / (double)1e6;
    printf("rebuilding from birthday to telephone:\n");
    printf("Real: %.7f System: %.7f User: %.7f\n\n",((double)(end-start))/CLOCKS_PER_SEC,s_e-s_s,u_e-u_s);

    return 0;
}
