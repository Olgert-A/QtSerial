import ctypes
from dll.pyserial import Serial


class SerialData:
    def __init__(self):
        # init callback functype and set
        cb_type = ctypes.CFUNCTYPE(None, ctypes.c_char_p)
        self.callback = cb_type(self.read_event)
        self.serial = Serial()
        opened = self.serial.open(4, 115200, self.callback)
        self.data = []
        print(opened)

    def __del__(self):
        if self.serial:
            self.serial.close()

    def read_event(self, data):
        self.data.append(data)
        print(self.data)
        self.check_packet()

    def write(self, data: bytes):
        if data:
            self.serial.write(data)

    def check_packet(self):
        if len(self.data) >= 6:
            packet = self.data[:6]
            print(packet)
            cmd_to_check = self.mask_packet(packet, [255, 255, 0, 0, 255, 0])
            if [2, 6, 0, 0, 3, 0] != cmd_to_check or not self.check_xor(packet):
                self.data = self.data[1:]
            else:
                data = b''.join(packet[2:4])
                data = int.from_bytes(data, byteorder='big')
                self.data = self.data[6:]
                print(data)


    def mask_packet(self, packet, mask):
        packet = b''.join(packet)
        mask = bytes(mask)
        return [a & b for a, b in zip(packet, mask)]

    def check_xor(self, data):
        return ord(data[1]) ^ ord(data[2]) ^ ord(data[3]) == ord(data[5])



