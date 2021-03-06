/*
 * main.cpp
 *
 *  Created on: Apr 24, 2017
 *      Author: serg
 */
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <cstdlib>
#include <ctime>

SDL_Surface* Background = NULL;
SDL_Surface* SpriteImage = NULL;
SDL_Surface* BackBuffer = NULL;

const int MaxSpriteSymb = 10;
int aReelSymb[5][5];

const int Xbase = 51;
const int Ybase = 90 - 130;
const int YendReel = 480;
const int YbegReel = 90;

bool ProgramIsRunning(); //event process
SDL_Surface* LoadImage(const char* filename, bool useMask);
SDL_Surface* LoadImageAlpha(const char* filename);
bool LoadFiles();
void FreeFiles();
void DrawImageFrame(SDL_Surface* image, SDL_Surface* destSurface,
		int x, int y, int width, int height, int frame);
void DrawImage(SDL_Surface* image, SDL_Surface* destSurface, int x, int y);

int main(int argc, char* args[])
{
	int X=0, Y=0;

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("Failed to initialize SDL!\n");
		return 0;
	}

	srand(time(NULL));

	BackBuffer = SDL_SetVideoMode(800,600,32,SDL_SWSURFACE); //SDL_DOUBLEBUF

	SDL_WM_SetCaption("Moving Image", NULL);

	if(!LoadFiles())
	{
		printf("Failed to load files!\n");
		FreeFiles();
		SDL_Quit();

		return 0;
	}

	for(int i=0; i < 5; i++)
		for(int j=0; j < 5; j++)
			aReelSymb[i][j] = rand()%10;

	X = Xbase ; Y = Ybase;

	while(ProgramIsRunning())
	{
		if(Y >= 90){
			Y = Ybase;
			for(int i=4; i>0; i--)
				for(int j=0; j<5; j++)
					aReelSymb[i][j] = aReelSymb[i-1][j];
			for(int i=0; i<5; i++)
				aReelSymb[0][i]  = rand()%10;
		}

		DrawImage(Background, BackBuffer, 0, 0);
		for(int row=0; row < 5; row++)
			for(int colmn=0; colmn < 5; colmn++)
				DrawImageFrame(SpriteImage, BackBuffer,	X + 142*colmn, Y + 130*row, 130, 130, aReelSymb[row][colmn]);

		Y += 10;

//		SDL_Delay(20);
		SDL_Flip(BackBuffer);

	}
	FreeFiles();
	SDL_Quit();
	return 0;
}

void DrawImage(SDL_Surface* image, SDL_Surface* destSurface, int x, int y)
{
	SDL_Rect destRect;
	destRect.x = x;
	destRect.y = y;
	SDL_BlitSurface(image, NULL, destSurface, &destRect);
}

void DrawImageFrame(SDL_Surface* image, SDL_Surface* destSurface,
		int x, int y, int width, int height, int frame)
{

	int begDifpos = (y < YbegReel) ? (y+height)-YbegReel : 0;
	int begDifneg = height - begDifpos;

	SDL_Rect destRect;
	destRect.x = x;
	destRect.y = y + (begDifpos?begDifneg:0);

	int columns = image->w/width;

	SDL_Rect sourceRect;
	sourceRect.y = (begDifpos ? begDifneg : 0);
	sourceRect.x = (frame%columns)*width ;
	sourceRect.w = width;
	sourceRect.h =  (y < YbegReel) ? begDifpos :
					((y+height) > YendReel) ?
					((YendReel - y)<0 ? 0 : YendReel - y) : height;

	SDL_BlitSurface(image, &sourceRect, destSurface, &destRect);
}

SDL_Surface* LoadImage(const char* filename, bool useMask)
{
	SDL_Surface* imageLoaded = NULL;
	SDL_Surface* processedImage = NULL;

	imageLoaded = IMG_Load(filename);

	if(imageLoaded != NULL)
	{
		processedImage = SDL_DisplayFormat(imageLoaded);
		SDL_FreeSurface(imageLoaded);

		if((processedImage != NULL)&&useMask)
		{
			Uint32 colorKey = SDL_MapRGB(processedImage->format, 255, 0, 255);
			SDL_SetColorKey(processedImage, SDL_SRCCOLORKEY, colorKey);
		}
	}
	return processedImage;
}

SDL_Surface* LoadImageAlpha(const char* filename)
{
	SDL_Surface* imageLoaded = NULL;
	SDL_Surface* processedImage = NULL;

	imageLoaded = IMG_Load(filename);

	if(imageLoaded != NULL)
	{
		processedImage = SDL_DisplayFormatAlpha(imageLoaded);
		SDL_FreeSurface(imageLoaded);

		if(processedImage != NULL)
		{
//			Uint32 colorKey = SDL_MapRGBA(processedImage->format, 255, 0, 255);
//			SDL_SetColorKey(processedImage, SDL_SRCCOLORKEY, colorKey);
			SDL_SetAlpha(processedImage, SDL_SRCALPHA,0);
		}
	}
	return processedImage;
}

bool LoadFiles()
{
	Background = LoadImage("./res/backfon.png", false);

	if(Background == NULL)
		return false;

	SpriteImage = LoadImageAlpha("./res/reel.png");

	if(SpriteImage == NULL)
		return false;

	return true;
}

void FreeFiles()
{
	if(Background != NULL)
	{
		SDL_FreeSurface(Background);
		Background = NULL;
	}
	if(SpriteImage != NULL)
	{
		SDL_FreeSurface(SpriteImage);
		SpriteImage = NULL;
	}
}

bool ProgramIsRunning()
{
	SDL_Event event;
	bool running = true;

	while(SDL_PollEvent(&event))
	{
		switch(event.type){
			case SDL_QUIT:	 {
				running = false;
				break;
			}
			case SDL_KEYDOWN: {
				running = false;
				break;
			}
		}
//		if(event.type == SDL_QUIT)
//			running = false;
	}
	return running;
}



