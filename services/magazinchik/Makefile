
CXX = g++
CXXFLAGS = -std=c++17 -m64 -fpermissive -g

SOURCES = magazinchik.cpp framework/router/router.cpp framework/template/template.cpp framework/server.cpp framework/parser/parser.cpp task/user/user.cpp task/authorization/auth.cpp task/orders/orders.cpp
OBJECTS = $(SOURCES:.cpp=.o)

EXEC = magazinchik

all: $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(EXEC)

.PHONY: all clean