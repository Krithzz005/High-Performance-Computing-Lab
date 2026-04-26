[23bcs074@mepcolinux e3]$cat ex3
ada@ada:~$ cat hello.c
#include <stdio.h>
#include <mpi.h>

int main(void)
{
    int rank, size;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Hello from process %d out of %d processes\n", rank, size);
    MPI_Finalize();
    return 0;
}

ada@ada:~$ mpicc hello.c
ada@ada:~$ mpiexec -n 4 ./a.out
Hello from process 1 out of 4 processes
Hello from process 2 out of 4 processes
Hello from process 0 out of 4 processes
Hello from process 3 out of 4 processes
ada@ada:~$ cat rank.c
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<mpi.h>

#define MAX 100

int isPali(char str[]){
        int i,len=strlen(str);
        for(i=0;i<len/2;i++){
                if(str[i]!=str[len-i-1]){
                        return 0;
                }
        }
        return 1;
}

int main(void){
        int rank,size;
        char msg[MAX];
        MPI_Init(NULL,NULL);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);
        MPI_Comm_size(MPI_COMM_WORLD,&size);
        if(rank!=0){
                char *str1="radar";
                char *str2="leveel";
                char *str3="legal";
                 if(rank==1){
                        MPI_Send(str1,MAX,MPI_CHAR,0,1,MPI_COMM_WORLD);
                 }else if(rank==2){
                         MPI_Send(str2,MAX,MPI_CHAR,0,2,MPI_COMM_WORLD);
                 }
                 else if(rank==3){
                         MPI_Send(str3,MAX,MPI_CHAR,0,1,MPI_COMM_WORLD);
                 }

        }else{
                MPI_Status status;
                char recvMsg[MAX];
                int i,j;
                for(i=1;i<size;i++){
                        MPI_Recv(recvMsg,MAX,MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
                        int sender=status.MPI_SOURCE;
                        int tag =status.MPI_TAG;
                        if(tag==1){
                                for(j=0;recvMsg[j];j++){
                                        recvMsg[j]=toupper(recvMsg[j]);
                                }
                                printf("From Rank %d|Type 1|Uppercase:%s\n",sender,recvMsg);
                        }else if(tag==2){
                                if(isPali(recvMsg)){
                                        printf("From Rank %d|Type 2|String :%s is Palindrome\n",sender,recvMsg);
                                }else{
                                        printf("From Rank %d|Type 2|String :%s is Not Palindrome\n",sender,recvMsg);
                                }
                        }
                }
        }
        MPI_Finalize();
        return 0;
        }



ada@ada:~$ mpicc rank.c
ada@ada:~$ mpiexec -n 4 ./a.out
From Rank 2|Type 2|String :leveel is Not Palindrome
From Rank 3|Type 1|Uppercase:LEGAL
From Rank 1|Type 1|Uppercase:RADAR
ada@ada:~$ exit

[23bcs074@mepcolinux e3]$exit
exit

