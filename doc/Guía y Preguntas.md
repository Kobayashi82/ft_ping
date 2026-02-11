# Guía y Preguntas

## Resumen de Implementación

### Arquitectura General

El proyecto está dividido en módulos:

- **main.c**: Bucle principal, control de timeouts, señales
- **icmp.c**: Construcción de paquetes ICMP, checksum, patterns
- **send.c**: Envío de paquetes con sendto()
- **receive.c**: Recepción y procesamiento de respuestas
- **socket.c**: Creación y configuración del socket RAW
- **options.c**: Parsing de argumentos con getopt_long()
- **output.c**: Estadísticas y salida formateada

---

## Preguntas Técnicas - ICMP y Redes

### 1. ¿Qué es ICMP y para qué sirve?
Internet Control Message Protocol. Protocolo de capa 3 (red) usado para diagnóstico y control. No transporta datos de aplicación, sino mensajes de error y control (ping, traceroute, errores de red).

### 2. ¿Qué diferencia hay entre ICMP Echo Request y Echo Reply?
Echo Request (tipo 8) lo envías tú. Echo Reply (tipo 0) es la respuesta del servidor. Ambos tienen el mismo formato: type, code, checksum, id, sequence.

### 3. ¿Qué otros tipos de ICMP hay aparte de Echo?
- Tipo 3: Destination Unreachable (host/puerto inalcanzable)
- Tipo 5: Redirect (cambio de ruta)
- Tipo 11: Time Exceeded (TTL=0, usado por traceroute)
- Tipo 0: Echo Reply
- Tipo 8: Echo Request

### 4. ¿Por qué el checksum ICMP usa complemento a 1 y no complemento a 2?
Complemento a 1 tiene propiedades útiles: es simétrico (da igual el orden de suma) y detecta errores de un solo bit de forma eficiente. Al recalcular el checksum incluyendo el campo checksum recibido, si el resultado es 0xFFFF significa que está correcto.

### 5. ¿Qué pasa si cambias 1 byte del payload? ¿Se detecta?
Sí, el checksum cambia y la validación falla. ICMP checksum cubre todo el paquete ICMP (header + data).

### 6. ¿Por qué necesitas un socket RAW?
Porque ICMP es un protocolo de capa 3 (red), no de transporte como TCP/UDP. Los sockets normales (SOCK_STREAM, SOCK_DGRAM) son para capa 4. RAW te da acceso directo a la capa de red.

### 7. ¿Qué información viene en la cabecera IP que recibes?
Versión, header length (IHL), TOS, longitud total, ID, flags, fragment offset, TTL, protocolo, checksum, IP origen, IP destino.

### 8. ¿Qué es el TTL y cómo funciona?
Time To Live. Contador que se decrementa en cada salto (router). Si llega a 0, el router descarta el paquete y envía ICMP Time Exceeded. Evita que paquetes circulen infinitamente en loops.

### 9. ¿Cómo usa traceroute el TTL?
Envía paquetes con TTL=1, 2, 3... incrementando. Cada router que descarta un paquete (TTL=0) responde con ICMP Time Exceeded, revelando su IP. Así mapeas la ruta.

### 10. ¿Qué es el campo TOS (Type of Service)?
Campo en la cabecera IP para marcar prioridad/calidad de servicio. En la práctica, muchos routers lo ignoran, pero puede usarse para QoS (Quality of Service).

---

## Preguntas sobre tu Implementación

### 11. ¿Por qué usas htons() y ntohs()?
Para convertir entre orden de bytes de host (puede ser little-endian o big-endian) y orden de red (siempre big-endian). htons = host to network short, ntohs = network to host short.

### 12. ¿Qué pasa si no haces htons() en el sequence number?
En arquitecturas little-endian (x86), los bytes estarían invertidos. Ejemplo: secuencia 1 (0x0001) se enviaría como 0x0100 (256).

### 13. ¿Por qué guardas el PID en el campo id?
Para distinguir tus paquetes de otros procesos ping corriendo simultáneamente. Si recibes un Echo Reply, validas que el id coincida con tu PID antes de procesarlo.

