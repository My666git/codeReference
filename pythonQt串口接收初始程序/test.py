import sys
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout
from PyQt5.QtCore import QTimer
import serial

class PlotWidget(QWidget):
    def __init__(self):
        super().__init__()
        
        # 创建 Matplotlib 图表
        self.figure, self.ax = plt.subplots()
        self.canvas = FigureCanvas(self.figure)
        
        # 初始化数据
        self.x = np.linspace(0, 10, 100)
        self.y = np.zeros(100)
        
        # 绘制折线图
        self.line, = self.ax.plot(self.x, self.y)
        self.ax.set_title('Dynamic Sine Wave')
        self.ax.set_xlabel('X axis')
        self.ax.set_ylabel('Y axis')
        
        # 布局
        layout = QVBoxLayout()
        layout.addWidget(self.canvas)
        self.setLayout(layout)
        
        self.setWindowTitle('PyQt5 with Matplotlib')
        
        # 设置串口
        self.ser = serial.Serial('COM3', 9600)  # 修改为你的串口端口和波特率
        
        # 设置定时器
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_plot)
        self.timer.start(1000)  # 每秒更新一次

    def update_plot(self):
        # 从串口读取数据
        if self.ser.in_waiting:
            data = self.ser.readline().decode('utf-8').strip()
            try:
                new_y = float(data)
                self.y = np.roll(self.y, -1)
                self.y[-1] = new_y
                self.line.set_ydata(self.y)
                self.canvas.draw()
            except ValueError:
                pass  # 忽略无效数据

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = PlotWidget()
    window.show()
    sys.exit(app.exec_())