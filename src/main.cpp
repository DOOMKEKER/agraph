#include <stdio.h>

#include <iostream>
#include <opencv2/opencv.hpp>

using points_ar = std::vector<std::vector<cv::Point>>;
using RotRect = std::vector<cv::Point>;

// struct RotRect {
//   cv::Point bl;
//   cv::Point br;
//   cv::Point ul;
//   cv::Point ur;
// };

float w = 522;
float h = 420;

void del_line(cv::Mat& input, cv::Mat const& kernel) {
  cv::Mat detected_lines;
  points_ar cnts;

  cv::morphologyEx(input, detected_lines, cv::MORPH_OPEN, kernel,
                   cv::Point(-1, -1), 2);
  cv::findContours(detected_lines, cnts, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_SIMPLE);
  cv::drawContours(input, cnts, -1, cv::Scalar(0, 0, 0), 3);
}

void get_rect(RotRect& points, const points_ar& contours, points_ar& poly) {
  int max_area = 0;
  int area;
  float per;

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

void img_wrap(cv::Mat& dil, RotRect& Rect, cv::Mat& wrap) {
  cv::Point2f src[4] = {Rect[0], Rect[1], Rect[2], Rect[3]};
  cv::Point2f dst[4] = {
      {0.0f, 0.0f},
      {w, 0.0f},
      {0.0f, h},
      {w, h},
  };

  cv::Mat matrix = cv::getPerspectiveTransform(src, dst);
  cv::warpPerspective(dil, wrap, matrix, cv::Point(w, h));
}

int main(int argc, char** argv) {
  cv::VideoCapture cap(0);

  cv::Mat img, can, dil, gray, gaus, thresh, detected_lines, result, wrap;
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_DILATE, cv::Size(3, 3));

  points_ar contours;
  points_ar poly;

  cv::Mat horizontal_kernel =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20, 1)); //  Add trashhold

  cv::Mat vertical_kernel =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 25));

  int area;
  float per;

  while (true) {
    // cap.read(img);

    img = cv::imread("../test.jpg");

    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // if (gray.empty()) continue;
    cv::GaussianBlur(gray, gaus, cv::Size(5, 5), 2);
    cv::Canny(gaus, can, 25, 75);
    cv::dilate(can, dil, kernel);


    cv::findContours(dil, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_TC89_KCOS);
    poly.resize(contours.size());

    RotRect Rect;
    get_rect(Rect, contours, poly);

    if (Rect.empty()) continue;

    reorder_pts(Rect);

    img_wrap(dil, Rect, wrap);

    del_line(wrap, horizontal_kernel);
    del_line(wrap, vertical_kernel);


    cv::imshow("Webcam1", wrap);
    cv::waitKey();


    for (size_t i = 0; i < contours.size(); i++) {
      area = cv::contourArea(contours[i]);
      if (area < 200) continue;

      cv::approxPolyDP(contours[i], poly[i], 2, true);
      cv::polylines(img, poly[i], false, cv::Scalar(i * 50, i * 25, 255));
    }

    cv::imshow("Webcam1", dil);
    cv::imshow("Webcam2", img);
    cv::imshow("Webcam3", wrap);
    cv::waitKey();
  }

  return 0;
}
