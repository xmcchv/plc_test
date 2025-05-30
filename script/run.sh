#!/bin/bash

# 获取脚本的目录
SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/../"; pwd)"
echo $SCRIPT_DIR $ROOT_DIR

cd $ROOT_DIR
if [ ! -d build ]; then
    mkdir build
fi
# 转到build目录
cd build

# 执行cmake并检查返回值
if cmake ..; then
    echo "cmake成功,继续执行..."
    
    if make; then
        echo "编译成功，继续执行..."

        # 获取第一个参数
        PARAM=$1
        # 根据参数的值执行不同的操作
        if [ "$PARAM" = "-d" ]; then
            echo "启动GDB模式..."
            gdb -ex run --args plctest_node
        elif [ "$PARAM" = "-n" ]; then
            echo "不启动程序..."
        else
            echo "直接运行..."
            ./plctest_node
        fi
    else
        echo "编译失败，停止执行!"
        exit 1
    fi
else
    echo "cmake失败，停止执行!"
    exit 1
fi