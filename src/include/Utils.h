#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <QImage>
#include <QObject>

class Utils
{
public:
	static cv::Mat qImageToMat(const QImage& image);
	static QImage matToQImage(const cv::Mat& mat);
};

#endif
