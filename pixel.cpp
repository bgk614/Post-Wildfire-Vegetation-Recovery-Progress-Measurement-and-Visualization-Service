#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

void resizeImage(Mat& img);
void removeMultiplicativeNoise(Mat& img);
void getGreen(Mat& img, vector<int>& green_pixel);
void calculateGreenSimilarPercent(vector<int>& green_pixel, vector<double>& green_similar_percent);
void calculateGreenProgress(vector<int>& green_pixel, vector<double>& green_progress);
void saveChangeImage(vector<int>& green_pixel);
void jsonReport();
void updatePaths(int& img_num);
bool isInColorRange(const Vec3b& pixel, const Scalar& lower_bound, const Scalar& upper_bound);
bool isWhite(const Vec3b& color);

vector<int> green_pixel;
vector<double> green_similar_percent, green_progress;

const Vec3b WHITE  = Vec3b(255, 255, 255);
const Vec3b ORANGE = Vec3b(0, 114, 255);
const Vec3b BLUE = Vec3b(208, 146, 8);
int min_width  = INT_MAX;
int min_height = INT_MAX;
int img_num = 1; 
Mat img;

string src_output_path    = "./project/srcImage/srcImage_" + to_string(img_num) + ".jpg";
string green_output_path  = "./project/greenPixelImage/greenArea_" + to_string(img_num) + ".jpg";
string before_path        = "./project/greenPixelImage/greenArea_" + to_string(img_num - 1) + ".jpg";
string change_output_path = "./project/changeImage/changeArea_" + to_string(img_num - 1) + "_" + to_string(img_num) + ".jpg";

int main(int argc, char* argv[]) 
{
    for (int i = 1; i < argc; i++) {
        img = imread(argv[i]);
        if (img.empty()) {
            cerr << "Image load failed: " << argv[i] << endl;
            return -1;
        }
        if (img.cols < min_width)  min_width  = img.cols;
        if (img.rows < min_height) min_height = img.rows;
    }
    
    green_pixel.clear();
    green_progress.clear();
    green_similar_percent.clear();

    for (img_num = 1; img_num < argc; img_num++) {
        img = imread(argv[img_num]);
        updatePaths(img_num);
        resizeImage(img);
        removeMultiplicativeNoise(img);
        imwrite(src_output_path, img);
        getGreen(img, green_pixel);
    }
    for (img_num = 2; img_num < argc; img_num++) {
        updatePaths(img_num);
        calculateGreenSimilarPercent(green_pixel, green_similar_percent);
        calculateGreenProgress(green_pixel, green_progress);
        saveChangeImage(green_pixel);
    }
    jsonReport();
    return 0;
}

void resizeImage(Mat& img) {
    resize(img, img, Size(min_width, min_height));
}

void removeMultiplicativeNoise(Mat& img) {
    GaussianBlur(img, img, Size(3, 3), 0);
}

void getGreen(Mat& img, vector<int>& green_pixel) {
    int green_count = 0;
    Mat img_hsv   = img.clone();
    Mat green_img = img.clone();                // 초록색 픽셀을 저장할 이미지

    cvtColor(img, img_hsv, COLOR_BGR2HSV);      // 이미지를 HSV 색 공간으로 변환 
    
    // HSV에서 초록색 범위 설정
    Scalar lower_green = Scalar(32, 44, 30);     // 낮은 경계값 (H, S, V)
    Scalar upper_green = Scalar(100, 255, 255);  // 높은 경계값 (H, S, V)

    for (int j = 0; j < img_hsv.rows; j++) {
        for (int i = 0; i < img_hsv.cols; i++) {
            Vec3b hsv_pixel = img_hsv.at<Vec3b>(j, i);
            
            // HSV에서 초록색 픽셀 범위 확인
            if (isInColorRange(hsv_pixel, lower_green, upper_green)) {
                green_count++;
            } else {
                green_img.at<Vec3b>(j, i) = WHITE;
            }
        }
    }   
    green_pixel.push_back(green_count);
    imwrite(green_output_path, green_img);
}

