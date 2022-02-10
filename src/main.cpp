#include <iostream>
#include "poly_math.hpp"
#include "scanner.hpp"

namespace cv {
bool operator<(cv::Point const& l, cv::Point const& r) { return l.x < r.x; }
}  // namespace cv

int main(int argc, char** argv) {
  cv::VideoCapture cap(0);
  cv::Mat img;

  points_ar contours;
  points_ar poly, poly1;

  //  Add tra shhold
  cv::Mat horizontal_kernel =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20, 1));

  cv::Mat vertical_kernel =
      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 25));

  int area;
  float per;

  std::vector<cv::Point> points2;

  while (true) {
    img = cv::imread("../test.jpg");

    Scanner file = Scanner(img);

    file.make_normilized_graph(horizontal_kernel, vertical_kernel);
    points2 = file.get_points();

    for (auto& i : points2) {
      cv::circle(img, i, 1, cv::Scalar(255, 255, 255));
      std::cout << i.x << std::endl;
    }

    for (size_t i = 0; i < points2.size(); ++i) {
      cv::circle(img, points2[i], 1, cv::Scalar(255, 0, 0));
    }

    auto mSharedPtr =
        std::make_shared<std::vector<cv::Point>>(std::move(points2));

    cv::imshow("Webcam1", dil);
    cv::imshow("Webcam2", img);
    cv::imshow("Webcam3", wrap);
    cv::waitKey();
  }

  return 0;
}
