import sys
from PyQt6.QtWidgets import QApplication
from view import MainView

app = QApplication([])
view = MainView()
view.show()
sys.exit(app.exec())
