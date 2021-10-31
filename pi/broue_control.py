from typing import List
import pigpio

from struct import Struct
from enum import IntEnum

I2C_BUS = 1
ARDUINO_I2C_ADDR = 0x20
BROUE_PACKET_SIZE = 17

class BrouePacketType(IntEnum):
    ACK             = 0
    ACTUATE         = 1
    READ_SENSOR     = 2
    HELLO           = 3
    SENSOR_RESPONSE = 4

broue_packet_struct = Struct("B16x")
broue_ack_struct = Struct("B16x")
broue_actuate_struct = Struct("BBB14x")
broue_read_sensor_struct = Struct("BB15x")
broue_hello_struct = Struct("B16x")
broue_sensor_response = Struct("Bd8x")

def str_to_bytes(data_as_str: str) -> List[int]:
    return [c for c in data_as_str]

class Broue:
    def __init__(self, bus_num = I2C_BUS) -> None:
        self.pi = pigpio.pi()
        self.bus = self.pi.i2c_open(I2C_BUS, ARDUINO_I2C_ADDR, 0)
        pass

    def send_command(self, reg: int, val: int) -> None:
        self.bus.write_byte(ARDUINO_I2C_ADDR, reg & 0xff)
        self.bus.write_byte(ARDUINO_I2C_ADDR, val & 0xff)

    def actuate_valve(self, act_id: int, val: int) -> None:
        data = broue_actuate_struct.pack(BrouePacketType.ACTUATE.value, act_id, val)
        print("{}, size={}".format(str_to_bytes(data), len(data)))

        # Send packet to I2C
        self.pi.i2c_write_device(self.bus, data)

        # Blocking read for ACK
        num_read, ack_data = self.pi.i2c_read_device(self.bus, BROUE_PACKET_SIZE)
        if num_read < 0:
            print("Error on reading ACK from slave")
            return
        
        ack_fields = broue_ack_struct.unpack(ack_data)
        if BrouePacketType(ack_fields[0]) != BrouePacketType.ACK:
            print("Received unexpected packet type from slave")
            return

    def read_sensor(self, sensor_id: int) -> float:
        data = broue_read_sensor_struct.pack(BrouePacketType.READ_SENSOR.value, sensor_id)
        print("{}, size={}".format(str_to_bytes(data), len(data)))

        # Send packet to I2C
        self.pi.i2c_write_device(self.bus, data)

        # Blocking read for ACK
        num_read, sensor_resp_data = self.pi.i2c_read_device(self.bus, BROUE_PACKET_SIZE)
        if num_read < 0:
            print("Error on reading SENSOR_RESPONSE from slave")
            return

        sensor_resp_fields = broue_sensor_response.unpack(sensor_resp_data)
        if BrouePacketType(sensor_resp_fields[0]) != BrouePacketType.SENSOR_RESPONSE:
            print("Received unexpected packet type from slave")
            return

        return sensor_resp_fields[1]

    def send_hello(self) -> bool:
        data = broue_hello_struct.pack(BrouePacketType.HELLO.value)
        print("{}, size={}".format(str_to_bytes(data), len(data)))
        
        # Send packet to I2C
        self.pi.i2c_write_device(self.bus, data)

        # Blocking read for ACK
        num_read, ack_data = self.pi.i2c_read_device(self.bus, BROUE_PACKET_SIZE)
        if num_read < 0:
            print("Error on reading ACK from slave")
            return
        
        ack_fields = broue_ack_struct.unpack(ack_data)
        if BrouePacketType(ack_fields[0]) != BrouePacketType.ACK:
            print("Received unexpected packet type from slave")
            return

    def __del__(self):
        self.pi.stop()
        self.pi.i2c_close(self.bus)
        pass

# Exemple d'utilisation / tests
br = Broue()
br.actuate_valve(56, 1)
br.read_sensor(42)
br.send_hello()