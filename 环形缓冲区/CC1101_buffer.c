#include "CC1101_buffer.h"


CircularBuffer cc1101_rxBuffer = {0};

/**
 * @brief 初始化环形缓冲区
 * 
 * @param cb 缓冲区指针
 */
void circular_buffer_init(CircularBuffer* cb) {
    cb->capacity = sizeof(cb->buffer); // 计算数组大小
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->mutex = xSemaphoreCreateMutex(); // 创建互斥锁
}

/**
 * @brief 销毁缓冲区资源（主要是互斥锁）
 * 
 * @param cb 缓冲区指针
 */
void circular_buffer_destroy(CircularBuffer* cb) {
    if(cb->mutex != NULL) {
        vSemaphoreDelete(cb->mutex);
        cb->mutex = NULL;
    }
}

/**
 * @brief 向缓冲区写入数据（线程安全）
 * 
 * @param cb 缓冲区指针
 * @param data 数据指针
 * @param length 数据长度
 * @return size_t 实际写入的数据量（当数据过长时会被截断）
 */
size_t circular_buffer_write(CircularBuffer* cb, const uint8_t* data, size_t length) 
{
    if(length == 0 || data == NULL || cb == NULL) 
        return 0;
    
    // 尝试获取互斥锁，等待100ms超时
    if(xSemaphoreTake(cb->mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return 0;
    }
    
    // 当数据超过缓冲区容量时，只保留最后的部分
    if(length > cb->capacity) {
        data += (length - cb->capacity);
        length = cb->capacity;
    }
    
    // 计算实际可写入的空间（考虑覆盖旧数据）
    size_t space_available = cb->capacity - cb->count;
    size_t actual_write = (length > space_available) ? space_available : length;
    
    if(actual_write == 0) {
        xSemaphoreGive(cb->mutex);
        return 0;
    }
    
    // 处理缓冲区环绕情况
    size_t space_to_end = cb->capacity - cb->head;
    
    if(actual_write <= space_to_end) {
        // 写入操作不需要环绕
        memcpy(&cb->buffer[cb->head], data, actual_write);
        cb->head = (cb->head + actual_write) % cb->capacity;
    } else {
        // 需要环绕处理
        memcpy(&cb->buffer[cb->head], data, space_to_end);
        memcpy(cb->buffer, data + space_to_end, actual_write - space_to_end);
        cb->head = actual_write - space_to_end;
    }
    
    // 更新数据计数
    cb->count += actual_write;
    
    // 如果覆盖了旧数据，调整读指针
    if(actual_write < length) {
        size_t overflow = length - actual_write;
        cb->tail = (cb->tail + overflow) % cb->capacity;
        cb->count = cb->capacity; // 此时缓冲区是满的
    }
    
    xSemaphoreGive(cb->mutex);
    return actual_write;
}

/**
 * @brief 从缓冲区读取数据（线程安全）
 * 
 * @param cb 缓冲区指针
 * @param output 输出缓冲区指针
 * @param max_length 输出缓冲区最大长度
 * @return size_t 实际读取的数据量
 */
size_t circular_buffer_read(CircularBuffer* cb, uint8_t* output, size_t max_length) {
    if(max_length == 0 || output == NULL || cb == NULL) 
        return 0;
    
    // 尝试获取互斥锁，等待100ms超时
    if(xSemaphoreTake(cb->mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return 0;
    }
    
    // 计算实际可读取的数据量
    size_t actual_read = (max_length < cb->count) ? max_length : cb->count;
    
    if(actual_read == 0) {
        xSemaphoreGive(cb->mutex);
        return 0;
    }
    
    // 处理缓冲区环绕情况
    size_t space_to_end = cb->capacity - cb->tail;
    
    if(actual_read <= space_to_end) {
        // 读取操作不需要环绕
        memcpy(output, &cb->buffer[cb->tail], actual_read);
        cb->tail += actual_read;
    } else {
        // 需要环绕处理
        memcpy(output, &cb->buffer[cb->tail], space_to_end);
        memcpy(output + space_to_end, cb->buffer, actual_read - space_to_end);
        cb->tail = actual_read - space_to_end;
    }
    
    // 确保读指针在有效范围内
    if(cb->tail >= cb->capacity) {
        cb->tail = 0;
    }
    
    // 更新数据计数
    cb->count -= actual_read;
    
    // 如果读完了所有数据，重置指针
    if(cb->count == 0) {
        cb->head = 0;
        cb->tail = 0;
    }
    
    xSemaphoreGive(cb->mutex);
    return actual_read;
}

/**
 * @brief 查看缓冲区数据（不取出数据）
 * 
 * @param cb 缓冲区指针
 * @param output 输出缓冲区
 * @param max_length 最大查看长度
 * @return size_t 实际查看的数据量
 */
size_t circular_buffer_peek(CircularBuffer* cb, uint8_t* output, size_t max_length) {
    if(max_length == 0 || output == NULL || cb == NULL) 
        return 0;
    
    if(xSemaphoreTake(cb->mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return 0;
    }
    
    size_t actual_peek = (max_length < cb->count) ? max_length : cb->count;
    
    size_t space_to_end = cb->capacity - cb->tail;
    
    if(actual_peek <= space_to_end) {
        memcpy(output, &cb->buffer[cb->tail], actual_peek);
    } else {
        memcpy(output, &cb->buffer[cb->tail], space_to_end);
        memcpy(output + space_to_end, cb->buffer, actual_peek - space_to_end);
    }
    
    xSemaphoreGive(cb->mutex);
    return actual_peek;
}

/**
 * @brief 清除缓冲区所有数据
 * 
 * @param cb 缓冲区指针
 */
void circular_buffer_clear(CircularBuffer* cb) {
    if(cb == NULL) 
        return;
    
    if(xSemaphoreTake(cb->mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return;
    }
    
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    
    xSemaphoreGive(cb->mutex);
}