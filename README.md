# WALLY-S_Proyecto-de-Aula_ED3
Este repositorio es netamente para el proyecto de aula del curso Electrónica Digital III. El cual tiene como nombre WALLY-S. El grupo está conformado por los estudiantes Mauricio Rafael Aguas Ramírez, Jharlin Castro Moreno y Jesús Alberto Córdoba Delgado. De la Universidad de Antioquia.

Nuestro proyecto consiste en el desarrollo de una carretilla autónoma diseñada para realizar entregas de domicilios en una zona delimitada. La carretilla estará equipada con un sistema de posicionamiento GPS que permitirá localizar su ubicación en tiempo real y dirigirse al destino indicado.

La conexión entre el usuario y la carretilla se realizará mediante un módulo Bluetooth o Wi-Fi, permitiendo un control directo a través de una aplicación móvil. Para garantizar un desplazamiento seguro, el dispositivo contará con sensores de ultrasonido que detectarán obstáculos y evitarán colisiones. Además, una cámara en tiempo real transmitirá video al usuario, brindando una visión completa del entorno durante el trayecto.

Este sistema está pensado para agilizar y optimizar pequeños envíos locales, ofreciendo una solución eficiente y autónoma para servicios de entrega en zonas de corta distancia.

Motivación:
Como principal motivación la película "WALL-E" un robot recolector de basura autónomo, en el cual vimos potencial para hacer una réplica de este pero adaptado a funciones de delivery. Nos inspiró su diseño compacto y eficiente, así como su personalidad carismática que genera empatía inmediata en las personas.

## 📌 Descripción
WALLY-S es una carretilla autónoma e inteligente diseñada para realizar entregas en distancias cortas. El sistema cuenta con dos modos de operación: modo "Follow Me", donde sigue a un usuario mediante datos GPS transmitidos por Bluetooth, y modo de envío a destino, donde se dirige de forma autónoma hacia unas coordenadas GPS previamente indicadas desde el teléfono móvil.

El cerebro del sistema es un Raspberry Pi Pico (RP2040), que gestiona la navegación, evita obstáculos y controla los motores. La comunicación con el usuario se realiza vía Bluetooth, mientras que los módulos de GPS y brújula digital (HMC5883L) permiten posicionamiento y orientación precisos. Para la seguridad del trayecto, un sensor ultrasónico detecta obstáculos en tiempo real.

