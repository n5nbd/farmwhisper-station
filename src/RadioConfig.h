#pragma once

/*
 * FarmWhisper radio configuration.
 *
 * This file keeps first-stage radio settings in one obvious place. During
 * prototyping that matters more than clever configuration plumbing. A builder
 * should be able to open one file, compare two nodes, and understand whether
 * they are supposed to hear each other.
 *
 * This is raw LoRa point-to-point traffic, not LoRaWAN. That is intentional for
 * FarmWhisper's local-first design: farm stations, sensors, annunciators, and
 * actuators should be able to communicate locally without a cloud account,
 * network join, gateway subscription, or Internet dependency.
 */

#include <stdint.h>

/*
 * Optional compile-time node override.
 *
 * The default node id is fine for a one-board smoke test. For two-board testing,
 * either edit DEFAULT_NODE_ID below or compile one board with FW_RADIO_NODE_ID
 * set differently. Keeping this as a macro option gives us a clean path toward
 * per-board build profiles later without changing application code.
 */
#ifndef FW_RADIO_NODE_ID
#define FW_RADIO_NODE_ID 0x0001
#endif

namespace FW {
namespace RadioConfig {

/*
 * US ISM test frequency.
 *
 * 915.000 MHz is a simple lab default for US915 hardware. Field deployments can
 * later move to a channel plan that avoids local noise and respects the region.
 * Every node in a bench test must use the same frequency.
 */
static constexpr uint32_t RF_FREQUENCY_HZ = 915000000UL;

/*
 * LoRa modem settings.
 *
 * These settings favor easy bring-up over maximum range. SF7/BW125 keeps packet
 * airtime short enough that serial testing feels responsive. If range is poor,
 * increase spreading factor later before changing FarmWhisper protocol logic.
 *
 * SX126x-Arduino uses numeric bandwidth and coding-rate values matching the
 * Semtech driver API. In that driver:
 *   bandwidth 0 = 125 kHz
 *   codingRate 1 = 4/5
 */
static constexpr uint8_t LORA_BANDWIDTH = 0;
static constexpr uint8_t LORA_SPREADING_FACTOR = 7;
static constexpr uint8_t LORA_CODING_RATE = 1;
static constexpr uint16_t LORA_PREAMBLE_LENGTH = 8;
static constexpr bool LORA_FIXED_LENGTH_PAYLOAD = false;
static constexpr bool LORA_IQ_INVERSION = false;

/*
 * Transmit power.
 *
 * Start modestly. 14 dBm is plenty for bench work and safer while antennas,
 * cable paths, power behavior, and duty cycle patterns are still being proven.
 */
static constexpr int8_t TX_OUTPUT_POWER_DBM = 14;

/*
 * Timing.
 *
 * HELLO packets prove TX without requiring the rest of the application layer.
 * Continuous RX keeps the node listening between beacons.
 */
static constexpr unsigned long HELLO_INTERVAL_MS = 10000UL;
static constexpr uint32_t TX_TIMEOUT_MS = 3000UL;
static constexpr uint32_t RX_CONTINUOUS_TIMEOUT_MS = 0UL;

/*
 * Prototype network identity.
 *
 * This is not security. It is only a cheap filter so unrelated LoRa traffic does
 * not look like a FarmWhisper packet. Authentication/encryption belongs above
 * this first hardware bring-up layer.
 */
static constexpr uint16_t NETWORK_ID = 0xF011;
static constexpr uint16_t BROADCAST_NODE = 0xFFFF;
static constexpr uint16_t DEFAULT_NODE_ID = FW_RADIO_NODE_ID;

} // namespace RadioConfig
} // namespace FW
