#include <algorithm>
#include <iterator>
#include "LaneDetector.hpp"
#include "LaneModels.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/cudafilters.hpp>
#include "PointsToLanesConverter.hpp"
#include "LaneMarkDetector.hpp"
#include <Eigen/Dense>
#include <iostream>

#define PI 3.141592

double center;
double curvature;

using namespace Eigen;

namespace vision
{

LaneDetector::LaneDetector()
{

}

void Mymouse(int event, int x, int y, int flag, void* param)
{
	int startX, startY;
	switch (event) {
		case CV_EVENT_LBUTTONDOWN:
		std::cout << "Left button down X= " << x/2 << ", Y= " << y/2 << std::endl;
		startX = x;
		startY = y;
		break;
	}
}

void LaneDetector::ComputePerspectiveTransformationMatrix(const int width, const int height)
{
	cv::Point2f src[] =
	{
		{ 0,       730 },							//top-left
		{ 1280,    730 },							//top -right
		{ 0,       static_cast<float>(height) },	//bottom-left
		{ 1280 - 100,    static_cast<float>(height) }//bottom-right
	};

	const float offset = width / 4;
	const float wo = width - offset;

	cv::Point2f dst[] =								//re-mapping coordinates
	{
		{ (offset - 800),     0 },
		{ (wo + 900),         0 },
		{ (offset + 120), static_cast<float>(height) },
		{ (wo - 160 ),     static_cast<float>(height) }
	};

	perspectiveTransform    = cv::getPerspectiveTransform(src, dst);
	invPerspectiveTransform = cv::getPerspectiveTransform(dst, src);
}

cv::cuda::GpuMat LaneDetector::GetRoadOnlyImage(const cv::cuda::GpuMat& input)
{
	cv::cuda::GpuMat road_gpu;

	const int width = input.cols;
	const int height = input.rows;

	static bool inited = false;

	if (!inited)
	{
		ComputePerspectiveTransformationMatrix(width, height);
		inited = true;
	}

	cv::cuda::warpPerspective(input, road_gpu, perspectiveTransform, { width, height });

	return road_gpu;
}

cv::cuda::GpuMat LaneDetector::SetRoI(const cv::cuda::GpuMat& input)
{
	cv::cuda::GpuMat road(input.clone());
	cv::Rect roi_out(input.cols * 2 / 7 -150, 0, input.cols * 4 / 7 + 80, input.rows);// ���ɿ��� ����
	cv::cuda::GpuMat RoI = road(roi_out);

	return RoI;
}

cv::Mat LaneDetector::RoIBack(const cv::cuda::GpuMat& base, const cv::Mat& input)
{
	cv::Mat road;
	base.download(road);
	cv::Mat background = cv::Mat::zeros(base.rows, base.cols, CV_8UC1);
	cv::Rect roi_out(road.cols * 2 / 7 -150, 0, road.cols * 4 / 7 + 80, road.rows);// ���ɿ��� ����
	cv::Mat RoIMat = background(roi_out);
	cv::Mat mask = input;

	mask.copyTo(RoIMat, mask);

	return background;
}

cv::Mat LaneDetector::FindPixelsThatMayBelongToLane(const cv::cuda::GpuMat& input)
{

	cv::cuda::GpuMat road(input.clone());
	cv::cuda::GpuMat grayscale_gpu;
	cv::cuda::cvtColor(road, grayscale_gpu, CV_BGR2GRAY);	// gray scale
	cv::cuda::GpuMat hls_gpu;
	cv::cuda::cvtColor(road, hls_gpu, CV_BGR2HLS);			// hls scale

	cv::Mat hls;
	hls_gpu.download(hls);
	cv::Mat hsl_yellow_mask;
	cv::inRange(hls, cv::Scalar(20, 0, 100), cv::Scalar(30, 255, 255), hsl_yellow_mask);

	cv::Mat grayscale;
	grayscale_gpu.download(grayscale);

	cv::Mat white_gray;
	cv::addWeighted(grayscale, 1, hsl_yellow_mask, 1, 0, white_gray);

	cv::Mat out(cv::Size(grayscale.cols, grayscale.rows), CV_8U);

	LaneMarkDetector laneMarkDetector;

	laneMarkDetector.tau_ = 30;
	laneMarkDetector.verticalOffset_ = 150;
	laneMarkDetector.Process(white_gray, out);

	return out;
}

std::vector<cv::Point2f> ConvertImageToPoints(const cv::Mat& input)
{
	std::vector<cv::Point2f> output;

	unsigned char *raw = (unsigned char*)(input.data);

	int x = 0, y = 0;

	for (y = 0; y < input.rows; ++y)
	{
		const auto raw = input.ptr(y);
		for (x = 0; x < input.cols; ++x)
		{
			if (raw[x])
			{
				output.emplace_back(cv::Point2f(x, y));
			}
		}
	}

	return output;
}

void DetectCurrentLane(RoadModel& roadModel)
{
	std::copy_if(roadModel.lanes_left.begin(), roadModel.lanes_left.end(), std::back_inserter(roadModel.current_lane_left), [](const lane_model::Parabola& a) { return true; });
	std::copy_if(roadModel.lanes_right.begin(), roadModel.lanes_right.end(), std::back_inserter(roadModel.current_lane_right), [](const lane_model::Parabola& a) { return true; });
}

void BuildCurrentLaneModel(RoadModel& roadModel)
{
	roadModel.current_lane_model_.valid = false;

	const int ROAD_WIDTH = 350;

	if (roadModel.current_lane_left.size() == 1 && roadModel.current_lane_right.size() == 1)
	{
		roadModel.current_lane_model_.left_  = roadModel.current_lane_left[0];
		roadModel.current_lane_model_.right_ = roadModel.current_lane_right[0];
		roadModel.current_lane_model_.center = lane_model::Parabola((roadModel.current_lane_left[0].a + roadModel.current_lane_right[0].a) * 0.5,
			(roadModel.current_lane_left[0].b + roadModel.current_lane_right[0].b) * 0.5,
			(roadModel.current_lane_left[0].c + roadModel.current_lane_right[0].c) * 0.5);
	}
	else if (roadModel.current_lane_left.size() == 1 && roadModel.current_lane_right.size() == 0)
	{
		roadModel.current_lane_model_.left_ = roadModel.current_lane_left[0];
		roadModel.current_lane_model_.right_ = lane_model::Parabola(roadModel.current_lane_left[0].a, roadModel.current_lane_left[0].b, roadModel.current_lane_left[0].c + ROAD_WIDTH * 3 / 5 - 17);
		roadModel.current_lane_model_.center = lane_model::Parabola(roadModel.current_lane_left[0].a, roadModel.current_lane_left[0].b, roadModel.current_lane_left[0].c + ROAD_WIDTH * 3 / 10 - 34);
	}
	else if (roadModel.current_lane_left.size() == 0 && roadModel.current_lane_right.size() == 1)
	{
		roadModel.current_lane_model_.left_ = lane_model::Parabola(roadModel.current_lane_right[0].a, roadModel.current_lane_right[0].b, roadModel.current_lane_right[0].c - ROAD_WIDTH * 3 / 5 + 17);
		roadModel.current_lane_model_.center = lane_model::Parabola(roadModel.current_lane_right[0].a, roadModel.current_lane_right[0].b, roadModel.current_lane_right[0].c - ROAD_WIDTH * 3 / 10 + 34);
		roadModel.current_lane_model_.right_ = roadModel.current_lane_right[0];
	}
	else return;

	roadModel.current_lane_model_.valid = true;
}

void Drawlane(cv::Mat& image, const lane_model::Parabola& parabola, const cv::Vec3b color)
{
	std::vector<cv::Point2f> parabolaLine;

	auto p = lane_model::Parabola(parabola.a * 0.25 * 2, parabola.b * 0.5 * 2, parabola.c * 2); // upscale
	for (int y = 0; y < image.rows; ++y)
	{
		const auto x = p(y);

		int x_1 = x;
		cv::Vec3b* data = image.ptr<cv::Vec3b>(y);

		if (x > 0 && x < 1280 && y > 200 && y < 960)
		{
			data[x_1 - 3] = color;
			data[x_1 - 2] = color;
			data[x_1 - 1] = color;
			data[x_1] = color;
			data[x_1 + 1] = color;
			data[x_1 + 2] = color;
			data[x_1 + 3] = color;
		}
	}
}

void DrawCenter(cv::Mat& image, const lane_model::Parabola& parabola, const cv::Vec3b color)
{
	std::vector<cv::Point2f> parabolaLine;

	auto p = lane_model::Parabola(parabola.a * 0.25 * 2, parabola.b * 0.5 * 2, parabola.c * 2); // upscale
	double y0 = 720;
	double y1 = 0;
	double y2 = 100;

	y1 = (y1 + y2) / 2;

	double x0 = p(y0);
	double x1 = p(y1);
	double x2 = p(y2);

	float a = sqrt(pow((x0 - x1), 2) + pow((y0 - y1), 2));
	float b = sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
	float c = sqrt(pow((x2 - x0), 2) + pow((y2 - y0), 2));

	float cos_A = (pow(b, 2) + pow(c, 2) - pow(a, 2)) / (2 * b * c);
	float alpha = acos(cos_A);
	float sin_A = cos(alpha - PI / 2);

	if (x0 != x1 && x1 != x2) {
		double radius = a / (2 * sin_A);
		curvature = 1 / radius * 1000;
	}
	else {
		curvature = 0;
	}

	for (int y = 0; y < image.rows; ++y)
	{
		const auto x = p(y);
		center = x;

		int x_1 = x;
		cv::Vec3b* data = image.ptr<cv::Vec3b>(y);

		if (x > 0 && x < 1280 && y > 600 && y < 955)
		{
			data[x_1 - 2] = color;
			data[x_1 - 1] = color;
			data[x_1] = color;
			data[x_1 + 1] = color;
			data[x_1 + 2] = color;
		}
	}
}

void DrawCenter_line(cv::Mat& image, const lane_model::Parabola& parabola, const cv::Vec3b color)
{
	std::vector<cv::Point2f> parabolaLine;

	auto p = lane_model::Parabola(parabola.a * 0.25 * 2, parabola.b * 0.5 * 2, parabola.c * 2); // upscale
	for (int y = 0; y < image.rows; ++y)
	{
		double gap = 640 - center;
		const auto x = p(y) + gap;
		int x_1 = x;
		cv::Vec3b* data = image.ptr<cv::Vec3b>(y);
		if (x > 0 && x < 1280 && y > 800 && y < 940)
		{
			data[x_1-1] = color;
			data[x_1] = color;
			data[x_1+1] = color;
		}
	}


}

RoadModel LaneDetector::BuildRoadModelFromPoints(const std::vector<cv::Point2f>& points)
{
	RoadModel roadModel;

	PointsToLanesConverter pointsToLanesCovnerter;

	roadModel.lanes_left = pointsToLanesCovnerter.Convert_left(points);
	roadModel.lanes_right = pointsToLanesCovnerter.Convert_right(points);
	roadModel.invPerspTransform = invPerspectiveTransform;
	if (roadModel.lanes_left.size() == 0 && roadModel.lanes_right.size() == 0) throw 1;

	DetectCurrentLane(roadModel);
	BuildCurrentLaneModel(roadModel);

	return roadModel;
}

cv::Mat LaneDetector::DownsampleImageByHalf(const cv::Mat& input)
{
	cv::Mat minified;
	cv::resize(input, minified, cv::Size(input.cols / 2, input.rows / 2));
	return minified;
}

double LaneDetector::FindingCenter() {
	return center;
}

double LaneDetector::FindingCurvature() {
	return curvature;
}

RoadModel LaneDetector::DetectLane(cv::Mat& inputFrame)
{
	cv::cuda::GpuMat inputframe_gpu;
	inputframe_gpu.upload(inputFrame);
	auto road        = GetRoadOnlyImage(inputframe_gpu);
	auto region		 = SetRoI(road);
	auto lanesPixels = FindPixelsThatMayBelongToLane(region);
	auto result		 = RoIBack(road, lanesPixels);
	auto points      = ConvertImageToPoints(DownsampleImageByHalf(result));
	auto roadModel   = BuildRoadModelFromPoints(points);

	cv::Mat down;
	// lanesPixels.download(down);
	//cv::namedWindow("filter", 0);
	//cv::resizeWindow("filter",640,480);
	//cv::imshow("filter", lanesPixels);
	//cv::setMouseCallback("filter", Mymouse);
	/////////////////////////////////////////
	cv::Mat background = cv::Mat::zeros(result.rows, result.cols, CV_8UC3);

	if (roadModel.current_lane_model_.valid)
	{
		DrawCenter(background, roadModel.current_lane_model_.center, { 255,255,255 });
		DrawCenter_line(background, roadModel.current_lane_model_.center, { 0,0,255 });
		Drawlane(background, roadModel.current_lane_model_.left_, { 0,255,0 });
		Drawlane(background, roadModel.current_lane_model_.right_, { 0,255,0 });
	}
	//cv::imshow("filter1", background);
	cv::cuda::GpuMat background_gpu;;
	background_gpu.upload(background);

	//inputFrame = road;
	cv::cuda::GpuMat warped;
	cv::cuda::warpPerspective(background_gpu, warped, roadModel.invPerspTransform, inputFrame.size());

	//inputFrame = warped;
	cv::cuda::addWeighted(warped, 1, inputframe_gpu, 1, 0, inputframe_gpu);
	inputframe_gpu.download(inputFrame);
	///////////////////////////////////////
	return roadModel;
}
}  // namespace vision
