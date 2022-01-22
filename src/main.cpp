#include <stdio.h>

#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
  cv::Mat image;

  cv::VideoCapture cap(0);
  cv::Mat img, can, dil, gray, thresh, detected_lines, result;
  int area;
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_DILATE, cv::Size(3, 3));
  std::vector<std::vector<cv::Point>> contours;
  std::vector<std::vector<cv::Point>> cnts;
  std::vector<std::vector<cv::Point>> poly;

  cv::Mat horizontal_kernel =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(75, 1));

  cv::Mat vertical_kernel =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 75));

  while (true) {
    cap.read(img);
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, can, 25, 75);
    cv::dilate(can, dil, kernel);

    // Delete horizontal line
    cv::morphologyEx(dil, detected_lines, cv::MORPH_OPEN, horizontal_kernel,
                     cv::Point(-1, -1), 2);
    cv::findContours(detected_lines, cnts, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);
    cv::drawContours(dil, cnts, -1, cv::Scalar(0, 0, 0), 3);

    // Delete vertical line
    cv::morphologyEx(dil, detected_lines, cv::MORPH_OPEN, vertical_kernel,
                     cv::Point(-1, -1), 2);
    cv::findContours(detected_lines, cnts, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);
    cv::drawContours(dil, cnts, -1, cv::Scalar(0, 0, 0), 3);

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
