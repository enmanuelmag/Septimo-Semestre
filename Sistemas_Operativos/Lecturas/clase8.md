## CPU Scheduling
Maximizar el uso del CPU obtenido con multiprogrmacion

Tiempo de despacho, prepracion, no hace tiempo util

- CPU-IO Burst uso de entrada y saldia
- CPU busrt uso solo del cpu

Busrt es el tiempo, suelen ser de 5 milisegundos, erores pueden ser mas

CPU-S o short-term scheduler toma deciciones cuando:
1. cambia de running a wait. son no apropiativos, cede el uso el mism proceso. Ej: resultado de I/O request o invocacion de `wait`
2. cambia de running a ready state. son apropiativos, hay un tercero que decido que pasa cone l uso de cpu. el SO puso el temp y el temp termino y el so quita al procesador. Ej: ocurre una interrupcion
3. cambia de wait a ready state. el SO decide ahora le toca a otro proceso. son aprpiativos. Ej: completacion de I/O
4. termina un proceso. son no apropiativo, dele el uso el mismo procesador

La mayoria son apropeitivos, el SO chequea como distivuir el uso del CPU. 

la latencia de despacho (dipach) tiene que ser muy pequeña para que no afecte el rendimiento dle sistema.
Los SCHEDULER son relativamente sencillos, para que l tiempo d electura y escritura sean RAPIDOS

## CPU Scheduler

Cuando el cpu se pone IDLE el SO tiene que estar seleccionado que procesos ejecutar ahora. Esta seleccion es llevada por el short-term scheduler, lo saca de la memoria principal. 

## Preeveemptive Scheduling

EL agendamiento del CPU tiene lugar bajo 4 circuntancias:

- cuando un proceso cambia de running a waiting (como resultado de i/o o `wait` para esperar por un hijo)
- cuando un proceso cambia de running a ready (cuando una interrupcion ocurre)
- cuando un proceso cambia de waiting a ready (cuando se completa una peticion de i/o)
- cuando un proceso termina

Para las circuntancias 1 y 4, no hay opciones en terminos de angendamiento. Un nuevo proceso (si es que ese uno existe en la ready queue) debe ser seleccionado para la ejecucion. Es para las circuntancias 2 y 3 donde hay opciones

Cuando el agendamiento es bajo las circuntacias 1 y 4 nosostros decimos que el esquema de agendamiento es nonpreemtive (no preventivo) o cooperativo, de otra  manera sería preventivo (preemtive). 

El esquema nonpreemptive un CPU es asignado a un proceso hasta que este termine o cambie a waiting. Esto fue usado por Windows 3.X, Windows 95 introdujo el agendamiento preemptive.

Desafortunamente el agendamiento preventivo incurre ne un costo asociado con el acceso a la data compartida. 

## Dispatcher

Otro componete presente en el agendamiento dle CPU es la funcion de dispatcher (o despacho). El dispatcher es el modulo que da el control de CPU al proceso selecionado por el shot-term scheduler. Esta funcion involucra lo siguiente:

- cambio de contexto
- cambio a modo usuario (SOLO A MODO USUARIO)
- saltar a la ubicación adecuada en el programa de usuario para reiniciar el programa

EL dispcehr deberia ser RAPIDO, este se invoca siemrpe en cada cambio de procesos. El tiempo que se toma para el dispacher para DETENER un proceso y INICIAR otro proceso se conoce como **dispacht latency**



## Criterior de agendamiento

![](../assets/optmi.png)
- cpu utilization ++
- throughput: numero de proceos que se ejecutan por segundo ++
- turnaround time: cantiadad de tiempo de un proceso en particular --
- waiting --
- response tiem: interfaz grafica tiempo de respuesta de nuestro programa. Scheduler cuando arga un proceso se pueda interactuar con el --

## First-Come, First-Served (FCFS) Scheduling
En una algoritmo en el que el primer proceso en llegar es el primero que se se agenda y permite el uso de CPU
Convoy effect - todos los rpocesos se atrasan por el proceso que esta primero y es lento.

## Shortest-Job-First (SJF)
Cada proceso tiene que decirnos cuando va a durar su burst, si dos procesos tienen igual next burst, se uso FCFS para resolver el empate
Este da el menor tiempo promedio de espera

