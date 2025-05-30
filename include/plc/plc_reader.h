#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <atomic>
#include <condition_variable>
#include "snap7.h"

class PLCBlockReader {
public:
    struct ReadBlock {
        int dbNumber;
        int startByte;
        int size;
        std::vector<uint8_t> buffer;
    };

    PLCBlockReader(TS7Client* client, int maxThreads = 4);
    ~PLCBlockReader();

    void addReadBlock(int dbNumber, int startByte, int size);
    void startReading();
    void stopReading();
    bool getBoolValue(int dbNumber, int byteOffset, int bitOffset);
    int16_t getInt16Value(int dbNumber, int byteOffset);
    int32_t getDIntValue(int dbNumber, int byteOffset);
    float getFloatValue(int dbNumber, int byteOffset);

private:
    void readerThreadFunc();
    int readPLCBlock(ReadBlock& block);
    
    TS7Client* client_;
    std::mutex clientMutex_;
    std::vector<ReadBlock> readBlocks_;
    std::vector<std::thread> readerThreads_;
    std::mutex dataMutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};
    int maxThreads_;
};