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

double center;
double curvature;

using namespace Eigen;

namespace vision
{

LaneDetector::LaneDetector()
{

}

void LaneDetector::ComputePerspectiveTransformationMatrix(const int width, const int height)
{
	cv::Point2f src[] =
	{
		{ 0,       470 },							//top-left
		{ 1280,    470 },							//top -right
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
	cv::Rect roi_out(input.cols * 2 / 7 + 50, 0, input.cols * 3 / 7 - 10, input.rows);// ���ɿ��� ����
	cv::cuda::GpuMat RoI = road(roi_out);

	return RoI;
}

cv::Mat LaneDetector::RoIBack(const cv::cuda::GpuMat& base, const cv::Mat& input)
{
	cv::Mat road;
	base.download(road);
	cv::Mat background = cv::Mat::zeros(base.rows, base.cols, CV_8UC1);
	cv::Rect roi_out(road.cols * 2 / 7 + 50, 0, road.cols * 3 / 7 - 10, road.rows);// ���ɿ��� ����
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

	const int ROAD_WIDTH = 250;

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
		roadModel.current_lane_model_.left_ = roadModel.current_lane_left[1];
		roadModel.current_lane_model_.right_ = lane_model::Parabola(roadModel.current_lane_left[0].a, roadModel.current_lane_left[0].b, roadModel.current_lane_left[0].c + ROAD_WIDTH * 3 / 5);
		roadModel.current_lane_model_.center = lane_model::Parabola(roadModel.current_lane_left[0].a, roadModel.current_lane_left[0].b, roadModel.current_lane_left[0].c + ROAD_WIDTH * 3 / 10);
	}
	else if (roadModel.current_lane_left.size() == 0 && roadModel.current_lane_right.size() == 1)
	{
		roadModel.current_lane_model_.left_ = lane_model::Parabola(roadModel.current_lane_right[0].a, roadModel.current_lane_right[0].b, roadModel.current_lane_right[0].c - ROAD_WIDTH * 3 / 5);
		roadModel.current_lane_model_.center = lane_model::Parabola(roadModel.current_lane_right[0].a, roadModel.current_lane_right[0].b, roadModel.current_lane_right[0].c - ROAD_WIDTH * 3 / 10);
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

		if (x > 0 && x < 1280 && y > 200 && y < 740)
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
	double y2 = 300;

	y1 = (y1 + y2) / 2;

	double x0 = p(y0);
	double x1 = p(y1);
	double x2 = p(y2);

	if (x0 != x1 && x1 != x2) {
		MatrixXd A = MatrixXd(3, 3);
		A(0, 0) = x0;
		A(0, 1) = y0;
		A(0, 2) = 1;
		A(1, 0) = x1;
		A(1, 1) = y1;
		A(1, 2) = 1;
		A(2, 0) = x2;
		A(2, 1) = y2;
		A(2, 2) = 1;

		MatrixXd B = MatrixXd(3, 1);
		B(0, 0) = -(pow(x0, 2) + pow(y0, 2));
		B(1, 0) = -(pow(x1, 2) + pow(y1, 2));
		B(2, 0) = -(pow(x2, 2) + pow(y2, 2));

		MatrixXd C = MatrixXd(3, 1);
		C = A.inverse() * pow(x0, 2);

		double radius = sqrt(pow((0, 0), 2) + pow((1, 0), 2) - C(2, 0));
		curvature = 1 / radius;
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

		if (x > 0 && x < 1280 && y > 480 && y < 720)
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
		if (x > 0 && x < 1280 && y > 640 && y < 705)
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

	/////////////////////////////////////////
	cv::Mat background = cv::Mat::zeros(result.rows, result.cols, CV_8UC3);

	if (roadModel.current_lane_model_.valid)
	{
		DrawCenter(background, roadModel.current_lane_model_.center, { 255,255,255 });
		DrawCenter_line(background, roadModel.current_lane_model_.center, { 0,0,255 });
		Drawlane(background, roadModel.current_lane_model_.left_, { 0,255,0 });
		Drawlane(background, roadModel.current_lane_model_.right_, { 0,255,0 });
	}

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
