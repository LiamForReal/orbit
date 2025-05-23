import win32file
import win32pipe 
import sys
import os
import time
from PyQt6.QtCore import QSize, Qt
from PyQt6.QtGui import QDesktopServices
from PyQt6.QtCore import QUrl
from PyQt6.QtWidgets import QApplication, QMainWindow, QPushButton, QVBoxLayout, QHBoxLayout, QWidget, QLabel, QSpinBox, QLineEdit, QTextBrowser
from PyQt6.QtGui import QPixmap, QPalette, QColor

BYTES_TO_GET = 10240
BYTES_TO_SEND = 10240

class OrbitMainWindow(QMainWindow):
    def __init__(self, pipe) -> None:
        super().__init__()

        self.pipe = pipe

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


    def pipe_read(self, bytes_amount: int) -> str:
        while True:
            try:
                result, data = win32file.ReadFile(self.pipe, bytes_amount)
                print("result:" + str(result) + "\n")
                if result == 234 or result == 0: # Success
                    return data.decode()
            except BaseException as e:
                print(f"Read error: {e}")
                exit(1)
                #return self.pipe_read(bytes_amount)


    def pipe_write(self, data) -> None:
        try:
            win32file.WriteFile(self.pipe, data.encode())
        except BaseException as e:
            print(f"Write error: {e}")
            sys.exit(1)


class BrowserWindow(OrbitMainWindow):
    def __init__(self, pipe) -> None:
        super().__init__(pipe)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(10, 10, 10, 10)

        label = QLabel(self, text="ORBIT Sentinel Search 🛡️")
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

        search_button = QPushButton("🔍 Search", self)
        search_button.setMinimumSize(QSize(140, 60))
        search_button.setMaximumSize(QSize(180, 60))
        search_button.clicked.connect(self.on_search_button_clicked)

        self.html_renderer = QTextBrowser()
        self.html_renderer.setOpenExternalLinks(False)  # Prevents links from opening externally
        self.html_renderer.anchorClicked.connect(self.on_link_clicked)  # Handle link clicks

        self.html_renderer.setHtml("""
            <h2 style="color:rgb(190,120,255);">Welcome to ORBit's HTML Renderer</h2>
            <p>This is a <b>QTextBrowser</b> that supports <span style="color:rgb(255,180,100);">HTML rendering.</span></p>
            <ul>
                <li><i>Lists</i>, <b>Bold</b>, <u>Underline</u></li>
                <li>Colors and <span style="color:red;">styled text</span></li>
                <li>Images and <a href="https://www.google.com" style="color:lightblue;">links</a></li>
            </ul>
        """)
        
        self.html_renderer.setFixedSize(QSize(730,350))
        self.html_renderer.setStyleSheet("margin-left: 20px;")
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
        if len(query) == 0:
            query = "https://www.example.com"
        else:
            if not (query.startswith('https://www.') or query.startswith('http://www.')):
                query = "https://www." + query
            if query.count('.') <= 1:
                query = query + ".com"
        
        self.search_bar.setText(query)

        self.error_label.setText("")

        self.pipe_write(str(len(query)) + "," + query)
        
        char = ''
        length = ""
        while char != ',':
            char = self.pipe_read(1)
            if char != ',':
                length += char
        
        print(length)
        length = int(length)
        if length != 0:
            html = self.pipe_read(length)
            html = html.strip("\x00\r\n")
            if "<title>301 Moved Permanently</title>" not in html or "<TITLE>301 Moved</TITLE>" not in html:
                try:
                    html = html.encode("utf-8", "ignore").decode("utf-8")
                except UnicodeDecodeError:
                    print("Error: HTML encoding issue detected.")
                    html = "<p style='color:red;'>No content received.</p>"  # Reset to avoid crashes
                self.html_renderer.setHtml(html)
            else: 
                link = ""
                title = "<title>301 Moved Permanently</title>"
                
                # Check if "301 Moved" is present in the HTML
                if "<TITLE>301 Moved</TITLE>" in html:
                    title = "<TITLE>301 Moved</TITLE>"
                    
                    # Try to extract the link from the <A> tag
                    start = html.find("<A")
                    end = html.find("</A>") + 4  # Include the </A> tag
                    if start != -1 and end != -1:
                        link = html[start:end]
                        print("Link is: " + link)
                    else:
                        link = "<p style='color:red;'>No redirection link found.</p>"
                
                # Generate the HTML content with title and link
                self.html_renderer.setHtml("""
                <!DOCTYPE html>
                <html lang="en">
                <head>
                    <meta charset="UTF-8">
                    <meta name="viewport" content="width=device-width, initial-scale=1.0">
                    <title>""" + title + """</title>
                    <style>
                        body {
                            font-family: Arial, sans-serif;
                            background-color: #f7f7f7;
                            display: flex;
                            justify-content: center;
                            align-items: center;
                            height: 100vh;
                            margin: 0;
                        }
                        .container {
                            background-color: white;
                            padding: 40px;
                            border-radius: 8px;
                            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
                            text-align: center;
                        }
                        h1 {
                            color: #d9534f;
                            font-size: 36px;
                            margin-bottom: 20px;
                        }
                        p {
                            font-size: 18px;
                            color: #333;
                        }
                        .link-button {
                            display: inline-block;
                            margin-top: 20px;
                            padding: 10px 20px;
                            background-color: #007bff;
                            color: white;
                            text-decoration: none;
                            font-weight: bold;
                            border-radius: 5px;
                            transition: background-color 0.3s;
                        }
                        .link-button:hover {
                            background-color: #0056b3;
                        }
                    </style>
                </head>
                <body>
                    <div class="container">
                        <h1>""" + title +  """</h1>
                        <p>The page you requested has been moved to a new location.</p>
                        <p>""" + link + """</p>
                    </div>
                </body>
                </html>
                """)

        else:
            self.html_renderer.setHtml("<p style='color:red;'>No content received.</p>")

    def on_link_clicked(self, url: QUrl) -> None:
        """When a user clicks a link in QTextBrowser, update the search bar."""
        self.search_bar.setText(url.toString())


