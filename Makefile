# 编译器
CXX = g++

# 编译选项
CXXFLAGS = -Wall -Wextra -std=c++11

# 链接选项
LDFLAGS = -lgpiod

# 目标二进制文件名
TARGET = ctrl

# 源文件目录
SRC_DIR = .

# 获取所有 .cpp 文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# 将 .cpp 文件转换为 .o 文件
OBJS = $(SRCS:.cpp=.o)

# 默认目标
all: $(TARGET)

# 生成目标二进制文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# 编译 .cpp 文件为 .o 文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

# 伪目标
.PHONY: all clean
