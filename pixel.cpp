#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

void getGreen(Mat src, vector<int>& all_pixel, vector<int>& green_pixel, vector<double>& green_percent, vector<double>& green_progress);
void calculateGreenPercent(Mat src, int& gCount, vector<double>& green_percent);
void calculateGreenProgress(vector<int>& green_pixel, vector<double>& green_progress); 
void removeMultiplicativeNoise(Mat& img);
string saveGreenImage(Mat& resultImg, int& index);
string saveChangeImage(Mat& afterImg, int& index);

int i = 0; 

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <image1> <image2> ..." << endl;
        return 1;
    }

    vector<int> all_pixel, green_pixel;
    vector<double> green_percent, green_progress;
    vector<string> savedImages; 

    for (int i = 1; i < argc; i++) {
        Mat img = imread(argv[i]);
        if (img.empty()) {
            cerr << "Image load failed: " << argv[i] << endl;
            return -1;
        }

        getGreen(img, all_pixel, green_pixel, green_percent, green_progress);
        savedImages.push_back(saveGreenImage(img, i));
    }

        cout << "{";

        cout << "\"all_pixel\": [";
        for (size_t i = 0; i < all_pixel.size(); ++i) {
            cout << all_pixel[i] << (i < all_pixel.size() - 1 ? ", " : "");
        }
        cout << "],";

        cout << "\"green_pixel\": [";
        for (size_t i = 0; i < green_pixel.size(); ++i) {
            cout << green_pixel[i] << (i < green_pixel.size() - 1 ? ", " : "");
        }
        cout << "],";

        cout << "\"green_percent\": [";
        for (size_t i = 0; i < green_percent.size(); ++i) {
            cout << green_percent[i] << (i < green_percent.size() - 1 ? ", " : "");
        }
        cout << "],";

        cout << "\"green_progress\": [";
        for (size_t i = 0; i < green_progress.size(); ++i) {
            cout << green_progress[i] << (i < green_progress.size() - 1 ? ", " : "");
        }
        cout << "],"; 

        cout << "\"savedImages\": [";
        for (size_t i = 0; i < savedImages.size(); ++i) {
            cout << "\"" << savedImages[i] << "\"" << (i < savedImages.size() - 1 ? ", " : "");
        }
        cout << "]";

        cout << "}";

    return 0;
}

// 가우시안 블러를 적용하여 고주파 잡음을 완화
void removeMultiplicativeNoise(Mat& img) {
    GaussianBlur(img, img, Size(5, 5), 0);
}

// 사진의 녹색 픽셀 추출
void getGreen(Mat src, vector<int>& all_pixel, vector<int>& green_pixel, vector<double>& green_percent, vector<double>& green_progress) {
      removeMultiplicativeNoise(src);                       // 이미지에 multiplicative noise 제거
    Mat src_hsv;                                            // 이미지를 HSV 색 공간으로 변환
    cvtColor(src, src_hsv, COLOR_BGR2HSV);

    Mat src_green = Mat::zeros(src_hsv.size(), CV_8UC1);    // 초록색 픽셀을 저장할 이미지

    // HSV에서 초록색 범위 설정
    Scalar lowerGreen = Scalar(30, 30, 30);                 // 낮은 경계값 (H, S, V)
    Scalar upperGreen = Scalar(90, 255, 255);               // 높은 경계값 (H, S, V)

    int gCount = 0;
    for (int j = 0; j < src.rows; j++) {
        for (int i = 0; i < src.cols; i++) {
            Vec3b hsvPixel = src_hsv.at<Vec3b>(j, i);

            // HSV에서 초록색 픽셀 범위 확인
            if ((lowerGreen[0] <= hsvPixel[0] && hsvPixel[0] <= upperGreen[0]) &&
                (lowerGreen[1] <= hsvPixel[1] && hsvPixel[1] <= upperGreen[1]) &&
                (lowerGreen[2] <= hsvPixel[2] && hsvPixel[2] <= upperGreen[2])) {

                gCount++;                           // 초록색 픽셀이면 그린카운트++
                src_green.at<uchar>(j, i) = 255;    // 흰색으로 설정(마스크 이미지에서 초록색 부분을 나타내기 위해)
            }
        }
    }

    int allpixel = (src.rows * src.cols);

    all_pixel.push_back(allpixel);
    green_pixel.push_back(gCount);

    Mat resultImg = src.clone();
    
    // 초록색이 아닌 부분은 흰색으로 설정
    for (int j = 0; j < src.rows; j++) {
        for (int i = 0; i < src.cols; i++) {
            if (src_green.at<uchar>(j, i) == 0) {
                resultImg.at<Vec3b>(j, i) = Vec3b(255, 255, 255);
            }
        }
    }

    calculateGreenPercent(src, gCount, green_percent);
    calculateGreenProgress(green_pixel, green_progress);

    string result = saveGreenImage(resultImg, i);

    // 2번째 마다 차이 이미지 생성
    if (i > 1) { string changeResult = saveChangeImage(resultImg, i); }

    i++;
}

// 사진의 녹섹 퍼센트 계산
void calculateGreenPercent(Mat src, int& gCount, vector<double>& green_percent) {
    double green_pc = ((double)gCount / (double)(src.rows * src.cols)) * 100;
    green_percent.push_back(green_pc);
}

// 변화율 계산
void calculateGreenProgress(vector<int> &green_pixel, vector<double> &green_progress)
{
    green_progress.clear();
    double gprogress;
    for(int i = 1; i < green_pixel.size() - 1; i++) {
        gprogress = ((double)(green_pixel[i+1] - green_pixel[i]) / green_pixel[0]) * 100;

        green_progress.push_back(gprogress);
    }
}

// 초록색 픽셀만 추출한 이미지를 저장하고 경로를 반환하는 부분
string saveGreenImage(Mat& resultImg, int& index) {
    string outputPath = "./greenArea_" + to_string(index) + ".jpg";
    imwrite(outputPath, resultImg);     // 결과 이미지를 파일로 저장
    return outputPath;                  // 저장된 이미지의 경로 반환
}

// 차이 이미지를 저장하고 경로를 반환하는 부분
string saveChangeImage(Mat& afterImg, int& index) {
    string beforPath = "greenArea_" + to_string(index - 1) + ".jpg";
    Mat beforImg = imread(beforPath);
    Mat changeImg = Mat::zeros(beforImg.size(), CV_8UC3);

    for (int j = 0; j < beforImg.rows; j++) {
        for (int i = 0; i < beforImg.cols; i++) {
            Vec3b& BeforeGreen = beforImg.at<Vec3b>(j, i);
            Vec3b& AfterGreen = afterImg.at<Vec3b>(j, i);
            Vec3b& changeGreen = changeImg.at<Vec3b>(j, i);

            if ((BeforeGreen[0] == 255 && BeforeGreen[1] == 255 && BeforeGreen[2] == 255) &&
                (AfterGreen[0] != 255 && AfterGreen[1] != 255 && AfterGreen[2] != 255)) {
                changeGreen = Vec3b(0, 114, 255);
            }
            else { changeGreen = Vec3b(255, 255, 255); }
        }
    }

    string outputPath = "./changeArea_" + to_string(index - 1) + "_" + to_string(index) + ".jpg";
    imwrite(outputPath, changeImg);
    return outputPath;
}
