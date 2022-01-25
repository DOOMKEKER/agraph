#include <stdio.h>

#include <iostream>
#include <opencv2/opencv.hpp>

// Delete line
void del_line(cv::Mat input, cv::Mat const& kernel) {
  cv::Mat detected_lines;
  std::vector<std::vector<cv::Point>> cnts;

  cv::morphologyEx(input, detected_lines, cv::MORPH_OPEN, kernel,
                   cv::Point(-1, -1), 2);
  cv::findContours(detected_lines, cnts, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_SIMPLE);
  cv::drawContours(input, cnts, -1, cv::Scalar(0, 0, 0), 3);
}

int main(int argc, char** argv) {
  cv::VideoCapture cap(0);

  cv::Mat img, can, dil, gray, thresh, detected_lines, result;
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_DILATE, cv::Size(3, 3));

  std::vector<std::vector<cv::Point>> contours;
  std::vector<std::vector<cv::Point>> poly;

  cv::Mat horizontal_kernel =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(75, 1));

  cv::Mat vertical_kernel =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 75));

  int area;

  while (true) {
    cap.read(img);
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, can, 25, 75);
    cv::dilate(can, dil, kernel);

    del_line(dil, horizontal_kernel);
    del_line(dil, vertical_kernel);

    cv::findContours(dil, contours, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_TC89_KCOS);
    poly.resize(contours.size());

    for (size_t i = 0; i < contours.size(); i++) {
      area = cv::contourArea(contours[i]);
      if (area < 200) continue;

      cv::approxPolyDP(contours[i], poly[i], 2, true);
      cv::polylines(img, poly[i], false, cv::Scalar(i * 50, i * 25, 255));
    }

    cv::imshow("Webcam", img);
    cv::waitKey(100);
  }

  return 0;
}
