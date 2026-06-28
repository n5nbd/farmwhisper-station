#include <Arduino.h>
#include <SPI.h>
#include <string.h>
#include <SX126x-RAK4630.h>

#include "BuildInfo.h"
#include "Logger.h"
#include "Radio.h"
#include "RadioConfig.h"

namespace FW {
namespace {

RadioEvents_t radioEvents;
RadioStats radioStats;
uint16_t localNodeId = RadioConfig::DEFAULT_NODE_ID;
uint16_t nextSequence = 1;
unsigned long nextHelloAt = 0;

volatile bool txDoneFlag = false;
volatile bool txTimeoutFlag = false;
volatile bool rxDoneFlag = false;
volatile bool rxTimeoutFlag = false;
volatile bool rxErrorFlag = false;

uint8_t rxBuffer[RadioPacket::MAX_WIRE_SIZE];
uint16_t rxSize = 0;
int16_t rxRssi = 0;
int8_t rxSnr = 0;

void onTxDone() {
    txDoneFlag = true;
}

void onTxTimeout() {
    txTimeoutFlag = true;
}

void onRxDone(uint8_t* payload, uint16_t size, int16_t rssi, int8_t snr) {
    const uint16_t copySize = size > RadioPacket::MAX_WIRE_SIZE ? RadioPacket::MAX_WIRE_SIZE : size;

    for (uint16_t i = 0; i < copySize; ++i) {
        rxBuffer[i] = payload[i];
    }

    rxSize = copySize;
    rxRssi = rssi;
    rxSnr = snr;
    rxDoneFlag = true;
}

void onRxTimeout() {
    rxTimeoutFlag = true;
}

void onRxError() {
    rxErrorFlag = true;
}

} // namespace

bool Radio::begin() {
    Logger::info("Radio begin");

    /*
     * RAK4630/4631 has fixed internal wiring between nRF52840 and SX1262.
     * The helper configures NSS/SCK/MOSI/MISO/BUSY/DIO1/RESET plus the RAK RF
     * switch and TCXO behavior expected by this module family. We keep that
     * board knowledge here so FarmWhisper application code never depends on it.
     */
    lora_rak4630_init();

    radioEvents.TxDone = onTxDone;
    radioEvents.RxDone = onRxDone;
    radioEvents.TxTimeout = onTxTimeout;
    radioEvents.RxTimeout = onRxTimeout;
    radioEvents.RxError = onRxError;

    ::Radio.Init(&radioEvents);
    ::Radio.SetChannel(RadioConfig::RF_FREQUENCY_HZ);

    ::Radio.SetTxConfig(MODEM_LORA,
                        RadioConfig::TX_OUTPUT_POWER_DBM,
                        0,
                        RadioConfig::LORA_BANDWIDTH,
                        RadioConfig::LORA_SPREADING_FACTOR,
                        RadioConfig::LORA_CODING_RATE,
                        RadioConfig::LORA_PREAMBLE_LENGTH,
                        RadioConfig::LORA_FIXED_LENGTH_PAYLOAD,
                        true,
                        0,
                        0,
                        RadioConfig::LORA_IQ_INVERSION,
                        RadioConfig::TX_TIMEOUT_MS);

    ::Radio.SetRxConfig(MODEM_LORA,
                        RadioConfig::LORA_BANDWIDTH,
                        RadioConfig::LORA_SPREADING_FACTOR,
                        RadioConfig::LORA_CODING_RATE,
                        0,
                        RadioConfig::LORA_PREAMBLE_LENGTH,
                        0,
                        RadioConfig::LORA_FIXED_LENGTH_PAYLOAD,
                        0,
                        true,
                        0,
                        0,
                        RadioConfig::LORA_IQ_INVERSION,
                        true);

    radioStats = RadioStats();
    radioStats.initialized = true;

    /*
     * Stagger the first beacon slightly by node id. If two boards are reset at
     * the same time, this reduces the chance that both immediately transmit
     * HELLO frames on top of each other every cycle during bench testing.
     */
    nextHelloAt = millis() + 2000UL + static_cast<unsigned long>((localNodeId * 137U) % 2000U);

    Logger::info("Radio configured for LoRa P2P");
    restartReceive();
    return true;
}

void Radio::update() {
    if (!radioStats.initialized) {
        return;
    }

    if (txDoneFlag) {
        txDoneFlag = false;
        radioStats.transmitting = false;
        radioStats.txPackets++;
        radioStats.lastTxAt = millis();
        Logger::info("Radio TX done");
        restartReceive();
    }

    if (txTimeoutFlag) {
        txTimeoutFlag = false;
        radioStats.transmitting = false;
        radioStats.txErrors++;
        Logger::warn("Radio TX timeout");
        restartReceive();
    }

    if (rxDoneFlag) {
        rxDoneFlag = false;
        processReceivedFrame();
        restartReceive();
    }

    if (rxTimeoutFlag) {
        rxTimeoutFlag = false;
        radioStats.receiving = false;
        Logger::warn("Radio RX timeout");
        restartReceive();
    }

    if (rxErrorFlag) {
        rxErrorFlag = false;
        radioStats.rxErrors++;
        radioStats.receiving = false;
        Logger::warn("Radio RX error");
        restartReceive();
    }

    const unsigned long now = millis();
    if (!radioStats.transmitting && timeDue(now, nextHelloAt)) {
        sendHello();
        nextHelloAt = now + RadioConfig::HELLO_INTERVAL_MS;
    }
}

bool Radio::sendHello() {
    char text[RadioPacket::MAX_PAYLOAD + 1];
    snprintf(text,
             sizeof(text),
             "name=%s fw=%s up=%lu",
             FW_NAME,
             FW_VERSION,
             millis() / 1000UL);

    return sendPacket(PacketType::Hello,
                      RadioConfig::BROADCAST_NODE,
                      reinterpret_cast<const uint8_t*>(text),
                      static_cast<uint8_t>(strlen(text)));
}

bool Radio::sendPacket(PacketType type, uint16_t destination, const uint8_t* payload, uint8_t payloadLength) {
    if (!radioStats.initialized) {
        Logger::warn("Radio send requested before init");
        return false;
    }

    if (radioStats.transmitting) {
        Logger::warn("Radio busy; send skipped");
        return false;
    }

    if (payloadLength > 0 && payload == nullptr) {
        Logger::warn("Radio payload pointer missing");
        return false;
    }

    if (payloadLength > RadioPacket::MAX_PAYLOAD) {
        Logger::warn("Radio payload too large");
        return false;
    }

    RadioPacket packet;
    packet.type = type;
    packet.flags = 0;
    packet.networkId = RadioConfig::NETWORK_ID;
    packet.source = localNodeId;
    packet.destination = destination;
    packet.sequence = nextSequence++;
    packet.payloadLength = payloadLength;

    for (uint8_t i = 0; i < payloadLength; ++i) {
        packet.payload[i] = payload[i];
    }

    uint8_t wire[RadioPacket::MAX_WIRE_SIZE];
    const size_t wireLength = encodePacket(packet, wire, sizeof(wire));
    if (wireLength == 0) {
        Logger::warn("Radio encode failed");
        return false;
    }

    logPacket("TX", packet);
    radioStats.receiving = false;
    radioStats.transmitting = true;
    ::Radio.Send(wire, static_cast<uint8_t>(wireLength));
    return true;
}

const RadioStats& Radio::stats() {
    return radioStats;
}

uint16_t Radio::nodeId() {
    return localNodeId;
}

void Radio::setNodeId(uint16_t id) {
    localNodeId = id;
}

void Radio::processReceivedFrame() {
    RadioPacket packet;

    radioStats.lastRssi = rxRssi;
    radioStats.lastSnr = rxSnr;
    radioStats.lastRxAt = millis();

    if (!decodePacket(rxBuffer, rxSize, packet)) {
        radioStats.rejectedFrames++;
        Logger::warn("Radio rejected frame");
        return;
    }

    if (packet.networkId != RadioConfig::NETWORK_ID) {
        radioStats.rejectedFrames++;
        Logger::warn("Radio ignored foreign network packet");
        return;
    }

    if (packet.destination != localNodeId && packet.destination != RadioConfig::BROADCAST_NODE) {
        Logger::info("Radio ignored packet for another node");
        return;
    }

    radioStats.rxPackets++;
    logPacket("RX", packet);
    handlePacket(packet);
}

void Radio::handlePacket(const RadioPacket& packet) {
    switch (packet.type) {
        case PacketType::Hello:
            sendAck(packet);
            break;

        case PacketType::Ack:
            radioStats.ackPackets++;
            Logger::info("Radio ACK received");
            break;

        default:
            Logger::info("Radio packet has no handler yet");
            break;
    }
}

void Radio::sendAck(const RadioPacket& packet) {
    char text[RadioPacket::MAX_PAYLOAD + 1];
    snprintf(text, sizeof(text), "ack=%u", packet.sequence);
    sendPacket(PacketType::Ack,
               packet.source,
               reinterpret_cast<const uint8_t*>(text),
               static_cast<uint8_t>(strlen(text)));
}

void Radio::restartReceive() {
    if (!radioStats.initialized || radioStats.transmitting) {
        return;
    }

    ::Radio.Rx(RadioConfig::RX_CONTINUOUS_TIMEOUT_MS);
    radioStats.receiving = true;
}

void Radio::logPacket(const char* prefix, const RadioPacket& packet) {
    Serial.print("[RADIO] ");
    Serial.print(prefix);
    Serial.print(" type=");
    Serial.print(packetTypeText(packet.type));
    Serial.print(" src=");
    Serial.print(packet.source);
    Serial.print(" dst=");
    Serial.print(packet.destination);
    Serial.print(" seq=");
    Serial.print(packet.sequence);
    Serial.print(" len=");
    Serial.print(packet.payloadLength);

    if (prefix != nullptr && prefix[0] == 'R') {
        Serial.print(" rssi=");
        Serial.print(radioStats.lastRssi);
        Serial.print(" snr=");
        Serial.print(radioStats.lastSnr);
    }

    if (packet.payloadLength > 0) {
        Serial.print(" payload=\"");
        for (uint8_t i = 0; i < packet.payloadLength; ++i) {
            const char c = static_cast<char>(packet.payload[i]);
            Serial.print((c >= 32 && c <= 126) ? c : '.');
        }
        Serial.print("\"");
    }

    Serial.println();
}

bool Radio::timeDue(unsigned long now, unsigned long dueAt) {
    return static_cast<long>(now - dueAt) >= 0;
}

} // namespace FW
