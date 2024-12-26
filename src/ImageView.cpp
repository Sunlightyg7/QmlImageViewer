#include "ImageView.h"
#include <QPainter>

using namespace cv;

ImageView::ImageView(QQuickPaintedItem* parent) : QQuickPaintedItem(parent)
{
}

void ImageView::zoomIn(double fMouseX, double fMouseY)
{
	if (nullptr == m_pImgMat)
		return;

	double fOldZoom = m_fZoomFactor;
	setZoomFactor(m_fZoomFactor * 1.1);
	calcScaleOffset(fMouseX, fMouseY, fOldZoom, m_fZoomFactor);
	scaleImage(*m_pImgMat, m_showImgMat, m_fZoomFactor);
	update();
}

void ImageView::zoomOut(double fMouseX, double fMouseY)
{
	if (nullptr == m_pImgMat)
		return;

	double fOldZoom = m_fZoomFactor;
	setZoomFactor(m_fZoomFactor * 0.9);
	calcScaleOffset(fMouseX, fMouseY, fOldZoom, m_fZoomFactor);
	scaleImage(*m_pImgMat, m_showImgMat, m_fZoomFactor);
	update();
}

void ImageView::createTransientImg()
{
	if (nullptr == m_pImgMat)
		return;

	m_transientImgMat = m_pImgMat->clone();
}

void ImageView::applyImgConfig(bool bApply)
{
	if (nullptr == m_pImgMat)
		return;

	if (bApply)
	{
		m_pImgMat = std::make_shared<Mat>(std::move(m_transientImgMat));
		resizeImage(*m_pImgMat, m_showImgMat, m_showImgMat.cols, m_showImgMat.rows);
		update();
	}
	else
	{
		m_transientImgMat = Mat();
	}
}

Q_INVOKABLE void ImageView::grayAdjust(int nVal)
{
	int nCols = m_showImgMat.cols;
	int nRows = m_showImgMat.rows;
	m_transientImgMat.convertTo(m_showImgMat, -1, 1, nVal); // alpha=1, beta=brightness
	resizeImage(m_showImgMat, m_showImgMat, nCols, nRows);
	update();
}

bool ImageView::isImageOpened()
{
	return m_pImgMat && !m_pImgMat->empty();
}

void ImageView::initImgPara()
{
	m_fBaseOffsetX = 0.0;
	m_fBaseOffsetY = 0.0;
	m_fZoomOffsetX = 0.0;
	m_fZoomOffsetY = 0.0;
	m_pImgMat = nullptr;
	m_fZoomFactor = 0.0;
}

void ImageView::resizeImage(const cv::Mat& src, cv::Mat& dst, int nWidth, int nHeight)
{
	if (src.empty() || 0 == nWidth || 0 == nHeight)
		return;

	// 宽高比
	double fImgFactor = static_cast<double>(src.cols) / src.rows;
	double fShowFactor = static_cast<double>(nWidth) / nHeight;

	// 横向拉伸窗口
	if (fShowFactor > fImgFactor)
		cv::resize(src, dst, cv::Size(getScaledNumber(src.cols, src.rows, nHeight), nHeight));
	// 纵向拉伸窗口
	else
		cv::resize(src, dst, cv::Size(nWidth, getScaledNumber(src.rows, src.cols, nWidth)));
}

void ImageView::scaleImage(const cv::Mat& src, cv::Mat& dst, double factor)
{
	if (src.empty())
		return;

	cv::resize(src, dst, cv::Size(), factor, factor);
}

int ImageView::winHeight() const
{
	return m_nWinHeight;
}

int ImageView::winWidth() const
{
	return m_nWinWidth;
}

double ImageView::baseOffsetX() const
{
	return m_fBaseOffsetX;
}

double ImageView::baseOffsetY() const
{
	return m_fBaseOffsetY;
}

double ImageView::zoomFactor() const
{
	return m_fZoomFactor;
}

void ImageView::setWinHeight(int nWinHeight)
{
	m_nWinHeight = nWinHeight;
	emit winHeightChanged();
}

void ImageView::setWinWidth(int nWinWidth)
{
	m_nWinWidth = nWinWidth;
	emit winWidthChanged();
}

void ImageView::setBaseOffsetX(double fOffsetX)
{
	m_fBaseOffsetX = fOffsetX;
	emit baseOffsetXChanged();
	update();
}

void ImageView::setBaseOffsetY(double fOffsetY)
{
	m_fBaseOffsetY = fOffsetY;
	emit baseOffsetYChanged();
	update();
}

void ImageView::setZoomFactor(double fZoomFactor)
{
	m_fZoomFactor = std::clamp(fZoomFactor, m_fMinZoom, m_fMaxZoom);
	emit zoomFactorChanged();
}

int ImageView::calcMidOffsetX(const cv::Mat& src)
{
	return static_cast<int>((width() - src.cols) / 2);
}

int ImageView::calcMidOffsetY(const cv::Mat& src)
{
	return static_cast<int>((height() - src.rows) / 2);
}

double ImageView::calcZoomFactor(const cv::Mat& scale, const cv::Mat& src)
{
	return std::min(static_cast<double>(scale.cols) / src.cols, static_cast<double>(scale.rows) / src.rows);
}

void ImageView::calcScaleOffset(double fMouseX, double fMouseY, double fOldZoom, double fNewZoom)
{
	// 计算以图片左上角为坐标起始点，X、Y的位置
	double fImageX = (fMouseX - m_fBaseOffsetX - m_fZoomOffsetX) / fOldZoom;
	double fImageY = (fMouseY - m_fBaseOffsetY - m_fZoomOffsetY) / fOldZoom;

	// 计算出缩放后的图片应该应该从哪个点开始绘制，paint时会加上m_fBaseOffsetX
	m_fZoomOffsetX = fMouseX - fImageX * fNewZoom - m_fBaseOffsetX;
	m_fZoomOffsetY = fMouseY - fImageY * fNewZoom - m_fBaseOffsetY;
}

QImage ImageView::mat2QImg(const cv::Mat& imgMat)
{
	return QImage(imgMat.data, imgMat.cols, imgMat.rows, imgMat.step, QImage::Format_RGB888);
}

int ImageView::getScaledNumber(int nOldNum1, int nOldNum2, int nNewNum)
{
	return static_cast<int>(std::round(static_cast<double>(nOldNum1) / nOldNum2 * nNewNum));
}

void ImageView::paint(QPainter* pPainter)
{
	if (m_showImgMat.empty())
		return;

	pPainter->setRenderHint(QPainter::Antialiasing);

	// 计算总偏移量
	double fTotalOffsetX = m_fBaseOffsetX + m_fZoomOffsetX;
	double fTotalOffsetY = m_fBaseOffsetY + m_fZoomOffsetY;

	pPainter->drawImage(fTotalOffsetX, fTotalOffsetY, mat2QImg(m_showImgMat));
}

void ImageView::onImageChanged(ImageViewModel* viewModel)
{
	initImgPara();
	m_pImgMat = viewModel->imgMat();
	resizeImage(*m_pImgMat, m_showImgMat, m_nWinWidth, m_nWinHeight);
	setZoomFactor(calcZoomFactor(m_showImgMat, *m_pImgMat));
	setBaseOffsetX(calcMidOffsetX(m_showImgMat));
	setBaseOffsetY(calcMidOffsetY(m_showImgMat));
	update();
}
