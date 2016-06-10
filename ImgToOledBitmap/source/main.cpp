#include <ffw.h>

///=============================================================================
static void printUsage(){
	std::cout << "Usage: " << std::endl;
	std::cout << "ImgToOledBitmap.exe -i path_to_image.xyz" << std::endl << "\tWill convert an image to OLED image" << std::endl << std::endl;
	std::cout << "ImgToOledBitmap.exe -f path_to_font_table.xyz" << std::endl << "\tWill convert an image to OLED font" << std::endl << std::endl;
	std::cout << "Image files must be either png, bmp, pbm, tiff, or tga. Do not use jpg files and the format must be either 8-bit Grayscale, 24-bit RGB, or 32-bit RGBA!" << std::endl;
	//system("pause");
}

///=============================================================================
int main(int argc, char *argv[]){
    std::string path;
	bool toImage = false;
	bool toFont = false;

	if(argc > 1){
		for(int i = 1; i < argc; i++){
			std::string arg(argv[i]);

			if(arg[0] == '-'){
				if(arg == "-i")toImage = true;
				else if(arg == "-f")toFont = true;
				else {
					std::cout << "Unknown argument: " << arg << std::endl;
					printUsage();
					return 0;
				}
				continue;
			}

			if(path.size() > 0)path += " ";
			path += arg;
		}
	}
	else {
		printUsage();
		return 0;
	}

	if(path.size() == 0 || (!toImage && !toFont)){
		printUsage();
		return 0;
	}

	std::cout << "Input file: \"" << path << "\"" << std::endl;

	std::string fileDir;
	std::string fileName;
	std::string fileExt;
	ffw::getFilePathProperties(path, &fileDir, &fileName, &fileExt);

	if(toFont){
		std::cout << "Converting to a font table..." << std::endl;
		std::string outPath = fileDir + fileName + "-oledfont.txt";
		std::cout << "Output file: " << outPath << std::endl;

		unsigned char* pixels = NULL;
		int width;
		int height;
		ffw::imageType type;

		if(!ffw::loadImage(path, &pixels, &width, &height, &type)){
			std::cout << "Failed to open an input image file!" << std::endl;
			//system("pause");
			return 1;
		}

		int total = 0;

		if(height == width/2){
			total = 128;
		}

		if(height == width){
			total = 128;
		}

		if(total == 0 || width % 8 != 0 || height % 8 != 0){
			std::cout << "To export the font table the image height must be either: height = width/2 or height = width, and width and height must be multiples of 8!" << std::endl;
			//system("pause");
			return 1;
		}

		std::fstream out(outPath, std::ios::out | std::ios::trunc);
		if(!out){
			std::cout << "Failed to open an output file!" << std::endl;
			//system("pause");
			return 1;
		}

		int count = 0;
		int chr = 0;
		unsigned char temp;

		int charHeight = width/16;
		int charWidth = charHeight;
		//if(charWidth % 2 != 0)charWidth--;

		int charOffset = (charHeight - charWidth) / 2;
		std::cout << "char width: " << charWidth << " height: " << charHeight << " offset: " << charOffset << std::endl;

		std::string fontName = fileName;
		for(auto& c : fontName){
			if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))continue;
			c = '_';
		}

		out << "// Font: " << fontName << " extracted from: " << fileName << "." << fileExt << " size: " << width << "x" << height << std::endl;
		out << "static const unsigned char " << fontName << "_width = " << charWidth << ";" << std::endl;
		out << "static const unsigned char " << fontName << "_height = " << charHeight << ";" << std::endl;
		out << "static const unsigned char " << fontName << "_font [128 * " << (charHeight * charWidth/2) << "] = {" << std::endl;

		for(int cy = 0; cy < 8; cy++){
			for(int cx = 0; cx < 16; cx++){

				out << "// Char: " << chr << std::endl;

				for(int y = 0; y < charHeight; y++){
					for(int x = 0; x < charWidth; x++){
						int pxWidth = cx*charHeight + x + charOffset;
						int pxHeight = cy*charHeight + y;

						unsigned char px;

						if(type == ffw::imageType::GRAYSCALE_8){
							px = pixels[(height - pxHeight -1) * width + pxWidth];
						} else if(type == ffw::imageType::RGB_888){
							ffw::vec3f v;
							v.x = pixels[(height - pxHeight -1) * width * 3 + pxWidth * 3 + 0] / 255.0f;
							v.y = pixels[(height - pxHeight -1) * width * 3 + pxWidth * 3 + 1] / 255.0f;
							v.z = pixels[(height - pxHeight -1) * width * 3 + pxWidth * 3 + 2] / 255.0f;
							px = v.length() * 255;
						} else if(type == ffw::imageType::RGB_ALPHA_8888){
							ffw::vec3f v;
							v.x = pixels[(height - pxHeight -1) * width * 4 + pxWidth * 4 + 0] / 255.0f;
							v.y = pixels[(height - pxHeight -1) * width * 4 + pxWidth * 4 + 1] / 255.0f;
							v.z = pixels[(height - pxHeight -1) * width * 4 + pxWidth * 4 + 2] / 255.0f;
							px = v.length() * 255;
						}

						if(count % 2 == 0){
							temp = (px >> 4) << 4;
						} else {
							temp = temp | (px >> 4);

							out << "0x" << ffw::valToHex(temp) << ", ";
						}

						//out << (px > 0 ? "X" : " ");
						//out << pxWidth << "x" << pxHeight << ", ";

						count++;

						if(count >= charWidth){
							count = 0;
							out << "\n";
						}
					}
				}

				out << std::endl;

				chr++;
			}
		}

		out << "};" << std::endl;

		delete[] pixels;
	}

	if(toImage){
		std::cout << "Converting to an image..." << std::endl;
		std::string outPath = fileDir + fileName + "-oledimg.txt";
		std::cout << "Output file: " << outPath << std::endl;

		unsigned char* pixels = NULL;
		int width;
		int height;
		ffw::imageType type;

		if(!ffw::loadImage(path, &pixels, &width, &height, &type)){
			std::cout << "Failed to open an input image file!" << std::endl;
			//system("pause");
			return 1;
		}

		std::fstream out(outPath, std::ios::out | std::ios::trunc);
		if(!out){
			std::cout << "Failed to open an output file!" << std::endl;
			//system("pause");
			return 1;
		}

		int count = 0;
		unsigned char temp;

		out << "// Image: " << fileName << "." << fileExt << " size: " << width << "x" << height << std::endl;
		out << "// Actual size: " << width/2 << "x" << height << " bytes!" << std::endl;
		out << "static const unsigned int " << fileName << "_width = " << width << ";" << std::endl;
		out << "static const unsigned int " << fileName << "_height = " << height << ";" << std::endl;
		out << "static const unsigned char " << fileName << "_pixels [" << width/2 << "*" << height << "] = {" << std::endl;

		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				unsigned char px;

				if(type == ffw::imageType::GRAYSCALE_8){
					px = pixels[(height - y -1) * width + x];
				} else if(type == ffw::imageType::RGB_888){
					ffw::vec3f v;
					v.x = pixels[(height - y -1) * width * 3 + x * 3 + 0] / 255.0f;
					v.y = pixels[(height - y -1) * width * 3 + x * 3 + 1] / 255.0f;
					v.z = pixels[(height - y -1) * width * 3 + x * 3 + 2] / 255.0f;
					px = v.length() * 255;
				} else if(type == ffw::imageType::RGB_ALPHA_8888){
					ffw::vec3f v;
					v.x = pixels[(height - y -1) * width * 4 + x * 4 + 0] / 255.0f;
					v.y = pixels[(height - y -1) * width * 4 + x * 4 + 1] / 255.0f;
					v.z = pixels[(height - y -1) * width * 4 + x * 4 + 2] / 255.0f;
					px = v.length() * 255;
				}

				if(count % 2 == 0){
					temp = (px >> 4) << 4;
				} else {
					temp = temp | (px >> 4);

					out << "0x" << ffw::valToHex(temp) << ", ";
				}

				count++;

				if(count >= width){
					count = 0;
					out << "\n";
				}
			}
		}

		out << "};" << std::endl;

		delete[] pixels;
	}

	return 0;
}
