#include "KBridge.h"

#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QFont>
#include <QTextCodec>
#include <QApplication>

QImage KBridge::cvMat2QImage(const cv::Mat &mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if (mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for (int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if (mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if (mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat ARGB is mean Alpha(透明度) RGB
        QImage image(pSrc, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat KBridge::QImage2cvMat(const QImage &image)
{
    cv::Mat mat;
    qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

std::string KBridge::QString2stdstring(const QString &qstr)
{
    std::string str = qstr.toStdString();
#ifdef WIN32
    QTextCodec *code = QTextCodec::codecForName("gb18030");
    if (code) str = code->fromUnicode(qstr).data();
#endif  //WIN32
    return str;
}

QString KBridge::stdstring2QString(const std::string &str)
{
    return QString::fromLocal8Bit(str.c_str());
}

cv::Mat KBridge::imread(const QString &fileName)
{
    std::string stdFileName = QString2stdstring(fileName);
    cv::Mat mat = cv::imread(stdFileName);
    return mat;
}

void KBridge::imwrite(const QString &fileName, const cv::Mat &mat)
{
    std::string stdFileName = QString2stdstring(fileName);
    cv::imwrite(stdFileName, mat);
}

void KBridge::putText(cv::Mat &mat, const QString &text, const QPoint &org, const QColor &color, int fontSize)
{
    int padding = 10;
    QPainter painter;
    QFont font = qApp->font();
    if (fontSize > 0) font.setPointSize(fontSize);

    QSize size(
        QFontMetrics(font).width(text) + padding * 2,
        QFontMetrics(font).height() + padding * 2);

    QPixmap pixmap(size);
    pixmap.fill(QColor(0, 0, 0, 0));

    painter.begin(&pixmap);

    painter.setFont(font);
    painter.setPen(QPen(color));
    painter.setBrush(QBrush(color));
    painter.drawText(padding, size.height() - padding, text);

    painter.end();

    int mw = mat.size().width;
    int mh = mat.size().height;

    QImage qimg = pixmap.toImage();
    for (int i = 0; i < qimg.width(); ++i)
    {
        for (int j = 0; j < qimg.height(); ++j)
        {
            QRgb pixel = qimg.pixel(i, j);
            int a = (pixel >> 24) & 0xff;
            int r = (pixel >> 16) & 0xff;
            int g = (pixel >> 8) & 0xff;
            int b = (pixel) & 0xff;

            if (a <= 0) continue;

            double fa = a / 255.0;
            int mx = i + org.x() - padding;
            int my = j + org.y() - size.height() + padding;

            if (mx < 0 || mx >= mw || my < 0 || my >= mh) continue;

            uchar &mb = mat.at<cv::Vec3b>(my, mx)[0];
            uchar &mg = mat.at<cv::Vec3b>(my, mx)[1];
            uchar &mr = mat.at<cv::Vec3b>(my, mx)[2];

            // cv::circle(mat, cv::Point(i, j), 0, CV_RGB(0, 255, 0), -1);

            mb = mb * (1 - fa) + b * fa;
            mg = mg * (1 - fa) + g * fa;
            mr = mr * (1 - fa) + r * fa;
        }
    }
}
