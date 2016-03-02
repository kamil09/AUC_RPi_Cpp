#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>
#include <unistd.h>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
//#include <opencv2/opencv.hpp>
#include <string> 
#include <iostream> 
 

using namespace cv;
using namespace std;
//DEFINICJE PINÓW
#define LASER_PIN 4
#define STEP_X 17
#define DIR_X 18
#define STEP_Y 21
#define DIR_Y 22
#define MOTOR_ENABLE 25
#define END_X 23
#define END_Y 24

//DEFINICJE STAŁYCH
#define KROK 17.77
#define DELAY 500
#define minX -66.0
#define minY -66.0
#define maxX 66.0
#define maxY 66.0

double nowX=0.0;
double nowY=0.0;
double nowX2=0.0;
double nowY2=0.0;

//TRYB PRACY
//JEŚLI tryb = 0 - namierzamy
//JEŚLI tryb = 1 - wyświetlamy okna 
int tryb = 0;

void laserON(){
	puts("laser On");
	digitalWrite(LASER_PIN,HIGH);
}
void laserOFF(){
	puts("laser Off");
	digitalWrite(LASER_PIN,LOW);
}
void laserSHOOT(int seconds){
	if(fork()==0){
		digitalWrite(LASER_PIN,HIGH);
		sleep(seconds);
		digitalWrite(LASER_PIN,LOW);
	}
}



void kalibruj(){
	//double licznik=0.0;
	puts("kalibruje X");
	while(1){
		digitalWrite(STEP_X,HIGH);
		delayMicroseconds(DELAY);
		digitalWrite(STEP_X,LOW);
		delayMicroseconds(DELAY);
		//licznik++;
		if (digitalRead(END_X)==0) break;
	}
	//printf("licznik: %f",licznik);
	//minX=-licznik/KROK;
	//licznik=0.0;
	puts("kalibruje Y");
	while(1){
		digitalWrite(STEP_Y,HIGH);
		delayMicroseconds(DELAY);
		digitalWrite(STEP_Y,LOW);
		delayMicroseconds(DELAY);
		//licznik++;
		if (digitalRead(END_Y)==0) break;
	}
	//minY=-licznik/KROK;
	//printf("minX: %f	minY: %f\n",minX,minY);
	//maxX=-minX;
	//maxY=-minY;
	nowX=minX;
	nowY=minY;
}


int move(double katX, double katY, double mnoznik){
	puts("poruszam sie");
	int x=1;
	int y=1;
	if (katX >= 0.0){
		digitalWrite(DIR_X,HIGH);
	}
	else{
		digitalWrite(DIR_X,LOW);
		katX=-katX;
		x=0;
	}
	if (katY >= 0.0){
		digitalWrite(DIR_Y,HIGH);
	}
	else{
		digitalWrite(DIR_Y,LOW);
		katY=-katY;
		y=0;
	}

	while(1){
		if(katX>0.0){
			digitalWrite(STEP_X,HIGH);
			if(x==1) nowX+=1/KROK;
			else nowX-=1/KROK;
			katX=katX - 1/KROK;
		}
		if(katY>0.0){
			digitalWrite(STEP_Y,HIGH);
			if(y==1) nowY+=1/KROK;
			else nowY-=1/KROK;
			katY=katY - 1/KROK;
		}
		delayMicroseconds((int) (DELAY/mnoznik));
		digitalWrite(STEP_X,LOW);
		digitalWrite(STEP_Y,LOW);
		delayMicroseconds((int) (DELAY/mnoznik));
		if((nowX<minX) || (nowY<minY) || (nowX>maxX) || (nowY>maxY)){
			return 1;
		}
		if((katX<=0.0) && (katY<=0.0)) break;
	}
	puts("koncze poruszanie");
	return 0;

}

void start(){
        puts("wracam do polozenia poczatkowego");
        move(-nowX,-nowY,5.0);
}

