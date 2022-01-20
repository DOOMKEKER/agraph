#include <stdio.h>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
  cv::Mat image;

  cv::VideoCapture cap(0);
  cv::Mat img;
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_DILATE, cv::Size(3, 3));

  while (true) {
    cap.read(img);
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
    cv::Canny(img, img, 25, 75);
    cv::dilate(img, img, kernel);

    cv::imshow("Webcam", img);
    cv::waitKey(1);
  }

  return 0;
}
