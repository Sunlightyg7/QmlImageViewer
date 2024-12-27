#include "ImageView.h"
#include <QPainter>

using namespace cv;
using namespace std;

ImageView::ImageView(QQuickPaintedItem* parent) : QQuickPaintedItem(parent)
{
}

void ImageView::onImageChanged(ImageViewModel* viewModel)
{
	initImgPara();
	m_pImgMat = viewModel->imgMat();
	m_transientImgMat = m_pImgMat->clone();
	resizeImage(*m_pImgMat, m_showImgMat, m_nWinWidth, m_nWinHeight);
	setZoomFactor(calcZoomFactor(m_showImgMat, *m_pImgMat));
	setBaseOffsetX(calcMidOffsetX(m_showImgMat));
	setBaseOffsetY(calcMidOffsetY(m_showImgMat));
	update();
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
		invokeSetParmsFunc(strWinName, m_mTmpConfig);
	}
	// 取消当前展示的图像调整
	else
	{
		invokeRestoreParmsFunc(strWinName, m_mTmpConfig);
	}

	m_mTmpConfig.clear();
	update();
}

void ImageView::invokeSetParmsFunc(const QString& strWinName, const QMap<QString, QVariant>& mParms)
{
	if (strWinName.isEmpty() || mParms.empty())
		return;

	for (auto it = mParms.begin(); it != mParms.end(); ++it)
	{
		const QString& strFuncName = it.key();
		QString strSetFuncName = "set" + strFuncName;

		// 将首字母置为大写来调用set方法，如gray -> setGray
		strSetFuncName[3] = strSetFuncName.at(3).toUpper();

		QMetaObject::invokeMethod(this, strSetFuncName.toStdString().c_str(), Q_ARG(QVariant, it.value()));
	}

	m_qUndoRedo.push(m_mTmpConfig);
}

Q_INVOKABLE void ImageView::invokeRestoreParmsFunc(const QString& strWinName, const QMap<QString, QVariant>& mParms)
{
	if (strWinName.isEmpty() || mParms.empty())
		return;

	for (auto it = mParms.begin(); it != mParms.end(); ++it)
	{
		const QString& strFuncName = it.key();
		QVariant varParms;
		// 获取当前已应用的图像参数，如调用gray()
		QMetaObject::invokeMethod(this, strFuncName.toStdString().c_str(), Q_RETURN_ARG(QVariant, varParms)); // 返回值类型和变量

		if (!varParms.isValid())
			qDebug() << "error";

		// 调用图像处理函数，从而取消展示的图像调整，如调用gray(const QVariant& nVal, const cv::Mat& pDstMat)
		QMetaObject::invokeMethod(this, strFuncName.toStdString().c_str(), Q_ARG(QVariant, varParms), Q_ARG(cv::Mat*, &m_showImgMat));
	}
	scaleImage(m_showImgMat, m_showImgMat, m_fZoomFactor);
	update();
}

void ImageView::invokeConfigFunc(QString strFuncName, QVariant varParms)
{
	if (strFuncName.isEmpty() || !varParms.isValid())
		return;

	// 如果有修改记录，则从原始图像开始应用
	if (m_qUndoRedo.size())
		m_transientImgMat = m_pImgMat->clone();

	QMap<QString, QVariant> mIncorpConfig = m_qUndoRedo.incorpUndoStack();

	// 应用修改列表中的图像调整到m_transientImgMat，然后再将最新的修改应用到m_showImgMat显示
	for (auto it = mIncorpConfig.begin(); it != mIncorpConfig.end(); ++it)
	{
		const QString& strAppliedFuncName = it.key();

		// 当调用-10图像处理，再调用+20图像处理，图像也不会恢复原状，所以同样的图像处理修改记录跳过
		if (strAppliedFuncName == strFuncName)
			continue;

		// 调用图像处理函数如：gray(const QVariant& nVal, const cv::Mat& pDstMat)
		QMetaObject::invokeMethod(this, strAppliedFuncName.toStdString().c_str(), Q_ARG(QVariant, it.value()), Q_ARG(cv::Mat*, &m_transientImgMat));
	}

	// 调用当前调整的图像处理函数，并映射到m_showImgMat上显示
	Mat tmpMat;
	QMetaObject::invokeMethod(this, strFuncName.toStdString().c_str(), Q_ARG(QVariant, varParms), Q_ARG(cv::Mat*, &tmpMat));
	resizeImage(tmpMat, m_showImgMat, m_showImgMat.cols, m_showImgMat.rows);
	m_mTmpConfig.insert(strFuncName, varParms);
	update();
}

void ImageView::gray(const QVariant& nVal, cv::Mat* pDstMat)
{
	if (nullptr == m_pImgMat)
		return;

	m_transientImgMat.convertTo(*pDstMat, -1, 1, nVal.toInt()); // alpha=1, beta=brightness
}

void ImageView::brightness(const QVariant& nVal, cv::Mat* pDstMat)
{
	if (nullptr == m_pImgMat)
		return;

	m_transientImgMat.convertTo(*pDstMat, -1, 1, nVal.toInt()); // alpha=1, beta=brightness
}

void ImageView::initImgPara()
{
	m_fBaseOffsetX = 0.0;
	m_fBaseOffsetY = 0.0;
	m_fZoomOffsetX = 0.0;
	m_fZoomOffsetY = 0.0;
	m_fZoomFactor = 0.0;
	m_nGray = 0;
	m_nBrightness = 0;
	m_pImgMat = nullptr;
	m_transientImgMat = Mat();
	m_showImgMat = Mat();
	m_qUndoRedo.clear();
	m_bChanged = false;
	m_mTmpConfig.clear();
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

QVariant ImageView::gray() const
{
	return m_nGray;
}

Q_INVOKABLE QVariant ImageView::brightness() const
{
	return m_nBrightness;
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

void ImageView::setGray(QVariant nGray)
{
	m_nGray = nGray.toInt();
	emit grayChanged();
}

Q_INVOKABLE void ImageView::setBrightness(QVariant nBrightness)
{
	m_nBrightness = nBrightness.toInt();
	emit brightnessChanged();
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

	pPainter->drawImage(fTotalOffsetX, fTotalOffsetY, Utils::matToQImage(m_showImgMat));
}
