## CPU Scheduling
Maximizar el uso del CPU obtenido con multiprogrmacion

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

![](../assets/optmi.png)
- cpu utilization ++
- throughput: numero de proceos que se ejecutan por segundo ++
- turnaround time: cantiadad de tiempo de un proceso en particular --
- waiting --
- response tiem: interfaz grafica tiempo de respuesta de nuestro programa. Scheduler cuando arga un proceso se pueda interactuar con el --