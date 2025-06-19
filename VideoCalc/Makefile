# 项目根目录（Makefile所在位置）
PROJECT_ROOT := .

# 源代码目录
SRC_DIR := $(PROJECT_ROOT)/src

# 构建目录
BUILD_DIR := $(PROJECT_ROOT)/build

# 目标可执行文件
TARGET := $(PROJECT_ROOT)/VideoCalc

# 源代码文件
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# 对象文件（放在build目录下）
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# 编译器设置
CXX := g++
CXXFLAGS := -Wall -g -std=c++11
INCLUDES := -I$(PROJECT_ROOT)/inc

# 默认目标
all: $(TARGET)

# 链接目标
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# 编译规则
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# 创建build目录
$(BUILD_DIR):
	mkdir -p $@

# 清理规则
clean:
	rm -rf $(BUILD_DIR)

# 伪目标
.PHONY: all clean