#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>

#include "Img.h"
#include "bmpFile.h"
using namespace std;

Img::Img() {
	//图像属性
	isInitOk = false;
	isRGB = false;
	width = 0;
	height = 0;
	memSize = 0;
	OtsuThre = 0;
	//内存空间
	pRGBImg = NULL;
	pGryImg = NULL;
	Histogram = NULL;
	pResImg = NULL;
	ClusCenter = NULL;
	cluster = NULL;
}

Img::~Img() {
	Dump();
}

void Img::Dump() {
	if (pRGBImg) { delete pRGBImg; pRGBImg = NULL; }
	if (pGryImg) { delete pGryImg; pGryImg = NULL; }
	if (Histogram) { delete Histogram; Histogram = NULL; }
	if (pResImg) { delete pResImg; pResImg = NULL; }
	if (ClusCenter) { delete ClusCenter; ClusCenter = NULL; }
	if (cluster) { delete cluster; cluster = NULL; }
	memSize = 0;
}

bool Img::ReadRGBImg(const char* filename) {
	isRGB = 1;
	pGryImg = new BYTE[width * height];
	Histogram = new int[256];
	pResImg = new BYTE[width * height];
	pRGBImg = Rmw_Read24BitBmpFile2Img(filename, &width, &height);
	memSize = width * height;
	isInitOk = (pRGBImg && pGryImg && Histogram && pResImg && memSize);
	cout << "successful" << width << " " << height << endl;
	return isInitOk;
}

bool Img::ReadGryImg(const char* filename) {
	isRGB = 0;
	pRGBImg = new BYTE[width * height*3];
	Histogram = new int[256];
	pResImg = new BYTE[width * height];
	memSize = width * height;
	if (pGryImg) { delete pGryImg; pGryImg = NULL; }
	pGryImg = Rmw_Read8BitBmpFile2Img(filename, &width, &height);
	memSize = width * height;
	isInitOk = (pRGBImg && pGryImg && Histogram && pResImg && memSize);
	cout << "successful" <<width<<" "<<height <<endl;
	return isInitOk;
}


int Img::getImgWidth(){return width; }
int Img::getImgHeight(){return height; }

void Img::GetHistogram() {
	unsigned char* pCur, * pEnd;
	pEnd = pGryImg + width * height;

	memset(Histogram, 0, sizeof(int) * 256);

	for (pCur = pGryImg; pCur < pEnd;) {
		Histogram[*(pCur++)]++;
	}
	cout << "get Histogram" << endl;
}

void Img::GetOtusThreshold(int nSize) {
	int thre;
	int i, gmin, gmax;
	double dist, f, max;
	int s1, s2, n1, n2, n;
	gmin = 0;
	while (Histogram[gmin] == 0)++gmin;
	gmax = nSize - 1;
	while (Histogram[gmax] == 0)--gmax;
	max = 0;
	thre = 0;
	s1 = n1 = 0;
	for (s2 = n2 = 0, i = gmin; i < gmax; i++) {
		s2 += Histogram[i] * i;
		n2 += Histogram[i];
	}
	for (i = gmin, n = n2; i < gmax; i++) {
		if (!Histogram[i])continue;
		s1 += Histogram[i] * i;
		s2 -= Histogram[i] * i;
		n1 += Histogram[i];
		n2 -= Histogram[i];
		dist = (s1 * 1.0 / n1 - s2 * 1.0 / n2);
		f = dist * dist * (n1 * 1.0 / n) * (n2 * 1.0 / n);
		if (f > max) {
			max = f;
			thre = i;
		}
	}
	OtsuThre = thre;
	cout << "get thre is " << thre << endl;
}

void Img::Binarization() {
	BYTE* pEnd = pGryImg + width * height;
	BYTE* pCur = pGryImg;
	while (pCur < pEnd) {
		*(pCur++) = (*pCur >= OtsuThre)*255;
	}
	//cout << "Binatization ok" << endl;
}

void Img::WriteGryImg(const char* filename) {
	Rmw_Write8BitImg2BmpFile(pGryImg, width, height, filename);
}
void Img::WriteRGBImg(const char* filename) {
	Rmw_Write24BitImg2BmpFile(pGryImg, width, height, filename);
	return;
}

