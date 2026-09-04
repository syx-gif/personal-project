PARSER=parser
DUG=debug
HTTP_SERVER=http_server
cc=g++
# 头文件搜索路径：cppjieba 分词库 + 系统级 jsoncpp
INCLUDE=-I./cppjieba/include -I/usr/local/include
# 库文件搜索路径（jsoncpp 若源码编译装在 /usr/local 时需要）
LIB=-L/usr/local/lib

.PHONY:all
all:$(PARSER) $(DUG) $(HTTP_SERVER)

$(PARSER):parser.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB) -lboost_system -lboost_filesystem -std=c++11
$(DUG):debug.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB) -ljsoncpp -std=c++11
$(HTTP_SERVER):http_server.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB) -ljsoncpp -lpthread -std=c++11
.PHONY:clean
clean:
	rm -f $(PARSER) $(DUG) $(HTTP_SERVER)
