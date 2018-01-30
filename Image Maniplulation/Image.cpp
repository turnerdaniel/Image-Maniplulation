#define _CRT_SECURE_NO_WARNINGS //allow use of std::localtime and std::put_time 
								//may cause data races in multi-threaded application
#include "Image.h"
#include <fstream> //output log file
#include <sstream> //concatenate strings
#include <chrono> //getting current date
#include <iomanip> //outputting time
#include <iostream> //alert user to error

//***Image RGB Structure***

//Constructors - member initialisation list
//default //set pixel to black
Image::Rgb::Rgb() : 
		r(0),
		g(0),
		b(0)
{}
//set pixel rgb values to parmeter value
Image::Rgb::Rgb(float c) : 
	r(c),
	g(c),
	b(c)
{}
//set individual pixel rgb to value of each parameter
Image::Rgb::Rgb(float _r, float _g, float _b) :  
	r(_r),
	g(_g),
	b(_b)
{}

//Rgb structure overloads
//return a copy of left Rgb value that has had addition operation performed
//passing first parameter by value allows for chained addition operations
/*friend*/ Image::Rgb operator+ (Image::Rgb rgb, const Image::Rgb &rgb2)
{
	rgb.r += rgb2.r, rgb.g += rgb2.g, rgb.b += rgb2.b;
	return rgb;
}
//return reference to Rgb struct where pixel rgb values have been added together
Image::Rgb& Image::Rgb::operator+= (const Image::Rgb &rgb)
{
	r += rgb.r, g += rgb.g, b += rgb.b;
	return *this;
}
//return a copy of left Rgb value that has had subtraction operation performed
//passing first parameter by value allows for chained subtraction operations
/*friend*/ Image::Rgb operator- (Image::Rgb rgb, const Image::Rgb &rgb2)
{
	rgb.r -= rgb2.r, rgb.g -= rgb2.g, rgb.b -= rgb2.b;
	return rgb;
}
//return reference to Rgb struct where pixel rgb values have been subtracted
Image::Rgb& Image::Rgb::operator-= (const Image::Rgb &rgb)
{
	r -= rgb.r, g -= rgb.g, b -= rgb.b;
	return *this;
}
//return reference to Rgb struct where pixel rgb values have been divided by related values of another Rgb struct
Image::Rgb& Image::Rgb::operator/ (const Image::Rgb &rgb)
{
	r /= rgb.r, g /= rgb.g, b /= rgb.b;
	return *this;
}
//return by reference Rgb where pixel values have been divided by related values of another Rgb struct
Image::Rgb& Image::Rgb::operator/= (const Image::Rgb &rgb)
{
	r /= rgb.r, g /= rgb.g, b /= rgb.b;
	return *this;
}
//copy assignemnt operator
//return by reference Rgb object with copied values
Image::Rgb& Image::Rgb::operator= (const Image::Rgb &rgb)
{
	r = rgb.r, g = rgb.g, b = rgb.b;
	return *this;
}

//***Image class***

//Image Constructors
//default //assigns default values
Image::Image() :
	w(0),
	h(0),
	b(0),
	name(""),
	readTime(0),
	timeToRead(0),
	zoom(1),
	pixels(nullptr) 
{/*empty image*/}
//3rd argument (c) has default value of kBlack
//assigns width, height and colour according to parameter
Image::Image(const unsigned int &_w, const unsigned int &_h, const Rgb &c) : 
	w(_w),
	h(_h),
	b(0),
	name(""),
	readTime(0),
	timeToRead(0),
	zoom(1),
	pixels(nullptr) 
{
	//dynamic array created = size of image (w * h) 
	pixels = new Rgb[w * h];

	//pixel array filled with colour parameter
	for (int i = 0; (unsigned int)i < w * h; ++i)
		pixels[i] = c; 
}
//copy constructor
Image::Image(const Image &img)
{
	//variables copied 
	w = img.w;
	h = img.h;
	b = img.b;
	name = img.name;
	readTime = img.readTime;
	timeToRead = img.timeToRead;
	zoom = img.zoom;

	//pixel array size copied
	pixels = new Rgb[img.w * img.h];
	//pixel array content copied
	for (int i = 0; (unsigned int)i < img.w * img.h; ++i)
		pixels[i] = img.pixels[i];
}
//Image class destructor
Image::~Image() 
{
	//releases memory used by pixels dynamic array
	if (pixels != nullptr)
	{
		delete[] pixels;
	}
}

