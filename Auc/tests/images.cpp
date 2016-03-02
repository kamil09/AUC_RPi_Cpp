#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

int main(int argc, char* argv[]){
  VideoCapture cap(0);
  if(cap.isOpened()==false){
    cout << "Nie udalo sie polaczyc z kamera" << endl;
  }

  double szerokosc = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  double wysokosc = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

  namedWindow("BGR video", CV_WINDOW_AUTOSIZE);

  while(true){
    Mat klatka;
    bool wczytywanie = cap.read(klatka);
    if(wczytywanie==false){
      cout << "nie udalo sie wczytac klatki" << endl;
      break;
    }

    imshow("BGR video",klatka);
    if(waitKey(1000/30)==27){
      break;
    }
  }

  return 0;
}
