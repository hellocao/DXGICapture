#include <stdio.h>
#include <Windows.h>
#include "SDL2\SDL.h"
//²Î¿¼²©¿Í https://blog.csdn.net/leixiaohua1020/article/details/40525591
#define LOAD_BGRA 1
#define LOAD_RGB24 0
#define LOAD_BGR24 0
#define LOAD_YUV420P 0
#if LOAD_BGRA
const int bpp = 32;
#elif LOAD_RGB24 | LOAD_BRG24
const int bpp = 24;
#elif LOAD_YUV420P
const int bpp = 12;
#endif

int screen_w = 500, screen_h = 500;
const int pixel_w = 320, pixel_h = 180;

unsigned char* pBuffer = NULL;
unsigned char* pBufferConvert = NULL;
//Convert RGB24/BGR24 to RGB32/BGR32
//And change Endian if needed
void Convert_24to32(unsigned char* image_in, unsigned char* image_out, int width, int height)
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
			{
				image_out[(i*width + j) * 4 + 0] = image_in[(i*width + j) * 3 + 2];
				image_out[(i*width + j) * 4 + 1] = image_in[(i*width + j) * 3 + 1];
				image_out[(i*width + j) * 4 + 2] = image_in[(i*width + j) * 3];
				image_out[(i*width + j) * 4 + 3] = '0';
			}
			else
			{
				image_out[(i*width + j) * 4] = '0';
				memcpy(image_out + (i*width + j) * 4 + 1, image_in + (i*width + j) * 3, 3);
			}
		}
	}
}

//Refesh Event
#define REFRESH_EVENT (SDL_USEREVENT + 1)

int thread_exit = 0;
int refresh_video(void* opaque)
{
	while (thread_exit == 0)
	{
		SDL_Event event;
		event.type = REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(40);
	}
	return 0;
}
int main()
{
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}
	SDL_Window* screen;
	screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!screen)
	{
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -1;
	}
	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	Uint32 pixformat = 0;
#if LOAD_BGRA
	pixformat = SDL_PIXELFORMAT_ARGB8888;
#elif LOAD_RGB24
	pixformat = SDL_PIXELFORMAT_RGB888;
#elif LOAD_BGR24
	pixformat = SDL_PIXELFORMAT_BGR888;
#elif LOAD_YUV420P
	pixformat = SDL_PIXELFORMAT_IYUV;
#endif
	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, pixel_w, pixel_h);
	FILE* fp = NULL;
#if LOAD_BGRA
	fp = fopen("../test_bgra_320x180.rgb", "rb+");
#elif LOAD_RGB24
	fp = fopen("../test_rgb24_320x180.rgb", "rb+");
#elif LOAD_BGR24
	fp = fopen("../test_bgr24_320x180.rgb", "rb+");
#elif LOAD_YUV420P
	fp = fopen("../test_yuv420p_320x180.yuv", "rb+");
#endif
	if (fp == NULL)
	{
		printf("connot open this file\n");
		return -1;
	}
	SDL_Rect sdlRect;
	SDL_Thread* refresh_thread = SDL_CreateThread(refresh_video, NULL, NULL);
	SDL_Event event;
	while (1)
	{
		SDL_WaitEvent(&event);
		if (event.type == REFRESH_EVENT)
		{
			if (fread(pBuffer, 1, pixel_w*pixel_h*bpp / 8, fp) != pixel_w*pixel_h*bpp / 8)
			{
				fseek(fp, 0, SEEK_SET);
				fread(pBuffer, 1, pixel_w*pixel_h*bpp / 8, fp);
			}
#if LOAD_BGRA
			SDL_UpdateTexture(sdlTexture, NULL, pBuffer, pixel_w * 4);
#elif LOAD_RGB24 | LOAD_BGR24
			CONVERT_24to32(pBuffer, pBufferConvert, pixel_w, pixel_h);
			SDL_UpdateTexture(sldTexture, NULL, pBufferConvert, pixel_w * 4);
#elif LOAD_YUV420P
			SDL_UpdateTexture(sdlTexture, NULL, pBuffer, piexel_w);
#endif
			sdlRect.x = 0;
			sdlRect.y = 0;
			sdlRect.w = screen_w;
			sdlRect.h = screen_h;
			SDL_RenderClear(sdlRenderer);
			SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
			SDL_RenderPresent(sdlRenderer);
			SDL_Delay(40);
		}
		else if (event.type == SDL_WINDOWEVENT)
		{
			SDL_GetWindowSize(screen, &screen_w, &screen_h);
		}
		else if (event.type == SDL_QUIT)
		{
			break;
		}
	}
	return 0;
}