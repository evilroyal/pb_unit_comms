#include <Wire.h>
#include <stdint.h>

#include "protocol.hpp"

#define I2C_PORT 0x20
constexpr uint8_t VALID_ACTUATOR_ID_MIN = 38;
constexpr uint8_t VALID_ACTUATOR_ID_MAX = 53;

enum Error {
  ERR_NONE = 0, ///< Correct return state.
  ERR_UNKNOWN ///< Unknown error.
};

typedef Error (*packet_handler_t)(const BrouePacket*);

static packet_handler_t packet_handlers[BROUE_TYPE_COUNT] = { nullptr };
static SlaveProtocolState s_state;
static BrouePacket response_packet = { 0 };

Error actuate_handler(const BrouePacket* pkt) {
  const BroueActuatePayload& payload = pkt->payload.actuate_payload;

  if (payload.actuator_id >= VALID_ACTUATOR_ID_MIN 
      && payload.actuator_id <= VALID_ACTUATOR_ID_MAX) {
    // True => >0 => ouvrir la valve
    // Sanity check on the pin value
    digitalWrite(payload.actuator_id, payload.actuator_command ? HIGH : LOW);
    return ERR_NONE;
  }
  return ERR_UNKNOWN;
}

void setup() {
  for (int i = VALID_ACTUATOR_ID_MIN; i <= VALID_ACTUATOR_ID_MAX; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  Wire.begin(I2C_PORT);  // join i2c bus (address optional for master)
  Serial.begin(9600);    // start serial for output

  Wire.onReceive(&onReceivePacket);
  Wire.onRequest(&onReadRequest);
}

void loop() {}

void onReceivePacket(int num_bytes) {
  if (num_bytes != BROUE_PACKET_SIZE) {
    Serial.print("Unexpected packet size on channel, got ");
    Serial.print(num_bytes);
    Serial.print(" bytes, expected ");
    Serial.println(BROUE_PACKET_SIZE);

    return;
  }

  if (s_state != SS_IDLE) {
    Serial.println("Received packet from master while not in idle state, dropping...");
  }

  // Read packet data from I2C channel.
  uint8_t pkt_data_buf[BROUE_PACKET_SIZE];
  for (int i = 0; i < BROUE_PACKET_SIZE; i++) {
    pkt_data_buf[i] = Wire.read();
  }
  BrouePacket* pkt = reinterpret_cast<BrouePacket*>(&pkt_data_buf);

  // Call handler for functionality.
  if (packet_handlers[pkt->type]) {
    Error status = packet_handlers[pkt->type](pkt);
  }

  // Change FSM state.  
  switch (pkt->type) {
    case BROUE_ACTUATE:
      s_state = SS_ACTUATE_WAIT_READ;
      break;

    case BROUE_READ_SENSOR:
      s_state = SS_READ_SENSOR_WAIT_READ;
      break;

    case BROUE_HELLO:
      s_state = SS_HELLO_WAIT_READ;
      break;

    default:
      Serial.print("Unexpected packet type : ");
      Serial.println(BROUE_PACKET_TYPE_STR[pkt->type]);
  }
}

void onReadRequest() {
  // Check that we're in a read-wait state.
  switch (s_state) {
    case SS_ACTUATE_WAIT_READ:
    case SS_READ_SENSOR_WAIT_READ:
    case SS_HELLO_WAIT_READ:
      break;

    default:
      Serial.println("Received read request while in invalid state");
      return;
  }

  // Write prepared response packet to channel.
  const uint8_t* pkt_data = reinterpret_cast<const uint8_t*>(&response_packet);
  Wire.write(pkt_data, BROUE_PACKET_SIZE);

  // Change FSM state.
  s_state = SS_IDLE;
}
