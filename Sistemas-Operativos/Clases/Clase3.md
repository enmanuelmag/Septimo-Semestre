## Llamadas al sistema

Con pilas o bloques de memoria se puede pasar mas paramaetros que por registros

### MS-DOS
- Un proceso a la vez
- Solo un proceso se queda en el cpu hasta terminar

### FreeBSD
- Unix variante
- Multitarea
- El shell permitia fork: ejecutava exec para cargar el proceso, se espera que termine
- El proceso terminana = 0 (no error) > 0 (error)
- Los procesos se turnaban el uso del procesador

## Programas dle sistema

Proveen un entorno para el desarollo y ejecucion de programas.
Permiten:
- manipular archivos
- compilador

Lo que tiene toda PC:
- SO
- Progrmaas dle sistema
- Progra  mas de usuario

## Background servicies
- Permite monitoreo en general, los progrmas de usuarios se pueden convertir en servicios
- Notaralmente se inician al inicio del SO hasta que se apagen, se puede administrar en lagun lugar del SO
- Corren en USER CONTEXT
- Proveen de facilidades como monitoreo de disco, agengdamiento y logs

File man: explorar de archivos

## Estructuras del sistema operativo

### Monolitocos
Son los mas comunes, puesto que su implemnetacion y diseño son los menos complejos. 
Todos los componetes de gestion y programas del sistema estan escritos en un solo codgio
Puede decirse que el nucleo es el SO. 

Ventajas:
- la velocidad, todos los compnetnes comparten las mismas direcciones, la separacion funcional solo se hace entre procesos del sistema y los de aplicacion, demora para ejecutar llamadas al sistema es minima.
  
Desventaja:
- es que como todo el sistema todo se ejecuta ene l kernel, es probable que ocurran cosas como que el sistema se apague, se bloquee, o se cuelguen, si courre algun fallo del hardware o existe algun error esto se depura ene l codgio del sis.

## Por capas
El objetivo es seprar la accion del codgio de los procesos del sistema, del nucleo y de los procesos del usuario.
Capas:
0. Aignacion de procesador y multiprogramacion
1. administracion de memoria
2. comunicacion de operardor y proceso
3. adm e/s
4. programas de usuario
5. el operador

Anillos concentricios los interior tienen mas privilegios.

Ventajas:
- depuracion y verificacion de procesos por capaz, tras depurar la priemra capa se descarta esa si no falla

Desventaja:
- exceso consumo de recursos, hardware, ya que cada capa exige un gasto extra
- llamadas entre capas, exige el envio de parametros entre capas, equivalente a una llamada al sistema.


## Micronucleo
La idea fundamental es obtener un nucleo lo mas pequeño y rapido posible y tratar el resto de la funciones y componetnes como procesos de aplicacion

El resto se ejecutan como aplicacion de usuario, es decir, con nivel minimo de privilegios.
En alguna medida deben incluirse en el nucleo otras funciones como el manjeo de hardware y algunos drivers

Ventajas:
- la desetralizacion de los fallos, un fallo en una parte dle sis, no se protapaga al todo el sis
- la facilidad para crear y depurrar controladores de disppsitivos

Desventajas;
- la complejidad en la sincronizacion de todos los modulos que componen el micronucleo y su acceso a la memoria
- mayor complejidad en el codigo, menor rendimiento y limitacion en diversas funciones

## Sistemas cliente-servidor
Es garantizar la comunuicacion entre procesos, en cada sistema independiente solo estaran presentes el nucleo y los componentes minimos para la ejecuccion de uno o algunos procesos

Se manifiesta mayor peso la necesidad de establecer llamadas al sistema robustas, para comunicacion entre procesos

Se escoge el modelo cliente-servidor, para esta comunucacion, porque se establece que cada proceso independte o no actua como servidor del resto.

V: 
- centralizacion de control, los acceso, recursos y la integridad de los datos son controlados por el servidor de forma que un programa defectuoso o no autorizado no dañe el sistema
- Escalabidad: se puede aumentar la capacidad de clientes y servidores por separado
- Facil mantenimiento: al estar distribuidas las funciones y responsabilidades entre varios ordenadores independientes, es posible reemplazar, reparar, actualizar, o incluso trasladar un servidor, mientras que sus clientes no se veran afectados por ese cambio.

D:

- la vogestion de trafico
- el paradigma de c/s no tinee la robustez de una red p2p, cuando el servidor esta caigo no se resuelven las peticiones
- se neceistan hadware y software esepcidifc, sobre todo en el lado del servidor, para satisfacer el trabajo, aumento el costo