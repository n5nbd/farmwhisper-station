#include "RadioPacket.h"

namespace FW {
namespace {

constexpr uint8_t MAGIC_0 = 'F';
constexpr uint8_t MAGIC_1 = 'W';
constexpr uint8_t PROTOCOL_VERSION = 1;
constexpr size_t HEADER_SIZE = 14;
constexpr size_t CRC_SIZE = 2;

void writeU16(uint8_t* buffer, size_t offset, uint16_t value) {
    buffer[offset] = static_cast<uint8_t>((value >> 8) & 0xFF);
    buffer[offset + 1] = static_cast<uint8_t>(value & 0xFF);
}

uint16_t readU16(const uint8_t* buffer, size_t offset) {
    return static_cast<uint16_t>((static_cast<uint16_t>(buffer[offset]) << 8) | buffer[offset + 1]);
}

} // namespace

const char* packetTypeText(PacketType type) {
    switch (type) {
        case PacketType::Hello:   return "HELLO";
        case PacketType::Ack:     return "ACK";
        case PacketType::Status:  return "STATUS";
        case PacketType::Command: return "COMMAND";
        case PacketType::Event:   return "EVENT";
        case PacketType::Debug:   return "DEBUG";
        case PacketType::Unknown:
        default:                  return "UNKNOWN";
    }
}

uint16_t crc16Ccitt(const uint8_t* data, size_t length) {
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < length; ++i) {
        crc ^= static_cast<uint16_t>(data[i]) << 8;

        for (uint8_t bit = 0; bit < 8; ++bit) {
            if ((crc & 0x8000) != 0) {
                crc = static_cast<uint16_t>((crc << 1) ^ 0x1021);
            } else {
                crc = static_cast<uint16_t>(crc << 1);
            }
        }
    }

    return crc;
}

size_t encodePacket(const RadioPacket& packet, uint8_t* out, size_t outCapacity) {
    if (out == nullptr) {
        return 0;
    }

    if (packet.payloadLength > RadioPacket::MAX_PAYLOAD) {
        return 0;
    }

    const size_t wireLength = HEADER_SIZE + packet.payloadLength + CRC_SIZE;
    if (wireLength > outCapacity || wireLength > RadioPacket::MAX_WIRE_SIZE) {
        return 0;
    }

    out[0] = MAGIC_0;
    out[1] = MAGIC_1;
    out[2] = PROTOCOL_VERSION;
    out[3] = static_cast<uint8_t>(packet.type);
    out[4] = packet.flags;
    writeU16(out, 5, packet.networkId);
    writeU16(out, 7, packet.source);
    writeU16(out, 9, packet.destination);
    writeU16(out, 11, packet.sequence);
    out[13] = packet.payloadLength;

    for (uint8_t i = 0; i < packet.payloadLength; ++i) {
        out[HEADER_SIZE + i] = packet.payload[i];
    }

    const uint16_t crc = crc16Ccitt(out, HEADER_SIZE + packet.payloadLength);
    writeU16(out, HEADER_SIZE + packet.payloadLength, crc);

    return wireLength;
}

bool decodePacket(const uint8_t* data, size_t length, RadioPacket& out) {
    if (data == nullptr) {
        return false;
    }

    if (length < HEADER_SIZE + CRC_SIZE || length > RadioPacket::MAX_WIRE_SIZE) {
        return false;
    }

    if (data[0] != MAGIC_0 || data[1] != MAGIC_1) {
        return false;
    }

    if (data[2] != PROTOCOL_VERSION) {
        return false;
    }

    const uint8_t payloadLength = data[13];
    if (payloadLength > RadioPacket::MAX_PAYLOAD) {
        return false;
    }

    if (length != HEADER_SIZE + payloadLength + CRC_SIZE) {
        return false;
    }

    const uint16_t expectedCrc = readU16(data, HEADER_SIZE + payloadLength);
    const uint16_t actualCrc = crc16Ccitt(data, HEADER_SIZE + payloadLength);
    if (expectedCrc != actualCrc) {
        return false;
    }

    out.type = static_cast<PacketType>(data[3]);
    out.flags = data[4];
    out.networkId = readU16(data, 5);
    out.source = readU16(data, 7);
    out.destination = readU16(data, 9);
    out.sequence = readU16(data, 11);
    out.payloadLength = payloadLength;

    for (uint8_t i = 0; i < payloadLength; ++i) {
        out.payload[i] = data[HEADER_SIZE + i];
    }

    for (uint8_t i = payloadLength; i < RadioPacket::MAX_PAYLOAD; ++i) {
        out.payload[i] = 0;
    }

    return true;
}

} // namespace FW
