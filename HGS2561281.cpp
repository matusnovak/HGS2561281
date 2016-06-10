#include "HGS2561281.h"
#include <SPI.h>

#define SEND_CMD 1
#define SEND_DAT 2

#define MAXROWS      64   // Still figuring these out...
#define MAXCOLS     240   // Still figuring these out...

///=============================================================================
HGS2561281::HGS2561281(uint8_t Reset, uint8_t CS1, uint8_t CS2, uint8_t DC, uint8_t WR):
rst(Reset),cs1(CS1),cs2(CS2),dc(DC),wr(WR)
{
	color = 0xFF;
	cspin = cs1;
	//SPI.setModule(SpiNum);
}

///=============================================================================
void HGS2561281::displaySend(uint8_t sendType, unsigned char v){
	digitalWrite(cspin, LOW);
	if (sendType == SEND_CMD){
		digitalWrite(dc, LOW);
	}
	if(sendType == SEND_DAT){
		digitalWrite(dc, HIGH);
	}

	SPI.transfer(v);
	digitalWrite(cspin, HIGH);
}

///=============================================================================
void HGS2561281::setWriteRam(){
	displaySend(SEND_CMD, 0x5C); // Enable MCU to Write into RAM
}

///=============================================================================
void HGS2561281::setScissors(uint8_t PosX, uint8_t PosY, uint8_t Width, uint8_t Height){
	PosX /= 2;
	Width /= 2;

	Width += PosX;
	Height += PosY-1;

	displaySend(SEND_CMD, 0x15);
	displaySend(SEND_DAT, 0x1c + PosX); // Column Start Address
	//displaySend(SEND_DAT, 0x19 + Width); // Column End Address
	displaySend(SEND_DAT, 0x1b + Width); // Column End Address
	displaySend(SEND_CMD, 0x75);
	displaySend(SEND_DAT, PosY); // Row Start Address
	displaySend(SEND_DAT, Height); // Row End Address
}

///=============================================================================
void HGS2561281::reset(){
	cspin = cs1;
	resetFn(false);
	cspin = cs2;
	resetFn(true);
}

