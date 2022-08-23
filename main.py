from ctypes import cdll

lib = cdll.LoadLibrary('.\\dll\\serial.dll')
print(lib.add(1, 2))
