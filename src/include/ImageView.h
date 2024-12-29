#ifndef IMAGEVIEWMODEL_H
#define IMAGEVIEWMODEL_H

#include <QObject>
#include <QQuickPaintedItem>
#include "ImageViewModel.h"
#include "Utils.h"
#include "ChangedQueue.h"

class ImageView : public QQuickPaintedItem
{
	Q_OBJECT
	Q_PROPERTY(int nWinWidth READ winWidth WRITE setWinWidth NOTIFY winWidthChanged)
	Q_PROPERTY(int nWinHeight READ winHeight WRITE setWinHeight NOTIFY winHeightChanged)
	Q_PROPERTY(double fBaseOffsetX READ baseOffsetX WRITE setBaseOffsetX NOTIFY baseOffsetXChanged)
	Q_PROPERTY(double fBaseOffsetY READ baseOffsetY WRITE setBaseOffsetY NOTIFY baseOffsetYChanged)
	Q_PROPERTY(double fZoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
	Q_PROPERTY(QVariant nGray READ gray WRITE setGray NOTIFY grayChanged)
	Q_PROPERTY(QVariant nBrightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
public:
	explicit ImageView(QQuickPaintedItem* parent = nullptr);

	Q_INVOKABLE void initImage(ImageViewModel* viewModel);
	Q_INVOKABLE bool isImageOpened();
	Q_INVOKABLE void zoomIn(double fMouseX, double fMouseY); 
	Q_INVOKABLE void zoomOut(double fMouseX, double fMouseY); 
	Q_INVOKABLE void applyImgConfig(QString strWinName);
	Q_INVOKABLE void cancelImgConfig(QString strWinName);
	Q_INVOKABLE void addImgConfig(QString strFuncName, QVariant varParms);
	Q_INVOKABLE QVariantList getStepsList() const;
	Q_INVOKABLE int getStepsListSize() const;

	void initImgPara();
	void resizeImage(const cv::Mat& src, cv::Mat& dst, int nWidth, int nHeight);
	void scaleImage(const cv::Mat& src, cv::Mat& dst, double fFactor); 

	/**
	 * @brief 应用图像上的修改，并将窗口名和窗口中所有的修改放进修改队列
	 *
	 * @param [in] strWinName 窗口名，如gray
	 * @param [in] mParms 包含窗口中所有的修改
	 */
	void invokeSetParmsFunc(const QString& strWinName, const QMap<QString, QVariant>& mParms);

	/**
	 * @brief 根据strFuncName调用相应的图像调整函数
	 *
	 * @param [in] strFuncName 图像调整函数名，如gray，动态调用gray函数
	 * @param [in] varParms 新的参数
	 */
	void invokeConfigFunc(const QString& strFuncName, const QVariant& varParms);

	// ------------图像处理------------
	Q_INVOKABLE void gray(const QVariant& nVal, cv::Mat* pDstMat);
	Q_INVOKABLE void brightness(const QVariant& nVal, cv::Mat* pDstMat);
	// -------------------------------

	// -----------get-----------
	int winHeight() const;
	int winWidth() const;
	double baseOffsetX() const;
	double baseOffsetY() const;
	double zoomFactor() const;
	Q_INVOKABLE QVariant gray() const;
	Q_INVOKABLE QVariant brightness() const;
	// -------------------------

	// -----------set-----------
	void setWinHeight(int nWinHeight);
	void setWinWidth(int nWinWidth);
	void setBaseOffsetX(double fBaseOffsetX);
	void setBaseOffsetY(double fBaseOffsetY);
	void setZoomFactor(double fZoomFactor);
	Q_INVOKABLE void setGray(QVariant nGray);
	Q_INVOKABLE void setBrightness(QVariant nBrightness);
	// -------------------------

	int calcMidOffsetX(const cv::Mat& src);
	int calcMidOffsetY(const cv::Mat& src);
	double calcZoomFactor(const cv::Mat& scale, const cv::Mat& src);
	void calcScaleOffset(double fMouseX, double fMouseY, double fOldZoom, double fNewZoom);
	int calcScaledNumber(int nOldNum1, int nOldNum2, int nNewNum);

public slots:
	void onStepsChanged();

protected:
	virtual void paint(QPainter* pPainter) override;

signals:
	//void imageChanged();
	void winHeightChanged();
	void winWidthChanged();
	void baseOffsetXChanged();
	void baseOffsetYChanged();
	void zoomFactorChanged();
	void grayChanged();
	void brightnessChanged();

private:
	int m_nWinHeight = 0;
	int m_nWinWidth = 0;
	double m_fBaseOffsetX = 0.0; // 图片左上角相对于窗口的偏移
	double m_fBaseOffsetY = 0.0;
	double m_fZoomOffsetX = 0.0;
	double m_fZoomOffsetY = 0.0;
	double m_fMinZoom = 0.01; // 0.01 = 1%
	double m_fMaxZoom = 5.0;
	double m_fZoomFactor = 0.0;

	// ---------图像参数-----------
	int m_nGray = 0;
	int m_nBrightness = 0;
	// ---------------------------

	// 临时保存的图像调整参数<参数名，参数值>
	QMap<QString, QVariant> m_mOldConfig;
	QMap<QString, QVariant> m_mNewConfig;

	// 图像是否调整过至少过一次
	bool m_bChanged = false;
	std::shared_ptr<cv::Mat> m_pImgMat = nullptr;
	cv::Mat m_showImgMat;
	ChangedQueue m_qChanged;
};

#endif // IMAGEVIEWMODEL_H