void Img::HistogramAver(int nSize,int FilterSize) {//邻域为k
	int i, sum = 0;
	int* newHis;
	newHis = new int[nSize];
	FilterSize = FilterSize / 2 * 2 + 1;//保证FilteSize为奇数
	int half = FilterSize / 2;
	//初始化列和
	for (i = 0; i < FilterSize; i++) {
		sum += Histogram[i];
	}
	for (i = 0; i < half; i++) {
		newHis[i] = Histogram[i];
	}
	for (i = nSize - 1; i > nSize - half - 1; i--) {
		newHis[i] = Histogram[i];
	}
	for (i = half; i < nSize - half; i++) {
		newHis[i] = sum * 1.0 / FilterSize;
		if (i < nSize - half - 1) {
			sum -= Histogram[i - half];
			sum += Histogram[i + half + 1];
		}
	}
	if (Histogram) { delete Histogram; Histogram = newHis; }

}

void Img::NarrowPixel(int k) {
	BYTE* pCur, * pEnd = pGryImg + width * height;
	for (pCur = pGryImg; pCur < pEnd;) *(pCur++) = *pCur / k;
}

void Img::Cluster(int k) {
	//----------------创建所需空间
	//从1开始存储每个聚类中心
	if (!ClusCenter) { ClusCenter = new double[k+1]; }
    //存储每个灰度值所属类别
	if (!cluster) { cluster = new int[256]; }
	double* preCenter = new double[k+1];//存储前一轮的聚类中心
	//初始化聚类中心
	memset(ClusCenter, 0, sizeof(double)*(k + 1));
	memset(cluster, 0,sizeof(int)*256);
	memset(preCenter, 0xff, sizeof(double)*(k + 1));//前一轮的初始化为很大的值
	bool flag = 0;  //聚类中心差距很小
	int pos = 0;//从pos开始找不为0的位置
	//---------------初始化聚类中心------------------//
	for (int i = 1; i <= k; i++) {
		while (Histogram[pos] == 0)++pos;
		ClusCenter[i] = pos;
		pos++;
	}
	while (!flag) {
		//----------------判断是否够结束条件--------------//
		bool flag1 = 1;
		for (int i = 1; i <= k; i++) {
			double sum;
			sum = abs(preCenter[i] - ClusCenter[i]);
			flag1 = flag1 & (sum < 0.1);
		}
		flag = flag1;
		//--------------动态聚类------------------------//
		
		for (int i = 0; i < 256; i++) {    //对每个灰度值分类
			double min=256;
			int index=0;
			for (int j = 1; j <= k; j++) {  //灰度值到每个聚类中心的距离
				double dis = abs(ClusCenter[j] - i);
				if (dis < min) {
					min = dis;
					index = j;
				}
			}
			cluster[i] = index;//这个灰度值就属于距离第index类
		}
	//存储旧的聚类中心
		for (int i = 1; i <= k; i++) {
			preCenter[i] = ClusCenter[i];
		}
		//计算新的聚类中心
		for (int i = 1; i <= k; i++) {
			int sum = 0;
			int num = 0;
			for (int j = 0; j < 256; j++) {
				if (cluster[j] == i) { sum += j*Histogram[j]; num += Histogram[j]; }
			}
			ClusCenter[i] = sum * 1.0 / num;
		}
	}
	cout << ClusCenter[1] << ClusCenter[2] << endl;
}

void Img::ClassByClus() {
	BYTE* pCur, *pEnd = pGryImg + width * height;
	pCur = pGryImg;
	while (pCur < pEnd) {
		if (cluster[*pCur] == 1)
			*(pCur++) = 0;
		else if(cluster[*pCur] == 2)
			*(pCur++) = 128;
		else
			*(pCur++) = 256;
	}
}



void Img::DoNext() {
	ReadGryImg("0502Gry_line.bmp");
	GetHistogram();
	HistogramAver(256, 5);
	GetOtusThreshold(256);
	Binarization();
	WriteGryImg("0501Gry_line_Otsu.bmp");
}