### 14. ¿Qué pasa si dos pings corren al mismo tiempo al mismo host?
Cada uno tiene diferente id (su PID), así que cada proceso solo procesa sus propias respuestas. Si no validaras el id, podrías procesar respuestas de otro ping.

### 15. ¿Por qué el sequence number va incrementando?
Para:
- Detectar paquetes perdidos (si recibes seq 1, 2, 4 → falta el 3)
- Mostrar al usuario qué paquete llegó
- Ordenar respuestas que lleguen fuera de orden (aunque con ping es raro)

### 16. ¿Cómo detectas paquetes duplicados?
El código actual **no lo detecta explícitamente**. Podrías guardar un array de sequences recibidos y marcar duplicados si recibes el mismo sequence dos veces.

### 17. ¿Por qué usas gettimeofday() y no clock_gettime()?
Ambos funcionan. gettimeofday() es más portable (POSIX.1-2001). clock_gettime() tiene más precisión (nanosegundos vs microsegundos) pero para ping, microsegundos son suficientes.

### 18. ¿Qué precisión tiene tu medida de RTT?
Microsegundos (gettimeofday usa struct timeval con tv_usec). Lo muestras en milisegundos con 3 decimales (0.001 ms de precisión mostrada).

### 19. ¿El RTT incluye el tiempo de procesamiento del servidor?
Sí, incluye todo: tiempo de red (ida), procesamiento del servidor (mínimo, solo hace echo), tiempo de red (vuelta).

### 20. ¿Cómo calculas la desviación estándar?
Fórmula: stddev = sqrt(E[X²] - (E[X])²) donde:
- E[X] = media de RTT = sum_rtt / rtt_count
- E[X²] = media de cuadrados = sum_rtt_sq / rtt_count
- Variance = E[X²] - (E[X])²
- Stddev = sqrt(variance)

### 21. ¿Por qué guardas sum_rtt_sq en lugar de recalcular al final?
Eficiencia. Si calcularas al final necesitarías un array con todos los RTT. Guardando la suma de cuadrados, puedes calcular stddev en O(1) con solo 3 valores (sum, sum_sq, count).

### 22. ¿Qué pasa si la varianza sale negativa?
Por errores de redondeo de punto flotante puede pasar. Por eso haces `if (variance < 0) variance = 0.0;` antes del sqrt().

### 23. ¿Por qué usas select() con timeout de 1 segundo?
Para no quedarte bloqueado indefinidamente. Si no hay respuesta en 1 segundo, select() devuelve 0 y puedes:
- Enviar el siguiente paquete (si toca)
- Verificar timeouts
- Seguir el bucle

### 24. ¿Qué hace FD_ZERO, FD_SET y FD_ISSET?
Macros para manipular el fd_set de select():
- FD_ZERO(&readfds): Limpia el set
- FD_SET(sockfd, &readfds): Añade el socket al set
- FD_ISSET(sockfd, &readfds): Verifica si hay datos en el socket

### 25. ¿Por qué compruebas errno == EINTR tras select()?
Porque select() puede ser interrumpido por señales (ej: Ctrl+C). Si errno == EINTR, significa que fue interrumpido pero no es un error real, solo continúas el bucle.

### 26. ¿Qué pasa si recibes un ICMP Destination Unreachable?
El código actual solo maneja ICMP_ECHOREPLY y ICMP_TIME_EXCEEDED. Un Destination Unreachable (tipo 3) sería ignorado. Podrías añadir un else if para mostrarlo.

### 27. ¿Por qué usas MSG_DONTWAIT en recvfrom()?
Aunque select() ya garantiza que hay datos, MSG_DONTWAIT asegura que recvfrom() no bloqueará. Es redundante pero defensivo.

### 28. ¿Qué significa ip->ihl << 2?
IHL (Internet Header Length) está en palabras de 32 bits (4 bytes). ihl << 2 = ihl * 4 = longitud en bytes de la cabecera IP. Necesario para saltar la cabecera IP y llegar al ICMP.

### 29. ¿Por qué validas received < expected_size?
Para evitar acceder a memoria fuera de rango. Si el paquete es más corto que IP header + ICMP header, algo está mal (paquete truncado/corrupto).