Requisitos funcionales:
1.	Modo "Follow Me":
o	El sistema debe seguir a un usuario en tiempo real mediante coordenadas GPS transmitidas vía Bluetooth.
o	Debe ajustar su trayectoria dinámicamente para mantenerse a una distancia segura del usuario (ejemplo: 1-2 metros).
o	Debe detenerse automáticamente si la señal Bluetooth se pierde por más de 5 segundos o si el usuario está dentro de un radio de 0.5 metros.
2.	Modo de envío a destino:
o	El sistema debe navegar de forma autónoma hacia coordenadas GPS predefinidas ingresadas desde la aplicación móvil.
o	Debe calcular la ruta óptima en función de la distancia y la orientación proporcionada por el módulo GPS y la brújula digital (HMC5883L).
o	Debe notificar al usuario mediante la app cuando llegue al destino o si no puede alcanzarlo (ejemplo: obstáculo insalvable).
3.	Detección y evitación de obstáculos:
o	El sistema debe detectar obstáculos en un rango de 10 cm a 2 metros usando el sensor ultrasónico HC-SR04.
o	Debe reaccionar a obstáculos en menos de 500 ms, deteniéndose o desviándose según la situación.
o	Debe reanudar la navegación una vez el obstáculo haya sido removido o evitado.
4.	Comunicación Bluetooth:
o	El sistema debe establecer una conexión estable con la aplicación móvil mediante el módulo HC-05.
o	Debe recibir y procesar comandos de los modos "Follow Me" y "Envío a destino", así como coordenadas GPS.
o	Debe enviar notificaciones en tiempo real al usuario (ejemplo: llegada a destino, obstáculo detectado).
5.	Gestión de energía:
o	El sistema debe operar al menos 2 horas con una carga completa de la batería LiPo 3s.
o	Debe incluir un modo de bajo consumo cuando esté inactivo por más de 5 minutos, reduciendo el consumo energético en un 50%.
6.	Interfaz de usuario móvil:
o	La aplicación móvil debe permitir al usuario seleccionar entre los modos "Follow Me" y "Envío a destino".
o	Debe mostrar un mapa simplificado con la ubicación en tiempo real de WALLY-S y el destino (si aplica).
o	Debe notificar al usuario sobre el estado del sistema (ejemplo: "En movimiento", "Obstáculo detectado", "Destino alcanzado").
7.	Control de motores:
o	El sistema debe controlar los motores DC mediante el driver L298N para moverse en todas las direcciones (adelante, atrás, giros).
o	Debe ajustar la velocidad de los motores según la distancia al destino o al usuario, manteniendo una velocidad máxima segura (ejemplo: 0.5 m/s).
8.	Precisión de navegación:
o	El sistema debe mantener un margen de error máximo de 1 metro en la navegación hacia coordenadas GPS.
o	Debe recalibrar su posición cada 10 segundos usando los datos del módulo GPS y la brújula digital.
9.	Seguridad:
o	El sistema debe detenerse inmediatamente si se detecta una inclinación peligrosa (ejemplo: > 15 grados) o si la batería está por debajo del 10%.
o	Debe incluir un botón de parada de emergencia físico en la carretilla.
10.	Escalabilidad:
o	El diseño del software y hardware debe permitir la incorporación futura de sensores adicionales (ejemplo: sensor de temperatura) o actuadores (ejemplo: canasta automática) sin requerir cambios mayores en la arquitectura.
Criterios de aceptación:
•	Cada requisito debe ser verificable mediante pruebas específicas (ejemplo: cronómetro para tiempos de respuesta, simulador de obstáculos).
•	El sistema debe cumplir con al menos el 80% de los requisitos funcionales para considerarse exitoso en la fase de pruebas.
•	La interfaz móvil debe ser evaluada por usuarios reales para confirmar su usabilidad (ejemplo: encuesta de satisfacción).


Requisitos no funcionales:
1.	Consumo energético:
o	Operar al menos 2 horas con una sola carga de batería.
2.	Usabilidad:
o	Interfaz móvil intuitiva para seleccionar modos de operación y enviar coordenadas.
3.	Fiabilidad:
o	Funcionamiento correcto en al menos el 80% de las pruebas sin intervención manual.
4.	Tiempos de respuesta:
o	Reaccionar a obstáculos en menos de 500 ms.
5.	Escalabilidad:
o	Diseño que permita la incorporación de sensores adicionales o mejoras como canasta automática sin modificar demasiado el hardware o sofware.


Escenario de pruebas:
Caso A – Modo Envío Automático
1.	El usuario ingresa coordenadas destino desde la app.
2.	WALLY-S calcula la distancia y dirección.
3.	Navega hacia el destino evitando obstáculos.
4.	Al llegar, notifica al usuario.
Caso B – Modo Follow Me
1.	El usuario activa el modo en la app y comienza a caminar.
2.	WALLY-S sigue al usuario recibiendo coordenadas GPS en tiempo real.
3.	Detecta obstáculos y se adapta a la trayectoria.
4.	Se detiene si pierde señal o llega al usuario.


## 🛠️ Presupuesto Estimado
| Componente               | Especificación               | Precio (COP) |
|--------------------------|-----------------------------|-------------|
| Microcontrolador         | Raspberry Pi Pico (RP2040)  | \$45.000    |
| Módulo Bluetooth         | HC-05                       | \$25.000    |
| Módulo GPS               | NEO-6M                      | \$50.000    |
| Brújula digital          | HMC5883L                    | \$20.000    |
| Sensor ultrasónico       | HC-SR04                     | \$8.000     |
| Motores + Driver         | DC 12V + L298N              | \$45.000    |
| Batería                  | LiPo 3s                     | \$30.000    |
| **Total**                |                             | **\$293.000** |

---

## Inicio y Desarrollo del proyecto WALLY-S🤖

