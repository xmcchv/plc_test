#include "plc_reader.h"
#include <iostream>
#include <chrono>

PLCBlockReader::PLCBlockReader(TS7Client* client, int maxThreads) 
    : client_(client), maxThreads_(maxThreads) {}

PLCBlockReader::~PLCBlockReader() {
    stopReading();
}

void PLCBlockReader::addReadBlock(int dbNumber, int startByte, int size) {
    ReadBlock block;
    block.dbNumber = dbNumber;
    block.startByte = startByte;
    block.size = size;
    block.buffer.resize(size);
    readBlocks_.push_back(block);
}

void PLCBlockReader::startReading() {
    if (running_) return;
    
    running_ = true;
    for (int i = 0; i < maxThreads_; ++i) {
        readerThreads_.emplace_back(&PLCBlockReader::readerThreadFunc, this);
    }
}

void PLCBlockReader::stopReading() {
    running_ = false;
    cv_.notify_all();
    
    for (auto& thread : readerThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    readerThreads_.clear();
}

void PLCBlockReader::readerThreadFunc() {
    while (running_) {
        ReadBlock* blockToRead = nullptr;
        
        // 查找未完成的读取块
        {
            std::unique_lock<std::mutex> lock(dataMutex_);
            for (auto& block : readBlocks_) {
                if (block.buffer.empty()) { // 简单判断是否已读取
                    blockToRead = &block;
                    break;
                }
            }
        }

        if (blockToRead) {
            int result = readPLCBlock(*blockToRead);
            if (result != 0) {
                std::cerr << "Failed to read block DB" << blockToRead->dbNumber 
                          << " at " << blockToRead->startByte << std::endl;
            }
        } else {
            // 所有块都已读取，等待下一轮
            std::unique_lock<std::mutex> lock(dataMutex_);
            cv_.wait_for(lock, std::chrono::milliseconds(100), [this]() {
                for (const auto& block : readBlocks_) {
                    if (block.buffer.empty()) return true;
                }
                return !running_;
            });
        }
    }
}

int PLCBlockReader::readPLCBlock(ReadBlock& block) {
    auto Start = std::chrono::high_resolution_clock::now();
    std::unique_lock<std::mutex> clientLock(clientMutex_);
    
    if (!client_->Connected()) {
        std::cerr << "PLC not connected!" << std::endl;
        return -1;
    }

    int result = client_->DBRead(block.dbNumber, block.startByte, block.size, block.buffer.data());
    clientLock.unlock();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - Start);
    std::cout<<"read block" << block.startByte<<"-"<< block.startByte+block.size<<" scan time: " << duration.count() << " ms" << std::endl;

    if (result == 0) {
        cv_.notify_all(); // 通知其他线程数据已更新
    }
    
    return result;
}

bool PLCBlockReader::getBoolValue(int dbNumber, int byteOffset, int bitOffset) {
    std::unique_lock<std::mutex> lock(dataMutex_);
    for (const auto& block : readBlocks_) {
        if (block.dbNumber == dbNumber && 
            byteOffset >= block.startByte && 
            byteOffset < block.startByte + block.size) {
            int relativeOffset = byteOffset - block.startByte;
            return (block.buffer[relativeOffset] >> bitOffset) & 0x01;
        }
    }
    return false;
}

int16_t PLCBlockReader::getInt16Value(int dbNumber, int byteOffset) {
    std::unique_lock<std::mutex> lock(dataMutex_);
    for (const auto& block : readBlocks_) {
        if (block.dbNumber == dbNumber && 
            byteOffset >= block.startByte && 
            byteOffset + 2 <= block.startByte + block.size) {
            int relativeOffset = byteOffset - block.startByte;
            return (static_cast<int16_t>(block.buffer[relativeOffset]) << 8) | 
                   block.buffer[relativeOffset+1];
        }
    }
    return 0;
}

int32_t PLCBlockReader::getDIntValue(int dbNumber, int byteOffset) {
    std::unique_lock<std::mutex> lock(dataMutex_);
    for (const auto& block : readBlocks_) {
        if (block.dbNumber == dbNumber && 
            byteOffset >= block.startByte && 
            byteOffset + 4 <= block.startByte + block.size) {
            int relativeOffset = byteOffset - block.startByte;
            return (static_cast<int32_t>(block.buffer[relativeOffset]) << 24) |
                   (static_cast<int32_t>(block.buffer[relativeOffset+1]) << 16) |
                   (static_cast<int32_t>(block.buffer[relativeOffset+2]) << 8) |
                   block.buffer[relativeOffset+3];
        }
    }
    return 0;
}

float PLCBlockReader::getFloatValue(int dbNumber, int byteOffset) {
    std::unique_lock<std::mutex> lock(dataMutex_);
    for (const auto& block : readBlocks_) {
        if (block.dbNumber == dbNumber && 
            byteOffset >= block.startByte && 
            byteOffset + 4 <= block.startByte + block.size) {
            int relativeOffset = byteOffset - block.startByte;
            uint32_t val = (static_cast<uint32_t>(block.buffer[relativeOffset]) << 24) |
                           (static_cast<uint32_t>(block.buffer[relativeOffset+1]) << 16) |
                           (static_cast<uint32_t>(block.buffer[relativeOffset+2]) << 8) |
                           block.buffer[relativeOffset+3];
            return *reinterpret_cast<float*>(&val);
        }
    }
    return 0.0f;
}