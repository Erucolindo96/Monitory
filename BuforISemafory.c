// Kompilacja gcc pam-dziel.c -o pam-dziel -lrt 
#include <sys/mman.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <string.h>
#include <unistd.h> 
#define SIZE 60   // Rozmiar bufora 
 
typedef struct { 
    int typ; 
    char text[SIZE]; 
 } buf_t; 

int main(int argc,char *argv[]) { 
 int i,stat,pid,k,res; 
 buf_t  *buf; 
 char name[16]; 
 int  fd; // Deskryptor segmentu  
 strcpy(name,"Bufor"); 
 shm_unlink(name);   
 
 // Utworzenie segmentu pamieci  ---------------------  
 if((fd=shm_open(name,O_RDWR|O_CREAT,0664))==-1) { 
    perror("shm_open"); 
    exit(-1); 
 } 
 printf("fh = %d\n",fd); 
 
// Okreslenie rozmiaru obszaru pamieci ---------------     
 res = ftruncate(fd,sizeof(buf_t)); 
 if(res < 0) { perror("ftrunc"); return 0; } 
 
// Odwzorowanie segmentu fd w obszar pamieci procesow  
 buf = (buf_t *) mmap(0,sizeof(buf_t), PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0); 
 if(buf == NULL) { perror("mmap"); exit(-1); } 
 printf("start\n"); 
 
// Proces potomny P2 - pisze do pamieci wspolnej ----- 
 if(fork() == 0) {   
     buf-> typ = 1; 
     for(k=0;k<10;k++) { // Zapis do bufora 
        printf("Zapis - Komunikat %d\n",k); 
        sprintf(buf->text,"Komunikat %d",k); 
        sleep(1); 
     } 
     exit(0); 
 }  


 // Proces macierzysty P1 czyta z pamięci wspólnej - 
 for(i=0;i<10;i++) { 
     printf("Odczyt %s\n",buf->text); 
     sleep(1); 
 } 
 // Czekam na potomny -- 
 //pid = wait(&stat); 
 sleep(60);
 return 0; 
} 
