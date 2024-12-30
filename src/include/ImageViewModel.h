#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H
#include <opencv2/opencv.hpp>
#include <QObject>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h> // 用于彩色输出控制台日志
#include <spdlog/async.h> // 用于异步日志记录（如果使用多线程）

class ImageViewModel : public QObject
{
    Q_OBJECT

public:
    explicit ImageViewModel(QObject* parent = nullptr);

    Q_INVOKABLE void openImage(QString filePath); // 打开图片
    std::shared_ptr<cv::Mat> imgMat() const;
    void setImgMat(const cv::Mat& imgMat);

signals:
    void imageChanged(ImageViewModel* viewModel);

private:
    std::shared_ptr<cv::Mat> m_pImgMat;
    inline static std::shared_ptr<spdlog::logger> m_pLogger = spdlog::stdout_color_mt("ImageViewModel");
};

#endif // IMAGEMODEL_H