### 30. ¿Qué hace inet_ntop() vs inet_ntoa()?
Ambos convierten IP binaria a string. inet_ntop() es más moderna, thread-safe y soporta IPv6. inet_ntoa() es legacy, no thread-safe (usa buffer estático).

---

## Preguntas sobre Opciones y Funcionalidad

### 31. ¿Para qué sirve -c (count)?
Limita el número de paquetes a enviar. Útil en scripts para no correr indefinidamente.

### 32. ¿Para qué sirve -i (interval)?
Controla el tiempo entre envíos. Por defecto 1 segundo. Con -i 0.1 envías 10 paquetes/seg (útil para flood ping, aunque limitado a 100ms mínimo).

### 33. ¿Para qué sirve -w (timeout global)?
Tiempo máximo total de ejecución. Ejemplo: -w 10 para que ping pare tras 10 segundos, independientemente de cuántos paquetes haya enviado.

### 34. ¿Para qué sirve -W (linger/timeout por paquete)?
Tiempo máximo de espera por la respuesta de un paquete. Si no llega en ese tiempo, se marca como perdido y se envía el siguiente.

### 35. ¿Cuál es la diferencia entre -w y -W?
- -w: Tiempo total del programa
- -W: Tiempo de espera por cada paquete individual

### 36. ¿Para qué sirve -s (size)?
Tamaño del payload de datos. Por defecto 56 bytes (que con 8 bytes de header ICMP = 64 bytes totales).

### 37. ¿Para qué sirve -p (pattern)?
Rellena el payload con un patrón hexadecimal específico. Útil para:
- Detectar problemas de red que corrompen ciertos bytes
- Testing de equipos de red
- Diagnosticar problemas de fragmentación

### 38. ¿Para qué sirve --ttl?
Establece el TTL inicial. Útil para:
- Testear routers específicos (con TTL bajo)
- Simular paquetes que viajan cierta distancia
- Debugging de rutas

### 39. ¿Para qué sirve -T (TOS)?
Marca el tipo de servicio. En teoría para QoS (prioridad de paquetes), pero muchos routers modernos lo ignoran o usan DSCP en su lugar.

### 40. ¿Para qué sirve -n (numeric)?
No hace resolución DNS inversa. En lugar de mostrar google.com (172.217.x.x), solo muestra la IP. Más rápido y útil si DNS está lento.

### 41. ¿Para qué sirve -r (ignore-routing)?
Envía directamente al host sin usar la tabla de rutas. Solo funciona si el host está en la misma red local. Útil para diagnosticar problemas de routing.

### 42. ¿Para qué sirve -d (debug)?
Activa SO_DEBUG en el socket. Dependiente del kernel, puede loggear información de bajo nivel sobre el socket. En la práctica, poco útil en Linux moderno.

### 43. ¿Para qué sirve -v (verbose)?
Muestra información extra:
- Detalles de cabeceras IP/ICMP en errores
- Paquetes corruptos
- Información de id del proceso

### 44. ¿Para qué sirve -q (quiet)?
Solo muestra estadísticas finales, no cada paquete recibido. Útil en scripts donde solo interesa el resultado final.

### 45. ¿Qué pasa si usas -q y -v a la vez?
Conflicto lógico. En el código, ambas flags se setean pero -q tiene precedencia en packet_receive() (se comprueba !(options & OPT_QUIET) antes de imprimir).

---

## Preguntas sobre Errores y Edge Cases

### 46. ¿Qué pasa si el host no existe?
getaddrinfo() falla en validate_host() y sale con error "unknown host".

### 47. ¿Qué pasa si la red está caída?
sendto() podría fallar (errno ENETUNREACH) o los paquetes simplemente no reciben respuesta (100% packet loss).

### 48. ¿Qué pasa si el host bloquea ICMP?
No recibes respuestas. Tras timeout (-W o por defecto), marcas paquetes como lost. 100% packet loss pero el programa funciona normal.

