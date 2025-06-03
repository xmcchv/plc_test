#include "plc_reader.h"
#include <iostream>
#include <chrono>

PLCBlockReader::PLCBlockReader() {}

PLCBlockReader::~PLCBlockReader() {
    stopAll();
}


/**
 * @brief 添加一个新的读取块并启动专用线程来读取 PLC 数据。
 *
 * 此函数创建一个新的读取块，连接到指定 IP 地址的 PLC，
 * 并启动一个新线程来周期性地读取指定数据块中的数据。
 *
 * @param ipAdress PLC 的 IP 地址。
 * @param dbNumber PLC 中数据块的编号。
 * @param startByte 数据块中读取的起始字节位置。
 * @param size 要读取的字节数。
 */
void PLCBlockReader::addReadBlock(const char *ipAdress, int dbNumber, int startByte, int size) {
    // 锁定互斥锁，确保对 blocks_ 向量的线程安全访问
    std::lock_guard<std::mutex> lock(blocksMutex_);
    
    // 创建一个新的 ReadBlock 智能指针
    auto block = std::make_unique<ReadBlock>();
    // 设置数据块编号
    block->dbNumber = dbNumber;
    // 设置读取的起始字节位置
    block->startByte = startByte;
    // 设置要读取的字节数
    block->size = size;
    // 调整缓冲区大小以容纳读取的数据
    block->buffer.resize(size);
    // 标记该读取块的线程为运行状态
    block->running = true;
    
    // 使用 std::make_unique 创建 TS7Client 实例
    block->client_ = std::make_unique<TS7Client>(); 
    // 尝试连接到指定 IP 地址的 PLC
    if (block->client_->ConnectTo(ipAdress, 0, 1) == 0)
    {
        std::cout << "===============block->client connect success!!=====================" << std::endl;
    }
    else
    {
        std::cout << "===============block->client connect failed!!=======================" << std::endl;
    }

    // 启动专用线程
    block->thread = std::thread(&PLCBlockReader::blockReadingThread, this, std::ref(*block));
    // 分离线程让它自己一边读取plc数据去
    block->thread.detach();
    // 塞到blocks_里去，然后通过getxxxxx去获取数据
    blocks_.push_back(std::move(block));
}

/**
 * @brief 读取 PLC 数据块的线程函数，用于周期性地从 PLC 读取指定数据块的数据。
 *
 * 此函数在一个独立线程中运行，持续检查运行状态标志，
 * 若 PLC 已连接，则周期性地读取指定数据块的数据到缓冲区，
 * 若读取失败则输出错误信息，直到运行状态标志被设置为停止。
 *
 * @param block 引用一个 ReadBlock 结构体，包含要读取的数据块的相关信息。
 */