class IntialSettingsWindow(OrbitMainWindow):
    def __init__(self, pipe) -> None:
        super().__init__(pipe)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(10, 10, 10, 10)

        label = QLabel(self, text="Connection Initial Settings")
        label.setAlignment(Qt.AlignmentFlag.AlignHCenter)
        label.setStyleSheet("font-size: 30px")

        amount_of_nodes_to_open_label = QLabel(text="Amount Of Nodes To Open")
        amount_of_nodes_to_open_label.setAlignment(Qt.AlignmentFlag.AlignHCenter)

        self.nodes_to_open_spinbox = QSpinBox(self)
        self.nodes_to_open_spinbox.setMinimum(1)
        self.nodes_to_open_spinbox.setMaximum(10)
        self.nodes_to_open_spinbox.setValue(3)

        path_length_label = QLabel(self, text="Path Length")
        path_length_label.setAlignment(Qt.AlignmentFlag.AlignHCenter)

        self.path_length_spinbox = QSpinBox(self)
        self.path_length_spinbox.setMinimum(1)
        self.path_length_spinbox.setMaximum(10)
        self.path_length_spinbox.setValue(3)
        
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
        layout.addWidget(self.nodes_to_open_spinbox, alignment=Qt.AlignmentFlag.AlignCenter)
        layout.addSpacing(20)
        layout.addWidget(path_length_label, alignment=Qt.AlignmentFlag.AlignCenter)
        layout.addSpacing(10)
        layout.addWidget(self.path_length_spinbox, alignment=Qt.AlignmentFlag.AlignCenter)
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
        #TODO: check logic here
        self.error_label.setText("")
        self.pipe_write(str(self.nodes_to_open_spinbox.value()) + "," + str(self.path_length_spinbox.value()))
        result = self.pipe_read(1)
        print("the result is: " + result)
        if str(result) == "0":
            self.error_label.setText("Error: ...TBD...")
        elif str(result) == "1":
            self.browserWindow = BrowserWindow(self.pipe)
            self.browserWindow.show()
            self.hide()


class MainWindow(OrbitMainWindow):
    def __init__(self, pipe) -> None:
        super().__init__(pipe)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(10, 10, 10, 10)

        image_label = QLabel(self)
        pixmap = QPixmap('images/orbit_white.png').scaled(400, 400, Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation)
        image_label.setPixmap(pixmap)

        self.start_button = QPushButton("Start Browsing!")
        self.start_button.setMinimumSize(QSize(180, 80))
        self.start_button.clicked.connect(self.next_window)

        layout.addWidget(image_label, alignment=Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(self.start_button, alignment=Qt.AlignmentFlag.AlignCenter)

        main_widget = QWidget()
        main_widget.setLayout(layout)

        self.setCentralWidget(main_widget)

    
    def next_window(self, checked) -> None:
        self.intialSettingsWindow = IntialSettingsWindow(self.pipe)
        self.intialSettingsWindow.show()
        self.hide()

def main():
    if len(sys.argv) < 2:
        print("Error: Missing required argument (random number).")
        sys.exit(1)

    # Append the argument to the pipe name
    pipe_name = fr"\\.\pipe\orbitPipe_{sys.argv[1]}"
   

    pipe = win32pipe.CreateNamedPipe(
        pipe_name,
        win32pipe.PIPE_ACCESS_DUPLEX,  # Read & write access
        win32pipe.PIPE_TYPE_MESSAGE | win32pipe.PIPE_READMODE_MESSAGE | win32pipe.PIPE_WAIT,
        1,  # Max instances
        BYTES_TO_SEND,  # Output buffer size
        BYTES_TO_GET,  # Input buffer size
        0,  # Timeout (0 = default)
        None  # Default security
    )
    
    print("[+] Waiting for a C++ client to connect on pipe" + pipe_name + "...")
    
    win32pipe.ConnectNamedPipe(pipe, None)
    
    print("[+] C++ Client Connected!")
    
    app = QApplication(sys.argv)
    window = MainWindow(pipe)
    window.show()
    sys.exit(app.exec())

    win32file.CloseHandle(pipe)

    #[Errno 2] No such file or directory: 'C:\\Users\\Magshimim\\orbit\\qss\\themes\\theme.qss'
if __name__ == "__main__":
    #TODO: run here backend
    main()
