#include <opencv2/highgui/highgui.hpp> 
#include "opencv2/imgproc/imgproc.hpp" 
#include <string> 
#include <iostream> 
 
using namespace cv; 
using namespace std; 
 
int main() 
{ 
    VideoCapture capture = VideoCapture(0); 


    capture.set(CV_CAP_PROP_FRAME_WIDTH,480);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,320); 
    int fps = 1000/22;
    // string window_name [] = { "obraz BGR","obraz binarnyBGR","obraz HSV", "obraz binarnyHSV" };
    string window_name [] = {"kontrola obrazu","contours","obraz binarny" };
    Mat frame, img, hsv_img, binary; 
    Mat binaryBGR, binaryHSV, cont;
    vector<Mat> bgr_split,hsv_split;
    for ( int i = 0; i < 3; i++ ) namedWindow(window_name[i], CV_WINDOW_AUTOSIZE);
    int lowerHUE = 61, upperHUE = 119;
    int lowerSATURATION = 0, upperSATURATION = 131;
    int lowerVALUE = 40, upperVALUE = 255;
    createTrackbar( "wartosc lowerHUE", window_name[0], &lowerHUE, 255, NULL );
    createTrackbar( "wartosc upperHUE", window_name[0], &upperHUE, 255, NULL );
    createTrackbar( "wartosc lowerSATURATION", window_name[0], &lowerSATURATION, 255, NULL );
    createTrackbar( "wartosc upperSATURATION", window_name[0], &upperSATURATION, 255, NULL );
    createTrackbar( "wartosc lowerVALUE", window_name[0], &lowerVALUE, 255, NULL );
    createTrackbar( "wartosc upperVALUE", window_name[0], &upperVALUE, 255, NULL );
    cout << "szerokosc okna: " << capture.get(CV_CAP_PROP_FRAME_WIDTH) << endl;
    cout << "wysokosc okna: " << capture.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;

    while ( waitKey(fps) != 27 ) 
    { 

        bool wczytywanie = capture.read(img);
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
        cv::Mat element(6,6,CV_8U,cv::Scalar(1));    //Okreslenie opcji erozji
        // blur(binaryHSV, binaryHSV, cv::Size(3,3) );        //Rozmycie
        blur(hsv_img, hsv_img, cv::Size(5,5) );        //Rozmycie
        // erode(binaryHSV, binaryHSV, element);            //Erozja
        // dilate(binaryHSV,binaryHSV,element);
        erode(hsv_img, hsv_img, element);            //Erozja
        dilate(hsv_img, hsv_img, element);
        dilate(hsv_img, hsv_img, element);
        erode(hsv_img, hsv_img, element); 



        //*** 
        vector<vector<Point> > contours; 
        vector<Point> contours_poly; 
        Rect boundRect; 
        hsv_img.copyTo(cont); 
        findContours( cont, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) ); 
        int maxr = 600, i_cont = -1; 
        Mat drawing = Mat::zeros( cont.size(), CV_8UC3 ); 
        for( int i = 0; i< contours.size(); i++ ) 
        { 
            if ( abs(contourArea(Mat(contours[i]))) > maxr ) 
            { 
                maxr = abs(contourArea(Mat(contours[i]))); 
                i_cont = i; 
            } 
        } 
        if ( i_cont >= 0 ) 
        { 
            approxPolyDP( Mat(contours[i_cont]), contours_poly, 3, true ); 
            boundRect = boundingRect( Mat(contours_poly) ); 
            fillConvexPoly(img, contours_poly, contours_poly.size() ); 
            rectangle( img, boundRect.tl(), boundRect.br(), Scalar(125, 250, 125), 2, 8, 0 ); 
            line( img, boundRect.tl(), boundRect.br(), Scalar(250, 125, 125), 2, 8, 0); 
            line( img, Point(boundRect.x + boundRect.width, boundRect.y), Point(boundRect.x, boundRect.y + boundRect.height), Scalar(250, 125, 125), 2, 8, 0); 
            string s; 
            stringstream out; 
            out << boundRect.x + boundRect.width/2 << "x" << boundRect.y + boundRect.height/2; 
            s = out.str(); 
            putText( img, s, Point(50, 50), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(20, 40, 80), 3, 8 ); 
            drawContours( drawing,  contours, i_cont, Scalar(125, 125, 250), 2 ); 
        } 
        imshow(window_name[1], drawing); 
        //*** 
 
        imshow(window_name[0], img ); 
        imshow(window_name[2], hsv_img); 
    } 
    capture.release(); 
    return 0; 
}
