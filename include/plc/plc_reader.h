#ifndef PLCBLOCKREADER_H
#define PLCBLOCKREADER_H

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <map>
#include <functional>
#include <iostream>
#include <iomanip>
#include "snap7.h"

class PLCBlockReader {
public:
    struct ReadBlock {
        int dbNumber;
        int startByte;
        int size;
        std::vector<uint8_t> buffer;
        std::atomic<bool> running{false};
        std::thread thread;
        std::unique_ptr<TS7Client> client_; 
    };

    PLCBlockReader();
    ~PLCBlockReader();

    // 添加读取块并启动专用线程
    void addReadBlock(const char *ipAdress, int dbNumber, int startByte, int size);
    
    // 停止所有读取线程
    void stopAll();
    
    // 数据访问方法
    bool getBoolValue(int dbNumber, int byteOffset, int bitOffset);
    bool getBoolValue(int dbNumber, int Offset);
    int16_t getInt16Value(int dbNumber, int byteOffset);
    int32_t getDIntValue(int dbNumber, int byteOffset);
    float getFloatValue(int dbNumber, int byteOffset);
    
    // 状态检查
    bool isRunning(int index) const;
    size_t blockCount() const;

private:
    // 线程执行函数
    void blockReadingThread(ReadBlock& block);
    
    // 字节序转换函数
    static int16_t swapBytes16(int16_t value);
    static int32_t swapBytes32(int32_t value);
    static float swapBytesFloat(float value);

    std::vector<std::unique_ptr<ReadBlock>> blocks_;
    std::mutex blocksMutex_;
    std::atomic<bool> globalRunning_{true};
};

#endif // PLCBLOCKREADER_H