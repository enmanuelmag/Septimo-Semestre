### Como se quita a un proceso?
En mainboard hay temporizadores, el so lo setea conel tiempo, este recibe el clcok del cpu, mientra el cpu ejecuta el proceso y el temp va abajdno. es un hard aparte aunque puede estar dentro. cuando llega a 0 el hardware activa una señal y el so se entera que el tiempo se acabo (time slice). 

Cuando el planificador de corto plazo mete un proceso, el so setea el timer y ejecuta el proceso, cuando llega a 0 el so toma decisiones. Quitar el uso del CPU o darle mas tiempo al proceso. Si no se segieu se guarda el contexto dle proceos saliente.

### Libreacion de recursos
Si un programa termina, el so se entera (exit y return), esas son systemcall, el so mira los recursos que hay involucrados y mira cuales son los bloques de memoria libres, trata de liberarlos, no siempre se puede

### Bloques de memoria comaprtida, que pasa cuando no son procesos adyacentes?

### El de largo plazo carga en memoria principal? la cola de procesos lista es la memoria principal?
EN EL KERNEL osea ocupa una fraccion en el memoria del computador RAM, UNA COSA POR CADA PROCESADOR. 

### Kernel
La memoeria se divivde en dos, los procesos y en otra parte el Kernel. EL vive en otro espcio de memoria, porque las llamadas priviligeadas nunca deben ser accecibles por un proceso. EL no tiene pid, PCB, nada de eso. El codigo de kernel de ejecuta cuando sale una interrupcion, son ejecutadas por el SO.

El kernel puede crear procesos

---------------

El proceso puede procesar señales (interrupciones) si es que tiene codigo asociado.

Short-term: CPU scheduler, cual proceso entra al CPU

Long-term: Job Scheduler, selecciona cual entra a la cola de procesos listos. EL que determine cuantos procesos hay en la ready-queue, el controla el grado de multiprogramacion.
- I/O-bound, pasan mayor tiempo con operacion de entrada y salida
- CPU-bound, pasan mayor tiempo haciendo computo.

## Swapping
PLanificador de temrino medio, de remover un proceos de la memoria y ponerlo en el disco duro. Caundo haya mas recursos de la memoria los coloca en la memoria.

## Sistemas Mobiles en multitareas
EL SO de estos dispositivos dan servicios en el kernel para que no se desperdicie recursos.
Estados:
- foreground, proceso que esta en primer plano
- background, se le da muy poco tiempo

En iOS te da lso serivicios, en android peudes crear servicios (sdk a prueba de tontos, ndk puedes usar C para crear los programas)

### Context Switch
El cambio de contexto es cuando se turnan procesos el uso de CPU. Se guarda el estado del proceso y se carga el estado del nuevo proceso entrante. EL contenido de contexto es el PCB, se represetna en el PCB.
Este cambio de contexto tiene que super rapido, para que el tiempo de uso nulo se lo menor posible.

El SO para:
- creacion de procesos
- terminacion de procesos
- y mas

### Creacion de procesos
Un proceso crea un proceso adicional, este nuevo se le llama hijo.
Se puede:
- hacer uqe padres e hijos peleean por el cpu
- hacer que el padre espere por el hijo

El proceso init (1) carga los servicios (por que se cargan al inicio del SO)

Fork si no s ele pasa arguemtnos, crea un proceso hijo con la misma memoria que el proceso padre.
