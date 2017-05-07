#pragma once
#include <opencv2/core/core.hpp>
#include <vector>

#include "LaneModels.hpp"

namespace vision
{

	struct CurrentLaneModel
	{
		CurrentLaneModel() : valid(false) {}

		lane_model::Parabola left_;
		lane_model::Parabola right_;
		lane_model::Parabola center;

		bool valid = false;
	};

	struct RoadModel
	{
		RoadModel() {}

		std::vector<lane_model::Parabola> lanes_left;
		std::vector<lane_model::Parabola> lanes_right;
		std::vector<lane_model::Parabola> current_lane_left;
		std::vector<lane_model::Parabola> current_lane_right;
		CurrentLaneModel current_lane_model_;

		cv::Mat invPerspTransform;
	};

	class LaneDetector
	{
	public:
		LaneDetector();
		RoadModel DetectLane(cv::Mat& inputFrame);
		double FindingCenter();
		double FindingCurvature();

	private:
		cv::Mat FindPixelsThatMayBelongToLane(const cv::cuda::GpuMat& input);
		cv::cuda::GpuMat GetRoadOnlyImage(const cv::cuda::GpuMat& input);
		cv::cuda::GpuMat SetRoI(const cv::cuda::GpuMat& input);
		cv::Mat RoIBack(const cv::cuda::GpuMat& base, const cv::Mat& input);
		void ComputePerspectiveTransformationMatrix(const int width, const int height);
		cv::Mat DownsampleImageByHalf(const cv::Mat& input);
		RoadModel BuildRoadModelFromPoints(const std::vector<cv::Point2f>& points);

		cv::Mat perspectiveTransform;
		cv::Mat invPerspectiveTransform;
	};


}   // namespace vision
