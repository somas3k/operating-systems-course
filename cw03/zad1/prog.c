#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char **argv){
    if(argc!=2){
	printf("Wrong number of parameters!!\n");
	exit(EXIT_FAILURE);
    }
    char *tmp=getenv(argv[1]);
    if(tmp!=NULL) printf("%s\n",tmp);
    else printf("Can't find variable!!\n");
    

    return 0;
}
