import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QPushButton, QLabel, QVBoxLayout
from UI.Gui import Ui_MainWindow
from PyQt5.QtCore import pyqtSignal
import QT_Thread
from serial import Serial
import serial.tools.list_ports


class ShowGraph(QMainWindow, Ui_MainWindow):
    updated_text_signal = pyqtSignal(str, name="my_signal")

    def __init__(self):
        super().__init__()
        self.baud_li = [1200, 4800, 9600, 38400, 57600, 115200, 230400, 460800, 921600]
        self.data_type_li = ["16进制", "10进制", "ascii"]
        self.switch_flag = 1
        self.serial = None
        self.init_ui()

    def init_ui(self):
        self.setupUi(self)
        for i in self.data_type_li:
            self.serial_data_type.insertItem(1, i)
        self.serial_data_type.setCurrentText("10进制")
        self.crc_verify_box.insertItem(1, "CRC校验")
        self.crc_verify_box.insertItem(1, "None")
        self.crc_verify_box.setCurrentText("None")
        self.init_ser_port()
        self.bind_func()

    def bind_func(self):
        self.updated_text_signal.connect(self.set_str_to_text)  # 信号
        self.conn_ser_btn.clicked.connect(self.conn_ser)  # 连接串口
        self.clear_text_btn.clicked.connect(self.clear_text)  # 清空文本框
        self.refresh_btn.clicked.connect(self.init_ser_port)

    def init_ser_port(self):
        self.port_box.clear()
        port_li = list(serial.tools.list_ports.comports())
        for i in port_li:
            # print(i)
            self.port_box.insertItem(1, (str(i).split("("))[0])
        for j in self.baud_li:
            self.buad_box.insertItem(-1, str(j))
        self.buad_box.setCurrentText("115200")

    def conn_ser(self):
        if self.switch_flag == 1:
            port = self.port_box.currentText()
            port = (str(port).split(" "))[0]
            baud = int(self.buad_box.currentText())
            self.serial = Serial(port, baud, timeout=0.5)
            if self.serial is not None:
                self.switch_flag += 1
                self.conn_ser_btn.setText("关闭串口")
            else:
                return
        elif self.switch_flag == 2:
            self.switch_flag = 1
            self.serial.close()
            self.serial = None
            self.conn_ser_btn.setText("连接串口")

    def clear_text(self):
        self.textBrowser.clear()

    def set_str_to_text(self, data):
        self.textBrowser.append(data)

    def my_thread(self):
        pass

    def serial_thread(self):
        pass

if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = ShowGraph()
    w.show()
    sys.exit(app.exec_())