tn = es la longitud real, la que existió, del n-egsimo bust dle CPU. ESTO PASADO

(tau)n+1 = prediccion de valor del sigueinte busrt

(alfa) = entre 0 y 1

Define: (tau)n+1 = (alfa)tn + (1-(alfa))(tau)n

(tau) y tn NO necesriamente son IGUALES

![](../assets/predic.png)

Ejemplo de una prediccion

![](../assets/ex-pred.png)

Si se pone (alfa) como 0 se toma solo la prediccion mia, si se pne 1 solo se toma lo que en realidad paso en le pasado. (alfa) puede cambiar y ser un vlaor en funcion de otra cosa como el error

![](../assets/simply-pred.png)

## Shosrtes-Remaing-Time-First

Si un prcoeso aparece y itnee menor burst que tiempo remanente que el que se esta ejecutando, este procesos nuevo entra al CPU

![](../assets/SRTF.png)

## Priority Scheduling

- un numero de prioridad (int) is asociedo con cada proceso
- el cpu es asignado al proceso que tiene una prioridad mas alta (menor int === alta prioridad)
  - preemptive
  - nonpreemptive
- SJF es agendador de prioridad donde prioridad es inveresa de siguiente burst time
- problem === starvation - proceoss de baja prioridad puede ser nunca ejecutado
- solution === aging - con el tiempo la prioridad del proceso se la aumenta

## Round Robin (RR)

Define quantum detiempo (q). A cada proceso se le un quantum de tiempo y le permitirmos que se ejecute q unidades de tiempo, si termina bine, si no no importa, se lo saca. Cada q unidades de tiempo se cambia entre procesos y se llama al agendador.

Cada proceso en la practiva, recibe 1/n. lo maximo que espera un proceso (n-1)*q. La variable q esta en el orden de los milisegundos. Si q es muy grande, esto se convierte FIFO, Si q es muy pequeño es similar al tiempo de cambio de contexto o despacho, pueda que se dmeore la misma cantiada de tiempo despachando que haciendo trbajao util

Los cmabio de contexto estan en el orden de los microsegundos

El quatum perfecto puede ser cuando los burst son menores al quatum, un 80% de ellos.

## Multilevel Queue

- foreground (interactive)
- background (batch)

Se tiene una cola multinivel. se planifican primera las colas de mas alto nivel. Otro enfoque es dar un q a un conjunto y a cada element un % de ese q. Y dentro de cada cola se puede planificar los procesos. Se puede planifcar no solo los proceoss si no tambien las mismas colas.

## Multilevel Feedback Queue
Los procesos se pueden mover entre las cosas. Es el concepto de Aging, ir subiendole la prioridad al proceso confirme pasa el tiempo. Tambien se puede darle downgrade, osea, "rejuvenecerlos" El procesos al regresar a las colas no siemrpe regresan a la cola donde iniciaron, depnde que estuvieron hacienod, los de i/o pueda que se pongan en las de mayor prioridad.

Ejemplo de como un proceo inica arriba, se le da un q de 8, si no termina va a la segunda con un q de 16 para darle mas chance y si no se le manda a la FIFO, donde puede terminar todo loq ue tenga que hacer (las ualitmas colas se ejecutan con menos probabildiad que las de arriba)

En generar una Multilevel Feedback Queue esta definido por los siguientes parametros

- el numero de queues
- el algortimos de agendamiento de la cola
- el metodo usado para determinar cuando subir un poceso a otra colar de mas prioridad
- the metodo usado para determinar cuando un proceso debe bajar a una cola de menor prioridad
- el metodo usado para determinar en cual copla entrada un proceso cuando este necesta ser servido

![](../assets/multi-level.png)

REALIZAR EJERCICIOS DE COLAS

## Thread Scheduling

En sistema que implemntat many-to-one y many-to-many models, la libreria de hilos agenda el hilo de usuario a correr en una LWP disponible. en un esquema conocido como process-contention-scope (PCS). Cuando decimos que la libreria agenda a los hilso de usuario en un LWP dispinible, no queire decir que el hilo esta actualmente corriendo en una CPU, esto requerie que el SO afende un hilo de kernel en una CPU física. Para decidir cual hilo de kernel se agenda en el CPU el kernel usa System-Contentio Scope (SCS). 

