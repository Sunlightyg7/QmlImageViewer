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

	/**
	* @brief 返回当前时间字符串，格式：2024-04-23-17.13.06
	*/
	static std::string calcCurrTime();
};

#endif
