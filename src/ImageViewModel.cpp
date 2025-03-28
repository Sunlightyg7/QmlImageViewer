#include "ImageViewModel.h"
#include <opencv2/core/utils/logger.hpp>

ImageViewModel::ImageViewModel(QObject* parent) : QObject(parent)
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
}

void ImageViewModel::openImage(QString filePath)
{
#if defined _WIN32
	filePath = filePath.remove("file:///");
#elif defined __linux__
	filePath = filePath.remove("file://");
#endif

	if (!setImgMat(cv::imread(filePath.toLocal8Bit().toStdString(), cv::IMREAD_COLOR_RGB)))
		m_pLogger->error("Error: Could not open or find the imgMat!");
}

std::shared_ptr<cv::Mat> ImageViewModel::imgMat() const
{
	return m_pImgMat;
}

bool ImageViewModel::setImgMat(const cv::Mat& imgMat)
{
	if (imgMat.empty())
	{
		m_pLogger->error("Error: Could not open or find the imgMat!");
		return false;
	}

	m_pImgMat = std::make_shared<cv::Mat>(imgMat);
	emit imageChanged(this);
	return true;
}