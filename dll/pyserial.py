import ctypes
from ctypes import cdll, CFUNCTYPE


class Serial(object):
    def __init__(self):
        self.lib = cdll.LoadLibrary('.\\dll\\serial.dll')
        self.lib.newCSerial.argtypes = []
        self.lib.newCSerial.restype = ctypes.c_void_p

        self.lib.delCSerial.argtypes = [ctypes.c_void_p]
        self.lib.delCSerial.restype = None

        self.lib.serialRead.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
        self.lib.serialRead.restype = None

        self.obj = self.lib.newCSerial()

    def read(self, callback):
        self.lib.serialRead(self.obj, callback)

    def __del__(self):
        if self.lib and self.obj:
            self.lib.delCSerial(self.obj)
