# FarmWhisper Radio Prototyping

## Purpose

This patch brings up the first FarmWhisper radio layer for a RAK4631 core on a RAK5005 base board.

This milestone is not the final mesh stack. It is the clean foundation we need before routing, repeating, retries, encryption, USB bridge mode, BLE configuration, or phone tooling make sense.

This patch proves:

- the SX1262 radio initializes,
- the station can transmit periodic HELLO frames,
- another station can receive those frames,
- the receiver can answer with ACK frames,
- packet metadata appears in serial logs,
- higher-level FarmWhisper code talks to `FW::Radio`, not directly to the vendor radio driver.

## Hardware target

```text
RAK4631 core
RAK5005 base
SX1262 LoRa radio inside the RAK4631
US915 test frequency
```

The implementation uses the RAK/SX126x Arduino stack:

```cpp
#include <SX126x-RAK4630.h>
lora_rak4630_init();
```

That helper keeps board-specific radio wiring inside the backend instead of spreading pin assumptions through FarmWhisper code.

## Files added

```text
src/RadioConfig.h    Frequency, LoRa settings, node id, intervals
src/RadioPacket.h    FarmWhisper packet type and decoded packet container
src/RadioPacket.cpp  Packet encode/decode and CRC16 validation
src/Radio.h          Stable FarmWhisper radio facade
src/Radio.cpp        RAK4631/SX1262 prototype backend
```

## Files replaced

```text
src/Boot.cpp         Adds radio startup and radio update loop
src/Station.h        Adds explicit subsystem status setters
src/Station.cpp      Tracks radio initialized status
```

Logger is not replaced in this pass. The radio code only depends on the existing `Logger::info()` and `Logger::warn()` calls, plus direct `Serial.print()` for packet detail lines.

## Packet format

The wire packet is intentionally small and explicit.

```text
+--------+--------+-------------------------------+
| Bytes  | Field  | Meaning                       |
+--------+--------+-------------------------------+
| 0..1   | magic  | 'F' 'W'                       |
| 2      | ver    | protocol version, currently 1 |
| 3      | type   | HELLO, ACK, STATUS, etc.      |
| 4      | flags  | reserved for future use       |
| 5..6   | net    | prototype network id          |
| 7..8   | src    | sending node id               |
| 9..10  | dst    | destination node id           |
| 11..12 | seq    | packet sequence number        |
| 13     | len    | payload length                |
| 14..n  | data   | payload bytes                 |
| n+1..2 | crc    | CRC16-CCITT over header/data  |
+--------+--------+-------------------------------+
```

## Current packet types

```text
0x01 HELLO   periodic station beacon
0x02 ACK     response to received HELLO
0x03 STATUS  reserved
0x04 COMMAND reserved
0x05 EVENT   reserved
0x7F DEBUG   reserved
```

## One-node smoke test

Compile and upload to one RAK4631 board.

Expected serial output includes:

```text
[INFO] Radio begin
[INFO] Radio configured for LoRa P2P
[RADIO] TX type=HELLO src=1 dst=65535 seq=1 len=...
[INFO] Radio TX done
```

A HELLO should transmit about every 10 seconds.

## Two-node HELLO/ACK test

Before uploading the second board, change the node id in:

```text
src/RadioConfig.h
```

Default:

```cpp
#define FW_RADIO_NODE_ID 0x0001
```

Second node:

```cpp
#define FW_RADIO_NODE_ID 0x0002
```

Both nodes must match on:

```text
RF_FREQUENCY_HZ
LORA_BANDWIDTH
LORA_SPREADING_FACTOR
LORA_CODING_RATE
NETWORK_ID
```

Expected behavior:

```text
Board A sends HELLO broadcast
Board B receives HELLO
Board B sends ACK to Board A
Board A receives ACK
```

Useful serial lines:

```text
[RADIO] RX type=HELLO src=1 dst=65535 seq=12 len=... rssi=-42 snr=9 payload="name=NBDR fw=0.0.1 up=120"
[RADIO] TX type=ACK src=2 dst=1 seq=7 len=6 payload="ack=12"
[RADIO] RX type=ACK src=2 dst=1 seq=7 len=6 rssi=-41 snr=10 payload="ack=12"
[INFO] Radio ACK received
```

## Design decisions

### Raw LoRa, not LoRaWAN

FarmWhisper needs local-first behavior. A coop station, generator node, barn annunciator, or field sensor should not require a cloud account, network provider, or Internet service before it can speak locally.

### Stable FarmWhisper facade

Application code calls:

```cpp
FW::Radio::sendPacket(...)
```

It does not call the vendor `::Radio` object directly. This lets us later add retries, duplicate filtering, routing, USB radio bridge mode, encryption, alternate boards, and field diagnostics without making farm application code care about driver details.

### Fixed packet buffer

Packets use fixed buffers. That keeps memory behavior predictable on small embedded boards and avoids heap allocation during radio callbacks.

### Conservative integration

This second pass intentionally avoids replacing Logger files. The first patch did that to add logging levels, but it was more invasive than the radio milestone requires.

## Known limitations

This is a prototype layer. It does not yet include:

- retransmit queue,
- ACK timeout/retry policy,
- duplicate detection,
- routing/repeating,
- persistent node identity,
- encryption/authentication,
- channel selection UI,
- BLE or USB control interface,
- structured event bus above radio packets,
- display page for RX/TX counters.

Those belong after basic radio TX/RX is verified on the bench.

## Troubleshooting

### Compile cannot find `SX126x-RAK4630.h`

Install the `SX126x-Arduino` library used by RAK examples.

### Board transmits but no receiver logs appear

Check both boards have identical values in `RadioConfig.h` except node id.

### RX shows rejected frames

That means bytes are arriving but the FarmWhisper packet checks failed. Likely causes:

- another LoRa device nearby,
- different packet format on the other node,
- corrupted signal,
- wrong modem settings,
- CRC mismatch.

### No ACKs

Confirm the two boards have different node IDs. Broadcast HELLO frames are ACKed by receiving nodes, but a real network requires unique node identities before routing and duplicate detection can work.

## Next milestone after this works

Once two boards exchange HELLO/ACK reliably, the next clean milestone is:

```text
RadioTransport
  - outbound queue
  - ack-needed flag
  - retry timer
  - duplicate suppression
  - packet age / seen table
  - first structured event callback
```

That gives us the bridge from radio bring-up to a real FarmWhisper message layer.
