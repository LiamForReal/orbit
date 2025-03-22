import sys
import os
from PyQt6.QtCore import QSize, Qt
from PyQt6.QtWidgets import QApplication, QMainWindow, QPushButton, QVBoxLayout, QHBoxLayout, QWidget, QLabel, QSpinBox, QLineEdit, QTextBrowser
from PyQt6.QtGui import QPixmap, QPalette, QColor

class OrbitMainWindow(QMainWindow):
    def __init__(self) -> None:
        super().__init__()

        # Load the QSS stylesheet
        self.__qss_theme = self.resource_path(os.path.join('qss', 'themes', 'theme.qss'))
        with open(self.__qss_theme, 'r') as theme_file:
            theme = theme_file.read()
        self.setStyleSheet(theme)

        self.setWindowTitle("ORBit Web Browser")
        self.setFixedSize(QSize(800, 600))
    

    def resource_path(self, relative_path) -> str:
        """
        Get absolute path to resource, works for dev and for PyInstaller
        """
        try:
            base_path = sys._MEIPASS
        except Exception:
            base_path = os.path.abspath(".")

        return os.path.join(base_path, relative_path)


class BrowserWindow(OrbitMainWindow):
    def __init__(self) -> None:
        super().__init__()

        layout = QVBoxLayout(self)
        layout.setContentsMargins(10, 10, 10, 10)

        label = QLabel(self, text="ORBit Browser Search Bar")
        label.setAlignment(Qt.AlignmentFlag.AlignHCenter)
        label.setStyleSheet("font-size: 30px")

        self.search_bar = QLineEdit(placeholderText="https://www.example.com", parent=self)
        self.search_bar.setFixedSize(QSize(500, 60))
        self.search_bar.setAlignment(Qt.AlignmentFlag.AlignLeft | Qt.AlignmentFlag.AlignVCenter)
        pal = self.search_bar.palette()
        pal.setColor(QPalette.ColorRole.PlaceholderText, QColor("grey"))
        self.search_bar.setPalette(pal)
        self.search_bar.show()

        self.error_label = QLabel(self, text="")
        self.error_label.setStyleSheet("color: red; font-size: 16px; font-weight: bold;")
        self.error_label.setAlignment(Qt.AlignmentFlag.AlignLeft | Qt.AlignmentFlag.AlignVCenter)

        search_button = QPushButton("I> Search", self)
        search_button.setMinimumSize(QSize(140, 60))
        search_button.setMaximumSize(QSize(180, 60))
        search_button.clicked.connect(self.on_search_button_clicked)

        self.html_renderer = QTextBrowser()

        self.html_renderer.setHtml("""
            <h2 style="color:rgb(190,120,255);">Welcome to ORBit's HTML Renderer</h2>
            <p>This is a <b>QTextBrowser</b> that supports <span style="color:rgb(255,180,100);">HTML rendering.</span></p>
            <ul>
                <li><i>Lists</i>, <b>Bold</b>, <u>Underline</u></li>
                <li>Colors and <span style="color:red;">styled text</span></li>
                <li>Images and <a href="https://www.google.com" style="color:lightblue;">links</a></li>
            </ul>
        """)

        layout.addWidget(label)

        search_layout = QHBoxLayout()
        search_layout.addSpacing(20)
        search_layout.addWidget(self.search_bar, alignment=Qt.AlignmentFlag.AlignCenter)
        search_layout.addWidget(search_button, alignment=Qt.AlignmentFlag.AlignCenter)
        
        layout.addSpacing(30)
        layout.addLayout(search_layout)

        layout.addWidget(self.error_label, alignment=Qt.AlignmentFlag.AlignLeft | Qt.AlignmentFlag.AlignVCenter)

        layout.addWidget(self.html_renderer)
        layout.addStretch()

        main_widget = QWidget()
        main_widget.setLayout(layout)

        self.setCentralWidget(main_widget)


    def on_search_button_clicked(self, checked) -> None:
        query = self.search_bar.text()
        if query.startswith('https://www.') or query.startswith('http://www.'):
            print("Good")
            self.error_label.setText("")
        else:
            self.error_label.setText("Invalid URL query!")