bool isInColorRange(const Vec3b& pixel, const Scalar& lower_bound, const Scalar& upper_bound) {
    return (lower_bound[0] <= pixel[0] && pixel[0] <= upper_bound[0]) &&
           (lower_bound[1] <= pixel[1] && pixel[1] <= upper_bound[1]) &&
           (lower_bound[2] <= pixel[2] && pixel[2] <= upper_bound[2]);
}

void calculateGreenSimilarPercent(vector<int>& green_pixel, vector<double>& green_similar_percent) {
    if (green_pixel[0] != 0) {
        double green_similarity_rate = ((double)(green_pixel[img_num - 1]) / (green_pixel[0])) * 100;
        green_similar_percent.push_back(green_similarity_rate);
    } else {
        green_similar_percent.push_back(0); // 적절한 기본값 처리
    }
}

void calculateGreenProgress(vector<int>& green_pixel, vector<double>& green_progress) {
    if (green_pixel[0] != 0) {
        double progress = ((double)(green_pixel[img_num - 1] - green_pixel[img_num - 2]) / green_pixel[0]) * 100;
        green_progress.push_back(progress);
    } else {
        green_progress.push_back(0); // 적절한 기본값 처리
    }
}

void saveChangeImage(vector<int>& green_pixel) {
    Mat before_img = imread(before_path);
    Mat after_img  = imread(green_output_path);

    Mat change_img = Mat::zeros(img.size(), CV_8UC3);

    for (int j = 0; j < before_img.rows; j++) {
        for (int i = 0; i < before_img.cols; i++) {
            Vec3b& before_green =  before_img.at<Vec3b>(j, i);
            Vec3b& after_green  =  after_img.at<Vec3b>(j, i);
            if (green_pixel[img_num - 2] < green_pixel[img_num - 1]) {
                if (isWhite(before_green) && !isWhite(after_green)) {
                    change_img.at<Vec3b>(j, i) = ORANGE;
                } else {
                    change_img.at<Vec3b>(j, i) = WHITE;
                }
            } else {
                if (!isWhite(before_green) && isWhite(after_green)) {
                    change_img.at<Vec3b>(j, i) = BLUE;
                } else {
                    change_img.at<Vec3b>(j, i) = WHITE;
                }
            }
        }
    }
    imwrite(change_output_path, change_img);
}

bool isWhite(const Vec3b& color) {
    return color[0] == 255 && color[1] == 255 && color[2] == 255;
}

void jsonReport() {
    cout << "{";
        
        cout << "\"all_pixel\": [" << (img.cols * img.rows) << "],";

        cout << "\"green_pixel\": [";
        for (size_t i = 0; i < green_pixel.size(); ++i) {
            cout << green_pixel[i] << (i < green_pixel.size() - 1 ? ", " : "");
        }
        cout << "],";

        cout << "\"green_similar_percent\": [";
        for (size_t i = 0; i < green_similar_percent.size(); ++i) {
            cout << green_similar_percent[i] << (i < green_similar_percent.size() - 1 ? ", " : "");
        }
        cout << "],";

        cout << "\"green_progress\": [";
        for (size_t i = 0; i < green_progress.size(); ++i) {
            cout << green_progress[i] << (i < green_progress.size() - 1 ? ", " : "");
        }
        cout << "]"; 

    cout << "}";
}

void updatePaths(int& img_num) {
    src_output_path    = "./project/srcImage/srcImage_" + to_string(img_num) + ".jpg";
    green_output_path  = "./project/greenPixelImage/greenArea_" + to_string(img_num) + ".jpg";
    before_path        = "./project/greenPixelImage/greenArea_" + to_string(img_num - 1) + ".jpg";
    change_output_path = "./project/changeImage/changeArea_" + to_string(img_num - 1) + "_" + to_string(img_num) + ".jpg";
}
