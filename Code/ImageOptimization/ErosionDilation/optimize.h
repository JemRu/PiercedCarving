#pragma once
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <climits>
#include <fstream>
#include <iomanip>
#include <vector>
#include "math.h"
using namespace std;
using namespace cv;
class optimize
{
public:
	optimize();
	~optimize();

	int get_black_lable(const char* inputImage);
	//��ȡ����components��boundary��������
	void get_all_boundary(int n);
	//��ȡ�ƶ�lable�����ظ��� ɾ��С����ֵminsize ��label
	void remove_lable(string src, string style, int size);
	//���ָ��ĳ��lable�µı߽���������;
	void boundary_class_define(int label_id);
	//ÿ�����ص�lable;
	std::vector<std::vector<int>> label;
	//conponents�µ�boundary����
	std::vector < std::vector<cv::Vec2i>> boundary_pixel;
	//��ȡ�ߴ�
	int get_size(const char* inputImage);
	int get_lable(const char* inputImage);
	//���в�ͬcomponents��ľ���;
	double distance_all();
	void merge(const char* inputImage, const char* outputImage);
	//���㲻ͬcomponents��ľ��룻
	double distance_components(int c_candidate);
	//��������֮��ľ��룻
	double distance_between(cv::Vec2i boundary_pixel1, cv::Vec2i boundary_pixel);

	//remove noise
	void remove_noise(int num, String inputstr, String outstr);

	//�����ߵĿ��
	void setWidth(int num);	
	int conWidth = 3;

	//ÿ��label���ܳ���¼
	std::vector<int> perimeter;
	//��¼ͼƬ�ߴ���Ϣ��
	int width;
	int height;


	//min��ʾcomponents�ı�ţ���label_id��min_id��¼�Ǹ�components��boudary����һ����
	int min1, min1_id;
	int min2, min2_id;


	//һ���¼����components��С����ĵ�ı���

	//g_min��¼components�ı�� g_min1��¼components���,g_min_id��¼����boundary�ϵ�ĳһ��
	int g_min1, g_min1_id;
	int g_min2, g_min2_id;
};

