# Guía Completa de Conflictos de Opciones - ft_ping

## 📋 Resumen de Conflictos

### Conflictos Duros (Error o Última Opción Gana)
- **Modos de salida**: `-q`, `-v`, `-f` (mutuamente excluyentes)
- **Tipos de petición ICMP**: `--address`, `--echo`, `--mask`, `--timestamp`, `-t` (mutuamente excluyentes)
- **Opciones de información**: `-h`, `--usage`, `-V` (mutuamente excluyentes, terminan programa)

### Conflictos de Contexto (Warning/Ignorar)
- **Opciones específicas de echo**: `-p`, `-s`, `-l`, `-R` solo con echo requests
- **Incompatibilidades de flood**: `-i`, `-l` ignorados en flood mode
- **Opciones inútiles en quiet**: `-R`, `--ip-timestamp` ignorados en quiet mode

---

## 🎯 Estructura de Datos Recomendada

```c
typedef enum {
    OUTPUT_NORMAL = 0,
    OUTPUT_QUIET,
    OUTPUT_VERBOSE,
    OUTPUT_FLOOD
} output_mode_t;

typedef enum {
    REQUEST_ECHO = 0,      // Default
    REQUEST_ADDRESS,
    REQUEST_MASK,
    REQUEST_TIMESTAMP,
    REQUEST_CUSTOM
} request_type_t;

typedef enum {
    INFO_NONE = 0,
    INFO_HELP,
    INFO_USAGE,
    INFO_VERSION
} info_mode_t;

typedef struct {
    // Modos de salida (mutuamente excluyentes)
    output_mode_t output_mode;
    
    // Tipos de petición (mutuamente excluyentes)
    request_type_t request_type;
    
    // Opciones de información (mutuamente excluyentes, terminan programa)
    info_mode_t info_mode;
    
    // Resto de opciones...
    char pattern[MAX_PATTERN_SIZE];
    int size;
    int preload;
    bool route;
    int interval;
    int custom_type;
    bool ip_timestamp;
    // ... otras opciones
} t_options;
```

---

## 🔍 Conflictos Detallados

### 1. Conflictos de Salida (Output Conflicts)

**Opciones afectadas**: `-q`, `-v`, `-f`

```c
// En el parsing:
case 'q':
    if (opts->output_mode != OUTPUT_NORMAL) {
        fprintf(stderr, "ft_ping: conflicting output options\n");
        return -1;
    }
    opts->output_mode = OUTPUT_QUIET;
    break;

case 'v':
    if (opts->output_mode != OUTPUT_NORMAL) {
        fprintf(stderr, "ft_ping: conflicting output options\n");
        return -1;
    }
    opts->output_mode = OUTPUT_VERBOSE;
    break;

case 'f':
    if (opts->output_mode != OUTPUT_NORMAL) {
        fprintf(stderr, "ft_ping: conflicting output options\n");
        return -1;
    }
    opts->output_mode = OUTPUT_FLOOD;
    break;
```

**Alternativa (última opción gana)**:
```c
case 'q':
    opts->output_mode = OUTPUT_QUIET;
    break;
// Sin verificación de conflictos
```

### 2. Conflictos de Tipo de Petición ICMP

**Opciones afectadas**: `--address`, `--echo`, `--mask`, `--timestamp`, `-t`

```c
// Función helper para verificar conflictos de tipo
static int set_request_type(t_options *opts, request_type_t new_type) {
    if (opts->request_type != REQUEST_ECHO && opts->request_type != new_type) {
        fprintf(stderr, "ft_ping: conflicting request type options\n");
        return -1;
    }
    opts->request_type = new_type;
    return 0;
}

// En el parsing:
case OPT_ADDRESS:
    return set_request_type(opts, REQUEST_ADDRESS);

case OPT_MASK:
    return set_request_type(opts, REQUEST_MASK);

case OPT_TIMESTAMP:
    return set_request_type(opts, REQUEST_TIMESTAMP);

case 't':
    if (set_request_type(opts, REQUEST_CUSTOM) < 0)
        return -1;
    opts->custom_type = atoi(optarg);
    break;
```

### 3. Conflictos de Opciones de Información

**Opciones afectadas**: `-h`, `--usage`, `-V`

```c
// Estas opciones terminan el programa inmediatamente
case 'h':
    opts->info_mode = INFO_HELP;
    return 0;  // Termina parsing inmediatamente

case OPT_USAGE:
    opts->info_mode = INFO_USAGE;
    return 0;

case 'V':
    opts->info_mode = INFO_VERSION;
    return 0;
```

---

## ⚠️ Conflictos de Contexto

### 1. Opciones Específicas de Echo

**Problema**: `-p`, `-s`, `-l`, `-R` solo tienen sentido con echo requests

```c
void validate_echo_specific_options(t_options *opts) {
    if (opts->request_type != REQUEST_ECHO) {
        if (opts->pattern[0] != '\0') {
            fprintf(stderr, "ft_ping: warning: -p only applies to echo requests\n");
            opts->pattern[0] = '\0';  // Reset
        }
        
        if (opts->size != DEFAULT_SIZE) {
            fprintf(stderr, "ft_ping: warning: -s only applies to echo requests\n");
            opts->size = DEFAULT_SIZE;
        }
        
        if (opts->preload > 0) {
            fprintf(stderr, "ft_ping: warning: -l only applies to echo requests\n");
            opts->preload = 0;
        }
        
        if (opts->route) {
            fprintf(stderr, "ft_ping: warning: -R only applies to echo requests\n");
            opts->route = false;
        }
    }
}
```

