#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

void getGreen(Mat src, vector<int>& all_pixel, vector<int>& green_pixel, vector<double>& green_percent, vector<double>& green_progress);
void calculateGreenPercent(Mat src, int& gCount, vector<double>& green_percent);
void calculateGreenProgress(vector<int>& green_pixel, vector<double>& green_progress); 
void drawBarGraph(const vector<double>& data, const string& graphTitle);

int main() 
{
    // 값을 저장할 벡터
    vector<int> all_pixel;
    vector<int> green_pixel;
    vector<double> green_percent;
    vector<double> green_progress;

    Mat src0 = imread("강원도0.png");   // 불 나기 전   2018.07
    Mat src1 = imread("강원도1.png");   // 불난 직후    2019.04
    Mat src2 = imread("강원도2.png");   // 1년 후      2020.02
    Mat src3 = imread("강원도3.png");   // 현재        2020.10
    
    // 이미지 저장 벡터
    vector<Mat> images = {src0, src1, src2, src3};

    for (const auto& img : images) {
        if(img.empty()) {
            cerr << "Image load failed." << endl;
            return -1;
        }
    }

    for (const auto& img : images) {
        getGreen(img, all_pixel, green_pixel, green_percent, green_progress);
    }
    
    // 그래프 그리기
    drawBarGraph(vector<double>(all_pixel.begin(), all_pixel.end()), "All Pixels");
    drawBarGraph(vector<double>(green_pixel.begin(), green_pixel.end()), "Green Pixels");
    drawBarGraph(green_percent, "Green Percent");
    drawBarGraph(vector<double>(green_progress.begin(), green_progress.end()), "Green Progress");

    // 결과 출력
    for(int i = 0; i < all_pixel.size(); i++) {
        if (i == 0) 
            cout << "== Begore a wildfire ==" << endl;
        else if (i == 1) 
            cout << "== After a wildfire ==" << endl;
        else if (i == all_pixel.size() - 1) 
            cout << "==     Present     ==" << endl;
        else 
            cout << "== " << i << " Years later ==" << endl;

        cout << "   All pixel : " << all_pixel[i] << endl;
        cout << "   Green pixel : " << green_pixel[i] << endl;
        cout << "   Green percent " << green_percent[i] << "%" << endl;
    }

    for(int i = 0; i < green_progress.size(); i++) {
        cout << "   Green progress : " << green_progress[i] << "%" << endl;
    }

    waitKey();
    destroyAllWindows();

    return 0;
}

int i = 0;  // 이미지 번호

void getGreen(Mat src, vector<int>& all_pixel, vector<int>& green_pixel, vector<double>& green_percent, vector<double>& green_progress) {
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

    // 결과 이미지를 표시
    imshow("Original Img" + to_string(i), src);
    imshow("Result Img" + to_string(i), resultImg);

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

// 그래프 그리기
void drawBarGraph(const vector<double>& data, const string& graphTitle) {
    int graphHeight = 400;
    int graphWidth = 400;
    int barWidth = cvRound((double)graphWidth / data.size());
    int axisThick = 2;

    Mat graphImg(graphHeight + 20, graphWidth + 40, CV_8UC3, Scalar(255, 255, 255));

    // 데이터의 최대값을 찾아서 그래프의 스케일을 결정
    double maxVal = *max_element(data.begin(), data.end());

    // 막대 그리기
    for (size_t i = 0; i < data.size(); i++) {
        int barHeight = cvRound((data[i] / maxVal) * (graphHeight - 150));
        rectangle(graphImg, Point(i * barWidth + 21, graphHeight - barHeight),
                  Point((i + 1) * barWidth, graphHeight), Scalar(100, 200, 100), FILLED);
    }

    // x축과 y축
    line(graphImg, Point(20, 0), Point(20, graphHeight), Scalar(0, 0, 0), axisThick);
    line(graphImg, Point(20, graphHeight), Point(graphWidth + 20, graphHeight), Scalar(0, 0, 0), axisThick);

    // 축 레이블 추가
    for (size_t i = 0; i < data.size(); i++) {
        int num = cvRound(data[i]);
        string text = to_string(num);
        putText(graphImg, text, Point(i * barWidth + 25, graphHeight - 5), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255));
    }

    // 그래프 제목 추가
    putText(graphImg, graphTitle, Point(20, 15), FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));

    // 그래프 이미지
    imshow(graphTitle, graphImg);
}
