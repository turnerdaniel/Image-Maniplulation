#pragma once
#include "Image.h"

class ImageZoom : public Image
{
public:
	//ImageZoom constructors
	ImageZoom();
	ImageZoom(const unsigned int &, const unsigned int &);
	ImageZoom(const ImageZoom &); //copy constructor
	
	//ImageZoom destructor
	virtual ~ImageZoom();

	//ImageZoom operator overload
	ImageZoom operator= (const ImageZoom &); //deep copy

	//ImageZoom functions
	virtual void log();
};

void nearestNeigbourZoom(Image*, int);