### 2. Incompatibilidades de Flood Mode

**Problema**: `-f` (flood) es incompatible con `-i` (interval) y `-l` (preload)

```c
void validate_flood_conflicts(t_options *opts) {
    if (opts->output_mode == OUTPUT_FLOOD) {
        if (opts->interval != DEFAULT_INTERVAL) {
            fprintf(stderr, "ft_ping: warning: -i ignored in flood mode\n");
            opts->interval = 0;  // Flood mode = sin intervalo
        }
        
        if (opts->preload > 0) {
            fprintf(stderr, "ft_ping: warning: -l ignored in flood mode\n");
            opts->preload = 0;
        }
    }
}
```

### 3. Opciones Inútiles en Quiet Mode

**Problema**: `-R`, `--ip-timestamp` no tienen sentido con `-q` (quiet)

```c
void validate_quiet_conflicts(t_options *opts) {
    if (opts->output_mode == OUTPUT_QUIET) {
        if (opts->route) {
            fprintf(stderr, "ft_ping: warning: -R ignored in quiet mode\n");
            opts->route = false;
        }
        
        if (opts->ip_timestamp) {
            fprintf(stderr, "ft_ping: warning: --ip-timestamp ignored in quiet mode\n");
            opts->ip_timestamp = false;
        }
    }
}
```

---

## 🎯 Función de Validación Completa

```c
int validate_options(t_options *opts) {
    // Verificar opciones específicas de echo
    validate_echo_specific_options(opts);
    
    // Verificar conflictos de flood mode
    validate_flood_conflicts(opts);
    
    // Verificar opciones inútiles en quiet mode
    validate_quiet_conflicts(opts);
    
    // Verificaciones adicionales...
    
    return 0;
}
```

---

## 📝 Matriz de Compatibilidad

| Opción | Echo | Address | Mask | Timestamp | Quiet | Verbose | Flood |
|--------|------|---------|------|-----------|-------|---------|-------|
| `-p`   | ✅   | ⚠️      | ⚠️   | ⚠️        | ✅    | ✅      | ✅    |
| `-s`   | ✅   | ⚠️      | ⚠️   | ⚠️        | ✅    | ✅      | ✅    |
| `-l`   | ✅   | ⚠️      | ⚠️   | ⚠️        | ✅    | ✅      | ⚠️    |
| `-R`   | ✅   | ⚠️      | ⚠️   | ⚠️        | ⚠️    | ✅      | ✅    |
| `-i`   | ✅   | ✅      | ✅   | ✅        | ✅    | ✅      | ⚠️    |

**Leyenda**:
- ✅ Compatible
- ⚠️ Warning/Ignorado
- ❌ Error

---

## 🔧 Implementación Sugerida

### 1. Orden de Validación

```c
int parse_options(int argc, char **argv, t_options *opts) {
    // 1. Inicializar opciones con defaults
    init_options(opts);
    
    // 2. Procesar argumentos
    int result = process_arguments(argc, argv, opts);
    if (result < 0) return result;
    
    // 3. Manejar opciones de información (terminan programa)
    if (opts->info_mode != INFO_NONE) {
        show_info(opts->info_mode);
        return 1;  // Indica que debe terminar el programa
    }
    
    // 4. Validar conflictos de contexto
    validate_options(opts);
    
    // 5. Validar argumentos restantes
    validate_remaining_args(argc, argv, opts);
    
    return 0;
}
```

### 2. Manejo de Errores

```c
typedef enum {
    PARSE_OK = 0,
    PARSE_EXIT = 1,      // Opciones de info (help, version)
    PARSE_ERROR = -1     // Error en parsing
} parse_result_t;
```

---

## 🎨 Mensajes de Error Sugeridos

```c
// Conflictos duros
"ft_ping: conflicting output options (-q, -v, -f are mutually exclusive)"
"ft_ping: conflicting request type options"

// Conflictos de contexto
"ft_ping: warning: -p only applies to echo requests"
"ft_ping: warning: -s only applies to echo requests"
"ft_ping: warning: -l only applies to echo requests"
"ft_ping: warning: -R only applies to echo requests"
"ft_ping: warning: -i ignored in flood mode"
"ft_ping: warning: -l ignored in flood mode"
"ft_ping: warning: -R ignored in quiet mode"
"ft_ping: warning: --ip-timestamp ignored in quiet mode"
```

---

## 🧪 Casos de Prueba

```bash
# Conflictos duros (deben fallar)
./ft_ping -q -v host
./ft_ping -f -q host
./ft_ping --address --mask host

# Conflictos de contexto (warnings)
./ft_ping --address -p AA host
./ft_ping -q -R host
./ft_ping -f -i 2 host

# Casos válidos
./ft_ping -q host
./ft_ping -v host
./ft_ping -f host
./ft_ping --address host
./ft_ping -t 8 host
```

---

## 📚 Notas de Implementación

1. **Orden de precedencia**: Última opción gana vs error inmediato
2. **Warnings vs errores**: Conflictos de contexto son warnings, conflictos duros son errores
3. **Cleanup**: Resetear opciones incompatibles en lugar de fallar
4. **Compatibilidad**: Seguir comportamiento del ping original
5. **Testing**: Probar todos los casos de conflicto identificados

---

*Este documento debe actualizarse conforme se identifiquen nuevos conflictos o se modifique el comportamiento del parsing.*