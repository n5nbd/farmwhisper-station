# FarmWhisper Radio Test Log

## 2026-06-28 — Radio Prototyping Milestone 1

Result: PASS

Hardware:
- Board 1: RAK4631 on RAK5005
- Board 2: RAK4631 on RAK5005
- Radio: SX1262 LoRa P2P

Firmware:
- fw-station
- Version: 0.0.1
- Node 1 source ID: 1
- Node 2 source ID: 2

Confirmed behavior:
- Node 1 boots and reports Radio OK
- Node 2 boots and reports Radio OK
- Node 1 transmits HELLO
- Node 2 receives HELLO from Node 1
- Node 2 sends ACK to Node 1
- Node 1 receives ACK from Node 2
- Node 2 transmits HELLO
- Node 1 receives HELLO from Node 2
- Node 1 sends ACK to Node 2
- Node 2 receives ACK from Node 1
- RSSI/SNR are reported on received packets
- Heartbeat continues while radio traffic is active

Representative signal readings:
- RSSI: approximately -52 to -60 dBm
- SNR: approximately 12 to 13 dB

Milestone conclusion:
FarmWhisper radio prototyping milestone 1 is complete. The first bidirectional LoRa HELLO/ACK link has been confirmed.
