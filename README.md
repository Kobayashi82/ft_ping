<div align="center">

![System & Kernel](https://img.shields.io/badge/System-brown?style=for-the-badge)
![Network Communication](https://img.shields.io/badge/Network-Communication-blue?style=for-the-badge)
![ICMP Protocol](https://img.shields.io/badge/Protocol-ICMP-green?style=for-the-badge)
![C Language](https://img.shields.io/badge/Language-C-red?style=for-the-badge)

*Una reimplementación del comando ping clásico*

</div>

# ft_ping

## 🎯 Descripción

**ft_ping** es una implementación desde cero del icónico comando `ping`, una herramienta fundamental en administración de redes y diagnóstico de conectividad. Este proyecto profundiza en los protocolos de red de bajo nivel, específicamente ICMP (Internet Control Message Protocol).

### ¿Qué es Ping?

Ping es una utilidad de red que:

- **Verifica conectividad** entre dispositivos en una red IP
- **Mide latencia** (Round-Trip Time - RTT) de los paquetes
- **Diagnóstica problemas** de red y pérdida de paquetes
- **Utiliza ICMP Echo Request/Reply** para su funcionamiento

### Funcionamiento Técnico

```
[Cliente] ----ICMP Echo Request----> [Servidor]
    ^                                     |
    |                                     v
[Cliente] <----ICMP Echo Reply------ [Servidor]
         
    RTT = Tiempo_Respuesta - Tiempo_Envío
```

El proceso básico:

1. **Construcción del paquete**: Crea un paquete ICMP Echo Request
2. **Envío**: Transmite el paquete al destino especificado
3. **Espera**: Aguarda la respuesta ICMP Echo Reply
4. **Medición**: Calcula el tiempo de ida y vuelta (RTT)
5. **Repetición**: Envía paquetes periódicamente hasta interrumpirse

## 🔧 Compilación

```bash
git clone git@github.com:Kobayashi82/ft_ping.git
cd ft_ping
make
```

## 🖥️ Uso

### Permisos

```bash
# ft_ping requiere privilegios root para crear raw sockets
sudo ./ft_ping destino.com

# Alternativa: configurar capabilities
sudo setcap cap_net_raw+ep ./ft_ping
./ft_ping destino.com
```

### Ejecución

```bash
sudo ./ft_ping [opciones] <destino>
```

| Opción | Tipo | Descripción | Ejemplo |
|-----------|------|-------------|---------|
| `destino` | IPv4/Hostname | Dirección IP o nombre de host | `8.8.8.8`, `google.com` |

#### Básicas
| Opción | Forma Larga | Descripción |
|--------|-------------|-------------|
| `-h`, `-?` | `--help` | Muestra información de ayuda |
| `-V` | `--version` | Versión del programa |
| | `--usage` | Mensaje corto de uso |

#### Control de Envío
| Opción | Forma Larga | Parámetro | Descripción |
|--------|-------------|-----------|-------------|
| `-c` | `--count=NUM` | Número | Detiene después de enviar NUM paquetes |
| `-i` | `--interval=NUM` | Segundos | Espera NUM segundos entre paquetes |
| `-w` | `--timeout=NUM` | Segundos | Detiene después de NUM segundos |
| `-W` | `--linger=NUM` | Segundos | Tiempo de espera para respuestas |

#### Configuración de Paquetes
| Opción | Forma Larga | Parámetro | Descripción |
|--------|-------------|-----------|-------------|
| `-s` | `--size=NUM` | Bytes | Envía NUM bytes de datos |
| `-p` | `--pattern=PATTERN` | Hex | Rellena paquetes ICMP con patrón hexadecimal |
| | `--ttl=NUM` | Número | Especifica NUM como Time-To-Live |
| `-T` | `--tos=NUM` | Número | Establece Type of Service (TOS) |

#### Opciones de Red
| Opción | Forma Larga | Descripción |
|--------|-------------|-------------|
| `-n` | `--numeric` | No resuelve direcciones de host en las respuestas |
| `-r` | `--ignore-routing` | Envía directamente a host en red adjunta |
| `-d` | `--debug` | Activa opción SO_DEBUG (dependiente del kernel) |

#### Salida y Formato
| Opción | Forma Larga | Descripción |
|--------|-------------|-------------|
| `-v` | `--verbose` | Salida detallada |
| `-q` | `--quiet` | Salida silenciosa |

### Valores TOS (Type of Service)

La opción `-T` permite configurar el campo TOS del header IP:

| Valor | Tipo | Descripción |
|-------|------|-------------|
| 16 | Low Delay | Baja latencia |
| 4 | High Reliability | Alta confiabilidad |
| 8 | High Throughput | Alto rendimiento |
| 136 | High Priority | Alta prioridad |
| 184 | Expedited Forwarding | Reenvío expedito |

## 📡 Protocolo ICMP

### Estructura del Paquete ICMP

```c
typedef struct icmp_header {
    uint8_t  type;          // Tipo de mensaje ICMP
    uint8_t  code;          // Código específico
    uint16_t checksum;      // Checksum del paquete
    uint16_t identifier;    // ID del proceso
    uint16_t sequence;      // Número de secuencia
    // Datos del payload...
};
```

### Tipos de ICMP Relevantes

| Tipo | Código | Descripción | Uso en ft_ping |
|------|--------|-------------|----------------|
| 8 | 0 | Echo Request | Paquete de salida |
| 0 | 0 | Echo Reply | Respuesta esperada |
| 3 | * | Destination Unreachable | Error de destino |
| 11 | 0 | Time Exceeded | TTL expirado |

### Cálculo de Checksum

El checksum ICMP es un mecanismo de detección de errores que garantiza la integridad del paquete durante su transmisión:

#### Proceso de Cálculo

1. **Suma de palabras de 16 bits**: Se toman todos los bytes del paquete ICMP y se agrupan en palabras de 16 bits (2 bytes). Cada palabra se suma a un acumulador de 32 bits.

2. **Manejo de bytes impares**: Si el paquete tiene un número impar de bytes, el último byte se trata como si fuera la parte alta de una palabra de 16 bits (se desplaza 8 bits a la izquierda).

3. **Propagación de carry**: Los bits de desbordamiento (carry) que excedan los 16 bits se suman de vuelta al resultado.

4. **Complemento a uno**: Finalmente, se aplica el complemento binario (NOT) al resultado de 16 bits.

#### Verificación de Integridad

- **Al enviar**: Se calcula el checksum con el campo checksum puesto a cero
- **Al recibir**: Se recalcula el checksum incluyendo el valor recibido
- **Validación**: Si el paquete está íntegro, el resultado debe ser 0xFFFF

Este método detecta eficientemente errores de un solo bit, intercambios de bytes y muchos errores de múltiples bits, siendo suficiente para las necesidades del protocolo ICMP.

## 🛡️ Consideraciones de Seguridad

### Uso Responsable

⚠️ **Advertencias importantes:**
- **Respetar políticas de red** organizacionales
- **Evitar saturar** conexiones lentas
- **Usar intervalos apropiados** en producción

### Detección de Ataques

Algunos firewalls pueden detectar:
- **Ping floods** (muchos paquetes rápidos)
- **Paquetes anómalos** (tamaños extraños)
- **Patrones sospechosos** (secuencias irregulares)

## 📄 Licencia

Este proyecto está licenciado bajo la WTFPL – [Do What the Fuck You Want to Public License](http://www.wtfpl.net/about/).

---

<div align="center">

**🌐 Desarrollado como parte del curriculum de 42 School 🌐**

*"Understanding networks, one ping at a time"*

</div>
