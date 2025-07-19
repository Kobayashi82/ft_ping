# TODO

## Tipos de Paquetes ICMP
| Opción | Descripción |
|--------|-------------|
| ☐ --address | send ICMP_ADDRESS packets (root only) |
| ■ --echo | send ICMP_ECHO packets (default) |
| ☐ --mask | same as --address |
| ☐ --timestamp | send ICMP_TIMESTAMP packets |
| ■ -t, --type=TYPE | send TYPE packets |

## Control de Transmisión
| Opción | Descripción |
|--------|-------------|
| ■ -c, --count=NUMBER | stop after sending NUMBER packets |
| ■ -d, --debug | set the SO_DEBUG option |
| ■ -i, --interval=NUMBER | wait NUMBER seconds between sending each packet |
| ■ -n, --numeric | do not resolve host addresses |
| ■ -r, --ignore-routing | send directly to a host on an attached network |
| ■ --ttl=N | specify N as time-to-live |
| ☐ -T, --tos=NUM | set type of service (TOS) to NUM |
| ☐ -v, --verbose | verbose output |
| ☐ -w, --timeout=N | stop after N seconds |
| ☐ -W, --linger=N | number of seconds to wait for response |

## Opciones Avanzadas
| Opción | Descripción |
|--------|-------------|
| ☐ -f, --flood | flood ping (root only) |
| ☐ --ip-timestamp=FLAG | IP timestamp of type FLAG, which is one of "tsonly" and "tsaddr" |
| ☐ -l, --preload=NUMBER | send NUMBER packets as fast as possible before falling into normal mode of behavior (root only) |
| ☐ -p, --pattern=PATTERN | fill ICMP packet with given pattern (hex) |
| ■ -q, --quiet | quiet output |
| ☐ -R, --route | record route |
| ☐ -s, --size=NUMBER | send NUMBER data octets |

## Ayuda
| Opción | Descripción |
|--------|-------------|
| ■ -?, --help | give this help list |
| ■ --usage | give a short usage message |
| ■ -V, --version | print program version |

- timeout en select y bucle
