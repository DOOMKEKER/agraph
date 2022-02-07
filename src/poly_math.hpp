#pragma once

#include <memory>
#include <vector>
#include <algorithm>

#include <opencv2/opencv.hpp>

using points_ar = std::vector<std::vector<cv::Point>>;
using RotRect = std::vector<cv::Point>;
using points_ar_ptr = std::shared_ptr<std::vector<cv::Point>>;
using points_ar_ptr_const = std::shared_ptr<const std::vector<cv::Point>>;

class InterPoint {
 public:
  InterPoint(cv::Point point, points_ar_ptr_const points_)
      : x0(point.x), y0(point.y), points(points_) {
    for (auto& x : *points) {
      den *= x0 - x.x;
    }
    den = y0 / den;
  }
  ~InterPoint();

  float calc(float x) {
    float num = 1;
    for (auto& x_n : *points) {
      num *= (x - x_n.x);
    }
    return num * den;
  }

 private:
  int x0;
  int y0;

  // think it's unnecessary here.
  points_ar_ptr_const points;

  float den = 1;
};

class Poly {
 public:
  explicit Poly(std::vector<cv::Point> &points_, float a, float b, float min, float max) {
    points = std::make_shared<std::vector<cv::Point>>(points_);
    n = points_.size();
    for (auto& point : *points) {
      poly_nodes.push_back(InterPoint(point, points));
    }
  }
  ~Poly() {}

  void normalize(float a_, float b_, float real_y_max, float real_y_min) {
    a = a_;
    b = b_;

    auto comp = [](cv::Point& l, cv::Point& r) { return l.x < r.x; };

    auto itr_max = std::max_element((*points).begin(), (*points).end(), comp);
    float pixel_max_y = (*itr_max).y;
    auto itr_min = std::min_element((*points).begin(), (*points).end(), comp);
    float pixel_min_y = (*itr_min).y;

    // is it C++...?
    float max_x = (*points)[0].x;
    float min_x = (*points).back().x;

    float ratio_x = (max_x - min_x) / (b - a);
    float ratio_y = (pixel_max_y - pixel_min_y) / (real_y_max - real_y_min);

    float x = a;
    float y;
    float pixel_min_x = (*points)[0].x;
    for (size_t i = 0; i < (*points).size(); i++) {
      x = (((*points)[i].x - pixel_min_x) * ratio_x) + a;
      y = (((*points)[i].y - pixel_min_y) * ratio_y) + real_y_min;
      (*normalized_points).push_back(cv::Point(x, y));
    }
  }

  float integral(void) {
    size_t size = (*normalized_points).size();
    std::vector<cv::Point> &pnts = *normalized_points;
    float sum = 0;
    for (size_t i = 0; i < size - 2; i+=2) {
      sum = pnts[i].y + pnts[i+1].y * 0.25 + pnts[i+2].y;
    }
    return sum * 1/3;
  }

 private:
  std::vector<InterPoint> poly_nodes;
  points_ar_ptr points;
  points_ar_ptr normalized_points;

  float a;  // start point on x axes
  float b;  // end point on x axes
  float n;  // number of partitions
  float step;
};