///=============================================================================
void HGS2561281::resetFn(bool Lower){
	if(!Lower){
		delay(15);
		digitalWrite(rst, LOW);
		delay(1);
		digitalWrite(rst, HIGH);
		delay(150);
	}

	// Ready to accept commands;
	displaySend(SEND_CMD, 0xFD);
	displaySend(SEND_DAT, 0x12); // 12H to accept, 16H reject 

	// Sleep switch;
	displaySend(SEND_CMD, 0xAE); // AEH open dormant, AF * Off Sleep

	// Set column window;
	displaySend(SEND_CMD, 0x15);
	displaySend(SEND_DAT, 0x1c);
	displaySend(SEND_DAT, 0x5b); // 256 columns by 

	// Set the line window;
	displaySend(SEND_CMD, 0x75);
	displaySend(SEND_DAT, 0x00);
	displaySend(SEND_DAT, 0x3f); // 64 rows 

	// Set reversible map;
	displaySend(SEND_CMD, 0xA0);
	if(Lower)
		displaySend(SEND_DAT, 0x14); // high byte first option, COM scanning backward, horizontal address mode 
	else
		displaySend(SEND_DAT, 0x04);
	displaySend(SEND_DAT, 0x11); /// / // selected dual mode 

	// Set the starting line;
	displaySend(SEND_CMD, 0xA1);
	displaySend(SEND_DAT, 0x00); // 0 

	// Set the display branch;
	displaySend(SEND_CMD, 0xA2);
	displaySend(SEND_DAT, 0x00); // * no branch 

	// Set the display mode;
	displaySend(SEND_CMD, 0xA6); // A4H fully closed, A5H full, A6H normal, A7H reverse display 

	// Set the highlighted;
	displaySend(SEND_CMD, 0xA6); // not * reverse video 

	// Set the display section;
	displaySend(SEND_CMD, 0xA9); // A8H into the display portion, A9H exit 

	// Vdd source;
	displaySend(SEND_CMD, 0xAB);
	displaySend(SEND_DAT, 0x01); // 00H external, 01H internal 

	// Set P1 P2;
	displaySend(SEND_CMD, 0xB1); // P1 = 29, P2 = 7 
	displaySend(SEND_DAT, 0xE7);

	// Set D and Fosc;
	displaySend(SEND_CMD, 0xB3);
	displaySend(SEND_DAT, 0xf1); // F = 9, D = 1 

	// Set the display quality and Vsl sources;
	displaySend(SEND_CMD, 0xB4);
	displaySend(SEND_DAT, 0xA0); // A0H external Vsl, A2H internal Vsl 
	displaySend(SEND_DAT, 0xfd); // FDH to improve the quality, B5H normal 

	// Set GPIO;
	displaySend(SEND_CMD, 0xB5);
	displaySend(SEND_DAT, 0x00);

	// Set Tp3
	displaySend(SEND_CMD, 0xB6);
	displaySend(SEND_DAT, 0x08);

	// Set the gray value;
	displaySend(SEND_CMD, 0xB8);
	displaySend(SEND_DAT, 0x0c);
	displaySend(SEND_DAT, 0x18);
	displaySend(SEND_DAT, 0x24);
	displaySend(SEND_DAT, 0x30);
	displaySend(SEND_DAT, 0x3c);
	displaySend(SEND_DAT, 0x48);
	displaySend(SEND_DAT, 0x54);
	displaySend(SEND_DAT, 0x60);
	displaySend(SEND_DAT, 0x6c);
	displaySend(SEND_DAT, 0x78);
	displaySend(SEND_DAT, 0x84);
	displaySend(SEND_DAT, 0x90);
	displaySend(SEND_DAT, 0x9c);
	displaySend(SEND_DAT, 0xa8);
	displaySend(SEND_DAT, 0xb4);

	// Enable grayscale;
	// displaySend(SEND_CMD, 0x00);

	// Set Vp2;
	displaySend(SEND_CMD, 0xBB);
	displaySend(SEND_DAT, 0x1F);

	// Set Vcomh;
	displaySend(SEND_CMD, 0xBE);
	displaySend(SEND_DAT, 0x07); // Vcomh = 0.86Vcc 

	// Set the contrast;
	displaySend(SEND_CMD, 0xC1);
	displaySend(SEND_DAT, 0x7F); // Selection of a total of 256 * 128 

	// Set SEG current range;
	displaySend(SEND_CMD, 0xC7);
	displaySend(SEND_DAT, 0x0F); // Full range 

	// Set the display line;
	displaySend(SEND_CMD, 0xCA);
	displaySend(SEND_DAT, 0x3F); // 64 

	// Set the display quality;
	displaySend(SEND_CMD, 0xD1);
	displaySend(SEND_DAT, 0xA2);
	displaySend(SEND_DAT, 0x20);

	displaySend(SEND_CMD, 0xAF); // start * Display /

	delay(10);
}

///=============================================================================
void HGS2561281::clear(){
	cspin = cs1;
	clearFn();
	cspin = cs2;
	clearFn();
}

///=============================================================================
void HGS2561281::clearFn(){
	// Turn off display while clearing (also hides noise at powerup)
	displaySend(SEND_CMD, 0xA4); // Set Display Mode = OFF

	displaySend(SEND_CMD, 0x15); // Set Column Address
	displaySend(SEND_DAT, 0x00);    //   Default => 0x00
	displaySend(SEND_DAT, 0x77);    //   Default => 0x77
	displaySend(SEND_CMD, 0x75); // Set Row Address
	displaySend(SEND_DAT, 0x00);    //   Default => 0x00
	displaySend(SEND_DAT, 0x7F);    //   Default => 0x7F
	setWriteRam();

	for(uint8_t y = 0; y < MAXROWS; y++){
		for(uint8_t x = 0; x < MAXCOLS/2; x++){
			displaySend(SEND_DAT, 0x00);
			displaySend(SEND_DAT, 0x00);
			displaySend(SEND_DAT, 0x00);
			displaySend(SEND_DAT, 0x00);
		}
	}

	displaySend(SEND_CMD, 0xA6); // Set Display Mode = Normal Display
}

///=============================================================================
void HGS2561281::test(){
  
	for(int y = 0; y < 64; y++){
		setScissors(0, y, y, 2);
		setWriteRam();

		for(int i = 0; i < y; i++){
			displaySend(SEND_DAT, 0xFF);
		}
	}
}

