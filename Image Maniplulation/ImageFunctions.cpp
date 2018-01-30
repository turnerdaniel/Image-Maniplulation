#include "Image.h"
#include <iostream> //outputting to screen
#include <fstream> //reading and writing images
#include <algorithm> //sorting vectors and removing values from vector
#include <sstream> //outputting strings to filestream
#include <chrono> //getting current time

//Read ppm files into the code
//They need to be in 'binary' format (P6) with no comments in the header
//The first line is the 'P'number - P6 indicates it is a binary file, then the image dimensions and finally the colour range
//eg:	P6
//3264 2448
//255
Image readPPM(const char *filename)
{
	//declare input file stream
	std::ifstream ifs;

	//open file location specified by parameter using binary mode
	ifs.open(filename, std::ios::binary);

	//declare new Image in automatic storage
	Image src;
	try {
		//check to see if file can be opened 
		if (ifs.fail())
		{
			throw("Can't open the input file - is it named correctly/is it in the right directory?");
		}

		//Variable decleration
		std::string header;
		int w; //width
		int h; //height
		int b; //bit depth

		//read image type (P6 for binary ppm files) and store in header
		ifs >> header;
		//compare image type to "p6" to check if binary file
		if (strcmp(header.c_str(), "P6") != 0)
			//if NOT binary file:
			throw("Can't read the input file - is it in binary format (Has P6 in the header)?");

		//read width, height and bit depth from header and assign to appropriate variable
		ifs >> w >> h >> b;
		src.setWidth(w);
		src.setHeight(h);
		src.setBitDepth(b);
		//assign filename to image object
		src.setName(filename);
		//gets current time and converts to type time_t for storage in current read time variable
		src.setReadTime(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

		//throw exception if bad_alloc
		//create dynamic array of Rgb structure with the total size of the image
		src.setPixels(new Image::Rgb[w * h]);

		//skip empty lines
		ifs.ignore(256, '\n');
		//declare uchar array to hold colour values
		unsigned char pix[3];

		//loop through each pixel in image
		//reads 3 sets of characters which hold colour values and adds them to same index of the array containg the Rgb structure
		for (int i = 0; i < w * h; ++i)
		{
			//reinterpret_cast used to convert between pointers of unsigned char and char
			//********* Can use "ifs.read((char*)pix, 3);" to be more strongly typed ***********//
			ifs.read(reinterpret_cast<char *>(pix), 3);
			//values divided by 255 to interval between 0 and 1
			src[i].r = pix[0] / 255.f;
			src[i].g = pix[1] / 255.f;
			src[i].b = pix[2] / 255.f;
		}

		//closes file
		ifs.close();
	}
	//catch error by reference
	catch (const char *err)
	{
		//print formtted message
		//stderr = output stream for error messages, "%s\n" = output string of characters & new line, err = data.
		fprintf(stderr, "%s\n", err);
		//closes file
		ifs.close();
	}

	//returns object
	return src;
}


//Write data out to a ppm file
//Constructs the header as above
void writePPM(const Image &img, const char *filename)
{
	//check to see if writing to image with no size
	if (img.getWidth() == 0 || img.getHeight() == 0)
	{
		//print formatted message
		fprintf(stderr, "Can't save an empty image\n");
		return;
	}

	//declare output file stream
	std::ofstream ofs;

	try {
		//open file location from parameter using binary mode 
		ofs.open(filename, std::ios::binary);

		//checks to see if file can be opened
		if (ofs.fail())
			throw("Can't open output file");

		//output header info in correct format
		ofs << "P6\n" << img.getWidth() << " " << img.getHeight() << "\n" << img.getBitDepth() << "\n";
		//std::cout << "P6\n" << img.w << " " << img.h << "\n255\n";

		//Variable decleration
		unsigned char r; //red
		unsigned char g; //green
		unsigned char b; //blue

		//loop through each pixel in the image, clamp and convert to byte format (0 - 1 value to 0 - 255 colour value)
		//write colour value for each pixel 
		for (int i = 0; (unsigned int)i < img.getSize(); ++i)
		{
			//static_cast used for conversion from float to unsigned char
			//min returns the smallest of 2 parameters
			r = static_cast<unsigned char>(std::min(1.f, img[i].r) * 255);
			g = static_cast<unsigned char>(std::min(1.f, img[i].g) * 255);
			b = static_cast<unsigned char>(std::min(1.f, img[i].b) * 255);
			ofs << r << g << b;
		}

		//closes file
		ofs.close();

		//Confirm image write
		std::cout << "Image written!\n" << std::endl;
	}
	//catch error by reference
	catch (const char *err)
	{
		//print formatted error message
		fprintf(stderr, "%s\n", err);
		//closes file
		ofs.close();
	}
}

//doubles used to help minimise rounding errors
double median(std::vector<float> &colour)
{
	//sort vector in ascending order
	std::sort(colour.begin(), colour.end()); 
	//store size of array fore effeciency
	int size = (int)colour.size();

	//size is even
	if (size % 2 == 0)
	{
		try
		{
			//return average of 2 middle values
			return (double)((colour.at(size / 2) + colour.at((size / 2) - 1)) / 2);
		}
		//catch vector index out of range range - happens when rounding error produces a vector with 0 size
		catch (const std::out_of_range &e)
		{
			//alert user to error
			std::cout << "There was an error calculting the true median of a pixel.\n" << "Error: " << e.what() << std::endl;
			//return middle value
			return (double)colour.at(size / 2);
		}
	}
	//size is odd
	else
	{
		//return middle value
		return (double)colour.at(size / 2);
	}
}
double sDeviation(std::vector<float> &colour)
{
	//hold squared sum of values 
	double standardDev = 0;
	//hold size of vector
	int size = (int)colour.size();
	//hold mean on vector
	double m = mean(colour);

	//loop through vector
	for (int i = 0; i < size; i++) 
	{
		//find the sum of each value minus the average of the entire vector, squared
		standardDev += pow(colour.at(i) - m, 2);
	}
	//divide the sum of squared averages by the size of the vector
	standardDev /= (double)size;

	//return square root of variance = standard deviation
	return sqrt(standardDev);
}
double mean(std::vector<float> &colour)
{
	//store sum
	double sum = 0;
	//store size of vector
	int noFloats = (int)colour.size();

	//loop through vector
	for (int i = 0; i < noFloats; ++i)
	{
		//add up value of each element and store in sum
		sum += (double)colour.at(i);
	}

	//return sum divided my number of elements
	return sum / (double)noFloats;
}
//converts a given pixel from an image into a the corresponding colour values
void toFloats(Image* &img, std::vector<float> &red, std::vector<float> &green, std::vector<float> &blue, int &pixel)
{
	//adds red value at pixel array to red vector
	red.push_back((*img)[pixel].r);
	//adds green value at pixel array to green vector
	green.push_back((*img)[pixel].g);
	//adds blue value at pixel array to blue vector
	blue.push_back((*img)[pixel].b);
}
//mean blending algorithm
void meanBlending(std::vector<Image*> &images)
{
	//notify user that blending has begun
	std::cout << "Mean blending started..." << std::endl;
	//get total size of first image and assign to variable
	int pixelCount = images.at(0)->getSize();
	//assign images vector size to variable
	int imageNo = (int)images.size();

	//create new temp image to output with size of first image in vector
	Image output(images.at(0)->getWidth(), images.at(0)->getHeight());
	//Set bit depth of output image to that of the first image
	output.setBitDepth(images.at(0)->getBitDepth());

	//create vectors to be used to store colour channels of each pixel
	std::vector<float> red, green, blue;

	//loop through each pixel
	for (int i = 0; i < pixelCount; ++i)
	{
		//empty vectors and set size to 0
		red.clear();
		green.clear();
		blue.clear();

		//loop through all images
		for (int j = 0; j < imageNo; ++j)
		{
			//convert colour channels for each image for a given pixel into vectors
			toFloats(images.at(j), red, green, blue, i);
		}

		//assign mean of values in red vector to the 'r' float value at the current index of the pixel array
		output[i].r = (float)mean(red);
		//mean of green values
		output[i].g = (float)mean(green);
		//mean of blue values
		output[i].b = (float)mean(blue);
	}
	//write output Image to PPM file named "Mean Blending"
	writePPM(output, "Mean Blending.ppm");
}
//median blendgin algorithm
void medianBlending(std::vector<Image*> &images)
{
	//notify user that blending has begun
	std::cout << "Median blending started..." << std::endl;
	//get totoal size of first image and assign to variable
	int pixelCount = images.at(0)->getSize();
	//assign images vector size to variable
	int imageNo = (int)images.size();

	//create new temp image to output with size of first image in vector
	Image output(images.at(0)->getWidth(), images.at(0)->getHeight());
	//Set bit depth of output image to that of the first image
	output.setBitDepth(images.at(0)->getBitDepth());

	//create vectors to be used to store colour channels of each pixel
	std::vector<float> red, green, blue;

	//loop trough each pixel
	for (int i = 0; i < pixelCount; ++i)
	{
		//empty vectors
		red.clear();
		green.clear();
		blue.clear();

		//loop through each image
		for (int j = 0; j < imageNo; ++j)
		{
			//convert 
			toFloats(images.at(j), red, green, blue, i);
		}

		//assign median of values in red vector to the 'r' float value at the current index of the pixel array
		output[i].r = (float)median(red);
		//median of green values
		output[i].g = (float)median(green);
		//median of blue values
		output[i].b = (float)median(blue);
	}
	//write output Image to PPM file named "Median Blending"
	writePPM(output, "Median Blending.ppm");
}
//sigma clipping algorithm based on iterations
void sigmaClipping(std::vector<Image*> &images, int iterations)
{
	//only performs algorithm if number of iterations is above 0
	if (iterations > 0)
	{
		//alert user that clipping has begun with the current parameters
		std::cout << "\nSigma Clipping until " << iterations << " iteration(s) have been performed..." << std::endl;

		//get total size of first image and assign to variable
		int pixelCount = images.at(0)->getSize();
		//create new temp image to output with size of first image in vector
		Image output(images.at(0)->getWidth(), images.at(0)->getHeight());
		//Set bit depth of output image to that of the first image
		output.setBitDepth(images.at(0)->getBitDepth());

		//Rgb struct to hold the standard devition values for the r, g and b vectors
		Image::Rgb sDeviationPixel, medianPixel;
		//Rgb struct to hold the uppper and lower bound values for the r, g and b vectors
		Image::Rgb upperBound, lowerBound;

		//create vectors to be used to store colour channels of each pixel
		std::vector<float> red, green, blue;
		//assign images vector size to variable
		int noImages = (int)images.size();

		//loop through each pixel
		for (int i = 0; i < pixelCount; ++i)
		{
			//empty vectors
			red.clear();
			green.clear();
			blue.clear();

			//loop through each image
			for (int j = 0; j < noImages; ++j)
			{
				//convert current pixel values to float vectors
				toFloats(images.at(j), red, green, blue, i);
			}

			//loop through each iteration
			for (int x = 0; x < iterations; ++x)
			{
				//find and store the median values of the r, g and b values
				medianPixel.r = (float)median(red);
				medianPixel.g = (float)median(green);
				medianPixel.b = (float)median(blue);

				//find and store the standard deviation values of the r, g and b values
				sDeviationPixel.r = (float)sDeviation(red);
				sDeviationPixel.g = (float)sDeviation(green);
				sDeviationPixel.b = (float)sDeviation(blue);

				//calculate and store the upper bound and lower bound by addition/subtraction of Rgbs
				upperBound = medianPixel + sDeviationPixel;
				lowerBound = medianPixel - sDeviationPixel;

				//remove_if moves all of the elements that do not satisfy the lambda expression to the back of the vector and returns the point at which those values begin
				//erase removes from the start of the unwanted values to the end of the vector

				//remove all values from red array that are smaller than the 'r' float value in lower bound and larger than the 'r' float value in upper bound
				red.erase(std::remove_if(red.begin(), red.end(), [&](float n) { return n < lowerBound.r || n > upperBound.r; }), red.end());
				//remove all values from green array that are smaller than the 'g' float value in lower bound and larger than the 'g' float value in upper bound
				green.erase(std::remove_if(green.begin(), green.end(), [&](float n) { return n < lowerBound.g || n > upperBound.g; }), green.end());
				//remove all values from blue array that are smaller than the 'b' float value in lower bound and larger than the 'b' float value in upper bound
				blue.erase(std::remove_if(blue.begin(), blue.end(), [&](float n) { return n < lowerBound.b || n > upperBound.b; }), blue.end());
			}
			//completed iterations

			//assign mean of remaining values in red vector to the 'r' float value at the current index of the pixel array
			output[i].r = (float)mean(red);
			//mean of green values
			output[i].g = (float)mean(green);
			//mean of blue values
			output[i].b = (float)mean(blue);

		}
		//gone through each pixel

		//write output Image to PPM file named "Sigma Clipping Iterations.ppm"
		writePPM(output, "Sigma Clipping Iterations.ppm");
	}
	//iterations less than 1
	else 
	{
		//alert user that they input an invalid number of iterations
		std::cout << "\nNo operations were performed since you entered an iteration value less than or equal to 0.\n" << std::endl;
	}
}
//sigma clipping algorithm based on iterations
void sigmaClipping(std::vector<Image*> &images, float tolerence)
{
	//only performs algorthm if tolerence is a positive number
	if (tolerence > 0)
	{
		//alert user that clipping has begun with the current parameters
		std::cout << "\nSigma Clipping until a tolerence level of " << tolerence << " is met..." << std::endl;

		//get total size of first image and assign to variable
		int pixelCount = images.at(0)->getSize();
		//create new temp image to output with size of first image in vector
		Image output(images.at(0)->getWidth(), images.at(0)->getHeight());
		//Set bit depth of output image to that of the first image
		output.setBitDepth(images.at(0)->getBitDepth());

		//Rgb struct to hold the standard devition and median values for the r, g and b vectors
		Image::Rgb originalSDeviationPixel, medianPixel, newSDeviationPixel;
		//Rgb struct to hold the upper and lower bound values for the r, g and b vectors
		Image::Rgb upperBound, lowerBound;

		//create vectors to be used to store colour channels of each pixel
		std::vector<float> red, green, blue;
		//assign images vector size to variable
		int noImages = (int)images.size();

		//Rgb struct to hold tolerence level for each colour channel
		Image::Rgb tolerneceLevel;
		//size variable to hold size of colour vectors to ensure infinte loops are escaped
		size_t size;


		//loop through each pixel
		for (int i = 0; i < pixelCount; ++i)
		{
			//empty contents on vectors
			red.clear();
			green.clear();
			blue.clear();

			//loop through each image
			for (int j = 0; j < noImages; ++j)
			{
				//convert current pixel values to float vectors
				toFloats(images.at(j), red, green, blue, i);
			}

			//find and store the median values of the r, g and b values
			medianPixel.r = (float)median(red);
			medianPixel.g = (float)median(green);
			medianPixel.b = (float)median(blue);

			//find and store the orignal standard deviaton values of the r, g and b channels
			originalSDeviationPixel.r = (float)sDeviation(red);
			originalSDeviationPixel.g = (float)sDeviation(green);
			originalSDeviationPixel.b = (float)sDeviation(blue);

			//calculate the upper and lower bound value figures for each of the colour channels 
			upperBound = medianPixel + originalSDeviationPixel;
			lowerBound = medianPixel - originalSDeviationPixel;

			//erase elements from vector that are larger than upper bounds or smaller than lower bounds
			red.erase(std::remove_if(red.begin(), red.end(), [&](float n) { return n < lowerBound.r || n > upperBound.r; }), red.end());

			green.erase(std::remove_if(green.begin(), green.end(), [&](float n) { return n < lowerBound.g || n > upperBound.g; }), green.end());

			blue.erase(std::remove_if(blue.begin(), blue.end(), [&](float n) { return n < lowerBound.b || n > upperBound.b; }), blue.end());

			//find and store new standard deviaton values of the r, g and b colour channlels
			newSDeviationPixel.r = (float)sDeviation(red);
			newSDeviationPixel.g = (float)sDeviation(green);
			newSDeviationPixel.b = (float)sDeviation(blue);

			//calculate tolerence level and store for each colour channel
			tolerneceLevel = (originalSDeviationPixel - newSDeviationPixel) / newSDeviationPixel;


			//loop until tolerenceLevel is greater than or equal to the user specified one
			while (tolerneceLevel.r < tolerence)
			{
				//calculate new median and store in median pixel
				medianPixel.r = (float)median(red);

				//calculate new upper and lower bounds 
				upperBound.r = medianPixel.r + newSDeviationPixel.r;
				lowerBound.r = medianPixel.r - newSDeviationPixel.r;

				//store size of red vector
				size = red.size();

				//erase elements from vector that are larger than upper bounds or smaller than lower bounds
				red.erase(std::remove_if(red.begin(), red.end(), [&](float n) { return n < lowerBound.r || n > upperBound.r; }), red.end());

				//calculate new standard deviation value
				newSDeviationPixel.r = (float)sDeviation(red);

				//check to see if the new deviation value is greater than 0 and that the new size is different to the old size
				if (newSDeviationPixel.r > 0 && size != red.size())
				{
					//calulate new tolerence value
					tolerneceLevel.r = (originalSDeviationPixel.r - newSDeviationPixel.r) / newSDeviationPixel.r;
				}
				else
				{
					//break while loop since it is unnecessary to perform operations on vectors which have already had all of their outlier values removed:
						//indicated by new sdeviation being 0 and by no chnage in size from the erase operation
					break;
				}
			}

			//operation repeated for each colour channel
			while (tolerneceLevel.g < tolerence)
			{
				medianPixel.g = (float)median(green);

				upperBound.g = medianPixel.g + newSDeviationPixel.g;
				lowerBound.g = medianPixel.g - newSDeviationPixel.g;

				size = green.size();

				green.erase(std::remove_if(green.begin(), green.end(), [&](float n) { return n < lowerBound.g || n > upperBound.g; }), green.end());

				newSDeviationPixel.g = (float)sDeviation(green);

				if (newSDeviationPixel.g > 0 && size != green.size())
					tolerneceLevel.g = (originalSDeviationPixel.g - newSDeviationPixel.g) / newSDeviationPixel.g;
				else
					break;
			}

			while (tolerneceLevel.b < tolerence)
			{
				medianPixel.b = (float)median(blue);

				upperBound.b = medianPixel.b + newSDeviationPixel.b;
				lowerBound.b = medianPixel.b - newSDeviationPixel.b;

				size = blue.size();

				blue.erase(std::remove_if(blue.begin(), blue.end(), [&](float n) { return n < lowerBound.b || n > upperBound.b; }), blue.end());

				newSDeviationPixel.b = (float)sDeviation(blue);

				if (newSDeviationPixel.b > 0 && size != blue.size())
					tolerneceLevel.b = (originalSDeviationPixel.b - newSDeviationPixel.b) / newSDeviationPixel.b;
				else
					break;
			}
			//completed checking tolerence value

			//assign mean of remaining values in red vector to the 'r' float value at the current index of the pixel array
			output[i].r = (float)mean(red);
			//mean of green values
			output[i].g = (float)mean(green);
			//mean of blue values
			output[i].b = (float)mean(blue);
		}		

		//gone through each pixel
		writePPM(output, "Sigma Clipping Tolerence.ppm");
	}
	//tolerence level less than or equal to 0
	else
	{
		//alert user of invalid tolerence level
		std::cout << "\nNo operations were performed since you entered a tolerence value less than or equal to 0.\n" << std::endl;
	}
}