### 49. ¿Qué pasa si recibes respuestas desordenadas?
No afecta. Cada respuesta tiene su sequence, y calculas RTT individual con el timestamp embebido en cada paquete.

### 50. ¿Qué pasa si un paquete tarda más que el interval en llegar?
Lo procesas cuando llegue. Podrías recibir respuestas mientras ya enviaste otros paquetes. Cada uno tiene su sequence y timestamp, así que no hay problema.

### 51. ¿Qué pasa si el tamaño del paquete excede MTU?
Se fragmenta a nivel IP. ICMP se encarga de reensamblar. Pero fragmentación puede causar pérdida de paquetes si algún fragmento se pierde.

### 52. ¿Qué pasa si pones -s 65000?
El código valida if (data_len > MAX_SIZE) y rechaza con error "data length too large". MAX_SIZE probablemente sea 65507 (65535 - 20 IP header - 8 ICMP header).

### 53. ¿Qué pasa si haces ping a localhost (127.0.0.1)?
Funciona normal. RTT muy bajo (< 0.1 ms típicamente) porque no sale de la máquina.

### 54. ¿Qué pasa si haces ping a una IP privada desde fuera de esa red?
Depende de routing. Si no hay ruta, sendto() falla con ENETUNREACH o simplemente no hay respuesta.

### 55. ¿Qué pasa si ejecutas sin permisos?
socket() falla con errno EPERM o EACCES, y muestras "Lacking privilege for icmp socket".

---

## Preguntas de Optimización y Mejoras

### 56. ¿Cómo podrías mejorar la precisión del RTT?
Usar clock_gettime() con CLOCK_MONOTONIC en lugar de gettimeofday(). Tiene resolución de nanosegundos y no se ve afectado por ajustes del reloj del sistema.

### 57. ¿Cómo añadirías soporte para IPv6?
- Cambiar AF_INET por AF_INET6
- Usar struct sockaddr_in6 en lugar de sockaddr_in
- Cambiar IPPROTO_ICMP por IPPROTO_ICMPV6
- Adaptar tipos de ICMP (ICMPv6 usa tipo 128/129 para echo)

### 58. ¿Cómo detectarías paquetes duplicados?
Mantener un bitmap o array de sequences recibidos. Si recibes el mismo sequence dos veces, marcarlo como duplicado y mostrarlo.

### 59. ¿Cómo harías flood ping (-f)?
Enviar paquetes tan rápido como sea posible (sin interval). Mostrar "." por paquete enviado y borrar al recibir respuesta. Requiere permisos de root y es agresivo para la red.

### 60. ¿Cómo añadirías timestamp visible en cada línea?
En packet_receive(), antes de printf(), obtener hora actual y mostrarla: [HH:MM:SS.mmm].

---

## Preguntas sobre Buenas Prácticas

### 61. ¿Por qué usas una variable global g_ping?
Porque los signal handlers solo pueden acceder a variables globales (no puedes pasar parámetros). g_ping.running debe ser global para que termination_handler() pueda ponerlo a false.

### 62. ¿Por qué g_ping.running debería ser volatile?
Porque la señal puede modificarlo asincrónicamente mientras el bucle principal lo lee. volatile evita que el compilador optimice el acceso y cachee el valor en un registro.

### 63. ¿Hay race conditions en tu código?
Potencialmente sí. termination_handler() escribe g_ping.running y main() lo lee. Deberías usar volatile sig_atomic_t para running.

### 64. ¿Por qué usas memset() antes de getaddrinfo()?
struct addrinfo hints puede tener padding/basura. Inicializar a 0 asegura que campos no seteados explícitamente están en estado conocido.

### 65. ¿Por qué liberas res con freeaddrinfo()?
getaddrinfo() reserva memoria dinámica. Si no la liberas, tienes memory leak.

### 66. ¿Cierras el socket al terminar?
Sí, close(g_ping.data.sockfd) en main() antes de terminar.

### 67. ¿Qué pasa si falla socket_create() o packet_create()?
Devuelves 1 (error) y terminas el programa sin ejecutar el bucle de ping.

### 68. ¿Validas que el hostname no sea NULL?
Sí, en parse_options() compruebas if (optind >= argc) para asegurar que hay al menos un argumento (el hostname).

