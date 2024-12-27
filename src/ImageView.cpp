#include "ImageView.h"
#include <QPainter>

using namespace cv;

ImageView::ImageView(QQuickPaintedItem* parent) : QQuickPaintedItem(parent)
{
}

bool ImageView::isImageOpened()
{
	return m_pImgMat && !m_pImgMat->empty();
}

void ImageView::zoomIn(double fMouseX, double fMouseY)
{
	if (nullptr == m_pImgMat)
		return;

	double fOldZoom = m_fZoomFactor;
	setZoomFactor(m_fZoomFactor * 1.1);
	calcScaleOffset(fMouseX, fMouseY, fOldZoom, m_fZoomFactor);
	scaleImage(m_transientImgMat, m_showImgMat, m_fZoomFactor);
	update();
}

void ImageView::zoomOut(double fMouseX, double fMouseY)
{
	if (nullptr == m_pImgMat)
		return;

	double fOldZoom = m_fZoomFactor;
	setZoomFactor(m_fZoomFactor * 0.9);
	calcScaleOffset(fMouseX, fMouseY, fOldZoom, m_fZoomFactor);
	scaleImage(m_transientImgMat, m_showImgMat, m_fZoomFactor);
	update();
}

void ImageView::applyImgConfig(QString strWinName, bool bApply)
{
	if (nullptr == m_pImgMat)
		return;

	// 应用当前图像调整
	if (bApply)
	{
		invokeSetParmsFunc(strWinName, m_varTmpConfig);
	}
	// 取消当前展示的图像调整
	else
	{

		// 获取当前已应用的图像参数
		QVariant varParms;
		QMetaObject::invokeMethod(this, strWinName.toStdString().c_str(), Q_RETURN_ARG(QVariant, varParms)); // 返回值类型和变量

		if (!varParms.isValid())
		{
			qDebug() << "error";
			return;
		}

		// 应用当前图像参数，从而取消展示的图像参数
		QMetaObject::invokeMethod(this, strWinName.toStdString().c_str(), Q_ARG(QVariant, varParms));
	}

	m_varTmpConfig.clear();
	update();
}

void ImageView::invokeSetParmsFunc(const QString& strWinName, QVariant varParms)
{
	if (strWinName.isEmpty() || !varParms.isValid())
		return;

	QString strSetFuncName = "set" + strWinName;
	// 将strWinName首字母置为大写来调用set方法
	strSetFuncName[3] = strSetFuncName.at(3).toUpper();

	QMetaObject::invokeMethod(this, strSetFuncName.toStdString().c_str(), Q_ARG(QVariant, varParms));
}

void ImageView::invokeConfigFunc(QString strFuncName, QVariant varParms)
{
	if (strFuncName.isEmpty() || !varParms.isValid())
		return;

	// 调用图像处理函数如：grayValue
	QMetaObject::invokeMethod(this, strFuncName.toStdString().c_str(), Q_ARG(QVariant, varParms));
	m_varTmpConfig = varParms;
}

void ImageView::grayValue(const QVariant& nVal)
{
	if (nullptr == m_pImgMat)
		return;

	m_transientImgMat.convertTo(m_transientImgMat, -1, 1, nVal.toInt()); // alpha=1, beta=brightness
	resizeImage(m_transientImgMat, m_showImgMat, m_showImgMat.cols, m_showImgMat.rows);
	update();
}

void ImageView::initImgPara()
{
	m_fBaseOffsetX = 0.0;
	m_fBaseOffsetY = 0.0;
	m_fZoomOffsetX = 0.0;
	m_fZoomOffsetY = 0.0;
	m_fZoomFactor = 0.0;
	m_nGrayValue = 0;
	m_pImgMat = nullptr;
	m_transientImgMat = Mat();
	m_showImgMat = Mat();
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

QVariant ImageView::grayValue() const
{
	return m_nGrayValue;
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

void ImageView::setGrayValue(QVariant nGrayValue)
{
	m_nGrayValue = nGrayValue.toInt();
	emit grayValueChanged();
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
	m_transientImgMat = m_pImgMat->clone();
	resizeImage(m_transientImgMat, m_showImgMat, m_nWinWidth, m_nWinHeight);
	setZoomFactor(calcZoomFactor(m_showImgMat, m_transientImgMat));
	setBaseOffsetX(calcMidOffsetX(m_showImgMat));
	setBaseOffsetY(calcMidOffsetY(m_showImgMat));
	update();
}
