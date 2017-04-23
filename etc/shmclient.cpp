#include <cstdio>
#include <thread>
#include <chrono>

#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

namespace chro = std::chrono;
namespace tthr = std::this_thread;

int main(void)
{
    int shmid;
    size_t shsize = 1024;
    const int key = 16000;
    char *shm;

    sem_t *mysem;
    if((mysem = sem_open("mysem", 0, 0777, 0)) == SEM_FAILED) {
        perror("Sem Open Error");
        exit(1);
    }

    if((shmid = shmget((key_t)key, shsize, IPC_CREAT|0666))<0) {
        perror("shmget");
        exit(1);
    }

    if((shm = (char*)shmat(shmid, NULL, 0)) == (char*)-1) {
        perror("shmat");
        exit(1);
    }

    for(;;) {
        sem_wait(mysem);
        for(int i=0; i<100; i++) {
            putchar(shm[i]);
        }
        sem_post(mysem);
        putchar('\n');
        tthr::sleep_for(chro::milliseconds(100));
    }

    if(shmdt(shm) == -1) {
       perror("shmdt");
       exit(1);
    }

    return 0;
}
