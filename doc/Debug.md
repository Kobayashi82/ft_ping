# DEBUG

SO_DEBUG: Habilita la depuración del socket, registrando información sobre sus operaciones (como envío/recepción de paquetes), pero su utilidad depende del sistema y del tipo de socket (puede ser limitada para sockets raw en ft_ping).
Dónde se guarda: En los logs del kernel (e.g., dmesg, /var/log/kern.log) o en herramientas de depuración como tcpdump o wireshark, si el sistema está configurado para ello. No se devuelve al programa directamente.
Contexto en ft_ping: Es una opción de bonificación para depuración, pero su efecto práctico puede ser mínimo sin configuraciones adicionales del sistema.
Si necesitas usar SO_DEBUG en ft_ping, probablemente debas combinarlo con herramientas externas para ver los resultados, y su implementación debe cumplir con las reglas del proyecto (usar libc y manejar errores).