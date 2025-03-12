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

	m_pImgMat = setImgMat(cv::imread(filePath.toLocal8Bit().toStdString(), cv::IMREAD_COLOR_RGB));
	if (!m_pImgMat || m_pImgMat->empty())
		return;

	emit imageChanged(this);
}

std::shared_ptr<cv::Mat> ImageViewModel::imgMat() const
{
	return m_pImgMat;
}

std::shared_ptr<cv::Mat> ImageViewModel::setImgMat(const cv::Mat& imgMat)
{
	if (imgMat.empty())
	{
		m_pLogger->error("Error: Could not open or find the imgMat!");
		return nullptr;
	}

	return std::make_shared<cv::Mat>(imgMat);
}