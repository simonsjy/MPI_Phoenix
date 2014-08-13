#ifndef _BMP_HEADER_
#define _BMP_HEADER_

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned int LONG;

typedef struct BMP_file
{
	WORD   bfType; 
	DWORD bfSize; //bmp文件长度
	WORD Reserved1;
	WORD Reserved2;
	DWORD bfOffset; //文件描述区长度，16色为118，256色为1078
}bitmapfile;
// 现在算一下，正好3*2+2*4=14字节


//图象信息区
typedef struct BMP_info
{
	DWORD biSize; // 本结构所占用字节数
	LONG biWidth; // 位图的宽度，以像素为单位
	LONG biHeight; // 位图的高度，以像素为单位
	WORD biPlanes; // 目标设备的级别，必须为1
	WORD biBitCount;// 每个像素所需的位数，必须是1(双色),4(16色)，8(256色)或24(真彩色)之一
	DWORD biCompression; // 位图压缩类型，必须是 0(不压缩), 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
	DWORD  biSizeImage; // 位图的大小，以字节为单位
	LONG biXPelsPerMeter; // 位图水平分辨率，每米像素数
	LONG biYPelsPerMeter; // 位图垂直分辨率，每米像素数
	DWORD biClrUsed;// 位图实际使用的颜色表中的颜色数
	DWORD biClrImportant;// 位图显示过程中重要的颜色数
}bitmapinfo;
// 现在算一下,正好是2+2*9*4=40字节。

#endif
