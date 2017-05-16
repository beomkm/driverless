Lidar_exe : LidarManager.o test.o
	g++ -o Lidar_exe LidarManager.o test.o -lLMS1xx -L/usr/local/lib

LidarManager.o : LidarManager.cpp
	g++ -c -o LidarManager.o LidarManager.cpp -lLMS1xx -L/usr/local/lib

test.o : test2.cpp
	g++ -c -o test.o test2.cpp -lLMS1xx -L/usr/local/lib
