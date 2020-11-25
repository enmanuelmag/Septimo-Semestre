#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define len_buff 1000

int main(int arc, char **arvg){


    char *path_src = arvg[1];
    printf("%s\n", path_src);

    char *path_dst = arvg[2];
    printf("%s\n", path_dst);

    umask(0);
    int origen = open(path_src, O_RDONLY, "666");
    if(origen == -1){
        perror("Error al abrir el archivo de origen");
        exit(0);
    }
    int destino = open(path_dst, O_RDONLY | O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, "666");
    if(destino == -1){
        perror("Error en el open del archivo destino");
        exit(0);
    }

    char *buff = (char *) calloc(len_buff, sizeof(char));
    int total = 0;
    int numByt;
    while((numByt = read(origen, buff, len_buff)) != 0){
        
        total += numByt;
        write(destino, buff, numByt);
        free(buff);
        buff = (char *) calloc(len_buff, sizeof(char));
    }

    printf("Bytes copiados: %d\n", total);
    close(origen);
    close(destino);

    free(buff);

    return 0;
}


