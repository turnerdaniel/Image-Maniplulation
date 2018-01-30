#pragma once
#include <vector> //use vector container

class Image
{
public:
	struct Rgb
	{
		//Rgb constructors
		Rgb(); //default
		Rgb(float); 
		Rgb(float, float, float);

		//Rgb operator overloads
		friend Rgb operator+ (Rgb, const Rgb &);
		Rgb& operator+= (const Rgb &);
		friend Rgb operator- (Rgb, const Rgb &);
		Rgb& operator-= (const Rgb &);
		Rgb& operator/ (const Rgb &);
		Rgb& operator/= (const Rgb &);
		Rgb& operator= (const Rgb &); //deep copy

		//Rgb variables
		float r; //pixel red value
		float g; //pixel green value
		float b; //pixel blue value
	};

	//Image constructors
	Image(); //default
	Image(const unsigned int &, const unsigned int &, const Rgb & = kBlack); //kBlack is a default parameter
	Image(const Image &); //copy
	virtual ~Image();

	//Image operator overloads
	Image& operator=(const Image &); //deep copy
	const Rgb& operator[] (const unsigned int &) const;
	Rgb& operator[] (const unsigned int &);

	//Image member functions
	virtual void log();

	//Getter functions
	Rgb* getPixels() const;
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	unsigned int getSize() const;
	unsigned int getBitDepth() const;
	std::string getName() const;
	time_t getReadTime() const;
	int getTimeToRead() const;
	Rgb getBlack() const;
	Rgb getWhite() const;
	Rgb getRed() const;
	Rgb getGreen() const;
	Rgb getBlue() const;
	int getZoom() const;

	//Setter functions
	void setPixels(Rgb*);
	void setWidth(unsigned int);
	void setHeight(unsigned int);
	void setBitDepth(unsigned int);
	void setName(std::string);
	void setReadTime(time_t);
	void setTimeToRead(int);
	void setZoom(int);

private:
	Rgb *pixels; //Pointer to 1D array of pixels 
	unsigned int w; //Image width
	unsigned int h; //Image height
	unsigned int b; //Image bit depth
	std::string name; //Image filename
	time_t readTime; //Time current image was read
	int timeToRead; //Time taken to read image
	static const Rgb kBlack, kWhite, kRed, kGreen, kBlue; //Preset colors 
	int zoom; //Image zoom level
};

//Image Functions

Image readPPM(const char*);
void writePPM(const Image &, const char*);

double median(std::vector<float> &);
double sDeviation(std::vector<float> &);
double mean(std::vector<float> &);
void toFloats(Image* &, std::vector<float> &, std::vector<float> &, std::vector<float> &, int &);

void meanBlending(std::vector<Image*> &);

void medianBlending(std::vector<Image*> &);

void sigmaClipping(std::vector<Image*> &, int);
void sigmaClipping(std::vector<Image*> &, float);