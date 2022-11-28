import ctypes
from dll.pyserial import Serial


class SerialData:
    def __init__(self, view_callback):
        self.read_buffer = []
        self.collecting = False  # show that cycled measurement collecting was started
        self.measurements = []
        self.serial = Serial()
        self.view_callback = view_callback
        # init callback functype and set
        cb_type = ctypes.CFUNCTYPE(None, ctypes.c_char_p)
        self.callback = cb_type(self.read_event)
        # init serial
        opened = self.serial.open(2, 115200, self.callback)

    def __del__(self):
        if self.serial:
            self.serial.close()

    def read_event(self, data):
        if data == b'':
            data = b'\x00'
        self.view_callback(data)
        self.read_buffer.append(data)

        if self.collecting:
            if self.is_meas_packet():
                self.measurements.append(self.get_meas())
                self.read_buffer.clear()
                self.send_meas_request()

    def write(self, data: bytes):
        if data:
            self.serial.write(data)

    def start_collecting(self):
        self.read_buffer.clear()
        self.measurements.clear()
        self.collecting = True
        self.send_meas_request()

    def stop_collecting(self):
        self.collecting = False

    def get_collected(self):
        return self.measurements

    def send_meas_request(self):
        packet = '02 43 B0 01 03 F2'
        bts = [int(b, 16) for b in packet.split()]
        self.serial.write(bytes(bts))

    def is_meas_packet(self):
        if len(self.read_buffer) == 6:
            if self.check_meas_cmd() and self.check_xor():
                return True
        return False

    def check_meas_cmd(self):
        data = self.read_buffer[:6]
        return ord(data[0]) == 2 and ord(data[1]) == 6 and ord(data[4]) == 3

    def check_xor(self):
        data = self.read_buffer[:6]
        return ord(data[1]) ^ ord(data[2]) ^ ord(data[3]) == ord(data[5])

    def get_meas(self):
        packet = self.read_buffer[:6]
        data_bytes = b''.join(packet[2:4])
        meas = int.from_bytes(data_bytes, byteorder='big')
        return self.convert_to_len(meas)

    def convert_to_len(self, meas):
        if meas <= int('0x1388', 16):
            return meas
        if meas >= int('0xEC78', 16):
            return meas - int('0xFFFF', 16) - 1
