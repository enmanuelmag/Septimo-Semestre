Short-term: es el que se encarga de escoger que proceso de la cola de procesos lista (ready queue) entra al CPU. Se ejcuta al menos una vez cada 100 milisegundos

Medium-term: es el que se encarga de realizar swaping(tanto in (mem prin) como out (disco))

Long-term: Es el encargdo de meter procesos "recientemen creados" a la cola de procesos lista (ready queue), osea los coloca en memoria

El cambio de contexto en un proceso que realiza el kernel en el cual se guarda el contexto (PCB) del proceso saliente y se carga el contexto (PCB) del proceso entrante

"""Datos como descriptores de arhivos, pool de hilos se guarda en la cache del CPU? El .text donde se carga"""

A: 0 B:2603 // C: 2603 D: 2600

A:  

| Tables   |      Ventajas |  Desventajas |
|:----------:|:-------------:|:-------------:|
| Sync     |  Se asegura que lo reciba y que el otro lo pueda leer | Los procesos se pueen quedar bloqueados por la espera de que el otro proceso envia o reciba segun sea el caso |
| Async    |    Los procesos siguen ejcutando sus intrucciones y no esperan por el otro   | No se sabe si el paquete enviado fue recibido satisfactoriamente |
| BUffering | crece segun la demanda | puede llegar a ocupar mucho espcio en la memoria |
| Send Copy | otros proceso no podran modificar el valor original  | cualquiera puede cambiar el valor |
| Fixed msg  | Se sabe siempre el tamaño y estructura del dato a leer  | No se puede enviar mas tipos de datos, por que pueden cambiar su tamaño / No se puede ennviar datos mas grandes  |
| Var msg | Permite enviar todo tipo de imgenes  | No se sabe hasta cuando leer |
    

# Hilos
