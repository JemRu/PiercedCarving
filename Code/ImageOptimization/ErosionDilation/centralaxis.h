#pragma once
#pragma once
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <set>
#include <queue>

using namespace  cv;
using namespace std;

class centralaxis
{
public:
	centralaxis();
	~centralaxis();
	//////ͨ��diff_f ������볡 �õ� diff_field diff_lfield diff_ledfield
	void  disboufield(const char* inputImage, const char* inputImage2, const char* outImage);
	//ͨ��diff_field������ ���ӻ����볡
	void  visual_field(const char* inputImage);
	//////ͨ��diff_f �õ� axis_threshold out_laplacian
	void  visual_field_Laplacian(const char* inputImage, const char* inputImage2);
	//////ͨ�� diff diff_c diff_f axis_threshold�õ� out_classifyabc//�ø�ʴ���ͺ��ͼ��out.png���� ����typeb��c ���out_classifyabc(_w)
	void  visual_classify_BC(const char* inputImage, const char* inputImage3, const char* outImage, int num);
	void  visual_classify_ABC(const char* inputImage, const char* inputImage2, const char* outImage);
	void  setClassifyThreshold(int num);
	
	std::vector<int> labelBC_weight;
	std::vector<int> labelBC_id; // B 1 C 0

	Mat D_B;
	Mat Ledfield,Axis;
	
	/*int classify_BC = 20;*/ //500 lion 1000 scenery
	/*int classify_BC = 30;*/ //1200x1200 scenery
	/*int classify_BC = 150;*/  //1000x1000 lion-d
	//int classify_BC =; //1000x1000 lion w 45 b 150 7x7
	//1000x1000 lion w 150 b 840 4x4
	//2000x2000 lion w 150 b 150
	//2000x2000 lion w 100 b 100
	//1200x1200 tree w 20 b 50

	//20190412 lion 4x4 test typyC Fig W:300 B:350
	int classify_BC = 350;
}; 