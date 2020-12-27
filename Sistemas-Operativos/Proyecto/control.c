// C program rocket simulation
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define SHMSZ 4
#define NUM 5
#define SHM_ADDR 233

int *param[NUM], *distancia, *nivel, *giro1, *giro2, *alarma;
int shmid[NUM];

int inicializar_memoria_compartida(void);

int main(int argc, char *argv[])
{

   if (inicializar_memoria_compartida() == -1)
      exit(-1);
   while (1)
   {            //leo datos
      sleep(1); //solo con propósitos ilustrativos
      printf("Valor actual distancia %d combustible %d giro1 giro2 alarma= %d %d %d \n", *distancia, *nivel, *giro1, *giro2, *alarma);
   }
}

int inicializar_memoria_compartida(void)
{
   int i;

   for (i = 0; i < NUM; i++)
   {
      if ((shmid[i] = shmget(SHM_ADDR + i, SHMSZ, 0666)) < 0)
      {
         perror("shmget");
         return (-1);
      }
      if ((param[i] = shmat(shmid[i], NULL, 0)) == (int *)-1)
      {
         perror("shmat");
         return (-1);
      }
   }

   distancia = param[0];
   nivel = param[1];
   giro1 = param[2];
   giro2 = param[3];
   alarma = param[4];

   return (1);
}
