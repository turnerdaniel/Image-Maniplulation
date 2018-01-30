#include "Image.h"
#include "ImageZoom.h"
#include <iostream> //output to screen and recieve inputs
#include <sstream> //generate successive filenames
#include <string> //use strings
#include <chrono> //get current time and find difference between times

int main()
{
	/************************************************************/
	/*Run in Release x64 for best results using images provided.*/
	/*			  Target 8.1 SDK using v140 toolkit			    */
	/************************************************************/

	std::cout << "**********************************" << std::endl;
	std::cout << "Image Stacker & Image Scaler" << std::endl;
	std::cout << "**********************************" << std::endl;

	//create STL vector to hold pointers to 13 images
	std::vector<Image*> images;
	//populate vector with object pointers
	for (int i = 0; i < 10; ++i)
	{
		//create objects
		images.push_back(new Image(3264, 2448));
	}

	//declares a timepoint that holds a steady clock start time
	std::chrono::steady_clock::time_point start;
	//declares a timepoint that holds a steady clock finish time
	std::chrono::steady_clock::time_point finish;
	//declare stringstream for generating filenames
	std::stringstream filename;

	//Notify user that images are about to be read
	std::cout << "Reading image:" << std::endl;

	//loop through number of images
	for (int i = 0; i < (int)images.size(); ++i)
	{
		//clear stringstream
		filename.str(std::string());
		//concatenate values and store in stringstream
		filename << "Images/ImageStacker/IMG_" << i + 1 << ".ppm";
		
		//store the start time in variable
		start = std::chrono::steady_clock::now();
		//read each file to current Image in images vector
		//converts string stream to characters
		*images.at(i) = readPPM(filename.str().c_str());
		//stores end time in variable
		finish = std::chrono::steady_clock::now();

		//counts the difference between the start and finish times in milliseconds
		//stores difference converted to int in timeToRead variable for current Image
		images.at(i)->setTimeToRead((int)std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count());

		//print out current progress
		std::cout << i + 1 << " ";
	}

	//notify users of image read success
	std::cout << "\nImages successfully read!\n" << std::endl;

	//perform mean blending on vector of image pointers
	meanBlending(images);

	//perform median blending on vector of image pointers
	medianBlending(images);

	//variable for holding user selections
	int selection;
	//Notify user of choices
	std::cout << "Which exit criteria do you request for sigma clipping?\n" << "1. Iterations\n" << "2. Tolerence level\n" << std::endl;
	std::cout << "Enter choice: ";
	//read in user choice and assign to selection variable
	std::cin >> selection;

	//choose code path based on user selection
	switch (selection) 
	{
	//enters '1'
	case 1: 
		//variable for holding number of iterations
		int iterations;
		//Ask user for value
		std::cout << "How many iterations: ";
		//loop to help validate input
		while (true)
		{
			//store value in iterations
			std::cin >> iterations;
			//validate input
			if (iterations >= 0 && !std::cin.fail())
			{
				if (iterations > 10)
					//notify user of consequence if they select a large number
					std::cout << "This may take a while..." << std::endl;
				//exit loop
				break;
			}
			else
				//notify user of invalid input and restart loop
				std::cout << "Invalid Input. Please ensure that you are inputting a valid number" << std::endl;
				//clear error flag from read in
				std::cin.clear();
				//clear previous input from buffer
				std::cin.ignore(10000, '\n');
		}

		//perform sigma clipping on vector of image pointers until iteration number is met
		sigmaClipping(images, iterations);
		//exit switch
		break;
	//enters '2'
	case 2:
		//variable for holding tolerence value
		float tolerence;
		//Alert user of requirements and ask for tolerece value
		std::cout << "Values must range from 0 to 1." << std::endl;
		std::cout << "A larger tolerence level results in a greater difference between the start and end standard deviation values." << std::endl;
		std::cout << "It is not guranteed that all pixels will meet the tolerence requirements!\n" << std::endl;
		std::cout << "Tolerence value: ";
		//loop to help validate input
		while (true)
		{
			//store value in tolerence variable
			std::cin >> tolerence;
			if (tolerence >= 0 && tolerence <= 1)
				//exit loop if within 0 and 1
				break;
			else
				//notify user of invalid input and restart loop
				std::cout << "Invalid Input. Please ensure that your value is between 0 and 1." << std::endl;
				//clear error flag from read in
				std::cin.clear();
				//clear previous input from buffer
				std::cin.ignore(10000, '\n');
		}

		//perform sigma clipping on vector of image pointers until tolerence number is met
		sigmaClipping(images, tolerence);
		//exit switch
		break;
	//else
	default:
		//notify user of invalid input
		std::cout << "Incorrect selection!\n" << std::endl;
		//clear error flag from read in
		std::cin.clear();
		//clear previous input from buffer
		std::cin.ignore(10000, '\n');
		//exit switch
		break;
	}
	
	//loop through objects in images vector
	for (int i = 0; i < (int)images.size(); ++i)
	{
		//send image info to log file
		images.at(i)->log();
		//deallocate memory used by Images
		delete images.at(i);
	}

	//create Image (base) pointer for Image Zoom (derived) object
	Image *zoom = new ImageZoom(750, 750);

	//store start time for reading image
	start = std::chrono::steady_clock::now();
	//read PPM image and assign to zoom
	*zoom = readPPM("Images/Zoom/zIMG_1.ppm");
	//store end time for reading image
	finish = std::chrono::steady_clock::now();

	//stores difference converted to int in timeToRead variable for current Image representing image read time
	zoom->setTimeToRead((int)std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count());

	//alert user fo zoom choice
	std::cout << "Select the amount you wish to zoom by:" << "\n1. 2x" << "\n2. 4x\n" << std::endl;
	std::cout << "Enter choice: ";

	//loop for input valdation
	while (true)
	{
		//store input in selection variable 
		std::cin >> selection;
		if (selection != 1 || selection != 2)
		{
			//exit loop if between 1 & 2
			break;
		}
		else
		{
			//alert user of invalid input and restart loop
			std::cout << "Invalid selection. Please enter either: 1 or 2." << std::endl;
			//clear error flag from read in
			std::cin.clear();
			//clear previous input from buffer
			std::cin.ignore(10000, '\n');
		}
	}

	//alter code path based on selection
	switch (selection)
	{
	//enter '1'
	case 1:
		//set zoom variable for object to selected value
		zoom->setZoom(2);
		//perform NN algorithm on zoom to scale x2
		nearestNeigbourZoom(zoom, 2);
		break;
	//enter '2'
	case 2:
		//set zoom variable for object to selected value
		zoom->setZoom(4);
		//perform NN algorithm on zoom to scale x4
		nearestNeigbourZoom(zoom, 4);
		break;
	//else
	default:
		//notify user of invalid input
		std::cout << "You did not input a valid zoom level.\n" << std::endl;
		//clear error flag from read in
		std::cin.clear();
		//clear previous input from buffer
		std::cin.ignore(10000, '\n');
		break;
	}

	//output zoom info to log file
	zoom->log();

	//deallocate memory used by zoom
	delete zoom;

	//alert user of log generation for image and imageZoom class
	std::cout << "Logs have been generated and are available within the 'Logs' directory." << std::endl;

	//Allow user to see output on screen before program close
	system("pause");
	return 0;
}
//end of main