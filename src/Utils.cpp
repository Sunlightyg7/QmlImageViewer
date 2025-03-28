#include "Utils.h"

using namespace cv;

cv::Mat Utils::qImageToMat(const QImage& image)
{
	switch (image.format()) {
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32:
	case QImage::Format_ARGB32_Premultiplied: {
		Mat mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
		Mat matNoAlpha;
		cvtColor(mat, matNoAlpha, COLOR_BGRA2BGR);
		return matNoAlpha.clone();
	}
	case QImage::Format_RGB888: {
		Mat mat(image.height(), image.width(), CV_8UC3, const_cast<uchar*>(image.bits()), image.bytesPerLine());
		Mat matBGR;
		cvtColor(mat, matBGR, COLOR_RGB2BGR);
		return matBGR.clone();
	}
	case QImage::Format_Grayscale8: {
		return Mat(image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.bits()), image.bytesPerLine()).clone();
	}
	default:
		return Mat();
	}
}

QImage Utils::matToQImage(const cv::Mat& mat)
{
	if (mat.empty())
		return QImage();

	switch (mat.type()) {
	case CV_8UC1: {
		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
		return image.copy();
	}
	case CV_8UC3: {
		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.copy();
	}
	case CV_8UC4: {
		QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	default:
		return QImage();
	}
}

std::string Utils::calcCurrTime()
{
	using namespace std::chrono;

	std::string strTime;
	char buffer[1024] = { 0 };

	// 保存记录时间
	system_clock::time_point now = system_clock::now();
	time_t tt = system_clock::to_time_t(now); //利用chrono库自带的方法转换为ctime中的time_t, 会导致精度降低
	// 格式化时间为：2024-04-23-17.13.06
	if (std::strftime(buffer, sizeof(buffer), "%F-%H.%M.%S", std::localtime(&tt)))
	{
		strTime = buffer;
	}

	return strTime;
}
