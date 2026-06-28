#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace FW {

/*
 * FarmWhisper packet type.
 *
 * The first milestone only needs HELLO and ACK. The extra named values are here
 * because a protocol should be readable from day one. Future station, sensor,
 * actuator, and diagnostic messages can grow into these slots without turning
 * early radio code into an undocumented demo sketch.
 */
enum class PacketType : uint8_t {
    Unknown = 0x00,
    Hello   = 0x01,
    Ack     = 0x02,
    Status  = 0x03,
    Command = 0x04,
    Event   = 0x05,
    Debug   = 0x7F
};

/*
 * Decoded packet container.
 *
 * Payload storage is fixed-size on purpose. Early embedded protocol code should
 * avoid heap allocation unless there is a clear reason to accept fragmentation
 * risk and more complex failure behavior.
 */
struct RadioPacket {
    static constexpr uint8_t MAX_PAYLOAD = 64;
    static constexpr uint8_t MAX_WIRE_SIZE = 80;

    PacketType type = PacketType::Unknown;
    uint8_t flags = 0;
    uint16_t networkId = 0;
    uint16_t source = 0;
    uint16_t destination = 0;
    uint16_t sequence = 0;
    uint8_t payloadLength = 0;
    uint8_t payload[MAX_PAYLOAD] = {0};
};

/* Convert packet type values into short log-safe text. */
const char* packetTypeText(PacketType type);

/*
 * Serialize a packet into a wire buffer.
 *
 * Returns bytes written, or 0 if the packet is invalid or the output buffer is
 * too small. The output includes a CRC so receivers reject corrupted frames
 * before higher-level code sees them.
 */
size_t encodePacket(const RadioPacket& packet, uint8_t* out, size_t outCapacity);

/* Decode and validate a wire buffer. */
bool decodePacket(const uint8_t* data, size_t length, RadioPacket& out);

/* Build a CRC16-CCITT checksum over the provided bytes. */
uint16_t crc16Ccitt(const uint8_t* data, size_t length);

} // namespace FW
