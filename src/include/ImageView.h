#ifndef IMAGEVIEWMODEL_H
#define IMAGEVIEWMODEL_H

#include <QObject>
#include <QQuickPaintedItem>
#include "ImageViewModel.h"
#include "Utils.h"
#include "ChangedQueue.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> // 用于彩色输出控制台日志
#include <spdlog/async.h> // 用于异步日志记录（如果使用多线程）

class ImageView : public QQuickPaintedItem
{
	Q_OBJECT
	Q_PROPERTY(int winWidth READ winWidth WRITE setWinWidth NOTIFY winWidthChanged)
	Q_PROPERTY(int winHeight READ winHeight WRITE setWinHeight NOTIFY winHeightChanged)
	Q_PROPERTY(double baseOffsetX READ baseOffsetX WRITE setBaseOffsetX NOTIFY baseOffsetXChanged)
	Q_PROPERTY(double baseOffsetY READ baseOffsetY WRITE setBaseOffsetY NOTIFY baseOffsetYChanged)
	Q_PROPERTY(double zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
	Q_PROPERTY(QVariant gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
	Q_PROPERTY(QVariant brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
public:
	explicit ImageView(QQuickPaintedItem* parent = nullptr);

	/**
	 * @brief 用于打开图像时初始化参数等
	 * 
	 * @param [in] viewModel 包含图像数据的viewModel
	 */
	Q_INVOKABLE void initImage(ImageViewModel* viewModel);

	/**
	 * @brief 将Q_PROPERTY中声明的变量名，利用反射初始化相应变量为m_mInitValue中的值
	 * 
	 * @param [in] szVariableName Q_PROPERTY声明的变量名
	 */
	Q_INVOKABLE void initImgConfig(const QString& szVariableName);

	/**
	 * @brief 是否已打开图像
	 */
	Q_INVOKABLE bool isImageOpened();

	/**
	 * @brief 放大图像
	 * 
	 * @param [in] fMouseX 鼠标X坐标
	 * @param [in] fMouseY 鼠标Y坐标
	 */
	Q_INVOKABLE void zoomIn(double fMouseX, double fMouseY); 

	/**
	 * @brief 缩小图像
	 * 
	 * @param [in] fMouseX 鼠标X坐标
	 * @param [in] fMouseY 鼠标Y坐标
	 */
	Q_INVOKABLE void zoomOut(double fMouseX, double fMouseY); 

	/**
	 * @brief 应用图像上的修改。利用反射调用变量的set函数。
	 * 
	 * @param [in] strWinName qml窗口名，如gray
	 */
	Q_INVOKABLE void applyImgConfig(QString strWinName);

	/**
	 * @brief 取消图像上的修改。利用反射调用变量的set函数，更新修改记录。
	 * 
	 * @param [in] strWinName qml窗口名，如gray
	 */
	Q_INVOKABLE void cancelImgConfig(QString strWinName);

	/**
	 * @brief 实时调整图像显示效果，更新修改记录。
	 * 
	 * @param [in] szVariableName Q_PROPERTY声明的变量名
	 * @param [in] varParms 新的参数
	 */
	Q_INVOKABLE void addImgConfig(QString szVariableName, QVariant varParms);

	/**
	 * @brief 移除修改记录上的指定项
	 * 
	 * @param [in] nIndex 索引
	 */
	Q_INVOKABLE void removeListItem(int nIndex);

	/**
	 * @brief 移动修改记录上的指定项
	 * 
	 * @param [in] nFrom 移动前的索引
	 * @param [in] nTo 移动后的索引
	 */
	Q_INVOKABLE void moveListItem(int nFrom, int nTo);
	
	/**
	 * @brief 选中修改记录上的指定项，将其bSelected设置为true，其他设置为false
	 * 
	 * @param [in] nIndex 索引
	 */
	Q_INVOKABLE void selectedListItem(int nIndex);

	/**
	 * @brief 获取修改记录
	 */
	Q_INVOKABLE QVariantList getStepsList() const;

	/**
	 * @brief 获取修改记录的大小
	 */
	Q_INVOKABLE int getStepsListSize() const;

	/**
	 * @brief 初始化图像参数
	 */
	void initImgPara();

	/**
	 * @brief 调整图像大小，使其匹配窗口尺寸
	 * 
	 * @param [in] src 原图像
	 * @param [out] dst 调整后的图像
	 * @param [in] nWidth 窗口宽度
	 * @param [in] nHeight 窗口高度
	 */
	void resizeImage(const cv::Mat& src, cv::Mat& dst, int nWidth, int nHeight);

	/**
	 * @brief 缩放图像
	 * 
	 * @param [in] src 原图像
	 * @param [out] dst 缩放后的图像
	 * @param [in] fFactor 缩放因子
	 */
	void scaleImage(const cv::Mat& src, cv::Mat& dst, double fFactor); 

	/**
	 * @brief 应用图像上的修改，并将窗口名和窗口中所有的修改放进修改队列
	 *
	 * @param [in] strWinName 窗口名，如gray
	 * @param [in] mParms 包含窗口中所有的修改
	 */
	void invokeSetParmsFunc(const QString& strWinName, const QMap<QString, QVariant>& mParms);

	/**
	 * @brief 根据szVariableName调用相应的图像调整函数
	 *
	 * @param [in] szVariableName 图像调整函数名，如gray，动态调用gray函数
	 * @param [in] varParms 新的参数
	 */
	void invokeConfigFunc(const QString & szVariableName, const QVariant& varParms);

	// ------------图像处理函数------------
	Q_INVOKABLE void gamma(const QVariant& nVal, cv::Mat* pDstMat);
	Q_INVOKABLE void brightness(const QVariant& nVal, cv::Mat* pDstMat);
	// -------------------------------

	// -----------get-----------
	int winHeight() const;
	int winWidth() const;
	double baseOffsetX() const;
	double baseOffsetY() const;
	double zoomFactor() const;
	Q_INVOKABLE QVariant gamma() const;
	Q_INVOKABLE QVariant brightness() const;
	// -------------------------

	// -----------set-----------
	void setWinHeight(int nWinHeight);
	void setWinWidth(int nWinWidth);
	void setBaseOffsetX(double fBaseOffsetX);
	void setBaseOffsetY(double fBaseOffsetY);
	void setZoomFactor(double fZoomFactor);
	Q_INVOKABLE void setGamma(QVariant nGamma);
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
	void changedListChanged();
	void winHeightChanged();
	void winWidthChanged();
	void baseOffsetXChanged();
	void baseOffsetYChanged();
	void zoomFactorChanged();
	void gammaChanged();
	void brightnessChanged();

private:
	int m_nWinHeight = 0;
	int m_nWinWidth = 0;
	double m_fBaseOffsetX = 0.0; // 图片左上角相对于窗口的偏移
	double m_fBaseOffsetY = 0.0;
	double m_fZoomOffsetX = 0.0;
	double m_fZoomOffsetY = 0.0;
	double m_fZoomFactor = 0.0;
	double m_fMinZoom = 0.01; // 0.01 = 1%
	double m_fMaxZoom = 5.0;

	// ---------图像参数-----------
	int m_nGamma;
	int m_nBrightness;
	// ---------------------------

	// 临时保存的图像调整参数<参数名，参数值>
	QMap<QString, QVariant> m_mOldConfig;
	QMap<QString, QVariant> m_mNewConfig;

	// 图像是否调整过至少过一次
	bool m_bChanged = false;
	std::shared_ptr<cv::Mat> m_pImgMat = nullptr;
	cv::Mat m_showImgMat;
	ChangedQueue m_qChanged;
	// 包含每个图像处理变量的初始化值
	QMap<QString, QVariant> m_mInitValue; 
	inline static std::shared_ptr<spdlog::logger> m_pLogger = spdlog::stdout_color_mt("ImageView");
};

#endif // IMAGEVIEWMODEL_H
