#include "img.hpp"
#include <iostream>
#include <stdexcept>

Img::Img() {
    // Constructor - img is automatically initialized as empty
}
Img& Img::read(const std::string& path,
               const std::pair<int, int>& size,
               bool keep_aspect,
               int interpolation)
{
    img = cv::imread(path, cv::IMREAD_UNCHANGED);

    if (img.empty())
    {
        throw std::runtime_error("Cannot load image: " + path);
    }

    // המרה לפורמט אחיד BGRA (4 channels)
    switch (img.channels())
    {
        case 1:
            cv::cvtColor(img, img, cv::COLOR_GRAY2BGRA);
            break;

        case 3:
            cv::cvtColor(img, img, cv::COLOR_BGR2BGRA);
            break;

        case 4:
            // כבר בפורמט הרצוי
            break;

        default:
            throw std::runtime_error("Unsupported image format: " + path);
    }

    // שינוי גודל במידת הצורך
    if (size.first > 0 && size.second > 0)
    {
        int target_w = size.first;
        int target_h = size.second;

        if (keep_aspect)
        {
            double scale = std::min(
                static_cast<double>(target_w) / img.cols,
                static_cast<double>(target_h) / img.rows);

            int new_w = static_cast<int>(img.cols * scale);
            int new_h = static_cast<int>(img.rows * scale);

            cv::resize(
                img,
                img,
                cv::Size(new_w, new_h),
                0,
                0,
                interpolation);
        }
        else
        {
            cv::resize(
                img,
                img,
                cv::Size(target_w, target_h),
                0,
                0,
                interpolation);
        }
    }

    return *this;
}
// Img& Img::read(const std::string& path,
//                const std::pair<int, int>& size,
//                bool keep_aspect,
//                int interpolation) {
//     img = cv::imread(path, cv::IMREAD_UNCHANGED);
//     if (img.empty()) {
//         throw std::runtime_error("Cannot load image: " + path);
//     }

//     if (size.first != 0 && size.second != 0) {  // Check if size is not empty
//         int target_w = size.first;
//         int target_h = size.second;
//         int h = img.rows;
//         int w = img.cols;

//         if (keep_aspect) {
//             double scale = std::min(static_cast<double>(target_w) / w, 
//                                    static_cast<double>(target_h) / h);
//             int new_w = static_cast<int>(w * scale);
//             int new_h = static_cast<int>(h * scale);
//             cv::resize(img, img, cv::Size(new_w, new_h), 0, 0, interpolation);
//         } else {
//             cv::resize(img, img, cv::Size(target_w, target_h), 0, 0, interpolation);
//         }
//     }

//     return *this;
// }
void Img::draw_on(Img& other_img, int x, int y) {
    if (img.empty() || other_img.img.empty()) return;

    // ����� ������� ������� ��� ���� 4 ������ (BGRA) ��� ����� ���� ����
    cv::Mat source = img;
    cv::Mat target = other_img.img;

    // ����� ������ �-4 ������ (B, G, R, Alpha)
    std::vector<cv::Mat> channels;
    cv::split(source, channels);

    // ����� ����� (Mask) ���� ���� ����� (����� �-4)
    cv::Mat mask = channels[3];

    // ����� ����� �������� �� ���� (ROI)
    cv::Mat roi = target(cv::Rect(x, y, source.cols, source.rows));

    // ����� �� ����� ������
    // �� ������� ������ ���� ��� 255 ������ ����� ����, �������� ������ �� ����� �� ����
    source.copyTo(roi, mask);
}
//void Img::draw_on(Img& other_img, int x, int y) {
//    if (img.empty() || other_img.img.empty()) {
//        throw std::runtime_error("Both images must be loaded before drawing.");
//    }
//
//    cv::Mat source_img = img;
//    cv::Mat target_img = other_img.img;
//
//    // ������� ���� ������� �� ����� ����� (���� BGR) ��� ����� ��-������
//    if (source_img.channels() == 4) {
//        cv::cvtColor(source_img, source_img, cv::COLOR_BGRA2BGR);
//    }
//    if (target_img.channels() == 4) {
//        cv::cvtColor(target_img, target_img, cv::COLOR_BGRA2BGR);
//    }
//
//    int h = source_img.rows;
//    int w = source_img.cols;
//    int H = target_img.rows;
//    int W = target_img.cols;
//
//    if (y + h > H || x + w > W) {
//        throw std::runtime_error("Image does not fit at the specified position.");
//    }
//
//    // ����� ����� �� ������ ���� ����
//    source_img.copyTo(target_img(cv::Rect(x, y, w, h)));
//}
void Img::draw_transparent(cv::Mat& target, int x, int y) {
    cv::Mat& source = this->img;
    for (int i = 0; i < source.rows; ++i) {
        for (int j = 0; j < source.cols; ++j) {
            
            if (y + i >= target.rows || x + j >= target.cols) continue;

            cv::Vec4b& src_pixel = source.at<cv::Vec4b>(i, j);
            float alpha = src_pixel[3] / 255.0f;

            if (alpha > 0) {
                cv::Vec4b& dst_pixel = target.at<cv::Vec4b>(y + i, x + j);
                dst_pixel[0] = src_pixel[0] * alpha + dst_pixel[0] * (1 - alpha);
                dst_pixel[1] = src_pixel[1] * alpha + dst_pixel[1] * (1 - alpha);
                dst_pixel[2] = src_pixel[2] * alpha + dst_pixel[2] * (1 - alpha);
                
            }
        }
    }
}
void Img::put_text(const std::string& txt, int x, int y, double font_size,
                   const cv::Scalar& color, int thickness) {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    
    cv::putText(img, txt, cv::Point(x, y),
                cv::FONT_HERSHEY_SIMPLEX, font_size,
                color, thickness, cv::LINE_AA);
}

void Img::show() {
    if (img.empty()) {
        throw std::runtime_error("Image not loaded.");
    }
    
    cv::imshow("Image", img);
    cv::waitKey(0);
    cv::destroyAllWindows();
} 