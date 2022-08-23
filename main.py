import ctypes
from ctypes import cdll


class Serial(object):
    def __init__(self):
        self.lib = cdll.LoadLibrary('.\\dll\\serial.dll')
        self.lib.newCSerial.argtypes = []
        self.lib.newCSerial.restype = ctypes.c_void_p

        self.lib.serialAdd.argtypes = [ctypes.c_void_p, ctypes.c_char, ctypes.c_char]
        self.lib.serialAdd.restype = ctypes.c_char

        self.obj = self.lib.newCSerial()

    def add(self, a, b):
        return self.lib.serialAdd(self.obj, a, b)

    def __del__(self):
        self.lib.delCSerial()


serial = Serial()
val = serial.add(251, 5)
print(val)

