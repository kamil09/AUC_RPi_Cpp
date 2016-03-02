#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
#include <iostream>

using namespace cv;
using namespace std;



int main()
{
  cout << "otrzymywanie obrazu z kamerki" << endl;
    VideoCapture obrazZKamery = VideoCapture(0);
    if(!obrazZKamery.isOpened()){
      CV_Assert("Nie udalo sie polaczyc z kamera");
    }
    obrazZKamery.set(CV_CAP_PROP_FRAME_WIDTH,480);
    obrazZKamery.set(CV_CAP_PROP_FRAME_HEIGHT,320);
    int fps = 1000/22;
    // string window_name [] = { "obraz BGR","obraz binarnyBGR","obraz HSV", "obraz binarnyHSV" };
    string window_name [] = {"kontrola obrazu","obraz binarny" };
    Mat frame, img, hsv_img, binaryBGR,binaryHSV;
    vector<Mat> bgr_split,hsv_split;
    for ( int i = 0; i < 2; i++ ) namedWindow(window_name[i], CV_WINDOW_AUTOSIZE);
    int lowerHUE = 61, upperHUE = 119;
    int lowerSATURATION = 0, upperSATURATION = 131;
    int lowerVALUE = 40, upperVALUE = 255;
    createTrackbar( "wartosc lowerHUE", window_name[0], &lowerHUE, 255, NULL );
    createTrackbar( "wartosc upperHUE", window_name[0], &upperHUE, 255, NULL );
    createTrackbar( "wartosc lowerSATURATION", window_name[0], &lowerSATURATION, 255, NULL );
    createTrackbar( "wartosc upperSATURATION", window_name[0], &upperSATURATION, 255, NULL );
    createTrackbar( "wartosc lowerVALUE", window_name[0], &lowerVALUE, 255, NULL );
    createTrackbar( "wartosc upperVALUE", window_name[0], &upperVALUE, 255, NULL );

    int lastX=-1;
    int lastY=-1;

    Mat imgTmp;
    obrazZKamery.read(imgTmp);
    Mat imgLines = Mat::zeros(imgTmp.size(),CV_8UC3);

    cout << "szerokosc okna: " << obrazZKamery.get(CV_CAP_PROP_FRAME_WIDTH) << endl;
    cout << "wysokosc okna: " << obrazZKamery.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;
    while ( waitKey(fps) != 27 ){
        //capture >> frame;
        //frame.copyTo(img);
        bool wczytywanie = obrazZKamery.read(img);
        if (wczytywanie==false){
          cout << "nie udalo sie wczytac klatki" << endl;
          break;
        }
        Scalar min(lowerHUE,lowerSATURATION,lowerVALUE);
        Scalar max(upperHUE,upperSATURATION,upperVALUE);
        cvtColor(img, hsv_img, CV_BGR2HSV);
        // split(hsv_img, hsv_split);
        // split(img,bgr_split);
        // inRange(bgr_split[0],lowerb,upperb,binaryBGR);
        // inRange(hsv_split[0], lowerb, upperb, binaryHSV);
        // inRange(hsv_split[0], min, max, binaryHSV);
        inRange(hsv_img, min, max, hsv_img);
        cv::Mat element(4,4,CV_8U,cv::Scalar(1));    //Okreslenie opcji erozji
        // blur(binaryHSV, binaryHSV, cv::Size(3,3) );        //Rozmycie
        blur(hsv_img, hsv_img, cv::Size(5,5) );        //Rozmycie
        // erode(binaryHSV, binaryHSV, element);            //Erozja
        // dilate(binaryHSV,binaryHSV,element);
        erode(hsv_img, hsv_img, element);            //Erozja
        dilate(hsv_img, hsv_img, element);

        dilate(hsv_img, hsv_img, element);
        erode(hsv_img, hsv_img, element);            //Erozja

        // dilate(binaryHSV,binaryHSV,element);
        // erode(hsv_img,hsv_img,element);
        // erode(binaryBGR, binaryBGR, element);            //Erozja
        // imshow(window_name[1], img );            //Obrazek Orginalny
        // imshow(window_name[1], binaryHSV);            //Obraz binarny
        // imshow(window_name[2], hsv_img);
        Moments oMoments = moments(hsv_img);
        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double dArea = oMoments.m00;

        if(dArea >10000){
          int posX = dM10/dArea;
          int posY = dM01/dArea;
          if(lastX >=0 && lastY>=0 && posX >=0 && posY>=0){
            line(imgLines,Point(posX,posY),Point(lastX,lastY),Scalar(0,0,255),2);
          }

          lastX=posX;
          lastY=posY;
        }
        //hsv_img=hsv_img+imgLines;
        img=img+imgLines;
        imshow("obraz",img);
        // hsv_img=hsv_img+imgLines;
        imshow(window_name[1], hsv_img);



    }
    obrazZKamery.release();                        //Zwolnienie uchwytu kamery
    return 0;
}
