from PyQt6.QtWidgets import QMainWindow, QListWidget, QPushButton, QHBoxLayout, QVBoxLayout, QWidget, QLineEdit, QPlainTextEdit
import pyqtgraph as pg
from data import SerialData


class MainView(QMainWindow):
    def __init__(self):
        super().__init__()

        self.serial = SerialData(self.read_event)

        self.read_list = QListWidget()
        self.write_data = QLineEdit()
        self.write_btn = QPushButton('Write')

        self.meas_btn = QPushButton('Start meas')
        self.graph = view = pg.PlotWidget()
        self.curve = view.plot(name="Line")

        self.write_btn.pressed.connect(self.write_btn_pressed)
        self.meas_btn.pressed.connect(self.meas_btn_pressed)

        row = QHBoxLayout()

        col1 = QVBoxLayout()
        col1.addWidget(self.read_list)
        col1.addWidget(self.write_data)
        col1.addWidget(self.write_btn)

        col2 = QVBoxLayout()
        col2.addWidget(self.meas_btn)
        col2.addWidget(self.graph)

        row.addLayout(col1)
        row.addLayout(col2)

        main_widget = QWidget()
        main_widget.setLayout(row)
        self.setCentralWidget(main_widget)

    def read_event(self, data):
        formatted_data = ' '.join([f'{byte:02X}' for byte in data])
        self.read_list.addItem(formatted_data)
        #self.read_list.addItem(f'Received {len(data)}: {formatted_data}')

    def write_btn_pressed(self):
        packet = self.write_data.text()
        #packet = '02 43 B0 01 03 F2'
        bts = [int(b, 16) for b in packet.split()]
        self.serial.write(bytes(bts))

    def meas_btn_pressed(self):
        # print(int('0x00', 16))
        # print(bytes([0]))
        if not self.serial.collecting:
            print('Start')
            self.meas_btn.setText("Stop")
            self.write_btn.setEnabled(False)
            self.serial.start_collecting()
        else:
            print('Stop')
            self.serial.stop_collecting()
            data = self.serial.get_collected()
            print(data)
            self.curve.setData(data)
            self.meas_btn.setText("Start")
            self.write_btn.setEnabled(True)
