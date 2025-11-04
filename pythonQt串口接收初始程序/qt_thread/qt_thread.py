from PyQt5.QtCore import QThread, pyqtSignal
import ctypes
import win32con
import logging

class WorkerThread(QThread):
    thread_finished = pyqtSignal(object)  # 线程完成信号
    error_occurred = pyqtSignal(str)  # 错误信号

    def __init__(self, fun):
        super(WorkerThread, self).__init__()
        self.handle = None
        self.func = fun
        self.working = True

    def run(self):
        try:
            self.handle = ctypes.windll.kernel32.OpenThread(
                win32con.PROCESS_ALL_ACCESS, False, int(QThread.currentThreadId()))
            logging.info(f'开启线程 ID:{self.handle}', )
            # 运行函数
            self.func()
        except Exception as e:
            error_msg = f"线程执行出错, ID:{self.handle} - {str(e)}"
            logging.info(error_msg)
            self.working = False
            self.error_occurred.emit(error_msg)
        finally:
            self.working = False
            self.thread_finished.emit(False)

    def stop(self):
        if not self.isRunning():
            return

        logging.info(f'终止线程 ID:{self.handle}', )
        self.working = False
        self.requestInterruption()
        self.quit()
        if not self.wait(2000):  # 等待2秒
            logging.info("警告: 线程没有在指定时间内终止")
            self.terminate()

    def isRunning(self):
        return self.working

