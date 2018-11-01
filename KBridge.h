#ifndef KBRIDGE_H
#define KBRIDGE_H

#include <opencv2/opencv.hpp>
#include <QImage>

/**
 * @brief The KBridge class 用作 Qt 和 OpenCV 数据转换的通道
 */
class KBridge
{
public:
    /**
     * @brief cvMat2QImage 将 cv::Mat 转化为 QImage
     * @param mat 待转化的 cv::Mat
     * @return 转化后的 QImage
     */
    static QImage cvMat2QImage(const cv::Mat &mat);
    /**
     * @brief QImage2cvMat 将 QImage 转化为 cv::Mat
     * @param image 待转化的 QImage
     * @return 转化后的 cv::Mat
     */
    static cv::Mat QImage2cvMat(const QImage &image);
    /**
     * @brief QString2stdstring 将 QString 转化为 std::string
     * @param qstr 待转化的 QString
     * @return 转化后的 std::string
     */
    static std::string QString2stdstring(const QString &qstr);
    /**
     * @brief stdstring2QString 将 std::string 转化为 QString
     * @param str 待转化的 std::string
     * @return 转化后的 QString
     */
    static QString stdstring2QString(const std::string &str);
    /**
     * @brief imread 读取一张图片，作用与 cv::imread 类似
     * @param fileName 读取的图片名
     * @return 以 cv::Mat 保存的图片
     */
    static cv::Mat imread(const QString &fileName);
    /**
     * @brief imwrite 保存一张图片，作用与 cv::imwrite 类似
     * @param fileName 写入的文件名
     * @param mat 以 cv::Mat 保存的图片
     */
    static void imwrite(const QString &fileName, const cv::Mat &mat);
    /**
     * @brief putText 在 cv::Mat 上显示文字，与 cv::putText 类似，但支持中文
     * @param mat 被写入的图片
     * @param text 待写入的文字
     * @param org 文字左下角坐标
     * @param color 文字颜色
     * @note mat 类型要是 CV_8UC3 格式的，否则会出现不可预测的结果
     */
    static void putText(cv::Mat &mat, const QString &text, const QPoint &org, const QColor &color, int fontSize = -1);

};

#endif // KBRIDGE_H
