#define _CRT_SECURE_NO_WARNINGS //allow use of std::localtime and std::put_time 
								//may cause data races in multi-threaded application
#include "ImageZoom.h"
#include <fstream> //outputting log file
#include <chrono> //getting current time
#include <sstream> //concatenating strings
#include <iomanip> //outputting time
#include <iostream> //output status of zoom

//constructors
ImageZoom::ImageZoom() :
	//call image default constructor
	Image()
{}
ImageZoom::ImageZoom(const unsigned int &w, const unsigned int &h) :
	//call image paramaterised constructor
	Image(w, h)
{}
//copy constructor
ImageZoom::ImageZoom(const ImageZoom &img) :
	//call image copy constructor
	Image(img)
{}

//destructors
ImageZoom::~ImageZoom()
{	
	//no dynamic memory allocated - cleanup not requireed
}

//operator overloads
ImageZoom ImageZoom::operator=(const ImageZoom &img)
{
	//call Image copy assignment operator
	Image::operator=(img);
	return *this;
}

void ImageZoom::log()
{
	//declares a time_t that holds the time now according to the system clock
	time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	//declare stringstream for generating filenames
	std::stringstream filename;
	//concatenate filename and current time 
	//format: YYYY-MM-DD
	filename << "Logs/zoom-" << std::put_time(std::localtime(&time), "%F") << ".txt";
	//declare output file stream for log
	std::ofstream log;
	try
	{
		//opens filename converted to array of chars and uses append mode
		log.open(filename.str().c_str(), std::ios::app);
		//check to see if file was opened correctly and throw error if not
		if (log.fail())
			throw("Error during log generation for image zoom operation.");
		//get the read time of the file and store within time_t variable so that address can be used
		time_t rTime = getReadTime();
		//writes filename, date read, dimensions, bitdepth, readtime and zoom level to log
		//time format eg. Mon 4 Dec 2017 HH:MM:SS Timezone 
		log << "Filename: " << getName() << "\nDate read: " << std::put_time(std::localtime(&rTime), "%a %d %b %Y %T %Z") << "\nDimensions: " << getWidth() << " x "
			<< getHeight() << "\nBit depth: " << getBitDepth() << "\nRead time: " << getTimeToRead() << "ms" << "\nZoom level: " << getZoom() << "x\n" << std::endl;
		//close filestream
		log.close();
	}
	//catch error message
	catch (const char &e)
	{
		//display error message
		std::cout << e << std::endl;
		//close filetream
		log.close();
	}
}

/**************************************************
Nearest neighbour zoom algorithm adapted for use from:
http://tech-algorithm.com/articles/nearest-neighbor-image-scaling/
***************************************************/
//since only zooming by x2 and x4, we wont run into the rounding errors described on the site
void nearestNeigbourZoom(Image* img, int zoom)
{
	//alert user that zoom algorithm is being used
	std::cout << "\nUsing nearest neigbour zoom algorithm to scale image " << zoom << "x..." << std::endl;

	//calculate new width and height according to zoom level and store as ints
	int newWidth = img->getWidth() * zoom;
	int newHeight = img->getHeight() * zoom;

	//create temp output Image with the new image size
	Image output(newWidth, newHeight);
	//Set bit depth of output image to to the same value as the image being zoomed
	output.setBitDepth(img->getBitDepth());

	//calculate the multiplier to reach new width from old width
	//2x = 0.5		4x = 0.25
	float xRatio = img->getWidth() / (float)newWidth;
	//calculate the multiplier to reach new height from old height
	float yRatio = img->getHeight() / (float)newHeight;
	//float variables to store coordinates of original image
	float px, py;

	//loop through the theoretical height of the image
	for (int i = 0; i < newHeight; i++)
	{
		//loop through theoretical width of the image
		for (int j = 0; j < newWidth; j++)
		{
			//calculate coordinates for original pixel image width 
			px = std::floor(j * xRatio);
			//calculate coordinates for original pixel image height
			py = std::floor(i * yRatio);

			//assign colour values to pixels from input image to output pixel array
			output[(int)(i * newWidth) + j] = (*img)[(int)((py * img->getWidth()) + px)];

			/*the width value of the image is used to navigate down to the selected 'row' of an image where the
			px value is added on to reach the desired pixel*/
		}
	}

	//define a new string stream named filestream
	std::stringstream filename;
	//concatenate strings abd variables to form filename
	filename << "x" << zoom << " Zoom.ppm";

	//write image to PPM file using filename converted to char array
	writePPM(output, filename.str().c_str());
}