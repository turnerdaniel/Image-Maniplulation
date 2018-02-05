# Image Manipulation

A console application which performs image processing techniques on portable pixmap (.ppm) images with the aim of reducing visual noise. Functionality is also present which enables an image to be scaled to various sizes. 

## Getting Started
This program was developed using Visual Studio Community 2015. It is recommend to install the latest version of Visual Studio Community from  the [Visual Studio website](https://www.visualstudio.com/downloads/) in order to view this project. 

Once installed, clone the repository to your computer. Now, open the Visual Studio solution file (.sln) to view the application code. From here, the program should be executed in release mode using the drop-down menu at the top. 

## Algorithms
 - Mean blending - uses mean to calculate the average pixel value.
 - Median blending - uses median to calculate average pixel value
 - Sigma clipped mean - removes values that are outside of median ± standard deviation (σ). Use the mean of the remaining pixel values.

## Authors

**Daniel Turner** - [turnerdaniel](https://github.com/turnerdaniel)

## License

This project is licensed under the MIT License - see the [LICENSE.md](https://github.com/turnerdaniel/Dice-Game/blob/master/LICENSE) file for details.

## Acknowledgements

 - [Scrathapixel](https://www.scratchapixel.com/lessons/digital-imaging/simple-image-manipulations?url=digital-imaging/simple-image-manipulations) - read/write PPM framework
 - [Tech Algorithm](http://tech-algorithm.com/articles/nearest-neighbor-image-scaling/) - nearest neighbor zoom algorithm 