void obraz() 
{ 
    VideoCapture capture = VideoCapture(0); 
    capture.set(CV_CAP_PROP_FRAME_WIDTH,480);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,320); 
    
    int fps = 1000/1000;

    // string window_name [] = { "obraz BGR","obraz binarnyBGR","obraz HSV", "obraz binarnyHSV" };
    string window_name [] = {"kontrola obrazu","contours","obraz binarny" };
    Mat frame, img, hsv_img, binary; 
    int bez=0, dX=0, dY=0;
    Mat binaryBGR, binaryHSV, cont;
    vector<Mat> bgr_split,hsv_split;
 
    if( tryb == 1 ){
    	for ( int i = 0; i < 3; i++ ) namedWindow(window_name[i], CV_WINDOW_AUTOSIZE);
    }    

    int lowerHUE = 113, upperHUE = 131;
    int lowerSATURATION = 0, upperSATURATION = 131;
    int lowerVALUE = 75, upperVALUE = 154;

    createTrackbar( "wartosc lowerHUE", window_name[0], &lowerHUE, 255, NULL );
    createTrackbar( "wartosc upperHUE", window_name[0], &upperHUE, 255, NULL );
    createTrackbar( "wartosc lowerSATURATION", window_name[0], &lowerSATURATION, 255, NULL );
    createTrackbar( "wartosc upperSATURATION", window_name[0], &upperSATURATION, 255, NULL );
    createTrackbar( "wartosc lowerVALUE", window_name[0], &lowerVALUE, 255, NULL );
    createTrackbar( "wartosc upperVALUE", window_name[0], &upperVALUE, 255, NULL );
    cout << "szerokosc okna: " << capture.get(CV_CAP_PROP_FRAME_WIDTH) << endl;
    cout << "wysokosc okna: " << capture.get(CV_CAP_PROP_FRAME_HEIGHT) << endl;

    while (1) 
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
        int maxr = 400, i_cont = -1; 
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
	    bez=0; 
            approxPolyDP( Mat(contours[i_cont]), contours_poly, 3, true ); 
            boundRect = boundingRect( Mat(contours_poly) ); 
//            fillConvexPoly(img, contours_poly, contours_poly.size() ); 
//            rectangle( img, boundRect.tl(), boundRect.br(), Scalar(125, 250, 125), 2, 8, 0 ); 
//            line( img, boundRect.tl(), boundRect.br(), Scalar(250, 125, 125), 2, 8, 0); 
//            line( img, Point(boundRect.x + boundRect.width, boundRect.y), Point(boundRect.x, boundRect.y + boundRect.height), Scalar(250, 125, 125), 2, 8, 0); 
//            string s; 
//            stringstream out; 
//            out << boundRect.x + boundRect.width/2 << "x" << boundRect.y + boundRect.height/2; 
//            s = out.str(); 
//            putText( img, s, Point(50, 50), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(20, 40, 80), 3, 8 ); 
//            drawContours( drawing,  contours, i_cont, Scalar(125, 125, 250), 2 ); 

		
		dX=(boundRect.x + boundRect.width/2) - (capture.get(CV_CAP_PROP_FRAME_WIDTH)/2);
		dY=(boundRect.y + boundRect.height/2) - (capture.get(CV_CAP_PROP_FRAME_HEIGHT)/2);
		dX-=20;
		dY-=30;

		int dXDiff = 8;
		int dYDiff = 8;

		if((abs(dX) > dXDiff ) || (abs(dY) > dYDiff ))
		{
			if(abs(dX) < dXDiff ) dX=0; 			
			if(abs(dY) < dYDiff ) dY=0; 			
		
			laserOFF();	
			double dxD = (double)(dX)/40;
			double dyD = (double)(-dY)/40;
			printf("kąt X: %f ; kąt Y: %f\n",dxD,dyD );
			int status = move(dxD, dyD, 5.0);
			
			if(status == 1 ) move(-nowX, -nowY, 5.0);
			
		//To będzie powodować problemy!
		//	nowX2+=dX;
		//	nowY2+=dY;
		}
		else{
			puts("czekam");
			laserON();
			
		//	delayMicroseconds(200000);
		}

        } 
	else
	{
		puts("Nie znalazłem obiektu");
		bez++;
		if(bez>50)
		{
			//Tak będzie bezpieczniej
			move(-nowX, -nowY, 2.0);
			bez=0;
			puts("wracam na start");			
		}
	}

	if(tryb == 1 ){
	        imshow(window_name[1], drawing); 
     		imshow(window_name[0], img ); 
      	  	imshow(window_name[2], hsv_img); 
	}
    } 
    capture.release(); 
}

int main(int argc, char** argv){
	
	if( (argc > 1) && (!(strcmp(argv[1],"X"))) ) tryb = 1;

	if(tryb == 0 ) {
		if (wiringPiSetupGpio() == -1){
			return -1;
		}

		pinMode(LASER_PIN,OUTPUT);
		pinMode(END_X,INPUT);
		pinMode(END_Y,INPUT);
		pinMode(STEP_X,OUTPUT);
		pinMode(DIR_X,OUTPUT);
		pinMode(STEP_Y,OUTPUT);
		pinMode(DIR_Y,OUTPUT);
		pinMode(MOTOR_ENABLE,OUTPUT);

		pullUpDnControl(END_X,PUD_UP);
		pullUpDnControl(END_Y,PUD_UP);

		digitalWrite(DIR_X,LOW);
		digitalWrite(DIR_Y,LOW);
		digitalWrite(MOTOR_ENABLE,LOW);
	

		laserOFF();
		kalibruj();
		start();
		//laserON();
	}

	obraz();

	//laserOFF();
	//delayMicroseconds(3000000);
	//digitalWrite(MOTOR_ENABLE,HIGH);
	return 0;
}
