#include "ImageView.h"
#include <QPainter>

using namespace cv;
using namespace std;

ImageView::ImageView(QQuickPaintedItem* parent) : QQuickPaintedItem(parent)
{
	//connect(this, &ImageView::imageChanged, this, &ImageView::onImageChanged);
	connect(&m_qChanged, &ChangedQueue::stepsChanged, this, &ImageView::onStepsChanged);

	m_mInitValue["gamma"] = 1;
	m_mInitValue["brightness"] = 0;
}

void ImageView::initImage(ImageViewModel* viewModel)
{
	initImgPara();
	m_pImgMat = viewModel->imgMat();
	resizeImage(*m_pImgMat, m_showImgMat, m_nWinWidth, m_nWinHeight);
	setZoomFactor(calcZoomFactor(m_showImgMat, *m_pImgMat));
	setBaseOffsetX(calcMidOffsetX(m_showImgMat));
	setBaseOffsetY(calcMidOffsetY(m_showImgMat));
	update();
}

void ImageView::initImgConfig(const QString& szVariableName)
{
	auto it = m_mInitValue.find(szVariableName);
	if (it == m_mInitValue.end())
	{
		m_pLogger->error("Error: Could not find the init value!");
		return;
	}

	const QMetaObject* metaObject = this->metaObject();

	for (int i = 0; i < metaObject->propertyCount(); ++i) 
	{
		QMetaProperty property = metaObject->property(i);
		QString propertyName = property.name();

		if (szVariableName == propertyName) 
		{
			QVariant value = it.value();
			// 使用反射来设置属性值
			property.write(this, value);
			m_pLogger->info("Initialized {} with value {}", propertyName.toStdString(), value.toString().toStdString());
		}
	}
}

bool ImageView::isImageOpened()
{
	return m_pImgMat && !m_pImgMat->empty();
}

void ImageView::zoomIn(double fMouseX, double fMouseY)
{
	if (nullptr == m_pImgMat)
		return;

	//double fOldZoom = m_fZoomFactor;
	//setZoomFactor(m_fZoomFactor * 1.1);
	//calcScaleOffset(fMouseX, fMouseY, fOldZoom, m_fZoomFactor);
	//scaleImage(*m_pImgMat, m_showImgMat, m_fZoomFactor);
	//update();
}

void ImageView::zoomOut(double fMouseX, double fMouseY)
{
	if (nullptr == m_pImgMat)
		return;

	//double fOldZoom = m_fZoomFactor;
	//setZoomFactor(m_fZoomFactor * 0.9);
	//calcScaleOffset(fMouseX, fMouseY, fOldZoom, m_fZoomFactor);
	//scaleImage(*m_pImgMat, m_showImgMat, m_fZoomFactor);
	//update();
}

void ImageView::applyImgConfig(QString strWinName)
{
	if (nullptr == m_pImgMat)
		return;

	invokeSetParmsFunc(strWinName, m_mNewConfig);
	m_mNewConfig.clear();
	m_mOldConfig.clear();
	update();
}

void ImageView::cancelImgConfig(QString strWinName)
{
	if (nullptr == m_pImgMat)
		return;

	invokeSetParmsFunc(strWinName, m_mOldConfig);

	for (auto it = m_mOldConfig.begin(); it != m_mOldConfig.end(); ++it)
	{
		const QString& szVariableName = it.key();
		m_qChanged.addOrUpdateStep({ szVariableName, it.value(), 0 });
	}

	m_mNewConfig.clear();
	m_mOldConfig.clear();
	emit changedListChanged();
	update();
}

void ImageView::addImgConfig(QString szVariableName, QVariant varParms)
{
	if (nullptr == m_pImgMat || szVariableName.isEmpty() || !varParms.isValid())
		return;

	Step oldStep = m_qChanged.getStep(szVariableName);
	m_qChanged.addOrUpdateStep({ szVariableName, varParms, 0, 0 });
	m_mNewConfig.insert(szVariableName, varParms);

	QVariant varRet;
	QMetaObject::invokeMethod(this, szVariableName.toStdString().c_str(), Q_RETURN_ARG(QVariant, varRet));
	m_mOldConfig.insert(szVariableName, varRet);

	emit changedListChanged();
}

void ImageView::removeListItem(int nIndex)
{
	Step step = m_qChanged.getStep(nIndex);
	initImgConfig(step.szVariableName);
	if (m_qChanged.removeStep(nIndex))
		emit changedListChanged();
}

void ImageView::moveListItem(int nFrom, int nTo)
{
	if (m_qChanged.moveStep(nFrom, nTo))
		emit changedListChanged();
}

