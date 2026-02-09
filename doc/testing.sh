#!/bin/bash

# Colores
GREEN='\033[32m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
NC='\033[0m'
BOLD='\033[1m'

# Configuración
FT_PING="./ft_ping"
INETUTILS_PING="./doc/inetutils/ping"
GOOD_IP="8.8.8.8"
BAD_IP="192.0.2.1"
GOOD_HOST="google.com"
BAD_HOST="invalid.hostname"

# Capturar Ctrl+C
trap 'echo -e "\n${YELLOW}⚠️  Script interrumpido${NC}\n"; exit 130' SIGINT SIGTERM
clear

run_comparison() {
	local title="$1"
	local description="$2"
	local cmd1="$3"
	local cmd2="$4"
	
	echo -e "${BOLD}${YELLOW}$title${NC}"
	echo -e "${CYAN}$description${NC}\n"
	echo -e "${CYAN}Comando:${NC} ${BOLD}$cmd1${NC}\n" && $cmd1
	echo -e "\n${CYAN}Comando:${NC} ${BOLD}$cmd2${NC}\n" && $cmd2
	
	while read -r -t 0; do read -r -n 1; done
	read -n 1 -s -r -p $'\n\033[32mPulsa cualquier tecla para continuar...\033[0m\n'
	clear
}

# Test 0: Versión
run_comparison \
	"VERIFICACIÓN DE VERSIÓN" \
	"Muestra información sobre la versión del programa." \
	"$FT_PING --version" \
	"$INETUTILS_PING --version"

# Test 1: IP válida
run_comparison \
	"TEST #1: Dirección IP válida" \
	"Prueba la funcionalidad básica de ICMP ECHO con el DNS de Google (8.8.8.8). Debe enviar y recibir paquetes correctamente." \
	"$FT_PING -c 3 $GOOD_IP" \
	"$INETUTILS_PING -c 3 $GOOD_IP"

# Test 2: IP inválida
run_comparison \
	"TEST #2: Dirección IP inválida" \
	"Prueba con la IP 192.0.2.1 que no responde. Debe mostrar 100% de pérdida de paquetes." \
	"$FT_PING -c 3 $BAD_IP" \
	"$INETUTILS_PING -c 3 $BAD_IP"

# Test 3: Verbose IP inválida
run_comparison \
	"TEST #3: Modo verbose con IP inválida" \
	"El modo verbose debe mostrar información adicional como el ID del proceso (id 0xXXXX = XXXXX)." \
	"$FT_PING -v -c 3 $BAD_IP" \
	"$INETUTILS_PING -v -c 3 $BAD_IP"

# Test 4: Hostname válido
run_comparison \
	"TEST #4: Hostname válido" \
	"Prueba la resolución DNS con google.com. Debe resolver el hostname a IP y hacer ping correctamente." \
	"$FT_PING -c 3 $GOOD_HOST" \
	"$INETUTILS_PING -c 3 $GOOD_HOST"

# Test 5: Hostname inválido
run_comparison \
	"TEST #5: Hostname inválido" \
	"Prueba el fallo de resolución DNS. Debe mostrar el error 'unknown host'." \
	"$FT_PING -c 3 $BAD_HOST" \
	"$INETUTILS_PING -c 3 $BAD_HOST"

# Test 6: Verbose IP inválido con TTL
run_comparison \
	"TEST #6: Modo verbose con dirección IP inválido y TTL modificado" \
	"Con TTL=1, los paquetes expiran en el primer router. El verbose debe mostrar 'Time to live exceeded' con volcado del header IP y detalles ICMP." \
	"$FT_PING -v --ttl=1 -c 3 $BAD_IP" \
	"$INETUTILS_PING -v --ttl=1 -c 3 $BAD_IP"

# Bonus 1: interval
run_comparison \
	"BONUS #1: -i, --interval=NUM" \
	"Controla el tiempo de espera entre el envío de cada paquete. Aquí establecemos 2 segundos de intervalo." \
	"$FT_PING -i 2 -c 3 $GOOD_IP" \
	"$INETUTILS_PING -i 2 -c 3 $GOOD_IP"

# Bonus 2: quiet
run_comparison \
	"BONUS #2: -q, --quiet" \
	"El modo silencioso suprime la salida de cada paquete, mostrando solo el encabezado y las estadísticas finales." \
	"$FT_PING -q -c 3 $GOOD_IP" \
	"$INETUTILS_PING -q -c 3 $GOOD_IP"

# Bonus 3: size
run_comparison \
	"BONUS #3: -s, --size=NUM" \
	"Establece el tamaño personalizado de datos en bytes. Por defecto es 56, aquí usamos 100 bytes." \
	"$FT_PING -c 3 -s 100 $GOOD_IP" \
	"$INETUTILS_PING -c 3 -s 100 $GOOD_IP"

# Bonus 4: pattern
run_comparison \
	"BONUS #4: -p, --pattern=PATTERN" \
	"Rellena los datos del paquete ICMP con el patrón hexadecimal especificado. Útil para probar problemas de red." \
	"$FT_PING -c 3 -p ff $GOOD_IP" \
	"$INETUTILS_PING -c 3 -p ff $GOOD_IP"

# Bonus 5: timeout
run_comparison \
	"BONUS #5: -w, --timeout=NUM" \
	"Establece el tiempo total de ejecución en segundos. El programa se detiene después del tiempo especificado." \
	"$FT_PING -w 3 $GOOD_IP" \
	"$INETUTILS_PING -w 3 $GOOD_IP"

# Bonus 6: linger
run_comparison \
	"BONUS #6: -W, --linger=NUM" \
	"Establece el tiempo de espera para la respuesta de cada paquete. Útil para detectar pérdida de paquetes más rápido." \
	"$FT_PING -W 1 -c 3 $BAD_IP" \
	"$INETUTILS_PING -W 1 -c 3 $BAD_IP"

# Bonus 7: numeric
run_comparison \
	"BONUS #7: -n, --numeric" \
	"Desactiva la búsqueda DNS inversa. Muestra solo direcciones IP, no nombres de host." \
	"$FT_PING -n -c 3 $GOOD_HOST" \
	"$INETUTILS_PING -n -c 3 $GOOD_HOST"

# Bonus 8: ttl
run_comparison \
	"BONUS #8: --ttl=N" \
	"Establece el valor Time-To-Live para los paquetes. Con TTL=1, el paquete expira en el primer router." \
	"$FT_PING --ttl=1 -c 3 $GOOD_IP" \
	"$INETUTILS_PING --ttl=1 -c 3 $GOOD_IP"

# Bonus 9: tos
run_comparison \
	"BONUS #9: -T, --tos=NUM" \
	"Establece el campo Type of Service en el header IP. Valores: 16=Baja Latencia, 4=Alta Fiabilidad, 8=Alto Rendimiento." \
	"$FT_PING -T 16 -c 3 $GOOD_IP" \
	"$INETUTILS_PING -T 16 -c 3 $GOOD_IP"

# Bonus 10: debug
run_comparison \
	"BONUS #10: -d, --debug" \
	"Establece la opción SO_DEBUG del socket (dependiente del kernel). Puede no mostrar diferencia visible en la salida." \
	"$FT_PING -d -c 3 $GOOD_IP" \
	"$INETUTILS_PING -d -c 3 $GOOD_IP"

# Bonus 11: ignore-routing
run_comparison \
	"BONUS #11: -r, --ignore-routing" \
	"Envía directamente al host en la red conectada, evitando las tablas de enrutamiento." \
	"$FT_PING -r -c 3 $GOOD_IP" \
	"$INETUTILS_PING -r -c 3 $GOOD_IP"
