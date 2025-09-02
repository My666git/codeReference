#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "stdbool.h"
#include <freertos/task.h>

// 定义缓冲区大小
#define CIRCULAR_BUFFER_SIZE 2048

// 环形缓冲区结构体
typedef struct {
    uint8_t buffer[CIRCULAR_BUFFER_SIZE]; // 静态缓冲区
    size_t capacity;      // 总容量（等于数组大小）
    size_t head;          // 写入位置（索引）
    size_t tail;          // 读取位置（索引）
    size_t count;         // 当前数据量（字节）
    SemaphoreHandle_t mutex; // 互斥锁
} CircularBuffer;

extern CircularBuffer cc1101_rxBuffer;

/**
 * @brief 初始化环形缓冲区
 * 
 * @param cb 缓冲区指针
 */
void circular_buffer_init(CircularBuffer* cb);

/**
 * @brief 销毁缓冲区资源（主要是互斥锁）
 * 
 * @param cb 缓冲区指针
 */
void circular_buffer_destroy(CircularBuffer* cb);

/**
 * @brief 向缓冲区写入数据（线程安全）
 * 
 * @param cb 缓冲区指针
 * @param data 数据指针
 * @param length 数据长度
 * @return size_t 实际写入的数据量（当数据过长时会被截断）
 */
size_t circular_buffer_write(CircularBuffer* cb, const uint8_t* data, size_t length);

/**
 * @brief 从缓冲区读取数据（线程安全）
 * 
 * @param cb 缓冲区指针
 * @param output 输出缓冲区指针
 * @param max_length 输出缓冲区最大长度
 * @return size_t 实际读取的数据量
 */
size_t circular_buffer_read(CircularBuffer* cb, uint8_t* output, size_t max_length);
/**
 * @brief 查看缓冲区数据（不取出数据）
 * 
 * @param cb 缓冲区指针
 * @param output 输出缓冲区
 * @param max_length 最大查看长度
 * @return size_t 实际查看的数据量
 */
size_t circular_buffer_peek(CircularBuffer* cb, uint8_t* output, size_t max_length);

/**
 * @brief 清除缓冲区所有数据
 * 
 * @param cb 缓冲区指针
 */
void circular_buffer_clear(CircularBuffer* cb);