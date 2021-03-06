// FreeTypeTest.cpp: 定义控制台应用程序的入口点。
//


#include "stdafx.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "BMP.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

//图片大小
#define WIDTH   640
#define HEIGHT  240

struct RGB
{
	RGB() {}
	RGB(unsigned char r, unsigned char b, unsigned char g) :B(b), G(g), R(r)
	{}
	unsigned char B = 0;
	unsigned char G = 0;
	unsigned char R = 0;
};

struct Padding
{
	int Up = 0;
	int Left = 0;
	int Bottom = 0;
	int Right = 0;
}padding;


struct GlyphInfo
{
	int vertAdvance = 0;
	int height = 0;
};

// 位图数据
RGB image[HEIGHT*WIDTH];

RGB textColor = RGB(245, 150, 170);
RGB backgroundColor = RGB(128, 128, 128);
RGB strokeColor = RGB(0, 0, 0);
FT_UInt strokeStrength = 4;
RGB underlineColor = RGB(245, 150, 170);
RGB  lerp(RGB A, RGB B, float V)
{
	RGB rgb;
	rgb.R = A.R * (1.0F - V) + B.R * V;
	rgb.G = A.G * (1.0F - V) + B.G * V;
	rgb.B = A.B * (1.0F - V) + B.B * V;


	return rgb;

}

int addRowSpacing = 10;
int addSpacing =20;

