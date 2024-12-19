from PyQt5.Qt import QThread
import ctypes
import win32con
from PyQt5.QtCore import pyqtSignal


class MyThread(QThread):
    thread_finished = pyqtSignal()  # 线程完成信号

    def __init__(self, fun):
        super(MyThread, self).__init__()
        self.handle = None
        self.func = fun
        self.working = True

    def run(self):
        try:
            self.handle = ctypes.windll.kernel32.OpenThread(
                win32con.PROCESS_ALL_ACCESS, False, int(QThread.currentThreadId()))
        except Exception as e:
            print('获取线程句柄失败', e)

        print('开启线程 ID:', self.handle)

        # 运行函数
        self.func()  # 传入当前线程对象

        self.thread_finished.emit()  # 发送线程完成信号

    def stop(self):
        print('终止线程 ID:', self.handle)
        # 更改线程状态以请求停止
        self.working = False
        self.wait()
        self.quit()  # 请求线程退出

    def is_running(self):
        return self.working