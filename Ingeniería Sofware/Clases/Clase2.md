## Modelo de procesos

### Waterfall Model

Este modelo es usado cuando se tiene muy clara el proyecto y todos sus requerimientos. Tambien se estima que la cantdad de deployments o entregas sean las mínimas.

Flujo de trabajo:
- Comunicación: inicio de requerimientos
- Planeación: estimación y agendamiento de trabajos
- Modelado: análisis y diseño
- Construción: codificación y testeo
- Deployment: entrega, soporte y retroalimentación

Problemas del modelo:
- Rara vez el flujo es secuancial.
- Los requerimientos no siempre estan fijos, y son cambiantes

### Incremental Model

Este modelo combina elementos de procesos lineales y paralelos, este modelo aplica en cada linea de secuancia se produce una entrega "incremental" del software, aumentando nuevas características al mismo.

En el modelo incremental, se aconseja que la primera entrega sea el core dle proyecto, con el que se pueda trabajar las siguiente entregas isn problemas, normalmente las sigueinte entregas se basan de lo ya implementado, sobre todo en el primero que es el CORE.

Flujo del modelo:
- Incremento #1: Comunitacion -> Planeacion -> Modelado -> Contruccion -> Deployment
- Incremento #2: Comunitacion -> Planeacion -> Modelado -> Contruccion -> Deployment
- Incremento #N: Comunitacion -> Planeacion -> Modelado -> Contruccion -> Deployment

### Evolutionary Model

Cuando el core del proyecto esta bien conocido pero los detalles del prodcuto o extension del sistema no, es cuando de podria usar este modelo. Este modelo acompaña al proyecto a crecer y cambiar con el tiempo.

Este modelo es iterativo, permite entregar mayor cantidad de versiones completas del software.

Ejemplos de evolutionary:
- Prototyping: permeite modelar el software e integrar mas requerimentos conforme van surgiendo en reunioes.

Fluijo de modelo:
- Comunicacion
- Planeo rapido -> Rapido modelado de diseño
- Contruccion del prototipo
- Entrega y retroalimentación

Problemas:
- No se piensa en la mantenibilidad
- No se piensa en la eficiencia o buenas practicas

## Spiral Model

Provee entregas rapida de versiones. Este permite un manejo de riegos en cada ciclo.


Flujo del modelo (ciclico):
- Planeación: agendación y analsiis de riegos
- Modelado: analisis y diseño
- Contrucción: code test
- Deployt: entrega y feedback


## Agile Development

Es mas que solo efectriva respuesta a los cambios. Es inlcuir al cliente en el desarollo, se elimina lo no esecnial para enfocarse en la entrega rapida y eficiente del software, enfocarse en una estrategia de entrega rápida.

### Costo del cambio
Con el uso de modelo agiles, el costo a los cambios se ve reducido pero no eliminado.

### Qué es un proveso agil?

Un proceso agile maneja 3 supociciones:

1. Es dificil predecir que se cambia y que persistira
2. Algunos tipo de sfotwarem diseño y contruccion son intercaldas, es dificil prodecir que tanto del diseño es necesario antes de que se utilice la contruccin para probar el diseño
3. Analisis, diseño, contruccion y testeo no son predecibles. 

### Principio del diseño agil

1. Our highest priority is to satisfy the customer through early and continuous
delivery of valuable software.
2. Welcome changing requirements, even late in development. Agile processes
harness change for the customer's competitive advantage.
3. Deliver working software frequently, from a couple of weeks to a couple of
months, with a preference to the shorter timescale.
4. Business people and developers must work together daily throughout the
project.
5. Build projects around motivated individuals. Give them the environment
and support they need, and trust them to get the job done.
6. The most effi cient and effective method of conveying information to and
within a development team is face-to-face conversation.
7. Working software is the primary measure of progress.
8. Agile processes promote sustainable development. The sponsors, developers,
and users should be able to maintain a constant pace indefi nitely.
9. Continuous attention to technical excellence and good design enhances
agility.
10. Simplicity—the art of maximizing the amount of work not done—is essential.
11. The best architectures, requirements, and designs emerge from selforganizing
teams.
12. At regular intervals, the team refl ects on how to become more effective,
then tunes and adjusts its behavior accordingly.