///=============================================================================
void HGS2561281::init(){
	pinMode(rst, OUTPUT);
	pinMode(cs1, OUTPUT);
	pinMode(dc, OUTPUT);
	pinMode(wr, OUTPUT);
	pinMode(cs2, OUTPUT);

	digitalWrite(cs1, HIGH);
	digitalWrite(cs2, HIGH);
	digitalWrite(wr, LOW);
	digitalWrite(rst, HIGH);

	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV2);

	reset();
	clear();
}

///=============================================================================
void HGS2561281::setDrawColor(uint8_t Color){
	color = (Color << 4) | Color;
}

///=============================================================================
void HGS2561281::drawRectangle(uint16_t PosX, uint8_t PosY, uint16_t Width, uint8_t Height){
	if(PosY >= 64){
		cspin = cs2;
		drawRectangleFn(256 - PosX - Width, PosY - 64, Width, Height);
	}
	else if(PosY < 64){
		if(Height + PosY > 64){
			uint8_t offset = 64 - PosY;
			cspin = cs1;
			drawRectangleFn(PosX, PosY, Width, offset);
			
			cspin = cs2;
			drawRectangleFn(256 - PosX - Width, 0, Width, Height - offset);
		}
		else {
			cspin = cs1;
			drawRectangleFn(PosX, PosY, Width, Height);
		}
	}
}

///=============================================================================
void HGS2561281::drawRectangleFn(uint16_t PosX, uint8_t PosY, uint16_t Width, uint8_t Height){
	PosX /= 2;
	Width /= 2;
	setScissors(PosX, PosY, Width, Height);
	setWriteRam();

	for(int i = 0; i < Width * Height * 2; i++){
		displaySend(SEND_DAT, color);
	}
}

///=============================================================================
void HGS2561281::drawString(uint8_t PosX, uint8_t PosY, const unsigned char* Font, uint8_t FontWidth, const char* Str){
	uint8_t x = PosX;
	int pixelsPerChar = FontWidth/2 * FontWidth;
	while(*Str != '\0'){
		if(*Str == '\n'){
			PosY += FontWidth;
			x = PosX;
		} else {
		
			int p = (*Str);
			const unsigned char* pixels = &Font[p * pixelsPerChar];
			
			drawPixels(x, PosY, FontWidth, FontWidth, pixels);
			x += FontWidth;
		}
		
		Str++;
	}
	
}

///=============================================================================
void HGS2561281::drawPixels(uint16_t PosX, uint8_t PosY, uint16_t Width, uint8_t Height, const uint8_t* Pixels){
	if(PosY >= 64){
		cspin = cs2;
		drawPixelsFn(256 - PosX - Width, PosY - 64, Width, Height, Pixels, true);
	}
	else if(PosY < 64){
		if(Height + PosY > 64){
			uint8_t offset = 64 - PosY;
			cspin = cs1;
			drawPixelsFn(PosX, PosY, Width, offset, Pixels, false);
			
			cspin = cs2;
			const uint8_t* ptr = &Pixels[offset * (Width/2)];
			drawPixelsFn(256 - PosX - Width, 0, Width, Height - offset, ptr, true);
		}
		else {
			cspin = cs1;
			drawPixelsFn(PosX, PosY, Width, Height, Pixels, false);
		}
	}
}

///=============================================================================
void HGS2561281::drawPixelsFn(uint16_t PosX, uint8_t PosY, uint16_t Width, uint8_t Height, const uint8_t* Pixels, bool Mirror){
	uint8_t upper = Height;
	if(upper > 64)upper = 64;
	
	setScissors(PosX/2, PosY, Width/2, upper);
	setWriteRam();
	
	/*for(int i = 0; i < Width * upper/2; i++){
		displaySend(SEND_DAT, Pixels[i]);
	}*/
	int half = Width/2;
	
	if(!Mirror){
		for(int y = 0; y < upper; y++){
			for(int x = 0; x < half; x++){
				displaySend(SEND_DAT, Pixels[y * half + x]);
			}
		}
	} else {
		for(int y = 0; y < upper; y++){
			for(int x = 0; x < half; x++){
				uint8_t px = Pixels[y * half + (half - x - 1)];
				uint8_t cp = ((px & 0xF0) >> 4) | ((px & 0x0F) << 4);
				displaySend(SEND_DAT, cp);
			}
		}
	}
	
	// Lower half
	if(Height > 64){
		
	}
}