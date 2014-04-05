//Program captures frames from a video feed, applies the necessary filters, and displays the binarized frames with lines drawn in blue
//HoughLinesP parameters:
//InputArray image: 8-bit, single-channel binary source image. The image may be modified by the function.
//OutputArray lines: Output vector of lines. Each line is represented by a 4-element vector  (x_1, y_1, x_2, y_2) , where  (x_1,y_1) and  (x_2, y_2) are the ending points of each detected line segment.
//double rho: Distance resolution of the accumulator in pixels.
//double theta: Angle resolution of the accumulator in radians.
//int threshold: Accumulator threshold parameter. Only those lines are returned that get enough votes (> threshold).
//double minLineLength: Minimum line length. Line segments shorter than that are rejected.
//double maxLineGap: Maximum allowed gap between points on the same line to link them.
#include "opencv2/opencv.hpp"

using namespace cv;

Mat edges;				//Mat object to store the output of Canny edge detector
Mat probabilistic_hough;		//Mat object to store the output of Probabilistic Hough Line Transform
int min_threshold = 50;			//Min value required for a potential line segment to be returned
int max_trackbar = 150;			//Max value for the trackbar, if used
int p_trackbar = max_trackbar;

//Utilizes HoughLinesP to find line segments in a binary image.
void Probabilistic_Hough(int, void*);

int main(int, char**)
{

    VideoCapture cap(1); // open camera: any camera: -1, default camera: 0, other USB cameras: 1+
    if(!cap.isOpened())  // check if we succeeded
        return -1;

   //Create window with the name "edges"
   //namedWindow("edges",1);

    for(;;)
    {
        Mat frame;	//Mat object to store a frame from video feed

	//Get new frame from camera: this captures every 6th frame from feed
        cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;

	//Convert frame from RGB to grayscale image
        cvtColor(frame, edges, CV_BGR2GRAY);

	//Apply Gaussian Blur: this reduces impact of noise on the algorithm
        GaussianBlur(edges, edges, Size(7,7), 0, 0);

	//Detect edges in image such as contours in an object, or sharp contrasts between adjacent colors; converts image to binary with edges assigned value 1
        Canny(edges, edges, 50, 150, 3);

	//Apply Porbabilistic Hough Line Transform: this will detect line segments based on the edges detected in the previous step
	Probabilistic_Hough(0,0);

        //Show a window with just canny edges; no Hough lines
	//imshow("edges", edges);

	//Press any key to exit the program
        if(waitKey(30) >= 0) break;
    }

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

void Probabilistic_Hough(int, void*)
{
	vector<Vec4i> p_lines;	//Stores detected lines

	//Apply another Gaussian Blur to image to further reduce noise (increases computation time significantly)
	GaussianBlur(edges, edges, Size(3,3), 0, 0);

	//Allow lines to be drawn in color
	cvtColor( edges, probabilistic_hough, CV_GRAY2BGR );

	/// 2. Use Probabilistic Hough Transform
	HoughLinesP( edges, p_lines, 1, CV_PI/180, min_threshold + p_trackbar, 30, 10 );

	/// Show the result
	for( size_t i = 0; i < p_lines.size(); i++ )
	{
		Vec4i l = p_lines[i];	//Grab detected lines individually

		//Draw lines
		line( probabilistic_hough, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 3, CV_AA);
	}

	//Show Binarized image with detected lines drawn
	imshow( "Probabilistic Hough Lines", probabilistic_hough );
}
