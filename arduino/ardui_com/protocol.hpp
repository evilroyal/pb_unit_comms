#pragma once

#include <stdint.h>

#define BROUE_PACKET_PAYLOAD_SIZE 16
#define BROUE_PACKET_SIZE (BROUE_PACKET_PAYLOAD_SIZE + 1)

enum BrouePacketType {
  BROUE_ACK             = 0,
  BROUE_ACTUATE         = 1,
  BROUE_READ_SENSOR     = 2,
  BROUE_HELLO           = 3,
  BROUE_SENSOR_RESPONSE = 4,
  BROUE_TYPE_COUNT      = 5
};

const char* BROUE_PACKET_TYPE_STR[] = {
  [BROUE_ACK] = "BROUE_ACK",
  [BROUE_ACTUATE] = "BROUE_ACTUATE",
  [BROUE_READ_SENSOR] = "BROUE_READ_SENSOR",
  [BROUE_HELLO] = "BROUE_HELLO",
  [BROUE_SENSOR_RESPONSE] = "BROUE_SENSOR_RESPONSE"
};

struct BroueAckPayload {
  uint8_t ack_payload[BROUE_PACKET_PAYLOAD_SIZE];
};

struct BroueActuatePayload {
  uint8_t actuator_id;
  uint8_t actuator_command;

  uint8_t pad[BROUE_PACKET_PAYLOAD_SIZE - 2];
};

struct BroueReadSensorPayload {
  uint8_t sensor_id;

  uint8_t pad[BROUE_PACKET_PAYLOAD_SIZE - 1];
};

struct BroueSensorValuePayload {
  uint8_t sensor_id;

  double sensor_value;

  uint8_t pad[BROUE_PACKET_PAYLOAD_SIZE - 1 - sizeof(double)];
};

union BrouePacketPayload {
  BroueAckPayload ack_payload;
  BroueActuatePayload actuate_payload;
  BroueReadSensorPayload read_sensor_payload;
  BroueSensorValuePayload sensor_value_payload;
};

struct BrouePacket {
  uint8_t type;
  union BrouePacketPayload payload;
};

enum SlaveProtocolState {
  SS_IDLE,
  SS_ACTUATE_WAIT_READ,
  SS_READ_SENSOR_WAIT_READ,
  SS_HELLO_WAIT_READ
};