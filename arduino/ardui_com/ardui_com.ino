#include <Wire.h>
#include <stdint.h>

enum RecvState { WAITING_COMMAND, WAITING_COMMAND_DATA };
enum RecvState state;
uint8_t valve;

#define I2C_PORT 0x20
#define VALID_PORTS_START 38
#define VALID_PORTS_END 53

void setup() {
  for (int i = VALID_PORTS_START; i <= VALID_PORTS_END; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  Wire.begin(I2C_PORT);  // join i2c bus (address optional for master)
  Serial.begin(9600);    // start serial for output
  // Setup the state
  state = WAITING_COMMAND;
  Wire.onReceive(onReceiveCommand);
}

void loop() {}

void onReceiveCommand() {
  // Read the octet on the wire
  uint8_t octet = Wire.read();

  // State mahine, initial state => WATING_COMMAND
  switch (state) {
    case WAITING_COMMAND:
      valve = octet;
      state = WAITING_COMMAND_DATA;
      break;

    case WAITING_COMMAND_DATA:
      if (valve >= VALID_PORTS_START && valve <= VALID_PORTS_END) {
        // True => >0 => ouvrir la valve
        // Sanity check on the pin value
        digitalWrite(valve, octet ? HIGH : LOW);
        Serial.print(valve);
        Serial.print(octet);
        Serial.print("\n");
      }
      state = WAITING_COMMAND;
      break;
  }
}
