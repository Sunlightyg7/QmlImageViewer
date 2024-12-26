#ifndef IMAGEVIEWMODEL_H
#define IMAGEVIEWMODEL_H

#include <QObject>
#include <QQuickPaintedItem>
#include "ImageViewModel.h"

class ImageView : public QQuickPaintedItem
{
	Q_OBJECT
		Q_PROPERTY(int nWinWidth READ winWidth WRITE setWinWidth NOTIFY winWidthChanged)
		Q_PROPERTY(int nWinHeight READ winHeight WRITE setWinHeight NOTIFY winHeightChanged)
		Q_PROPERTY(double fBaseOffsetX READ baseOffsetX WRITE setBaseOffsetX NOTIFY baseOffsetXChanged)
		Q_PROPERTY(double fBaseOffsetY READ baseOffsetY WRITE setBaseOffsetY NOTIFY baseOffsetYChanged)
		Q_PROPERTY(double fZoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
public:
	explicit ImageView(QQuickPaintedItem* parent = nullptr);

	Q_INVOKABLE void zoomIn(double fMouseX, double fMouseY); // 放大
	Q_INVOKABLE void zoomOut(double fMouseX, double fMouseY); // 缩小

	// ------------图像处理------------
	Q_INVOKABLE void createTransientImg();
	Q_INVOKABLE void applyImgConfig(bool bApply);
	Q_INVOKABLE void grayAdjust(int nVal);
	// -------------------------------


	Q_INVOKABLE bool isImageOpened();

	void initImgPara();
	void resizeImage(const cv::Mat & src, cv::Mat& dst, int nWidth, int nHeight);
	void scaleImage(const cv::Mat & src, cv::Mat& dst, double fFactor); // 缩放图片

	int winHeight() const;
	int winWidth() const;
	double baseOffsetX() const;
	double baseOffsetY() const;
	double zoomFactor() const;

	void setWinHeight(int nWinHeight);
	void setWinWidth(int nWinWidth);
	void setBaseOffsetX(double fBaseOffsetX);
	void setBaseOffsetY(double fBaseOffsetY);
	void setZoomFactor(double fZoomFactor);

	int calcMidOffsetX(const cv::Mat& src);
	int calcMidOffsetY(const cv::Mat& src);
	double calcZoomFactor(const cv::Mat & scale, const cv::Mat& src);
	void calcScaleOffset(double fMouseX, double fMouseY, double fOldZoom, double fNewZoom);

	QImage mat2QImg(const cv::Mat& imgMat);
	int getScaledNumber(int nOldNum1, int nOldNum2, int nNewNum);

protected:
	virtual void paint(QPainter* pPainter) override;

public slots:
	void onImageChanged(ImageViewModel* viewModel);

signals:
	void winHeightChanged();
	void winWidthChanged();
	void baseOffsetXChanged();
	void baseOffsetYChanged();
	void zoomFactorChanged();
	void imageChanged();

private:
	int m_nWinHeight = 0;
	int m_nWinWidth = 0;
	double m_fBaseOffsetX = 0.0; // 图片左上角相对于窗口的偏移
	double m_fBaseOffsetY = 0.0;
	double m_fZoomOffsetX = 0.0;
	double m_fZoomOffsetY = 0.0;
	double m_fMinZoom = 0.01; // 0.01 = 1%
	double m_fMaxZoom = 5.0;
	std::shared_ptr<cv::Mat> m_pImgMat = nullptr;
	cv::Mat m_transientImgMat;
	cv::Mat m_showImgMat;
	double m_fZoomFactor = 0.0;
};

#endif // IMAGEVIEWMODEL_H
