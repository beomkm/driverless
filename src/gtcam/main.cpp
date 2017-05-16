#include <iostream>
#include <cstdio>
#include <opencv2/opencv.hpp>
#include <time.h>
#include "../common/UDSServer.hpp"

#include "LaneDetector.hpp"

#define _CRT_SECURE_NO_WARNINGS

#ifdef _DEBUG
#pragma comment (lib, "opencv_world320d.lib")
#else
#pragma comment (lib, "opencv_world320.lib")
#endif // _DEBUG

using namespace cv;
int main()
{

	UDSServer server("/tmp/gtserver");
	server.start();
	std::cout << "uds on" << std::endl;

	int key_pressed = 0;

	VideoCapture capture("bin/tracking.mp4");

	if (!capture.isOpened()) {
		perror("Invalid video files");
		return -1;
	}

	// Calculating Size of frame
	double dWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

	std::cout << "Frame Size = " << dWidth << "x" << dHeight << std::endl;

	Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
	namedWindow("Processed Video", 0);
	resizeWindow("Processed Video",640,480);
	// Declare matrices
	Mat frame, gpu_frame_output;
	cuda::GpuMat gpu_frame_input;
	// Vision indicator
	vision::LaneDetector laneDetector;

	// Record
	//VideoWriter writer;
	//writer.open("Hi1.avi", writer.fourcc('M', 'J', 'P', 'G'), 25, cv::Size(1280, 720));

	// C.P measurement
	char str[200];

	// Time measurement
	float begin, end;

	// Data printing
	std::ofstream out;
	//out.open("result.txt");

while (key_pressed != 27) {
		capture >> frame;		//���� ����

		gpu_frame_input.upload(frame);
		begin = clock();
		auto local = frame.clone();
		auto roadModel = laneDetector.DetectLane(local);
		auto center = laneDetector.FindingCenter();
		auto curvature = laneDetector.FindingCurvature();
		end = clock();
		std::cout << "Elapsed time : " << ((end - begin) / 1000) << "ms" << std::endl;

		sprintf(str, "Gap from center : %.3f", 640.0f - center);
		putText(local, str, Point2f(40, 60), FONT_HERSHEY_TRIPLEX, 1, Scalar(0, 255, 0));
		sprintf(str, "Curvature : %.3f", curvature);
		putText(local, str, Point2f(40, 100), FONT_HERSHEY_TRIPLEX, 1, Scalar(0, 255, 0));
		//out << float(640 - center);

		//out << " ";

		server.sendFloat(640.0f - center);

		//writer.write(local);
		imshow("Processed Video", local);


		key_pressed = waitKey(25);
	}

	server.close();
	//out.close();
}
