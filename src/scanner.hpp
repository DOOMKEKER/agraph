#pragma once

#include <algorithm>
#include <opencv2/opencv.hpp>
#include <vector>

class Scanner {
 public:
  explicit Scanner(cv::Mat& img_) : img(img_) {
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gaus, cv::Size(5, 5), 2);
    cv::Canny(gaus, can, 25, 75);
    cv::dilate(can, dil, kernel);
  }

  void make_normilized_graph(cv::Mat const& horizontal_kernel,
                             cv::Mat const& vertical_kernel) {
    cv::findContours(wrap, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    poly1.resize(contours.size());

    RotRect Rect;
    get_rect(Rect, contours);

    reorder_pts(Rect);

    img_wrap(dil, Rect, wrap);

    del_line(wrap, horizontal_kernel);
    del_line(wrap, vertical_kernel);
  }

  std::vector<cv::Point> get_points(void) {
    cv::findContours(wrap, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    poly1.resize(contours.size());

    for (size_t i = 0; i < contours.size(); i++) {
      area = cv::contourArea(contours[i]);
      if (area < 50) continue;

      cv::approxPolyDP(contours[i], poly1[i], 0.2, false);
      cv::polylines(img, poly1[i], false, cv::Scalar(100, 100, 100));
    }

    std::vector<cv::Point> points;

    for (auto& vec : contours) {
      area = cv::contourArea(vec);
      if (area < 50) continue;
      std::cout << vec.size() << std::endl;
      points.insert(points.end(), vec.begin(), vec.end());
    }

    std::sort(points.begin(), points.end());

    std::vector<cv::Point> points2;

    points2.reserve(points.size() / 2);

    int j = points[0].x;
    int c = 0;
    for (size_t i = 0; i < points.size() - 1; ++i) {
      int a = points[i].x - j;
      if (a == 5) {
        points2.push_back(points[i]);
        j = points2.back().x;
      } else if (a > 5) {
        points2.push_back(points[i]);
        c = points2.back().x;
        j = c - (c % 5);
      }
    }
    return points2;
  }

  ~Scanner() {}

  void del_hv_lines(cv::Mat const& horizontal_kernel,
                    cv::Mat const& vertical_kernel) {
    del_line(wrap, horizontal_kernel);
    del_line(wrap, vertical_kernel);
  }

  void del_line(cv::Mat& input, cv::Mat const& kernel_) {
    cv::Mat detected_lines;
    points_ar cnts;

    cv::morphologyEx(input, detected_lines, cv::MORPH_OPEN, kernel_,
                     cv::Point(-1, -1), 2);
    cv::findContours(detected_lines, cnts, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);
    cv::drawContours(input, cnts, -1, cv::Scalar(0, 0, 0), 3);
  }

  void img_wrap(cv::Mat& dil, RotRect& Rect, cv::Mat& wrap) {
    cv::Point2f src[4] = {Rect[0], Rect[1], Rect[2], Rect[3]};
    cv::Point2f dst[4] = {
        {0.0f, 0.0f},
        {width, 0.0f},
        {0.0f, height},
        {width, height},
    };

    cv::Mat matrix = cv::getPerspectiveTransform(src, dst);
    cv::warpPerspective(dil, wrap, matrix, cv::Point(width, height));
  }

  void reorder_pts(RotRect& rect) {
    std::vector<int> sum, sub;
    RotRect buf_rect = rect;
    for (auto& pnt : rect) {
      sum.push_back(pnt.x + pnt.y);
      sub.push_back(pnt.x - pnt.y);
    }
    rect[0] = buf_rect[std::min_element(sum.begin(), sum.end()) - sum.begin()];
    rect[1] = buf_rect[std::max_element(sub.begin(), sub.end()) - sub.begin()];
    rect[2] = buf_rect[std::min_element(sub.begin(), sub.end()) - sub.begin()];
    rect[3] = buf_rect[std::max_element(sum.begin(), sum.end()) - sum.begin()];
  }

  void get_rect(RotRect& points, const points_ar& contours) {
    int max_area = 0;
    int area;
    float per;
    points_ar poly;
    poly.resize(contours.size());

    for (size_t i = 0; i < contours.size(); i++) {
      area = cv::contourArea(contours[i]);
      if (area < 1000) continue;

      per = cv::arcLength(contours[i], true);
      cv::approxPolyDP(contours[i], poly[i], 0.02 * per, true);
      if (max_area < area && poly[i].size() == 4) {
        max_area = area;
        points = {poly[i][0], poly[i][1], poly[i][2], poly[i][3]};
      }
    }
  }

  //  need getters
 private:
  // Some of these matrices are only needed for one time, mb delete them?
  cv::Mat img;
  cv::Mat can;
  cv::Mat dil;
  cv::Mat gray;
  cv::Mat gaus;
  cv::Mat result;
  cv::Mat wrap;
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_DILATE, cv::Size(3, 3));

  points_ar contours;
  points_ar poly, poly1;

  int area;
  float per;

  int width = 520;
  int height = 420;
};