class IntialSettingsWindow(OrbitMainWindow):
    def __init__(self) -> None:
        super().__init__()

        layout = QVBoxLayout(self)
        layout.setContentsMargins(10, 10, 10, 10)

        label = QLabel(self, text="Connection Initial Settings")
        label.setAlignment(Qt.AlignmentFlag.AlignHCenter)
        label.setStyleSheet("font-size: 30px")

        amount_of_nodes_to_open_label = QLabel(text="Amount Of Nodes To Open")
        amount_of_nodes_to_open_label.setAlignment(Qt.AlignmentFlag.AlignHCenter)

        nodes_to_open_spinbox = QSpinBox(self)
        nodes_to_open_spinbox.setMinimum(1)
        nodes_to_open_spinbox.setMaximum(10)
        nodes_to_open_spinbox.setValue(3)

        path_length_label = QLabel(self, text="Path Length")
        path_length_label.setAlignment(Qt.AlignmentFlag.AlignHCenter)

        path_length_spinbox = QSpinBox(self)
        path_length_spinbox.setMinimum(1)
        path_length_spinbox.setMaximum(10)
        path_length_spinbox.setValue(3)
        
        self.error_label = QLabel(self, text="")
        self.error_label.setStyleSheet("color: red; font-weight: bold;")


        connect_button = QPushButton(self, text="Connect To ORBit Network")
        connect_button.setMinimumSize(QSize(180, 80))
        connect_button.setMaximumSize(QSize(300, 80))
        connect_button.clicked.connect(self.next_window)

        layout.addWidget(label)
        layout.addSpacing(30)
        layout.addWidget(amount_of_nodes_to_open_label, alignment=Qt.AlignmentFlag.AlignCenter)
        layout.addSpacing(10)
        layout.addWidget(nodes_to_open_spinbox, alignment=Qt.AlignmentFlag.AlignCenter)
        layout.addSpacing(20)
        layout.addWidget(path_length_label, alignment=Qt.AlignmentFlag.AlignCenter)
        layout.addSpacing(10)
        layout.addWidget(path_length_spinbox, alignment=Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(self.error_label, alignment=Qt.AlignmentFlag.AlignCenter)
        layout.addSpacing(80)
        layout.addWidget(connect_button, alignment=Qt.AlignmentFlag.AlignCenter)
        layout.addStretch()

        main_widget = QWidget()
        main_widget.setLayout(layout)

        self.setCentralWidget(main_widget)


    def next_window(self, checked) -> None:
        # Firstly, send the input to the server for validation,
        # and for circuit opening and connection.
        #TODO: implement logic here
        error = False

        if error:
            self.error_label.setText("Error: ...TBD...")


        self.browserWindow = BrowserWindow()
        self.browserWindow.show()
        self.hide()


class MainWindow(OrbitMainWindow):
    def __init__(self) -> None:
        super().__init__()

        layout = QVBoxLayout(self)
        layout.setContentsMargins(10, 10, 10, 10)

        image_label = QLabel(self)
        pixmap = QPixmap('orbit_white.png').scaled(400, 400, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation)
        image_label.setPixmap(pixmap)

        start_button = QPushButton("Start Browsing!")
        start_button.setMinimumSize(QSize(180, 80))
        start_button.clicked.connect(self.next_window)

        layout.addWidget(image_label, alignment=Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(start_button, alignment=Qt.AlignmentFlag.AlignCenter)

        main_widget = QWidget()
        main_widget.setLayout(layout)

        self.setCentralWidget(main_widget)

    
    def next_window(self, checked) -> None:
        self.intialSettingsWindow = IntialSettingsWindow()
        self.intialSettingsWindow.show()
        self.hide()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
