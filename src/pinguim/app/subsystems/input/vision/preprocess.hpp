#include <opencv2/improc.hpp>

inline void PreProcess::execute(cv::Mat src, cv::Mat &dst) {

    cv::cvtColor(src, dst, cv::COLOR_BGR2HSV);
}