# Resumen: Implementación de ft_ping

## Arquitectura General

### Flujo Principal
1. **Setup inicial:**
   - Crear socket una vez (SOCK_RAW o SOCK_DGRAM)
   - Configurar opciones con `setsockopt()`
   - Lanzar hilo de recepción

2. **Loop de envío:**
   - Construir header ICMP (si SOCK_RAW)
   - Construir cuerpo del paquete
   - Añadir paquete a estructura de tracking
   - Enviar paquete con `sendto()`
   - Control de timing (flood vs normal)

3. **Hilo de recepción:**
   - `recvfrom()` continuo para recibir respuestas
   - Procesar respuestas y marcar paquetes como recibidos

## Tipos de Socket

### SOCK_RAW
- **Control total** sobre el header ICMP
- **Requiere privilegios** (root)
- **Header ICMP manual:**
  - Type: 8 (echo request)
  - Code: 0
  - Checksum: calculado
  - Identifier: normalmente PID
  - **Sequence Number: en el header**
- **Ventaja:** Control completo del protocolo

### SOCK_DGRAM
- **Más simple** - kernel maneja el header ICMP automáticamente
- **Sin privilegios** especiales
- **Limitación:** No puedes cambiar el sequence number del header
- **Solución:** Usar tu propio sequence number en el payload/data

## Gestión de Timing

### Envío Normal (1 paquete/segundo)
```c
// Opción 1: SIGALRM (asíncrono)
signal(SIGALRM, enviar_paquete);
alarm(1);

// Opción 2: sleep() (más simple)
for (int i = 0; i < count; i++) {
    enviar_paquete();
    sleep(1);
}
```

### Flood Mode (--flood)
- **Envío tan rápido como sea posible**
- **No usar `alarm()`** - mínimo 1 segundo
- **Estrategias:**
  - Hilos separados para envío/recepción
  - `usleep()` para microsegundos
  - `select()` con timeout muy corto

## Recepción y Timeouts

### Problema con `recvfrom()`
- **Bloqueante por defecto** - se para hasta recibir datos
- **No sabe qué paquete específico esperar**
- **Paquetes pueden llegar fuera de orden**

### Soluciones
1. **Hilo separado** para recepción continua
2. **Timeout en socket:**
   ```c
   struct timeval timeout;
   timeout.tv_sec = 1;
   setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
   ```
3. **Tracking de paquetes** con sequence numbers

### Manejo de Ctrl+C
```c
void signal_handler(int sig) {
    close(sockfd);  // Cancela recvfrom()
    exit(0);
}

signal(SIGINT, signal_handler);
```

## Tracking de Paquetes

### Estructura Básica
```c
struct ping_packet {
    int sequence;
    struct timespec sent_time;
    struct timespec recv_time;
    int received;
    int lost;
};
```

### Estrategias según Modo

#### Modo Normal (-c pequeño)
- **Array/lista completa** de paquetes
- **Tracking individual** de cada paquete
- **Estadísticas detalladas**

#### Modo Flood (-c grande o --flood)
- **Buffer circular** para evitar malloc excesivo
- **Solo estadísticas básicas** (contadores)
- **Memoria fija**

## Buffer Circular

### Implementación
```c
#define BUFFER_SIZE 1000

struct ping_buffer {
    struct ping_packet packets[BUFFER_SIZE];
    int head;          // Próxima posición para escribir
    int total_sent;    // Total de paquetes enviados
};

void add_packet(int sequence) {
    buffer.packets[buffer.head] = /* datos */;
    buffer.head++;
    if (buffer.head >= BUFFER_SIZE) {
        buffer.head = 0;  // Volver al principio
    }
    buffer.total_sent++;
}
```

### Ventajas
- **Memoria fija** - no crece indefinidamente
- **Sin malloc** - todo en stack
- **Eficiente** - acceso directo por índice
- **Automático** - datos antiguos se sobrescriben

## Consideraciones Importantes

### Límites de -c
- **Técnico:** ~2.1 mil millones (int 32 bits)
- **Práctico:** Usar límite razonable (ej: 1 millón)
- **Validación:** Verificar rango válido

### Gestión de Memoria
- **Evitar malloc por paquete** - muy ineficiente
- **Alternativas:**
  - Array estático
  - Un solo malloc grande
  - Buffer circular (mejor para flood)

### Detección de Pérdidas
- **Timeout global** después de enviar todos
- **Tracking individual** con timestamps
- **No esperar indefinidamente** por paquetes específicos

## Flujo de Datos

### Orden de Llegada
- **Paquetes pueden llegar fuera de orden**
- **Usar sequence number** para identificar
- **Hilo receptor** debe estar siempre listo

### Identificación de Respuestas
- **SOCK_RAW:** Sequence number del header ICMP
- **SOCK_DGRAM:** Sequence number personalizado en payload

## Beneficios de Hilos vs SIGALRM

### Hilos
- **Más flexible** para flood mode
- **Más fácil** de entender y debuggear
- **Mejor control** sobre timing

### SIGALRM
- **Asíncrono** - no bloquea hilo principal
- **Mejor para timing preciso** en modo normal
- **Más complejo** de manejar

## Recomendaciones Finales

1. **Para aprender:** Empezar con hilos y sleep()
2. **Para rendimiento:** Considerar SIGALRM para modo normal
3. **Para flood:** Siempre usar hilos separados
4. **Para memoria:** Buffer circular con counts grandes
5. **Para robustez:** Manejar timeouts y señales correctamente