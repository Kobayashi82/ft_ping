#!/bin/bash

PING_BIN="$HOME/.local/ping"
PING_LOG="ping_results.log"
FT_LOG="ft_ping_results.log"

echo "=== FT_PING EVALUATION ==="
> $PING_LOG
> $FT_LOG

test_case() {
	name=$1
	args=$2
	ttl=$3

	echo "=== $name ==="

	# --- PING ---
	echo -e "\n=== PING $name ===" | tee -a $PING_LOG
	echo "CMD: $PING_BIN $args -c 4 $ttl" | tee -a $PING_LOG
	$PING_BIN $args -c 4 $ttl 2>&1 | tee -a $PING_LOG

	# --- FT_PING ---
	echo -e "\n=== FT_PING $name ===" | tee -a $FT_LOG
	echo "CMD: ./ft_ping $args -c 4 $ttl" | tee -a $FT_LOG
	./ft_ping $args -c 4 $ttl 2>&1 | tee -a $FT_LOG

	echo ""
}

# Tests
test_case "GOOD IP" "8.8.8.8" ""
test_case "BAD IP" "192.0.2.1" ""
test_case "-v BAD IP" "-v 192.0.2.1" "--ttl 3"
test_case "GOOD HOSTNAME" "google.com" ""
test_case "BAD HOSTNAME" "notarealhostname.abc" ""
test_case "-v BAD HOSTNAME" "-v notarealhostname.abc" "--ttl 3"

echo "=== COMPARING RESULTS ==="
diff -u <(grep "bytes from" $PING_LOG) <(grep "bytes from" $FT_LOG) && echo "✅ OUTPUT SIMILAR" || echo "❌ DIFFERENCES FOUND"

echo "Logs guardados en:"
echo " - $PING_LOG"
echo " - $FT_LOG"
