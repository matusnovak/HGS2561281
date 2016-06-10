#ifndef HGS2561281_LIB
#define HGS2561281_LIB

#include <Arduino.h>

class HGS2561281 {
	public:
		HGS2561281(uint8_t Reset, uint8_t CS1, uint8_t CS2, uint8_t DC, uint8_t WR);
		
		void init();
		void setWriteRam();
		void clear();
		void fill();
		void reset();
		void setScissors(uint8_t PosX, uint8_t PosY, uint8_t Width, uint8_t Height);
		void setDrawColor(uint8_t Color);
		void drawRectangle(uint16_t PosX, uint8_t PosY, uint16_t Width, uint8_t Height);
		void drawPixels(uint16_t PosX, uint8_t PosY, uint16_t Width, uint8_t Height, const uint8_t* Pixels);
		void drawString(uint8_t PosX, uint8_t PosY, const unsigned char* Font, uint8_t FontWidth, const char* Str);

	private:
		void test();
		void displaySend(uint8_t sendType, unsigned char v);
		
		void drawRectangleFn(uint16_t PosX, uint8_t PosY, uint16_t Width, uint8_t Height);
		void drawPixelsFn(uint16_t PosX, uint8_t PosY, uint16_t Width, uint8_t Height, const uint8_t* Pixels, bool Mirror);
		void resetFn(bool Lower);
		void clearFn();
		
		uint8_t color;
		uint8_t cspin;
		uint8_t rst;
		uint8_t cs1;
		uint8_t cs2;
		uint8_t dc;
		uint8_t wr;
};

#endif
