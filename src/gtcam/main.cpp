#include <iostream>
#include <cstdio>
#include <opencv2/opencv.hpp>
#include <time.h>
#include "Vimba/VimbaCam.hpp"
#include "Vimba/FrameEvent.hpp"
#include "../common/UDSServer.hpp"

#include "LaneDetector.hpp"

#define _CRT_SECURE_NO_WARNINGS

#ifdef _DEBUG
#pragma comment (lib, "opencv_world320d.lib")
#else
#pragma comment (lib, "opencv_world320.lib")
#endif // _DEBUG

using namespace cv;

UDSServer server("/tmp/gtserver");

// Declare matrices
Mat frame, gpu_frame_output;
cuda::GpuMat gpu_frame_input;
// Vision indicator
vision::LaneDetector laneDetector;

// C.P measurement
char str[200];

// Time measurement
float begin, end;

// Data printing
std::ofstream out;
VideoWriter writer;

int onFrameReceived(Mat frame);

int main()
{


	//server.start();
	std::cout << "uds on" << std::endl;

	int key_pressed = 0;

	FrameEvent *fevent = FrameEvent::getInstance();
	fevent->callback = onFrameReceived;
	VimbaCam vimba;
	vimba.start();

	//VideoCapture capture("bin/output1832.avi");

	//if (!capture.isOpened()) {
	//	perror("Invalid video files");
	//	return -1;
	//}

	// Calculating Size of frame
	//double dWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	//double dHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	double dWidth = 1280.0;
	double dHeight = 960.0;

	std::cout << "Frame Size = " << dWidth << "x" << dHeight << std::endl;

	Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
	namedWindow("Processed Video", 0);
	resizeWindow("Processed Video",640,480);


	// Record
	writer.open("Hi1.avi", writer.fourcc('M', 'J', 'P', 'G'), 18, cv::Size(1280,960 ));

	//out.open("result.txt");


	VideoCapture vc0(0);
	if(!vc0.isOpened()) {
		printf("cannot open vc0!\n");	
		return 1;
	}
	VideoCapture vc1(1);
	if(!vc1.isOpened()) {
		printf("cannot open vc1!\n");	
		return 1;
	}

	Mat img0;
	Mat img1;
	
	namedWindow("cam0", WINDOW_NORMAL);
	namedWindow("cam1", WINDOW_NORMAL);
	resizeWindow("cam0", 400, 400);
	resizeWindow("cam1", 400, 400);
	
	while(1) {
		vc0 >> img0;
		vc1 >> img1;
		if(img0.empty()) continue;
		if(img1.empty()) continue;
		moveWindow("cam1", 0, 350);
		imshow("cam0", img0);
		imshow("cam1", img1);
		if(cvWaitKey(30) == 'q')
			break;
	}


	int lock;
	std::cin >> lock;

	server.close();
	vimba.stopAcquisition();
	server.close();
	//out.close();

	return 0;
}


int onFrameReceived(Mat frame)
{
	//capture >> frame;		//���� ����
	//cvtColor(frame2, frame, CV_BGR2RGB);
	//int nnn;
	//std::cin >> nnn;
	gpu_frame_input.upload(frame);
	begin = clock();
	auto iserror = 0;
	auto local = frame.clone();
	try {
		auto roadModel = laneDetector.DetectLane(local);
	}
	catch (int exception) {
		std::cout << "exception : " << exception << std::endl;
		iserror = 1;
	}
	if (iserror == 0) {
		auto center = laneDetector.FindingCenter();
		auto curvature = laneDetector.FindingCurvature();
		end = clock();
		//std::cout << "수행시간 : " << ((end - begin) / 1000) << "초" << std::endl;
		float t = 1 / ((end - begin) / 1000);

		sprintf(str, "Gap from center : %.3f", 640.0f - center);
		putText(local, str, Point2f(40, 60), FONT_HERSHEY_TRIPLEX, 1, Scalar(0, 255, 0));
		sprintf(str, "FPS : %.2f", 1000 * t);
		putText(local, str, Point2f(40, 100), FONT_HERSHEY_TRIPLEX, 1, Scalar(0, 255, 0));
		sprintf(str, "Curvature : %.3f", curvature);
		putText(local, str, Point2f(40, 140), FONT_HERSHEY_TRIPLEX, 1, Scalar(0, 255, 0));
		//out << float(640 - center);		// test renew
		//while (key_pressed != 32)
			//key_pressed = waitKey(0);
			server.sendFloat(640.0f - center);
	}


	writer.write(frame);
	imshow("Processed Video", local);

	return 0;
}
