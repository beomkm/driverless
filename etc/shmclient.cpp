#include <cstdio>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

int main(void)
{
    int shmid;
    size_t shsize = 1024;
    const int key = 16000;
    char *shm;

    if((shmid = shmget((key_t)key, shsize, IPC_CREAT|0666))<0) {
        perror("shmget");
        exit(1);
    }

    if((shm = (char*)shmat(shmid, NULL, 0)) == (char*)-1) {
        perror("shmat");
        exit(1);
    }

    putchar(shm[0]);
    putchar(shm[1]);
    putchar(shm[2]);
    putchar('\n');

    if(shmdt(shm) == -1) {
       perror("shmdt");
       exit(1);
    }

    return 0;
}
