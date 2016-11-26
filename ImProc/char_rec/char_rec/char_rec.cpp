// char_rec.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>

#include<iostream>
#include<sstream>


using namespace cv;
// global variables ///////////////////////////////////////////////////////////////////////////////
const int MIN_CONTOUR_AREA = 30;
const int MAX_CONTOUR_AREA = 25000;
const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 30;

///////////////////////////////////////////////////////////////////////////////////////////////////
class ContourWithData {
public:
	// member variables ///////////////////////////////////////////////////////////////////////////
	std::vector<cv::Point> ptContour;           // contour
	std::vector<cv::Point> convexHull;
	cv::Rect boundingRect;                      // bounding rect for contour
	float fltArea;                              // area of contour
	float width;
	float height ;
	float aspect_ratio;
	float extent ;
	float solidity;
	float occupyRate;
	float compactness;
	///////////////////////////////////////////////////////////////////////////////////////////////
	bool checkIfContourIsValid() {                             
		/*if (fltArea > MIN_CONTOUR_AREA && fltArea < MAX_CONTOUR_AREA && aspect_ratio < 3 && solidity > 0.3 && extent > 0.2 && extent < 0.9) {
			return true;
		}
		else return false;*/
		if (fltArea > MIN_CONTOUR_AREA && fltArea < MAX_CONTOUR_AREA && occupyRate >= 0.03 && occupyRate <= 0.95 && compactness >= 0.003 && compactness < 0.95) {
			return true;
		}
		else return false;
	}

};

void adaptiveAlgorithm(Mat &input, Mat &output) {
	cv::Mat matBlurred;             
	cv::Mat matThresh;
	// blur
	cv::GaussianBlur(input,         
		matBlurred,                
		cv::Size(7, 7),            
		1.5);                       
	//OPEN
	cv::Mat opened;

	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(3, 3));

	cv::morphologyEx(matBlurred, opened, MORPH_OPEN, element);

	// filter image from grayscale to black and white
	cv::adaptiveThreshold(matBlurred,                           
		matThresh,                            
		255,                                  
		cv::ADAPTIVE_THRESH_GAUSSIAN_C,       
		cv::THRESH_BINARY_INV,                
		51,                                   
		10);                                 


	cv::Mat element_close = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(5, 5));

	cv::morphologyEx(matThresh, output, MORPH_CLOSE, element_close);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[]) {
	std::vector<ContourWithData> allContoursWithData;           // declare empty vectors,
	std::vector<ContourWithData> validContoursWithData;         // we will fill these shortly

	// read in training classifications ///////////////////////////////////////////////////

	cv::Mat matClassificationInts;      // we will read the classification numbers into this variable as though it is a vector


	cv::Mat matTrainingImagesAsFlattenedFloats;         // we will read multiple images into this single image variable as though it is a vector



	cv::Mat matTestingNumbers = cv::imread("E:\\Egyetem\\Képfelalk\\sample.png");            // read in the test numbers image

	if (matTestingNumbers.empty()) {                                // if unable to open image
		std::cout << "error: image not read from file\n\n";         // show error message on command line
		return(1);                                                  // and exit program
	}

	cv::Mat matGrayscale;           //
	cv::Mat matBlurred;             // declare more image variables
	cv::Mat matThresh;              //
	cv::Mat matThreshCopy;          //

	cv::cvtColor(matTestingNumbers, matGrayscale, CV_BGR2GRAY);         // convert to grayscale

	adaptiveAlgorithm(matGrayscale, matThresh);
	matThreshCopy = matThresh.clone();              

	std::vector<std::vector<cv::Point> > ptContours;        
	std::vector<cv::Vec4i> v4iHierarchy;                  

	cv::findContours(matThreshCopy,             // input image, make sure to use a copy since the function will modify this image in the course of finding contours
		ptContours,                             // output contours
		v4iHierarchy,                           // output hierarchy
		cv::RETR_EXTERNAL,                      // retrieve the outermost contours only
		cv::CHAIN_APPROX_SIMPLE);               // compress horizontal, vertical, and diagonal segments and leave only their end points


	
	//imgThreshCopy = eroded.clone();

	for (int i = 0; i < ptContours.size(); i++) {               // for each contour
		ContourWithData contourWithData;                                                    // instantiate a contour with data object
		contourWithData.ptContour = ptContours[i];                                          // assign contour to contour with data
		contourWithData.boundingRect = cv::boundingRect(contourWithData.ptContour);         // get the bounding rect
		contourWithData.fltArea = cv::contourArea(contourWithData.ptContour);
		contourWithData.width = contourWithData.boundingRect.width;
		contourWithData.height = contourWithData.boundingRect.height;
		contourWithData.aspect_ratio = contourWithData.boundingRect.width / contourWithData.boundingRect.height;
		contourWithData.extent = contourWithData.fltArea / contourWithData.boundingRect.area();
		cv::convexHull(ptContours[i], contourWithData.convexHull);
		contourWithData.solidity = contourWithData.fltArea / contourArea(contourWithData.convexHull);
		contourWithData.occupyRate = contourWithData.fltArea / (contourWithData.width * contourWithData.height);
		float perimeter = cv::arcLength(ptContours[i], true);
		contourWithData.compactness = contourWithData.fltArea / (perimeter * perimeter);
		allContoursWithData.push_back(contourWithData);                                     // add contour with data object to list of all contours with data
	}

	for (int i = 0; i < allContoursWithData.size(); i++) {                      // for all contours
		if (allContoursWithData[i].checkIfContourIsValid()) {                   // check if valid
			validContoursWithData.push_back(allContoursWithData[i]);            // if so, append to valid contour list
		}
	}

	for (int i = 0; i < validContoursWithData.size(); i++) {            // for each contour

		// draw a green rect around the current char
		cv::rectangle(matTestingNumbers,                            // draw rectangle on original image
			validContoursWithData[i].boundingRect,        // rect to draw
			cv::Scalar(0, 255, 0),                        // green
			2);                                           // thickness

	}

	
	//namedWindow("matTestingNumbers", WINDOW_NORMAL);
	//cv::imshow("matTestingNumbers", matTestingNumbers);
	cv::imwrite("E:\\Egyetem\\Képfelalk\\ered.jpg", matTestingNumbers);

	//cv::waitKey(0);                                         // wait for user key press

	return(0);
}

