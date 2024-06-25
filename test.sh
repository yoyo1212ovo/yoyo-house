#!/bin/bash
# 这是一个简单的Shell脚本示例

# 打印消息
echo "Hello, World!"

# 定义变量
NAME="John"
echo "Hello, $NAME!"

# 条件判断
if [ -d "/home" ]; then
  echo "目录 /home 存在"
else
  echo "目录 /home 不存在"
fi

# 循环
for i in {1..5}; do
  echo "循环第 $i 次"
done

# 函数
function greet {
  echo "Hello, $1!"
}
greet "Alice"
