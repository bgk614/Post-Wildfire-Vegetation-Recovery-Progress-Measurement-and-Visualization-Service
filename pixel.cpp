#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {

    // 이미지 불러오기
    Mat src = imread("강원도.png");  //주영이 강원도 이미지

    //이미지 불러오기 실패했을 때
    if (src.empty()) {
        cerr << "이미지를 불러올 수 없습니다." << endl;
        return -1;
    }

    // 이미지를 HSV 색 공간으로 변환
    Mat hsvImage;
    cvtColor(src, hsvImage, COLOR_BGR2HSV);


    // 초록색 픽셀을 저장할 이미지
    Mat greenPixels = Mat::zeros(src.size(), CV_8UC1);


    //그린카운트
    int greenCount = 0;


    // HSV에서 초록색 범위 설정
    Scalar lowerGreen = Scalar(30, 30, 30);  // 낮은 경계값 (H, S, V)
    Scalar upperGreen = Scalar(80, 255, 255);  // 높은 경계값 (H, S, V)


    // 초록색 픽셀 추출
    for (int j = 0; j < src.rows; j++) {
        for (int i = 0; i < src.cols; i++) {
            Vec3b hsvPixel = hsvImage.at<Vec3b>(j, i);

            // HSV에서 초록색 픽셀 범위 확인
            if ((lowerGreen[0] <= hsvPixel[0] && hsvPixel[0] <= upperGreen[0]) &&
                (lowerGreen[1] <= hsvPixel[1] && hsvPixel[1] <= upperGreen[1]) &&
                (lowerGreen[2] <= hsvPixel[2] && hsvPixel[2] <= upperGreen[2])) {
                
                // 초록색 픽셀이면 그린카운트++
                greenCount++;
                greenPixels.at<uchar>(j, i) = 255;  // 흰색으로 설정(마스크 이미지에서 초록색 부분을 나타내기 위해)
            }
        }
    }

    cout << "탐지된 녹색 픽셀: " << greenCount << endl;


    // 초록색이 아닌 부분은 회색으로 설정
    Mat resultImg = src.clone();
    for (int j = 0; j < src.rows; j++) {
        for (int i = 0; i < src.cols; i++) {
            if (greenPixels.at<uchar>(j, i) == 0) {
                resultImg.at<Vec3b>(j, i) = Vec3b(128, 128, 128);  // 회색으로 설정
            }
        }
    }

    // 결과 이미지를 표시
    imshow("Original Img", src);
    imshow("Result Img", resultImg);

    waitKey(0);

    return 0;
}

//greenCount 는 여러 초록 사진의 픽셀 값을 알아내서 각 사진마다 몇 퍼센트 차이가 나는지 알아야하기 때문에 필요하다....

/*

초반에 만든 rgb를 이용한 색 추출하기,,,
풀밭 초원에서는 레드랑 블루도 어느정도 높아야 초록색이 다 나오는데
이미지를 갈색 나무가 좀 들어간 숲으로 바꾸면 빨간색이 높은 탓에 나무도 같이 추출이 되더라고용?
그래서 hsv로 추출하는 것으로 바꿨습니다.

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void main()
{
    //이미지 불러오기
    Mat src = imread("미국2017.png");

    //이미지 불러오기 실패시
    if (src.empty()) {
        cerr << "이미지를 불러올 수 없습니다!!!" << endl;
        return;
    }

    //이미지의 폭과 높이
    int w = src.cols;
    int h = src.rows;

    int greenCount = 0;




    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            Vec3b& pixel = src.at<Vec3b>(j, i);
            uchar b = pixel[0];
            uchar g = pixel[1];
            uchar r = pixel[2];

            uchar gray = (uchar)((b + g + r) / 3);

            if ((50 <= g && g <= 255) && (b <= 60) && (r <= 60)) {
                greenCount++;
            }
            else {
                pixel[0] = gray;
                pixel[1] = gray;
                pixel[2] = gray;
            }
        }
    }

    cout << "탐지된 녹색 픽셀: " << greenCount << endl;



    // 결과 이미지 표시

        imshow("green", src);


        waitKey();
        destroyWindow;
}


*/
