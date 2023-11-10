#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {

    // �̹��� �ҷ�����
    Mat src = imread("������.png");  //�ֿ��� ������ �̹���

    //�̹��� �ҷ����� �������� ��
    if (src.empty()) {
        cerr << "�̹����� �ҷ��� �� �����ϴ�." << endl;
        return -1;
    }

    // �̹����� HSV �� �������� ��ȯ
    Mat hsvImage;
    cvtColor(src, hsvImage, COLOR_BGR2HSV);


    // �ʷϻ� �ȼ��� ������ �̹���
    Mat greenPixels = Mat::zeros(src.size(), CV_8UC1);


    //�׸�ī��Ʈ
    int greenCount = 0;


    // HSV���� �ʷϻ� ���� ����
    Scalar lowerGreen = Scalar(30, 30, 30);  // ���� ��谪 (H, S, V)
    Scalar upperGreen = Scalar(80, 255, 255);  // ���� ��谪 (H, S, V)


    // �ʷϻ� �ȼ� ����
    for (int j = 0; j < src.rows; j++) {
        for (int i = 0; i < src.cols; i++) {
            Vec3b hsvPixel = hsvImage.at<Vec3b>(j, i);

            // HSV���� �ʷϻ� �ȼ� ���� Ȯ��
            if ((lowerGreen[0] <= hsvPixel[0] && hsvPixel[0] <= upperGreen[0]) &&
                (lowerGreen[1] <= hsvPixel[1] && hsvPixel[1] <= upperGreen[1]) &&
                (lowerGreen[2] <= hsvPixel[2] && hsvPixel[2] <= upperGreen[2])) {
                
                // �ʷϻ� �ȼ��̸� �׸�ī��Ʈ++
                greenCount++;
                greenPixels.at<uchar>(j, i) = 255;  // ������� ����(����ũ �̹������� �ʷϻ� �κ��� ��Ÿ���� ����)
            }
        }
    }

    cout << "Ž���� ��� �ȼ�: " << greenCount << endl;


    // �ʷϻ��� �ƴ� �κ��� ȸ������ ����
    Mat resultImg = src.clone();
    for (int j = 0; j < src.rows; j++) {
        for (int i = 0; i < src.cols; i++) {
            if (greenPixels.at<uchar>(j, i) == 0) {
                resultImg.at<Vec3b>(j, i) = Vec3b(128, 128, 128);  // ȸ������ ����
            }
        }
    }

    // ��� �̹����� ǥ��
    imshow("Original Img", src);
    imshow("Result Img", resultImg);

    waitKey(0);

    return 0;
}

//greenCount �� ���� �ʷ� ������ �ȼ� ���� �˾Ƴ��� �� �������� �� �ۼ�Ʈ ���̰� ������ �˾ƾ��ϱ� ������ �ʿ��ϴ�....

/*

�ʹݿ� ���� rgb�� �̿��� �� �����ϱ�,,,
Ǯ�� �ʿ������� ����� ��絵 ������� ���ƾ� �ʷϻ��� �� �����µ�
�̹����� ���� ������ �� �� ������ �ٲٸ� �������� ���� ſ�� ������ ���� ������ �Ǵ�����?
�׷��� hsv�� �����ϴ� ������ �ٲ���ϴ�.

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void main()
{
    //�̹��� �ҷ�����
    Mat src = imread("�̱�2017.png");

    //�̹��� �ҷ����� ���н�
    if (src.empty()) {
        cerr << "�̹����� �ҷ��� �� �����ϴ�!!!" << endl;
        return;
    }

    //�̹����� ���� ����
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

    cout << "Ž���� ��� �ȼ�: " << greenCount << endl;



    // ��� �̹��� ǥ��

        imshow("green", src);


        waitKey();
        destroyWindow;
}


*/