### 69. ¿Qué pasa si pasas múltiples hostnames?
El código solo procesa el primero (argv[optind]). Ignoras los demás. El ping real puede hacer ping a varios hosts en secuencia.

### 70. ¿Validás que los argumentos numéricos no sean negativos?
Sí, ft_strtoul() usa strtoul() que trata valores negativos como muy grandes (unsigned), y luego validas con max_value.

---

## Algoritmo de Checksum ICMP

```c
unsigned short checksum(void *data, int len) {
    unsigned long sum = 0;
    unsigned short *buf = data;
    
    // Sum 16-bit words
    while (len > 1) { 
        sum += *buf++; 
        len -= 2; 
    }
    
    // Handle odd byte
    if (len == 1) 
        sum += *(unsigned char *)buf;
    
    // Fold 32-bit sum to 16 bits
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    
    // One's complement
    return (unsigned short)(~sum);
}
```

**Explicación:**
- Suma todas las palabras de 16 bits
- Si hay byte impar, lo trata como parte alta de una palabra
- Propaga el carry (bits > 16) sumándolos de vuelta
- Aplica complemento a 1 (~)

---

## Cálculo de RTT

Al **enviar**: guardas struct timeval en los primeros bytes del payload

Al **recibir**: extraes ese timestamp y restas con el timestamp actual

```c
rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0 
    + (recv_time.tv_usec - send_time.tv_usec) / 1000.0
```

Resultado en milisegundos con precisión de microsegundos.

---

## Tipos de Timeout

Tu implementación maneja **3 tipos**:

1. **Global (-w)**: Tiempo máximo total de ejecución
2. **Por paquete (-W)**: Tiempo máximo de espera de respuesta por paquete  
3. **Select timeout**: 1 segundo para no bloquear indefinidamente el select()

---

## Flujo del Programa

```
1. Parse argumentos (destino + opciones)
2. Resolver hostname → IP (getaddrinfo)
3. Crear socket RAW (IPPROTO_ICMP)
4. Bucle principal:
   - Verificar timeouts globales (-w)
   - Verificar timeout por paquete (-W)
   - Si toca enviar (según -i interval):
     * Construir paquete ICMP Echo Request
     * Guardar timestamp en payload
     * Calcular checksum
     * Enviar con sendto()
   - select() con timeout de 1s
   - Si hay datos disponibles:
     * recvfrom() para leer respuesta
     * Validar tipo ICMP, id, checksum
     * Extraer timestamp, calcular RTT
     * Actualizar estadísticas
     * Mostrar resultado
5. Al terminar (Ctrl+C, -c, o -w):
   - Mostrar estadísticas finales
   - Min/Avg/Max/Stddev RTT
   - % paquetes perdidos
```

---

## Puntos Destacables

- Implementación completa y funcional con todas las opciones importantes
- Código bien estructurado en módulos lógicos
- Manejo correcto de errores y timeouts
- Estadísticas con stddev (no todos los pings lo calculan)
- Soporte para verbose mode con detalles de cabeceras IP/ICMP
- Resolución inversa de DNS en respuestas (si no -n)
- Uso de select() para I/O no bloqueante
- Gestión de señales para terminación limpia

---

## Comandos de Prueba Útiles

```bash
# Ping básico
sudo ./ft_ping google.com

# Con count
sudo ./ft_ping -c 5 8.8.8.8

# Con interval corto
sudo ./ft_ping -i 0.2 google.com

# Con timeout global
sudo ./ft_ping -w 10 google.com

# Con tamaño custom
sudo ./ft_ping -s 1000 google.com

# Con patrón hex
sudo ./ft_ping -p ff00ff google.com

# Con TTL bajo (para ver Time Exceeded)
sudo ./ft_ping --ttl 1 google.com

# Verbose
sudo ./ft_ping -v google.com

# Quiet (solo stats)
sudo ./ft_ping -q -c 10 google.com

# Sin resolver DNS
sudo ./ft_ping -n google.com
```

---

**¡Buena suerte en la evaluación!**