//Image overloads
//copy assignment operator
//return reference to Image where values have been copied
Image& Image::operator=(const Image& img)
{
	//pixel array size copied to new dynamic array
	Rgb *newPixels = new Rgb[img.w * img.h];
	//pixel array content copied
	for (int i = 0; (unsigned int)i < img.w * img.h; ++i)
		newPixels[i] = img.pixels[i];
	//releases memory of old array
	delete[] pixels;
	//array pointer & variables copied 
	pixels = newPixels;
	w = img.w;
	h = img.h;
	b = img.b;
	name = img.name;
	readTime = img.readTime;
	timeToRead = img.timeToRead;
	zoom = img.zoom;

	return *this;
}
//Allow read only access to private pixel array index
//returns pixel array value at the index chosen with subscript operator
const Image::Rgb& Image::operator[] (const unsigned int &i) const
{
	return pixels[i]; 
}
//Allow read and write access to private pixel array index
Image::Rgb& Image::operator[] (const unsigned int &i)
{
	return pixels[i];
}

//***Image Variable Definitions***

const Image::Rgb Image::kBlack = Image::Rgb(0); //#000000
const Image::Rgb Image::kWhite = Image::Rgb(1); //#FFFFFF
const Image::Rgb Image::kRed = Image::Rgb(1, 0, 0); //#FF0000
const Image::Rgb Image::kGreen = Image::Rgb(0, 1, 0); //#00FF00
const Image::Rgb Image::kBlue = Image::Rgb(0, 0, 1); //#0000FF

//Image Member Functions
//Outputs log to file containing info on Image
void Image::log()
{
	//declares a time_t that holds the time now according to the system clock
	time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	//declare stringstream for generating filenames
	std::stringstream filename;
	//concatenate strings and current time 
	//format: YYYY-MM-DD
	filename << "Logs/image-" << std::put_time(std::localtime(&time), "%F") << ".txt";
	//declare output file stream for log
	std::ofstream log;
	try
	{
		//opens filename converted to array of chars and uses append mode
		log.open(filename.str().c_str(), std::ios::app);
		//check to see if file was opened correctly and throw error if not
		if (log.fail())
			throw("Error during log generation for image blend operation.");
		//get the read time of the file and store within time_t variable so that address can be used
		time_t rTime = getReadTime();
		//writes filename, date read, dimensions, bitdepth and readtime to log
		//time format eg. Mon 4 Dec 2017 HH:MM:SS Timezone 
		log << "Filename: " << getName() << "\nDate read: " << std::put_time(std::localtime(&rTime), "%a %d %b %Y %T %Z") << "\nDimensions: " << getWidth() << " x " << getHeight() << "\nBit depth: " << getBitDepth() << "\nRead time: " << getTimeToRead() << "ms\n" << std::endl;
		//close filestream
		log.close();
	}
	catch (const char &e)
	{
		//display error message
		std::cout << e << std::endl;
		//close filetream
		log.close();
	}
}

//Getter functions
//returns pointer to pixel array
//const since value wont be changed by getters
Image::Rgb* Image::getPixels() const
{
	return pixels;
}
unsigned int Image::getWidth() const
{
	return w;
}
unsigned int Image::getHeight() const
{
	return h;
}
//returns height * width of image instead of performing to get calls
unsigned int Image::getSize() const
{
	return w * h;
}
unsigned int Image::getBitDepth() const
{
	return b;
}
std::string Image::getName() const
{
	return name;
}
time_t Image::getReadTime() const
{
	return readTime;
}
int Image::getTimeToRead() const
{
	return timeToRead;
}
Image::Rgb Image::getBlack() const
{
	return kBlack;
}
Image::Rgb Image::getWhite() const
{
	return kWhite;
}
Image::Rgb Image::getRed() const
{
	return kRed;
}
Image::Rgb Image::getGreen() const
{
	return kGreen;
}
Image::Rgb Image::getBlue() const
{
	return kBlue;
}
int Image::getZoom() const  
{ 
	return zoom;
}

//Setter functions
//allows new memory to be allocated to pixels
void Image::setPixels(Rgb *rgb)
{
	pixels = rgb;
}
void Image::setWidth(unsigned int width)
{
	w = width;
}
void Image::setHeight(unsigned int height)
{
	h = height;
}
void Image::setBitDepth(unsigned int bitDepth)
{
	b = bitDepth;
}
void Image::setName(std::string n)
{
	name = n;
}
void Image::setReadTime(time_t time)
{
	readTime = time;
}
void Image::setTimeToRead(int time)
{
	timeToRead = time;
}
void Image::setZoom(int z) 
{
	zoom = z;
}