Normalmente el PCS esta acuerdo a la prioridad, el agendar selecciona el hilo ejecutable con la prioridad mas alta
Un hilo puede ser planififcado en procesos y a nivel de sistema operativo

### Pthread Scheduling

- PTHREAD SCOPE PROCESS schedules threads using PCS scheduling.
- PTHREAD SCOPE SYSTEM schedules threads using SCS scheduling.

![](../assets/s-hilos.png)

## Multiple-Processor Scheduling

- asimetrica: un procesador controla a todos los demas
- simetricaa: nignu procesador controla al resto y todos tienen acceso a las estructuras del sistema operativos. cada procesador puede tener su propia ready queue o comparti una en general

- soft affinity: decirle que solo en uno pero puede migrar
- hard affinity: un proceso se ejecute en un solo procesador
- procesosr sets: decirle que puede ejecutar en un conjunto de proceadores

se trata evitar que un proceso migre de cpu para que los datos que se guardaron en la cache ya no estan en la cache del proceador donde se va a ejecutar de nuevo. por que se tiene que copiar la ifnromacion de otro procesador, esto genera latencias

NUMA and CPU Scheduling (Non-Uniform Memory Access)

## Balanceo de carga
Distribuir la carga entre procesadores. 
- Push migration: meten un procesos en un procesador que tiene poca carga. UN subsistema que chequea si un procesaodr tiene mucha carga, y se le quita y se lo da a los otros. De muchos a menos
- Pull migration: sacar procesos de una cola de otro para que el otro lo haga. Un pocesador no hace nada y busca tareas que estan esperando en otro procesador y el se encarga de planificarlas (las toma?)

## Multithreaded Multicore System

Que en ves de no hacer nada de los memoery stall cyle (esperando a al repseusta de la memoria) planifica otro proceso. En un solo core se ejecutan dos thread. Es decir, mientras un rpcoes emepozo a esperar por metoria, el cpu lo detecta y planfica otro para que ejhecute cosas durante ese tiempo (memory stall)

Existen:

- coarse-grained: un hilo se ejecuta en un procesador hasta que ocurra a long-latency como resultado de alguna memory stall ocurrs.
- fine-grained: cambia entre hilos al que tenga un nivel mas fino de granularidad (neos trabajo, tareas peuqeña) inluyen a logic para en intercabio de hilos (supongo que quizo decir alguna parte logica de cpu especialdiad para ello)

Actua lemnte se requede de dos difertnes niveles de agnedamiento. En un nivel estan las deciciones de agendamiento que deben ser hecas por el SO como escoger cual software thread corre en ca  da hardware thread (logical processor) PAra este nivel de agnedamiento, el SO pueda escoger cualquier algoritmo de agendamiento. EL segundo nivel de agedamiento espeficica como cada core decide cual hardware thread correr

![](../assets/memory-stall.png)

## Real-Time CPU Scheduling

La idea es que una tarea se ejecute dentro de un intervalode tiempo (corot genralmente) si no se da la tarea pro falldia. Esto se ve en donde el numero de procesos pequeños, como los autos, aviones, sistema guiados de misiles, naves espaciles, equipamiento medico. 

- soft real-time: lo mas rapido posible
- hard real-time: la tarea debe ser ejecutada en el deadline

Hay tiempo de interrupcion y latencia de despacho

![](../assets/latencias-despacho.png)

![](../assets/dispach2.png)

Si trabajamos en real-time deber ser procesos apropiativos, al menos tenemos difernetes clases de proceos, tareas con real-time y otras que no. La mayoria de las tareas son periodicas en los sistemas real-time

tiempo de tarea
deadline
Periodo

t < d < P

![](../assets/ex-p.png)

## Earliest Deadline First Scheduling (EDF)

Se planifica primera el que tiene el dealine mas cercano o temprano. 

the earline the dealdine, mas alta prioridad
later deadline, mas baja prioridad


![](../assets/sche-thread.png)

set_priority para pthread


