<div align="center">

![System & Kernel](https://img.shields.io/badge/System-brown?style=for-the-badge)
![Network Communication](https://img.shields.io/badge/Network-Communication-blue?style=for-the-badge)
![ICMP Protocol](https://img.shields.io/badge/Protocol-ICMP-green?style=for-the-badge)
![C Language](https://img.shields.io/badge/Language-C-red?style=for-the-badge)

*A reimplementation of the classic ping command*

</div>

# ft_ping

[README en Español](README_es.md)

<div align="center">
  <img src="/ft_ping.png">
</div>

## 🎯 Description

**ft_ping** is a from-scratch implementation of the iconic `ping` command, a fundamental tool for network administration and connectivity diagnostics. This project dives into low-level network protocols, specifically ICMP (Internet Control Message Protocol).

### What is Ping?

Ping is a network utility that:

- **Verifies connectivity** between devices on an IP network
- **Measures latency** (Round-Trip Time - RTT) of packets
- **Diagnoses network issues** and packet loss
- **Uses ICMP Echo Request/Reply** to operate

### Technical Flow

```
[Cliente] ----ICMP Echo Request----> [Servidor]
    ^                                     |
    |                                     v
[Cliente] <----ICMP Echo Reply------ [Servidor]
         
    RTT = Response_Time - Send_Time
```

The basic process:

1. **Packet construction**: Creates an ICMP Echo Request packet
2. **Sending**: Transmits the packet to the specified destination
3. **Waiting**: Waits for the ICMP Echo Reply response
4. **Measurement**: Calculates the round-trip time (RTT)
5. **Repeat**: Sends packets periodically until interrupted

## 🔧 Build

```bash
git clone https://github.com/Kobayashi82/ft_ping.git
cd ft_ping
make
```

## 🖥️ Usage

### Permissions

```bash
# ft_ping requires root privileges to create raw sockets
sudo ./ft_ping destino.com

# Alternative: configure capabilities
sudo setcap cap_net_raw+ep ./ft_ping
./ft_ping destino.com
```

### Execution

```bash
sudo ./ft_ping [opciones] <destino>
```

| Option | Type | Description | Example |
|-----------|------|-------------|---------|
| `destination` | IPv4/Hostname | IP address or hostname | `8.8.8.8`, `google.com` |

#### Basic
| Option | Long form | Description |
|--------|-------------|-------------|
| `-h`, `-?` | `--help` | Shows help information |
| `-V` | `--version` | Program version |
| | `--usage` | Short usage message |

#### Send control
| Option | Long form | Parameter | Description |
|--------|-------------|-----------|-------------|
| `-c` | `--count=NUM` | Number | Stops after sending NUM packets |
| `-i` | `--interval=NUM` | Seconds | Waits NUM seconds between packets |
| `-w` | `--timeout=NUM` | Seconds | Stops after NUM seconds |
| `-W` | `--linger=NUM` | Seconds | Response wait time |

#### Packet configuration
| Option | Long form | Parameter | Description |
|--------|-------------|-----------|-------------|
| `-s` | `--size=NUM` | Bytes | Sends NUM bytes of data |
| `-p` | `--pattern=PATTERN` | Hex | Fills ICMP packets with a hex pattern |
| | `--ttl=NUM` | Number | Sets NUM as Time-To-Live |
| `-T` | `--tos=NUM` | Number | Sets Type of Service (TOS) |

#### Network options
| Option | Long form | Description |
|--------|-------------|-------------|
| `-n` | `--numeric` | Does not resolve hostnames in replies |
| `-r` | `--ignore-routing` | Sends directly to a host on an attached network |
| `-d` | `--debug` | Enables SO_DEBUG option (kernel-dependent) |

#### Output and format
| Option | Long form | Description |
|--------|-------------|-------------|
| `-v` | `--verbose` | Detailed output |
| `-q` | `--quiet` | Quiet output |

### TOS values (Type of Service)

The `-T` option allows configuring the TOS field of the IP header:

| Value | Type | Description |
|-------|------|-------------|
| 16 | Low Delay | Low latency |
| 4 | High Reliability | High reliability |
| 8 | High Throughput | High throughput |
| 136 | High Priority | High priority |
| 184 | Expedited Forwarding | Expedited forwarding |

## 📡 ICMP Protocol

### ICMP Packet Structure

```c
typedef struct icmp_header {
    uint8_t  type;          // ICMP message type
    uint8_t  code;          // Specific code
    uint16_t checksum;      // Packet checksum
    uint16_t identifier;    // Process ID
    uint16_t sequence;      // Sequence number
    // Payload data...
};
```

### Relevant ICMP Types

| Type | Code | Description | Use in ft_ping |
|------|--------|-------------|----------------|
| 8 | 0 | Echo Request | Outgoing packet |
| 0 | 0 | Echo Reply | Expected response |
| 3 | * | Destination Unreachable | Destination error |
| 11 | 0 | Time Exceeded | TTL expired |

### Checksum calculation

The ICMP checksum is an error-detection mechanism that ensures packet integrity during transmission:

#### Calculation process

1. **Sum 16-bit words**: Take all bytes of the ICMP packet and group them into 16-bit words (2 bytes). Each word is added to a 32-bit accumulator.

2. **Handle odd bytes**: If the packet has an odd number of bytes, the last byte is treated as the high part of a 16-bit word (shifted left by 8 bits).

3. **Carry propagation**: Overflow bits (carry) beyond 16 bits are added back into the result.

4. **One's complement**: Finally, apply the one's complement (NOT) to the 16-bit result.

#### Integrity verification

- **On send**: Compute checksum with the checksum field set to zero
- **On receive**: Recompute checksum including the received value
- **Validation**: If the packet is intact, the result should be 0xFFFF

This method efficiently detects single-bit errors, byte swaps, and many multi-bit errors, which is sufficient for ICMP needs.

## 🛡️ Security Considerations

### Responsible use

⚠️ **Important warnings:**
- **Respect organizational network policies**
- **Avoid saturating** slow connections
- **Use appropriate intervals** in production

### Attack detection

Some firewalls can detect:
- **Ping floods** (many rapid packets)
- **Anomalous packets** (odd sizes)
- **Suspicious patterns** (irregular sequences)

## 📄 License

This project is licensed under the WTFPL – [Do What the Fuck You Want to Public License](http://www.wtfpl.net/about/).

---

<div align="center">

**📡 Developed as part of the 42 School curriculum 📡**

*"Understanding networks, one ping at a time"*

</div>
