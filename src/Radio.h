#pragma once

#include <Arduino.h>
#include <stdint.h>
#include "RadioPacket.h"

namespace FW {

/*
 * Radio status snapshot.
 *
 * Keeping counters in one plain struct makes display, logging, and future USB or
 * BLE diagnostics simple. Counters are not reset automatically because rising
 * values are useful while chasing range, antenna, and packet-format problems.
 */
struct RadioStats {
    bool initialized = false;
    bool receiving = false;
    bool transmitting = false;
    unsigned long lastTxAt = 0;
    unsigned long lastRxAt = 0;
    uint32_t txPackets = 0;
    uint32_t rxPackets = 0;
    uint32_t ackPackets = 0;
    uint32_t rejectedFrames = 0;
    uint32_t rxErrors = 0;
    uint32_t txErrors = 0;
    int16_t lastRssi = 0;
    int8_t lastSnr = 0;
};

/*
 * FarmWhisper radio facade.
 *
 * Application code should talk to FW::Radio instead of the vendor driver. That
 * gives us one stable FarmWhisper API while the implementation underneath can
 * change from RAK SX126x, to RadioLib, to a USB-controlled bridge radio, or to a
 * different board class later.
 */
class Radio {
public:
    /* Initialize the radio hardware and enter receive mode. */
    static bool begin();

    /*
     * Main non-blocking radio service.
     *
     * Call this once per Boot::loop(). It handles TX/RX completion flags,
     * periodic HELLO beacons, ACK responses, and returning the radio to RX.
     */
    static void update();

    /* Send a HELLO packet now instead of waiting for the beacon interval. */
    static bool sendHello();

    /* Send a packet. This is the low-level test entry point for future modules. */
    static bool sendPacket(PacketType type, uint16_t destination, const uint8_t* payload, uint8_t payloadLength);

    /* Current health and diagnostic counters. */
    static const RadioStats& stats();

    /* Local node id used in packet headers. */
    static uint16_t nodeId();

    /* Update the local node id. Call before begin() for clean startup logs. */
    static void setNodeId(uint16_t id);

private:
    static void processReceivedFrame();
    static void handlePacket(const RadioPacket& packet);
    static void sendAck(const RadioPacket& packet);
    static void restartReceive();
    static void logPacket(const char* prefix, const RadioPacket& packet);
    static bool timeDue(unsigned long now, unsigned long dueAt);
};

} // namespace FW
