#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H
#include <opencv2/opencv.hpp>
#include <QObject>

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
};

#endif // IMAGEMODEL_H