void PLCBlockReader::blockReadingThread(ReadBlock& block) {
    // 定义读取数据的时间间隔为 20 毫秒
    constexpr auto READ_INTERVAL = std::chrono::milliseconds(20);
    
    // 当该数据块的读取线程运行标志和全局运行标志都为真时，持续循环读取数据
    while (block.running && globalRunning_) {
        {
            // auto Start = std::chrono::high_resolution_clock::now();
            // std::lock_guard<std::mutex> clientLock(clientMutex_);
            
            // 检查 PLC 是否连接，如果未连接，输出错误信息并休眠 1 秒后继续下一次循环
            if (!block.client_->Connected()) {
                std::cerr << "PLC not connected! Block DB" << block.dbNumber 
                          << " start:" << block.startByte << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            
            // 调用 snap7 库的 DBRead 函数从 PLC 读取指定数据块的数据
            int result = block.client_->DBRead(
                block.dbNumber,  // 数据块编号
                block.startByte, // 起始字节位置
                block.size,      // 要读取的字节数
                block.buffer.data() // 存储读取数据的缓冲区指针
            );
            // auto end = std::chrono::high_resolution_clock::now();
            // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - Start);
            // std::cout<<"read block " << block.startByte<<"-"<< block.startByte+block.size<<" scan time: " << duration.count() << " ms" << std::endl;

            // 检查读取操作是否失败，如果失败，获取错误信息并输出
            if (result != 0) {
                char msg[1024];
                // 调用 Cli_ErrorText 函数获取错误信息
                Cli_ErrorText(result, msg, sizeof(msg));
                std::cerr << "[blockReadingThread] Read block DB" << block.dbNumber 
                          << " at " << block.startByte << " failed: " << msg << std::endl;
            }
        }
        
        // 线程休眠指定的读取间隔时间
        std::this_thread::sleep_for(READ_INTERVAL);
    }
    
    // 线程退出时，输出提示信息
    std::cout << "Thread for block DB" << block.dbNumber 
              << " start:" << block.startByte << " exiting." << std::endl;
}

void PLCBlockReader::stopAll() {
    globalRunning_ = false;
    
    std::lock_guard<std::mutex> lock(blocksMutex_);
    // 通知所有线程停止运行
    for (auto& block : blocks_) {
        block->running = false;
    }
    
    // 等待所有线程退出
    for (auto& block : blocks_) {
        if (block->thread.joinable()) {
            block->thread.join();
        }
    }
    // 清空blocks_
    blocks_.clear();
}

/**
 * @brief 从 PLC 数据块的指定位中获取布尔值。
 *
 * 此函数根据传入的位偏移量计算字节偏移量和位位置，
 * 接着遍历已注册的读取块，查找对应的 PLC 数据块。
 * 若找到该数据块且偏移量在其有效范围内，则提取指定位的值。
 *
 * @param dbNumber PLC 中数据块的编号。
 * @param Offset 数据块内的位偏移量。
 * @return bool 若指定位被置位则返回 true，若未找到对应数据块或偏移量超出范围则返回 false。
 */
bool PLCBlockReader::getBoolValue(int dbNumber, int Offset) {
    // 通过将位偏移量除以 8 计算字节偏移量（因为一个字节有 8 位）
    int byteOffset = Offset / 8;
    // 通过位偏移量对 8 取余计算字节内的位位置
    int bitOffset = Offset % 8;
    // 锁定互斥锁，确保对 blocks_ 向量的线程安全访问
    std::lock_guard<std::mutex> lock(blocksMutex_);
    
    // 遍历所有已注册的读取块
    for (const auto& block : blocks_) {
        // 检查当前块是否与指定的数据块编号匹配
        // 并且字节偏移量是否在该块的缓冲区范围内
        if (block->dbNumber == dbNumber && 
            byteOffset >= block->startByte && 
            byteOffset < block->startByte + block->size) {
            
            // 计算字节在块缓冲区中的相对偏移量
            int relOffset = byteOffset - block->startByte;
            // 将字节右移指定位数，再与 0x01 进行按位与运算
            // 提取指定的位，并将其作为布尔值返回
            return (block->buffer[relOffset] >> bitOffset) & 0x01;
        }
    }
    // 若未找到匹配的块或偏移量超出范围，返回 false
    return false;
}

bool PLCBlockReader::getBoolValue(int dbNumber, int byteOffset, int bitOffset) {
    std::lock_guard<std::mutex> lock(blocksMutex_);
    
    for (const auto& block : blocks_) {
        if (block->dbNumber == dbNumber && 
            byteOffset >= block->startByte && 
            byteOffset < block->startByte + block->size) {
            
            int relOffset = byteOffset - block->startByte;
            return (block->buffer[relOffset] >> bitOffset) & 0x01;
        }
    }
    return false;
}

/**
 * @brief 从 PLC 数据块的指定字节偏移位置获取一个 16 位有符号整数。
 *
 * 此函数遍历所有已注册的读取块，查找匹配的数据块编号，
 * 并检查指定的字节偏移位置是否在块的有效范围内。
 * 若找到匹配块且偏移位置有效，则从块的缓冲区中提取 16 位整数。
 *
 * @param dbNumber PLC 中数据块的编号。
 * @param byteOffset 数据块内的字节偏移位置。
 * @return int16_t 若找到匹配块且偏移有效，返回提取的 16 位有符号整数；否则返回 0。
 */
int16_t PLCBlockReader::getInt16Value(int dbNumber, int byteOffset) {
    // 锁定互斥锁，确保对 blocks_ 向量的线程安全访问
    std::lock_guard<std::mutex> lock(blocksMutex_);
    
    // 遍历所有已注册的读取块
    for (const auto& block : blocks_) {
        // 检查当前块是否与指定的数据块编号匹配
        // 并且字节偏移位置加上 2 （16 位整数占用 2 字节）是否在该块的缓冲区范围内
        if (block->dbNumber == dbNumber && 
            byteOffset >= block->startByte && 
            byteOffset + 2 <= block->startByte + block->size) {
            
            // 计算字节在块缓冲区中的相对偏移量
            int relOffset = byteOffset - block->startByte;
            // 从缓冲区中提取 2 个字节，组合成一个 16 位无符号整数
            // 第一个字节左移 8 位，然后与第二个字节按位或
            uint16_t value = (static_cast<uint16_t>(block->buffer[relOffset]) << 8) |
                             block->buffer[relOffset + 1];
            // 将 16 位无符号整数转换为 16 位有符号整数并返回
            return static_cast<int16_t>(value);
        }
    }
    // 若未找到匹配的块或偏移位置无效，返回 0
    return 0;
}

/**
 * @brief 从 PLC 数据块的指定字节偏移位置获取一个 32 位有符号整数。
 *
 * 此函数遍历所有已注册的读取块，查找匹配的数据块编号，
 * 并检查指定的字节偏移位置是否在块的有效范围内。
 * 若找到匹配块且偏移位置有效，则从块的缓冲区中提取 32 位整数。
 *
 * @param dbNumber PLC 中数据块的编号。
 * @param byteOffset 数据块内的字节偏移位置。
 * @return int32_t 若找到匹配块且偏移有效，返回提取的 32 位有符号整数；否则返回 0。
 */
int32_t PLCBlockReader::getDIntValue(int dbNumber, int byteOffset) {
    // 锁定互斥锁，确保对 blocks_ 向量的线程安全访问，防止多线程同时修改或读取数据
    std::lock_guard<std::mutex> lock(blocksMutex_);
    
    // 遍历所有已注册的读取块
    for (const auto& block : blocks_) {
        // 检查当前块是否与指定的数据块编号匹配
        // 并且字节偏移位置加上 4 （32 位整数占用 4 字节）是否在该块的缓冲区范围内
        if (block->dbNumber == dbNumber && 
            byteOffset >= block->startByte && 
            byteOffset + 4 <= block->startByte + block->size) {
            
            // 计算字节在块缓冲区中的相对偏移量
            int relOffset = byteOffset - block->startByte;
            // 从缓冲区中提取 4 个字节，组合成一个 32 位无符号整数
            // 第一个字节左移 24 位，第二个字节左移 16 位，第三个字节左移 8 位，
            // 然后与第四个字节按位或
            uint32_t value = (static_cast<uint32_t>(block->buffer[relOffset]) << 24) |
                            (static_cast<uint32_t>(block->buffer[relOffset + 1]) << 16) |
                            (static_cast<uint32_t>(block->buffer[relOffset + 2]) << 8) |
                            block->buffer[relOffset + 3];
            // 将 32 位无符号整数转换为 32 位有符号整数并返回
            return static_cast<int32_t>(value);
        }
    }
    // 若未找到匹配的块或偏移位置无效，返回 0
    return 0;
}

float PLCBlockReader::getFloatValue(int dbNumber, int byteOffset) {
    int32_t intValue = getDIntValue(dbNumber, byteOffset);
    float result =*(float*)&intValue;
    return result;
}

bool PLCBlockReader::isRunning(int index) const {
    if (index >= 0 && index < static_cast<int>(blocks_.size())) {
        return blocks_[index]->running;
    }
    return false;
}

size_t PLCBlockReader::blockCount() const {
    return blocks_.size();
}