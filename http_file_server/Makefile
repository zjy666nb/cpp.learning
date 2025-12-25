# 编译器和标志
CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -Wextra -O2
LDFLAGS = -lstdc++fs

# 目录结构
SRCDIR = src
OBJDIR = obj
BINDIR = bin
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/server

# 默认目标
all: $(TARGET)

# 链接可执行文件
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# 编译源文件
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 创建目录
$(BINDIR):
	mkdir -p $(BINDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

# 清理
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# 运行
run: $(TARGET)
	@echo "启动HTTP服务器..."
	@echo "访问: http://localhost:8080"
	@./$(TARGET)

# 调试版本
debug: CXXFLAGS += -g -DDEBUG
debug: clean all

# 静态分析
check:
	cppcheck --enable=all --suppress=missingIncludeSystem $(SRCDIR)

# 安装（如果需要）
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/cpp-http-server

.PHONY: all clean run debug check install