#include <iostream>
#include <chrono>
#include <thread>
#include <snap7.h>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "plc_reader.h"

int main() {
    TS7Client client;
    if (client.ConnectTo("192.168.100.181", 0, 1) != 0) {
        std::cerr << "Connection failed!" << std::endl;
        return 1;
    }

    // 创建读取器（使用4个线程）
    PLCBlockReader reader(&client, 4);
    int db_read_num = 16;

    // 添加要读取的块（DB16中的不同区域）
    reader.addReadBlock(16, 0, 100);    // 读取DB16的前100字节
    reader.addReadBlock(16, 100, 100);  // 读取DB16的100-199字节
    reader.addReadBlock(16, 200, 100);  // 读取DB16的200-299字节

    // 启动读取线程
    reader.startReading();

    // 主线程可以继续做其他工作
    while (true) {
        // 获取读取的值（线程安全）
        int16_t windSpeed = reader.getInt16Value(db_read_num, 124);
        bool windNormal = reader.getBoolValue(db_read_num, 53, 5);
        
        int16_t grabh = reader.getInt16Value(db_read_num, 58);  // 前加图型 抓斗高度
        int16_t grabv = reader.getInt16Value(db_read_num, 60);  //  抓斗速度
        int16_t carpos = reader.getInt16Value(db_read_num, 66);  // 前加图型 小车位置
        int16_t carv = reader.getInt16Value(db_read_num, 68);  //  小车速度
        int16_t trollypos = reader.getInt16Value(db_read_num, 70);  // 前加图型 大车位置
        int16_t trollyv = reader.getInt16Value(db_read_num, 72);  //  大车速度
        int16_t graboc = reader.getInt16Value(db_read_num, 62);  // 前加图型 抓斗行程
        int16_t grabpercent = reader.getInt16Value(db_read_num, 64);  //  抓斗速度

        std::cout << "Wind speed: " << windSpeed 
                  << ", Normal: " << windNormal 
                  << ", Grabh: " << grabh
                  << ", Grabv: " << grabv
                  << ", Carpos: " << carpos
                  << ", Carv: " << carv
                  << ", Trollypos: " << trollypos
                  << ", Trollyv: " << trollyv
                  << ", Graboc: " << graboc
                  << ", Grabpercent: " << grabpercent
                  << std::endl;
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    reader.stopReading();
    client.Disconnect();
    return 0;
}