void ImageView::selectedListItem(int nIndex)
{
	if (m_qChanged.stepSelected(nIndex))
		emit changedListChanged();
}

QVariantList ImageView::getStepsList() const
{
	return m_qChanged.getStepsList();
}

int ImageView::getStepsListSize() const
{
	return m_qChanged.size();
}

void ImageView::initImgPara()
{
	m_fBaseOffsetX = 0.0;
	m_fBaseOffsetY = 0.0;
	m_fZoomOffsetX = 0.0;
	m_fZoomOffsetY = 0.0;
	m_fZoomFactor = 0.0;
	m_pImgMat = nullptr;
	m_showImgMat = Mat();
	m_qChanged.clear();
	m_bChanged = false;
	m_mNewConfig.clear();
	m_mOldConfig.clear();

	for(auto it = m_mInitValue.begin(); it != m_mInitValue.end(); ++it)
		initImgConfig(it.key());
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
		cv::resize(src, dst, cv::Size(calcScaledNumber(src.cols, src.rows, nHeight), nHeight));
	// 纵向拉伸窗口
	else
		cv::resize(src, dst, cv::Size(nWidth, calcScaledNumber(src.rows, src.cols, nWidth)));
}

void ImageView::scaleImage(const cv::Mat& src, cv::Mat& dst, double factor)
{
	if (src.empty())
		return;

	cv::resize(src, dst, cv::Size(), factor, factor);
}

void ImageView::invokeSetParmsFunc(const QString& strWinName, const QMap<QString, QVariant>& mParms)
{
	if (strWinName.isEmpty() || mParms.empty())
		return;

	for (auto it = mParms.begin(); it != mParms.end(); ++it)
	{
		const QString& szVariableName = it.key();
		QString szSetVariableName = "set" + szVariableName;

		// 将首字母置为大写来调用set方法，如gray -> setGray
		szSetVariableName[3] = szSetVariableName.at(3).toUpper();

		QMetaObject::invokeMethod(this, szSetVariableName.toStdString().c_str(), Q_ARG(QVariant, it.value()));
	}
}

void ImageView::invokeConfigFunc(const QString& szVariableName, const QVariant& varParms)
{
	if (szVariableName.isEmpty() || !varParms.isValid())
		return;

	//调用图像处理函数如：gray(const QVariant& nVal, const cv::Mat& pDstMat)
	QMetaObject::invokeMethod(this, szVariableName.toStdString().c_str(), Q_ARG(QVariant, varParms), Q_ARG(cv::Mat*, &m_showImgMat));
}

void ImageView::gamma(const QVariant& nVal, cv::Mat* pDstMat)
{
	if (nullptr == m_pImgMat)
		return;

	int gamma = nVal.toInt();
	CV_Assert(gamma > 0);  // 确保 gamma > 0

	// 创建查找表，存储每个像素的 gamma 校正值
	cv::Mat lookupTable(1, 256, CV_8U);

	// 计算 gamma 曲线
	for (int i = 0; i < 256; i++) {
		lookupTable.at<uchar>(i) = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
	}

	// 应用查找表对图像进行 Gamma 校正
	cv::LUT(m_showImgMat, lookupTable, *pDstMat);
}

void ImageView::brightness(const QVariant& nVal, cv::Mat* pDstMat)
{
	if (nullptr == m_pImgMat)
		return;

	m_showImgMat.convertTo(*pDstMat, -1, 1, nVal.toInt()); // alpha=1, beta=brightness
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

QVariant ImageView::gamma() const
{
	return m_nGamma;
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

void ImageView::setGamma(QVariant nGamma)
{
	m_nGamma = nGamma.toInt();
	emit gammaChanged();
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

int ImageView::calcScaledNumber(int nOldNum1, int nOldNum2, int nNewNum)
{
	return static_cast<int>(std::round(static_cast<double>(nOldNum1) / nOldNum2 * nNewNum));
}

void ImageView::onStepsChanged()
{
	// TODO: 每次看看能不能合并多个StepsChanged信号
	m_showImgMat = m_pImgMat->clone();

	for (auto it = m_qChanged.begin(); it != m_qChanged.end(); it++)
	{
		auto&& [szVariableName, varValue, nPriority, bSelected] = *it;
		invokeConfigFunc(szVariableName, varValue);
		m_pLogger->info("func name: {}, value: {}, priority: {}", szVariableName.toStdString(), varValue.toString().toStdString(), nPriority);
	}
	resizeImage(m_showImgMat, m_showImgMat, m_nWinWidth, m_nWinHeight);
	update();
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
