#pragma once
#ifndef IMG_H
#define IMG_H


class Img {
public:
	Img();
	~Img();
	bool DoNext(BYTE* pImgData);
	int getImgWidth();
	bool ReadGryImg(const char* filename);
	int getImgHeight();
	bool ReadRGBImg(const char* filename);

	void GetHistogram();
	void GetOtusThreshold(int nSize);
	void Binarization();
	void WriteGryImg(const char* filename);
	void WriteRGBImg(const char* filename);
	void DoNext();
	void HistogramAver(int nSize,int FilterSize);
	void NarrowPixel(int k);//把图像缩小K倍
	void Cluster(int k);
	void ClassByClus();
	void findTarget();

	
private:
	//释放内存
	void Dump();
	void Debug();
private:
	//初始化成功
	bool isInitOk;
	
	bool isRGB;//图像的属性
	int width;
	int height;
	int OtsuThre;

	double* ClusCenter;
	BYTE *pRGBImg;
	BYTE* pGryImg;
	BYTE* pResImg;
	int* Histogram;
	int* cluster; //直方图每个灰度值聚类结果

	//已经申请的内存大小
	int memSize ;
};
#endif
