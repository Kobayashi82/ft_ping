🎯 Explicación detallada de cada valor TOS:
TOS básicos (controlan el tipo de servicio):

0x02 - Minimal cost: Pide a los routers que elijan la ruta más barata (menor costo económico/ancho de banda)
0x04 - Reliability: Pide rutas más confiables (menos pérdida de paquetes, enlaces más estables)
0x08 - Throughput: Pide rutas de alto ancho de banda (para transferir grandes volúmenes de datos)
0x10 - Low delay: Pide rutas más rápidas (menor latencia, para aplicaciones en tiempo real)

Precedence (controlan la prioridad):

0x20 - Priority: Prioridad alta - los routers deberían procesar estos paquetes antes que los normales
0x40 - Immediate: Urgente - para tráfico que necesita atención inmediata
0x60 - Flash: Muy urgente - originalmente para comunicaciones militares urgentes
0x80 - Flash Override: Crítico - anula otros tráfico flash
0xa0 - Critical: Crítico para la red - servicios esenciales (DNS, DHCP)
0xc0 - Internet Control: Control de infraestructura - protocolos de routing (BGP, OSPF)
0xe0 - Net Control: Control de red máximo - reservado para el funcionamiento básico de la red


📋 Lista COMPLETA de valores estándar:
TOS básicos:

0x00/0 = Normal
0x02/2 = Minimal cost
0x04/4 = Reliability
0x08/8 = Throughput
0x10/16 = Low delay

Precedence:

0x20/32 = Priority
0x40/64 = Immediate
0x60/96 = Flash
0x80/128 = Flash Override
0xa0/160 = Critical
0xc0/192 = Internet Control
0xe0/224 = Net Control ← Este sí lo había mencionado antes

Rango total: 0-255 (0x00-0xFF)