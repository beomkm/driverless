CC = clang++


Lidar.elf : LidarManager.o test.o
	$(CC) -o Lidar.elf LidarManager.o test.o ../common/UDSServer.o -L/usr/local/lib -lLMS1xx 

LidarManager.o : LidarManager.cpp
	$(CC) -c -o LidarManager.o LidarManager.cpp -L/usr/local/lib -lLMS1xx 

test.o : test2.cpp
	$(CC) -c -o test.o test2.cpp -L/usr/local/lib -lLMS1xx 
