CXX=g++
CXXFLAGS=-g -O0
TARGET=server

all: WONConnection.o SocketStream.o Packet.o
	$(CXX) main.cpp WONConnection.o SocketStream.o Packet.o -o $(TARGET)

WONConnection.o: WONConnection.cpp WONConnection.h
	$(CXX) $(CXXFLAGS) -c WONConnection.cpp
SocketStream.o: SocketStream.cpp SocketStream.h
	$(CXX) $(CXXFLAGS) -c SocketStream.cpp
Packet.o: Packet.cpp Packet.h
	$(CXX) $(CXXFLAGS) -c Packet.cpp
	
clean:
	rm -rf *.o
	rm -rf $(TARGET)
