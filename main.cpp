#include <fstream>
#include <cmath>
#include <cstdint>
#include <vector>
#include <iostream>

#pragma pack(push, 1)
struct TGA_Header{
	uint8_t id_len;			// Длина идентификатора
	uint8_t pal_type;		// Тип палитры
	uint8_t img_type;		// Тип изображения
	uint8_t pal_desc[5];	// Описание палитры
	uint16_t x_pos;			// Положение по оси X
	uint16_t y_pos;			// Положение по оси Y
	uint16_t width;			// Ширина
	uint16_t height;		// Высота
	uint8_t depth;			// Глубина цвета
	uint8_t img_desc;		//Описатель изображения
};
#pragma pack(pop)

constexpr uint16_t IMG_WIDTH = 1280;
constexpr uint16_t IMG_HEIGHT = 720;
constexpr uint32_t COL_BACKGROUND = 0xFFFFB28B;
constexpr uint32_t COL_FOREGROUND = 0xFF4F273A;

constexpr double xx = 18., yy = 18., zz = 300.,
				 x_min = -18., y_min = -18., x_max = 18., y_max = 18.,
				 step = 0.5, step2 = 0.01;

double sinc(double x){
	if (x == 0) return 1.;
	return sin(x)/x;
}

double my_evil_function(double x, double y){
	return sinc(hypot(x, y));
}

int main(){

	std::vector<uint32_t> picture(IMG_WIDTH * IMG_HEIGHT);
	for (auto && p : picture) p = COL_BACKGROUND;

	std::vector<int> horizont(IMG_WIDTH);
	for (auto && p : horizont) p = IMG_HEIGHT;

	int sx = 0, sy = 0;
	double z;

	for (double i = x_max; i >= x_min; i-= step){
		for (double j = y_max; j >= y_min; j-= step2){
			z = my_evil_function(i, j);
			sx = int(IMG_WIDTH/2 - xx * i * cos(M_PI/6) + yy * j * cos(M_PI/6));
			sy = int(IMG_HEIGHT/2 + xx * i * sin(M_PI/6) + yy * j * sin(M_PI/6) - zz * z);
			if (sx >= 0 && sx < IMG_WIDTH){
				if (sy <= horizont[sx]){
					horizont[sx] = sy;
					if (sy >= 0){
						if (sy * IMG_WIDTH + sx >= 0 && sy * IMG_WIDTH + sx < IMG_WIDTH * IMG_HEIGHT)
							picture[sy * IMG_WIDTH + sx] = COL_FOREGROUND;
					}
				}
			}
		}
	}

	for (auto && p : horizont) p = IMG_HEIGHT;

	for (double i = x_max; i >= x_min; i-= step2){
		for (double j = y_max; j >= y_min; j-= step){
			z = my_evil_function(i, j);
			sx = int(IMG_WIDTH/2 - xx * i * cos(M_PI/6) + yy * j * cos(M_PI/6));
			sy = int(IMG_HEIGHT/2 + xx * i * sin(M_PI/6) + yy * j * sin(M_PI/6) - zz * z);
			if (sx >= 0 && sx < IMG_WIDTH){
				if (sy <= horizont[sx]){
					horizont[sx] = sy;
					if (sy >= 0){
						if (sy * IMG_WIDTH + sx >= 0 && sy * IMG_WIDTH + sx < IMG_WIDTH * IMG_HEIGHT)
							picture[sy * IMG_WIDTH + sx] = COL_FOREGROUND;
					}
				}
			}
		}
	}

	TGA_Header hdr {};
	hdr.width = IMG_WIDTH;
	hdr.height = IMG_HEIGHT;
	hdr.depth = 32;
	hdr.img_type = 2;
	hdr.img_desc = 0x28;

	std::fstream tga_file { "output.tga", std::ios::binary };
	tga_file.open("output.tga",std::ios::out|std::ios::binary);
	tga_file.write(reinterpret_cast<char*>(&hdr),sizeof(hdr));
	tga_file.write(reinterpret_cast<char*>(&picture[0]), picture.size() * sizeof(uint32_t));
	tga_file.close();
	return 0;
}
