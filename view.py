import ctypes
from PyQt6.QtWidgets import QMainWindow, QListWidget, QPushButton, QVBoxLayout, QWidget
from dll.pyserial import Serial
import ctypes


class MainView(QMainWindow):
    def __init__(self):
        super().__init__()

        #init callback functype and set
        cb_type = ctypes.CFUNCTYPE(None, ctypes.c_char_p)
        self.callback = cb_type(self.read_event)
        self.serial = None

        self.read_list = QListWidget()
        self.open_port = QPushButton('Open port')

        self.open_port.pressed.connect(self.open_port_pressed)

        layout = QVBoxLayout()
        layout.addWidget(self.open_port)
        layout.addWidget(self.read_list)

        main_widget = QWidget()
        main_widget.setLayout(layout)
        self.setCentralWidget(main_widget)

    def open_port_pressed(self):
        if self.serial is None:
            self.serial = Serial()

        print(self.serial.open(1, 115200, self.callback))
        #self.serial.read(self.callback)

    def read_event(self, data):
        formatted_data = ' '.join([f'{byte:02X}' for byte in data])
        self.read_list.addItem(f'Received {len(data)}: {formatted_data}')
