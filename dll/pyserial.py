import ctypes
from ctypes import cdll, c_void_p, c_int, c_char_p, c_bool


class Serial(object):
    def __init__(self):
        self.lib = cdll.LoadLibrary('.\\dll\\serial.dll')
        self.init_methods()
        self.serial_object = self.lib.CreateSerial()

    def init_methods(self):
        self.lib.CreateSerial.argtypes = []
        self.lib.DeleteSerial.argtypes = [c_void_p]
        self.lib.SerialOpen.argtypes = [c_void_p, c_int, c_int, c_void_p]
        self.lib.SerialConnected.argtypes = [c_void_p]
        self.lib.SerialClose.argtypes = [c_void_p]
        self.lib.SerialWrite.argtypes = [c_void_p, c_char_p, c_int]

        self.lib.CreateSerial.restype = ctypes.c_void_p
        self.lib.DeleteSerial.restype = None
        self.lib.SerialOpen.restype = c_bool
        self.lib.SerialConnected.restype = c_bool
        self.lib.SerialClose.restype = None
        self.lib.SerialWrite.restype = c_int

    def open(self, port_number, baud_rate, callback) -> bool:
        return self.lib.SerialOpen(self.serial_object, port_number, baud_rate, callback)

    def connected(self) -> bool:
        return self.lib.SerialConnected(self.serial_object)

    def close(self) -> None:
        self.lib.SerialClose(self.serial_object)

    def write(self, data: bytes) -> int:
        return self.lib.SerialWrite(self.serial_object, data, len(data))

    def __del__(self):
        if self.lib and self.serial_object:
            self.lib.DeleteSerial(self.serial_object)
