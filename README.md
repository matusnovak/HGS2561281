# HGS2561281 OLED 256x128 Display library
(For Arduino and Energia)

This library is adaptation from <https://github.com/MartyMacGyver/OLED_SSD1322>

This library provides support for the HGS2561281 256x128 display which uses two SSD1322 chips to control two 256x64 OLED displays. Currently only SPI is supported. This library also contains an application to convert images to 4-bit image useable by this display. The application can also generate a font. The application can be found under ImgToOledBitmap/release/ folder.

# Features
* Drawing a rectangle with colors from 0x0 to 0xF
* Drawing a string of characters with user defined font
* Drawing images
 
# Limitations
* X coordinates must be in multiple of 4, this affects X position and width of images, fonts, and shapes - rectangles. *Y coordinates are not affected*. (For example, drawing a rectangle on position [50, 50] will not be displayed properly (50 mod 4 != 0) instead draw it on [48, 50])

# Install (Arduino)
1. [Download HGS2561281 library](https://github.com/matusnovak/HGS2561281/archive/master.zip)
2. Find "HGS2561281-master" folder inside the ZIP archive and copy it to `My Documents\Arduino\libraries`
3. Rename "HGS2561281-master" to "HGS2561281"

# Install (Energia)
1. [Download HGS2561281 library](https://github.com/matusnovak/HGS2561281/archive/master.zip)
2. Find "HGS2561281-master" folder inside the ZIP archive and copy it to `My Documents\Energia\libraries`
3. Rename "HGS2561281-master" to "HGS2561281"

# Example code
``` C++
#include <SPI.h> // Sometimes Arduino and Energia forgets to include SPI dir if this line is not present
#include <HGS2561281.h>

// Instance to our LCD
// You will need to replace the arguments
// with the correct pin numbers
HGS2561281 lcd(RESET, CS1, CS2, DC, WR);

void setup(){
  
  // This may be needed by Energia...
  pinMode(CSLK_PIN, OUTPUT);
  pinMode(SDIN_PIN, OUTPUT);

  // Initialise the display
  lcd.init();
  
  // Clear pixels
  lcd.clear();
  
  // Now we can draw things!
  
  // Set the draw color to 0xF (15)
  // 0xF is a default color
  lcd.setDrawColor(0xF);
  
  // Draw single rectangle at pos [16, 10] with a size of [32, 12]
  lcd.drawRectangle(16, 10, 32, 12);
}

void loop(){
}
```

# How to display images

To display an image, you will need to convert your image to a grayscale 4-bit `unsigned char` array. Each `unsigned char` must contain two pixels. You can use to following Python script to convert any image to a grayscale 4-bit array that will appear in the stdout.

```python
# Install PIL as:
# python3 -m pip install Pillow

from PIL import Image
import sys

HEX = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f']

def main():
    if len(sys.argv) != 2:
        print('Invalid number of arguments!')
        print(f'Usage: {sys.argv[0]} input.png')
        sys.exit(1)
        
    img = Image.open(sys.argv[1]).convert('L')
    pixels = img.load()
    width, height = img.size

    if width % 2 != 0:
        print('Image width must be multiple of 2')

    for y in range(0, height):
        for x in range(0, width, 2):
            a = pixels[x, y] >> 4
            b = pixels[x + 1, y] >> 4
            print(f'0x{HEX[a]}{HEX[b]}, ', end='')

        print('')

if __name__ == "__main__":
    main()
```

Usage of above script:

```
> python convert_to_4bit.py letter_a.png > output.txt
```

Then, define your image in your source code as:

```cpp
// Assuming the image is 16x16 pixels in size
// sizeof = width * height / 2
// We divide by 2 because one byte contains two 4-bit pixels!
static const unsigned char pixels[ 16 * 16 / 2 ] = {
    // the stdout from the python script above
};
```

And render your image as:

```cpp
// Assuming the image is 16x16 pixels in size
lcd.drawPixels(posX, posY, 16, 16, pixels)
```

You can find more information in this issue: [HGS2561281/issues/2](https://github.com/matusnovak/HGS2561281/issues/2)

# How to display images (outdated)

(An example is provided in the example folder)

First we need to generate 4-bit image... for that we need the convertor!

1. Find "ImgToOledBitmap/release" folder inside the library folder.
2. Copy or move it to a safe place.

To generate 4-bit image with:

1. Open Command Line Terminal (CMD)
2. Use CD command to go to the directory where `ImgToOledBitmap.exe` is located
3. Run: `ImgToOledBitmap.exe -i path_to_image/your_image_name.png` (notice the -i option!)
4. A new file will be generated `your_image_name-oledimg.txt` containing 4-bit pixel array
5. Copy this pixel array `static const char your_image_name_pixels[] = { ... };` found in the generated txt file to your Arduino/Energia sketch.
6. Use `lcd.drawPixels(posX, posY, width, height, pointer to pixel array)` function to draw the pixels. Note that the width and height are the dimensions of the image. For example, if the original image dimensions are 128x128, the pixel array will be size of 128x128 / 2 bytes, then to draw the image at [0, 0], use it like this: `lcd.drawPixels(0, 0, 128, 128, your_image_name_pixels);`

Supported image formats: PNG, BMP, PBM, TGA, TIFF
Supported pixel formats: 8bpp (Grayscale), 24bpp (RGB), 32bpp (RGBA)

RGB/RGBA images will be automatically converted to grayscale

# How to display a string with a custom font (outdated)

(An example is provided in the example folder)

This process works almost the same as generating an image... First, you need to generate an image containing all characters of your font. (Google "bitmap font"). This image needs to be 16x16 or 16x8 grid of ASCII characters. The characters must start from 0 (left top) up to 255 (bottom right) and needs to be placed in the grid. The generated bitmap dimensions need to be in power of two! (For example, 128x128, 256x256, 512x512).

To generate 4-bit pixel array out of bitmap font:

1. Open Command Line Terminal (CMD)
2. Use CD command to go to the directory where `ImgToOledBitmap.exe` is located
3. Run: `ImgToOledBitmap.exe -f path_to_font/your_font.png` (notice the -f option!)
4. A new file will be generated `your_font-oledfont.txt` containing 4-bit pixel array
5. Copy this pixel array `static const char your_font_font[] = { ... };` found in the generated txt file to your Arduino/Energia sketch.
6. Use `lcd.drawString(posX, posY, fontPixels, charWidth, string)` to draw a string. Note that the "charWidth" is the width of a single character (width_of_your_bitmap_font / 16 = char_width, for example, if you are converting a 128x128 bitmap font, then the charWidth is 8 pixels). 

