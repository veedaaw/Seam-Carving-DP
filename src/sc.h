#ifndef SEAMCARVINGCOMP665156
#define SEAMCARVINGCOMP665156

#include <opencv2/opencv.hpp>

// the function you need to implement - by defaiult it calls seam_carving_trivial
bool seam_carving(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image);


bool seam_carving_trivial(cv::Mat& in_image, int new_width, int new_height, cv::Mat& out_image);

bool reduce_horizontal_seam_trivial(cv::Mat& in_image, cv::Mat& out_image);


bool reduce_vertical_seam_trivial(cv::Mat& in_image, cv::Mat& out_image);

void reduceSeam(cv::Mat& in_image, cv::Mat& out_image, bool vertical);

cv::Mat1f calculateEnergy(const cv::Mat& image);

cv::Mat1f gradientMagnitude(const cv::Mat& img);

std::vector<int> getSeam(const cv::Mat& image, bool vertical);
#endif
