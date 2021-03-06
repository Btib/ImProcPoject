// char_rec.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>
#include <opencv2/opencv.hpp>

#include<iostream>
#include<sstream>
#include <string>


using namespace cv;
using namespace std;

const int MIN_CONTOUR_AREA = 70;
const int MAX_CONTOUR_AREA = 25000;

///////////////////////////////////////////////////////////////////////////////////////////////////
class ContourWithData {
public:
	// member variables ///////////////////////////////////////////////////////////////////////////
	std::vector<cv::Point> ptContour;           
	std::vector<cv::Point> convexHull;
	cv::Rect boundingRect;                     
	RotatedRect rRect;
	bool segedv=false;
	bool votma = false;
	float fltArea;                             
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
		if (fltArea > MIN_CONTOUR_AREA && fltArea < MAX_CONTOUR_AREA && occupyRate >= 0.03 && occupyRate <= 0.97 && compactness >= 0.003 && compactness < 0.95) {
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
		cv::Size(5, 5),            
		1.5);                       
	//OPEN
	cv::Mat opened;

	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(3, 3));

	cv::morphologyEx(matBlurred, opened, MORPH_OPEN, element);

	cv::adaptiveThreshold(matBlurred,                           
		matThresh,                            
		255,                                  
		cv::ADAPTIVE_THRESH_GAUSSIAN_C,       
		cv::THRESH_BINARY_INV,                
		51,                                   
		10);                                 


	cv::Mat element_close = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(3, 3));

	cv::morphologyEx(matThresh, output, MORPH_CLOSE, element_close);
}

void otsuAlgorithm(Mat &input, Mat &output) {
	cv::Mat matBlurred;
	// blur
	cv::GaussianBlur(input,
		matBlurred,
		cv::Size(5, 5),
		1.5);
	cv::threshold(matBlurred, output, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
}


void MSERDetection(Mat &input, std::vector<std::vector<cv::Point> >  &output) {
	std::vector< Rect > asd;
	cv::Mat matBlurred;
	cv::GaussianBlur(input,
		matBlurred,
		cv::Size(3, 3),
		1.5);
	//OPEN
	cv::Mat opened;

	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(1, 1));

	cv::morphologyEx(matBlurred, opened, MORPH_OPEN, element);
	Ptr<MSER> detektor = MSER::create();
	detektor->detectRegions(input, output,asd);


}
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[]) {
	std::vector<ContourWithData> allContoursWithData;           
	std::vector<ContourWithData> validContoursWithData;         
	float distance = 50;
	float sizee = 200;
	string elso = "1";
	string masodik = "2";
	int alg=1;
	if (elso.compare(argv[2]) == 0) {
		alg = 1;
	}
	else if (masodik.compare(masodik) == 0) {
		alg = 2;
	}
	else return 2;
	
	distance = stof(argv[4]);
	sizee = stof(argv[3]);
	
	cv::Mat matTestingNumbers = cv::imread(argv[1]);           

	if (matTestingNumbers.empty()) {                               
		std::cout << argv[1];        
		return(1);                                                  
	}
	
	cv::Mat matGrayscale;          
	cv::Mat matBlurred;            
	cv::Mat matThresh;             
	cv::Mat matThreshCopy;         

	cv::cvtColor(matTestingNumbers, matGrayscale, CV_BGR2GRAY);     

	if (alg == 1) {
		adaptiveAlgorithm(matGrayscale, matThresh);
	}
	else if(alg== 2) {
		otsuAlgorithm(matGrayscale, matThresh);
	}
	
	matThreshCopy = matThresh.clone();              

	std::vector<std::vector<cv::Point> > ptContours;        
	std::vector<cv::Vec4i> v4iHierarchy;                  

	cv::findContours(matThreshCopy,             
		ptContours,                            
		v4iHierarchy,                          
		cv::RETR_EXTERNAL,                      
		cv::CHAIN_APPROX_SIMPLE);               
	

	//MSERDetection(matGrayscale, ptContours);

	
	//imgThreshCopy = eroded.clone();

	for (int i = 0; i < ptContours.size(); i++) {              
		ContourWithData contourWithData;                                                  
		contourWithData.ptContour = ptContours[i];                                        
		contourWithData.boundingRect = cv::boundingRect(contourWithData.ptContour);        
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
		allContoursWithData.push_back(contourWithData);                         
	}

	int maxHeight=0;
	for (int i = 0; i < allContoursWithData.size(); i++) {                      
		if (allContoursWithData[i].checkIfContourIsValid()) {                   
			validContoursWithData.push_back(allContoursWithData[i]);          
			if (maxHeight < allContoursWithData[i].height) {
				maxHeight = allContoursWithData[i].height;
			}
		}
	}

	std::vector<ContourWithData> validContoursWithDataNew;

	for (int i = 0; i < validContoursWithData.size(); i++) {
		if (validContoursWithData[i].fltArea < sizee ) {
			for (int j = 0; j < validContoursWithData.size(); j++) {
				Moments m1 = moments(Mat(validContoursWithData[i].ptContour), false);
				Moments m2 = moments(Mat(validContoursWithData[j].ptContour), false);
				float x1 = m1.m10 / m1.m00;
				float y1 = m1.m01 / m1.m00;
				float x2 = m2.m10 / m2.m00;
				float y2 = m2.m01 / m2.m00;
				vector<Point> dist;
				dist.push_back(Point(x1, y1));
				dist.push_back(Point(x2, y2));
				float d = arcLength(dist, false);
				if (validContoursWithData[i].boundingRect != validContoursWithData[j].boundingRect) {
					if (d < distance && validContoursWithData[j].height < (maxHeight * 0.8) && validContoursWithData[j].fltArea > sizee) {
						ContourWithData contourData;
						std::vector<cv::Point> points;
						validContoursWithData[j].votma = true;
						validContoursWithData[i].votma = true;
						points.insert(points.end(), validContoursWithData[i].ptContour.begin(), validContoursWithData[i].ptContour.end());
						points.insert(points.end(), validContoursWithData[j].ptContour.begin(), validContoursWithData[j].ptContour.end());
						contourData.ptContour = points;
						contourData.rRect = minAreaRect(points);
						contourData.segedv = true;
						validContoursWithDataNew.push_back(contourData);
					}
				}
			}
		}
	}

	for (int i = 0; i < validContoursWithData.size(); i++) {
		if (!validContoursWithData[i].votma) {
			validContoursWithDataNew.push_back(validContoursWithData[i]);
		}
	}
	for (int i = 0; i < validContoursWithDataNew.size(); i++) {            

		if (!validContoursWithDataNew[i].votma) {
			cv::rectangle(matTestingNumbers,                            
				validContoursWithDataNew[i].boundingRect,    
				cv::Scalar(0, 255, 0),                        
				2);                                      
		}
		if (validContoursWithDataNew[i].segedv) {
			cv::Point2f vertices[4];
			validContoursWithDataNew[i].rRect.points(vertices);
			for (int i = 0; i < 4; ++i)
			{
				cv::line(matTestingNumbers, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 1, CV_AA);
			}
		}
	}

	
	//namedWindow("matTestingNumbers", WINDOW_NORMAL);
	//cv::imshow("matTestingNumbers", matTestingNumbers);
	cv::imwrite("Img\\ered.jpg", matTestingNumbers);
	//cv::waitKey(0);                                        
	return 0;
}

