#pragma once

#include <vector>
#include "LaneModels.hpp"
#include "RANSAC.hpp"

namespace vision
{

class PointsToLanesConverter
{
public:
	std::vector<lane_model::Parabola> Convert_left(std::vector<cv::Point2f> points)
	{
		std::vector<lane_model::Parabola> model;
		
		std::vector<cv::Point2f> left_points;

		float average_left = 0;
		float average = 0;
		int size = 1;
		bool standard = true;

		int pointSize = points.size() - 1;
		for (int k = pointSize; k > 0; k--)
		{

			float x = points[k].x;
			float y = points[k].y;
			
			if (x < 320)
			{
				if (standard == true)
				{
					standard = false;
					average_left = x;
					size = 1;
					k = pointSize;
				}

				if (abs(average_left - x) < 30)
				{
					average = average_left * size + x;
					size++;
					average_left = average / size;
					left_points.emplace_back(x, y);
				}
			}
		}

		if (left_points.size() > 50)
		{
			auto parabola_left = RANSAC_Parabola(RANSAC_ITERATIONS, RANSAC_MODEL_SIZE, static_cast<int>(RANSAC_INLINERS * left_points.size()), RANSAC_ERROR_THRESHOLD, left_points);
			model.emplace_back(parabola_left);
		}
				
		return model;
	}

	std::vector<lane_model::Parabola> Convert_right(std::vector<cv::Point2f> points)
	{
		std::vector<lane_model::Parabola> model;

		std::vector<cv::Point2f> right_points;

		float average_right = 0;
		float average = 0;
		int size = 0;
		bool standard = true;
		
		int pointSize = points.size() - 1;
		for (int k = pointSize; k > 0; k--)
		{

			float x = points[k].x;
			float y = points[k].y;

			if (x > 320)
			{
				if (standard == true)
				{
					standard = false;
					average_right = x;
					size = 1;
					k = pointSize;
				}

				if (abs(average_right - x) < 30)
				{
					average = average_right * size + x;
					size++;
					average_right = average / size;
					right_points.emplace_back(x, y);
				}
			}
		}

		if (right_points.size() > 50)
		{
			auto parabola_right = RANSAC_Parabola(RANSAC_ITERATIONS, RANSAC_MODEL_SIZE, static_cast<int>(RANSAC_INLINERS * right_points.size()), RANSAC_ERROR_THRESHOLD, right_points);
			model.emplace_back(parabola_right);
		}

		return model;
	}

private:
	const int RANSAC_ITERATIONS = 100;
	const int RANSAC_MODEL_SIZE = 3;
	const int RANSAC_ERROR_THRESHOLD = 30;
	const double RANSAC_INLINERS = 0.55;
};

} // namespace vision