void draw_bitmap(FT_Bitmap*  bitmap, FT_Int      x, FT_Int      y, RGB colorin)
{
	FT_Int  i, j, p, q;
	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;


	for (i = x, p = 0; i < x_max; i++, p++)
	{
		for (j = y, q = 0; j < y_max; j++, q++)
		{
			if (i < 0 || j < 0 ||
				i >= WIDTH || j >= HEIGHT)
				continue;


			RGB color;

			//color.R  = (bitmap->buffer[q * bitmap->width + p])*((float)colorin.R/255.0F);
			//color.B = (bitmap->buffer[q * bitmap->width + p])*((float)colorin.B / 255.0F);
			//color.G = (bitmap->buffer[q * bitmap->width + p])*((float)colorin.G/ 255.0F);


			color = lerp(image[j* WIDTH + i], colorin, ((float)bitmap->buffer[q * bitmap->width + p]) / 255.F);



			image[j* WIDTH + i] = color;

		}
	}
}
void drawLine_bitmap(FT_Bitmap*  bitmap, FT_Int      x, FT_Int      y, FT_Int      maxx, FT_Int      maxy)
{
	FT_Int  i, j, p, q;
	FT_Int  x_max = x + maxx;
	FT_Int  y_max = y + bitmap->rows;


	for (i = x, p = 0; i < x_max; i++, p++)
	{
		for (j = y, q = 0; j < y_max; j++, q++)
		{
			if (i < 0 || j < 0 ||
				i >= WIDTH || j >= HEIGHT)
				continue;


			RGB color;
			//color.R = (bitmap->buffer[q * bitmap->width + p])*((float)textColor.R / 255.0F);
			//color.B = (bitmap->buffer[q * bitmap->width + p])*((float)textColor.B / 255.0F);
			//color.G = (bitmap->buffer[q * bitmap->width + p])*((float)textColor.G / 255.0F);

			color = lerp(image[j* WIDTH + i], underlineColor, ((float)(bitmap->buffer[q * bitmap->width + p] != 0 ? 255 : 0)) / 255.F);


			image[j* WIDTH + i] = color;

		}
	}
}
GlyphInfo GetGlyphInfo(FT_Face&    face)
{
	FT_UInt  glyph_index;
	glyph_index = FT_Get_Char_Index(face, L'E');//获取一个字符码的字形索引
	FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);//加载字形数据

	

	GlyphInfo ls;
	ls.vertAdvance=  face->glyph->metrics.vertAdvance / 64;
	ls.height= face->height / 64;
	return ls;
}
int main(int argc, char** argv)
{

	memset(image, backgroundColor.R, HEIGHT*WIDTH * 3);

	FT_Library    library;
	FT_Face       face;


	FT_Vector     pen;
	FT_Error      error;

	const char*         filename = "msyh.ttc";      //字体文件
	const wchar_t*         text = L"这是测试文字1,这是测试文字2,这是测试文字3\n大家好";    //需要渲染的文字

	double        angle;
	int           n, num_chars;
	
	//num_chars = strlen(text);
	num_chars = wcslen(text);

	//num_chars = 1;



	error = FT_Init_FreeType(&library); //初始化库
	error = FT_New_Face(library, filename, 0, &face);//加载字体文件并初始化“面”
	//”面“包含了文字的样式及图像信息
	error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	error = FT_Set_Char_Size(face, 50 * 64, 0, 72, 0); //设置“面”的字符大小
	//face->underline_thickness = 10;
	//face->underline_position = 10;

	if (error)
	{
		fprintf(stderr, "Error!\n");
		getchar();
		exit(1);
	}
	//OK

	padding.Left = 10;
	padding.Up = 10;
	padding.Right = 10;
	//padding.Up = 10;

	GlyphInfo glyphInfo = GetGlyphInfo(face);

	pen.x = padding.Left * 64;
	pen.y = (HEIGHT - glyphInfo.height - padding.Up) * 64;
	for (n = 0; n < num_chars; n++)
	{

		if (text[n] == '\r')
		{
		}
		else if (text[n] == '\n')
		{
			pen.y -= (glyphInfo.vertAdvance * 64+ addRowSpacing*64);
			pen.x = padding.Left * 64;
		}
		else
		{


			//设置样式的变换
			//FT_Set_Transform(face, &matrix, &pen);
			FT_Set_Transform(face, NULL, &pen);

			FT_UInt  glyph_index;
			glyph_index = FT_Get_Char_Index(face, text[n]);//获取一个字符码的字形索引
			FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);//加载字形数据

															  //int strength = 5*64;
															  //FT_Outline_Embolden(&face->glyph->outline, strength);

			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);//渲染字形



																//上面操作的简化操作
																//FT_Load_Char(face, text[n], FT_LOAD_RENDER);

																//最终渲染结果在face的字形槽里
			FT_GlyphSlot glyphSlot = face->glyph;
			//glyphSlot->metrics.height;

			draw_bitmap(&glyphSlot->bitmap, glyphSlot->bitmap_left + 2, HEIGHT - glyphSlot->bitmap_top + 2, strokeColor);

			draw_bitmap(&glyphSlot->bitmap, glyphSlot->bitmap_left, HEIGHT - glyphSlot->bitmap_top, textColor);




			//draw_bitmap2(&slot->bitmap);

			// 递增渲染位置 
			pen.x += glyphSlot->advance.x+ addSpacing*64;
			pen.y += glyphSlot->advance.y;


			int   FFFF = (pen.x +glyphSlot->advance.x ) / 64 * 72 / 72;

			if (FFFF>(WIDTH- padding.Right))
			{
				pen.y -= (glyphInfo.vertAdvance * 64+ addRowSpacing*64);
				pen.x = padding.Left * 64;
			}

		}
	}




	/*


	error = FT_Init_FreeType(&library); //初始化库
	error = FT_New_Face(library, "msyh.ttc", 0, &face);//加载字体文件并初始化“面”
													 //”面“包含了文字的样式及图像信息
	error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	error = FT_Set_Char_Size(face, 50 * 64, 0, 72, 0); //设置“面”的字符大小

	FT_Vector     pen2;
	pen2.x = 50 * 64;
	pen2.y = (HEIGHT - 100) * 64;

	while (1)
	{
		//设置样式的变换
		FT_Set_Transform(face, NULL, &pen2);


		FT_UInt  glyph_index;
		glyph_index = FT_Get_Char_Index(face, '_');//获取一个字符码的字形索引
		FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);//加载字形数据

														  //int strength = 5*64;
														  //FT_Outline_Embolden(&face->glyph->outline, strength);

		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);//渲染字形



															//上面操作的简化操作
															//FT_Load_Char(face, text[n], FT_LOAD_RENDER);

															//最终渲染结果在face的字形槽里
		FT_GlyphSlot glyphSlot = face->glyph;

		int max;

		if ((pen.x) < (pen2.x+ glyphSlot->advance.x))
		{
			max = glyphSlot->bitmap.width;
			max -= ((pen2.x + glyphSlot->advance.x) - (pen.x ))/64;
		}
		else
		{
			max = glyphSlot->bitmap.width;
		}


		//draw_bitmap(&glyphSlot->bitmap, glyphSlot->bitmap_left, HEIGHT - glyphSlot->bitmap_top);
		//draw_bitmap2(&slot->bitmap);
		drawLine_bitmap(&glyphSlot->bitmap, glyphSlot->bitmap_left, HEIGHT - glyphSlot->bitmap_top, max,0);

		// 递增渲染位置
		pen2.x += (glyphSlot->advance.x);
		pen2.y += glyphSlot->advance.y;

		if (pen2.x>= pen.x)
		{
			break;
		}
	}
	*/
	SaveBitmap(WIDTH, HEIGHT, (unsigned char*)image, WIDTH*HEIGHT * 3);


	FT_Done_Face(face);
	FT_Done_FreeType(library);

	fprintf(stdout, "OK!\n");
	getchar();
	return 0;
}

