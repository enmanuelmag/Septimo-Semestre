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