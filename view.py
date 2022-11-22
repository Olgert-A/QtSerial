from PyQt6.QtWidgets import QMainWindow, QListWidget, QPushButton, QVBoxLayout, QWidget, QLineEdit
from data import SerialData


class MainView(QMainWindow):
    def __init__(self):
        super().__init__()

        self.serial = SerialData()

        self.read_list = QListWidget()
        self.write_data = QLineEdit()
        self.write_btn = QPushButton('Write')

        self.write_btn.pressed.connect(self.write_btn_pressed)

        layout = QVBoxLayout()
        layout.addWidget(self.read_list)
        layout.addWidget(self.write_data)
        layout.addWidget(self.write_btn)

        main_widget = QWidget()
        main_widget.setLayout(layout)
        self.setCentralWidget(main_widget)

    def read_event(self, data):
        formatted_data = ' '.join([f'{byte:02X}' for byte in data])
        self.read_list.addItem(f'Received {len(data)}: {formatted_data}')

    def write_btn_pressed(self):
        packet = '02 43 B0 01 03 F2'
        bts = [int(b, 16) for b in packet.split()]
        self.serial.write(bytes(bts))