El proyecto se inició con una fase de planificación colaborativa que involucró a todo el equipo, enfocada en definir el diseño y seleccionar los materiales para la estructura física de la carretilla autónoma. Tras varias discusiones, se tomó la decisión estratégica de construir la carrocería utilizando madera, tornillos, puntillas y llantas de plástico reciclado. Esta elección no solo se alineó con principios de sostenibilidad, sino que también ofreció flexibilidad para adaptaciones durante la fase de prototipado.

La adquisición de los componentes electrónicos y mecánicos esenciales se concretó el 26 de junio. Al día siguiente, 27 de junio, el equipo se dirigió a la carpintería de la Universidad de Antioquia para obtener los materiales de madera y realizar los cortes precisos según el diseño preliminar.

La primera semana del proyecto, comprendida aproximadamente entre el 28 de junio y el 4 de julio, se dedicó intensivamente al desarrollo y ensamblaje de la estructura física de WALLY-S. Esta etapa fue un proceso iterativo de construcción, prueba y rediseño. Durante este periodo, surgieron diversas ideas que llevaron a cambios significativos en el diseño original, como la selección de motores más adecuados para la tracción y la adaptación del tipo y tamaño de las llantas para optimizar la movilidad. Estas modificaciones fueron cruciales para asegurar una funcionalidad óptima y una estructura robusta, superando los desafíos iniciales de ingeniería y diseño.

Una vez que se logró un prototipo funcional de la carrocería, se procedió con la fase de desarrollo de software. Esta etapa implicó la creación y optimización de los algoritmos de control para los motores, la integración del módulo GPS para el posicionamiento y la navegación, y el desarrollo de la lógica para los diferentes modos de operación. Se escribieron y depuraron los códigos necesarios para la comunicación Bluetooth, la detección de obstáculos mediante el sensor ultrasónico y la gestión general del sistema.

Finalmente, se dio paso a la fase de montaje e integración, donde todos los componentes electrónicos y mecánicos adquiridos se ensamblaron en la estructura de la carretilla. Esta etapa ha sido fundamental para realizar las primeras pruebas de funcionamiento, identificar posibles puntos de mejora y refinar las estrategias de control y navegación. Actualmente, el proyecto se encuentra en esta fase de ajuste fino y optimización, preparando a WALLY-S para cumplir con los requisitos funcionales y no funcionales definidos inicialmente.

## Viabilidad Económica de WALLY-S: Inversión y Proyección Detallada

* Materiales Físicos y de Estructura:
Puntillas (paquete de 20 unidades): $5.000 COP
Tornillos (paquete de 20 unidades): $6.000 COP
Pintura en aerosol negra (unidad): $25.000 COP
Hoja de lija (unidad): $3.000 COP
Pegamento (uso general): $10.000 COP
Llantas (3 unidades x $10.000 COP c/u): $30.000 COP
Madera: Donación de carpintería UdeA.

* Componentes Electrónicos y de Conectividad:
Cables Jumper Macho-Macho (paquete de 10 unidades): $8.000 COP
Cables Jumper Macho-Hembra (paquete de 10 unidades): $8.000 COP
Cables Jumper Hembra-Hembra (paquete de 10 unidades): $8.000 COP
Rollo de Estaño (unidad): $25.000 COP
Protoboard (tamaño medio): $12.000 COP
Raspberry Pi Pico (YD-RP2040): $30.000 COP
Puente H (Driver L298N): $12.000 COP
Módulo GPS (NEO-6M): $23.250 COP
Switches (2 unidades x $3.000 COP c/u): $6.000 COP
Módulo Bluetooth (HC-05): $17.000 COP
Magnetómetro (Brújula Digital HMC5883L): $21.990 COP
Baterías (6 unidades x 4V 2500 mAh, aprox. $15.000 COP c/u): $90.000 COP
Portapilas (2 unidades x $5.000 COP c/u): $10.000 COP
Cable USB para Raspberry Pi Pico: $10.000 COP
Cinta Adhesiva (rollo): $5.000 COP
Cinta Doble Faz (rollo): $8.000 COP
Sensor de Ultrasonido (HC-SR04): $14.000 COP
Motores DC (2 unidades x $45.000 COP c/u): $90.000 COP

TOTAL ESTIMADO PROTOTIPO: $477.240 COP


