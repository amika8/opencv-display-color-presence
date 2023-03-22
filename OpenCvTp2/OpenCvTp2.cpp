#pragma comment(lib, "winmm.lib") //for audio

#include<opencv2/opencv.hpp>

#include<iostream>

#include <Windows.h>

#include <stdlib.h>

#include <mmsystem.h>




using namespace cv;
using namespace std;

Point computeCentroid(const Mat& mask) {
    Moments m = moments(mask, true);
    Point center(m.m10 / m.m00, m.m01 / m.m00);
    return center;
}

Point incrementCircle(Point& ball) {
    //ball.x++;
    ball.y = ball.y + 15;
    return ball;
}

Point incrementLeft(Point& centrePique, Mat myImage) {
    if (centrePique.x > 3) {
        centrePique.x = centrePique.x - 10;
    }
    return centrePique;

}

Point incrementRight(Point& centrePique, Mat myImage) {
    if (centrePique.x < myImage.cols - 3) {
        centrePique.x = centrePique.x + 10;
    }
    return centrePique;
}

void play()
{
    PlaySound(TEXT("BubbleSound.wav"), NULL, SND_FILENAME | SND_ASYNC); // plays sound.wav once
}


bool inRange(unsigned low, unsigned high, unsigned x) {
    return (low <= x && x <= high);
}

int main() {

    Mat myImage;//Declaring a matrix to load the frames
    VideoCapture cap(0);//Declaring an object to capture stream of frames
    if (!cap.isOpened()) { //This section prompt an error message if no video stream is found
        cout << "No video stream detected" << endl;
        system("pause");
        return-1;
    }

    //Ballon qu'il faut detruire
    Point ball(100, 0);

    //cercle qui detruit les ballons
    Point centrePique(myImage.cols / 2, 450);

    //Score
    int score = 0;


    while (true) { //Taking an everlasting loop to show the video//

        cap >> myImage;

        if (myImage.empty()) { //Breaking the loop if no video frame is detected//
            break;
        }

        //Clone de l'image original pour pouvoir comparer visuellement
        Mat resultat = myImage.clone();

        //Conversion de l'image RGB en HSV
        Mat resultatHsv;
        cvtColor(resultat, resultatHsv, COLOR_BGR2HSV);

        // Ajout d'un masque. HSV avec une teinte (hue) focalisé sur le rouge.
        Mat1b mask1, mask2;
        inRange(resultatHsv, Scalar(0, 100, 20), Scalar(10, 255, 255), mask1);
        inRange(resultatHsv, Scalar(160, 100, 20), Scalar(179, 255, 255), mask2);

        Mat1b redMask = mask1 | mask2; // | : tout a 1 sauf quand mask 1 et 2 sont a 0.

        //Mat1b greenMask;
        //inRange(resultatHsv, Scalar(40, 70, 120), Scalar(90, 255, 125), greenMask);

        Mat1b blueMask;
        inRange(resultatHsv, Scalar(78, 158, 124), Scalar(138, 255, 255), blueMask);


        //count non - zero pixels in mask
        int pixelsCprtRed = countNonZero(redMask);
        int pixelsCptrBlue = countNonZero(blueMask);

        cout << "nbr de pixel du mask rouge: " << pixelsCprtRed << endl;
        cout << "nbr de pixel du mask bleu: " << pixelsCptrBlue << endl;

        // POUR LE BLEUE A GAUCHE
        // Rectangle de fond
        //myImage.rows = 480:
        int widthBaseBlue = 70;
        int heightBaseBlue = myImage.rows - 40;
        Rect rectBaseBlue(20, 20, widthBaseBlue, heightBaseBlue);
        rectangle(myImage, rectBaseBlue, cv::Scalar(255, 0, 0), 0);
        //cout << pixelsCptrBlue / 698 <<endl;
        // Rectangle de remplissage
        int widthFillBlue = 70;
        int heightFillBlue = pixelsCptrBlue / 698;
        Rect rectFillBlue(20, 20, widthFillBlue, heightFillBlue);
        rectangle(myImage, rectFillBlue, cv::Scalar(255, 0, 0), -1);

        // POUR LE ROUGE A DROITE
        // Rectangle de fond
        //myImage.cols = 640;
        int widthBaseRed = 70;
        int heightBaseRed = myImage.rows - 40;
        Rect rectBaseRed(540, 20, widthBaseRed, heightBaseRed);
        rectangle(myImage, rectBaseRed, cv::Scalar(0, 0, 255), 0);
        //cout << pixelsCprtRed / 698 << endl;
        // Rectangle de remplissage
        int widthFillRed = 70;
        int heightFillRed = pixelsCprtRed / 698;
        Rect rectFillRed(540, 20, widthFillRed, heightFillRed);
        rectangle(myImage, rectFillRed, cv::Scalar(0, 0, 255), -1);

        if ((pixelsCprtRed / 698) < (pixelsCptrBlue / 698)) {
            circle(myImage, incrementLeft(centrePique, myImage), 25, Scalar(255, 0, 0), -1);
        }
        else {
            circle(myImage, incrementRight(centrePique, myImage), 25, Scalar(255, 0, 0), -1);
        }


        cout << "ball x : " << ball.x << endl;
        cout << "ball y : " << ball.y << endl;

        cout << "centrePique x : " << centrePique.x << endl;
        cout << "centrePique y : " << centrePique.y << endl;



        if (((inRange(ball.y - 30, ball.y + 30, centrePique.y)) && (inRange(ball.x - 30, ball.x + 30, centrePique.x)))) {
            ball.y = 0;
            ball.x = rand() % 500 + 50;
            play();
            score += 100;
        }

        if (ball.y > (myImage.rows + 10)) {
            ball.y = 0;
            ball.x = rand() % 500;

            score -= 50;
            //Point ball(rand() % 100, 0);
        }

        circle(myImage, incrementCircle(ball), 20, Scalar(rand() % 255, rand() % 255, rand() % 255), -1);


        //Affichage du score
        putText(myImage, //target image
            "SCORE : " + std::to_string(score), //text
            Point((myImage.cols / 2) - 100, 30), //top-left position
            FONT_HERSHEY_DUPLEX,
            1.0,
            CV_RGB(118, 185, 0), //font color
            2);

        imshow("Video Player - original image", myImage);//Showing the original video//
        //imshow("Video Player - resultat image HSV", resultatHsv);//Showing the result//
        //imshow("Video Player - result red", redMask);//Showing the result//
        //imshow("Video Player - result blue", blueMask);//Showing the result//

        char c = (char)waitKey(25);//Allowing 25 milliseconds frame processing time and initiating break condition//
        if (c == 27) { //If 'Esc' is entered break the loop//
            break;
        }


    }
    cap.release();//Releasing the buffer memory//
    return 0;


}


