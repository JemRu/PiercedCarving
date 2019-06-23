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
	//获取所有components的boundary像素坐标
	void get_all_boundary(int n);
	//获取制定lable的像素个数 删除小于阈值minsize 的label
	void remove_lable(string src, string style, int size);
	//获得指定某个lable下的边界像素坐标;
	void boundary_class_define(int label_id);
	//每个像素的lable;
	std::vector<std::vector<int>> label;
	//conponents下的boundary坐标
	std::vector < std::vector<cv::Vec2i>> boundary_pixel;
	//获取尺寸
	int get_size(const char* inputImage);
	int get_lable(const char* inputImage);
	//所有不同components间的距离;
	double distance_all();
	void merge(const char* inputImage, const char* outputImage);
	//计算不同components间的距离；
	double distance_components(int c_candidate);
	//计算两点之间的距离；
	double distance_between(cv::Vec2i boundary_pixel1, cv::Vec2i boundary_pixel);

	//remove noise
	void remove_noise(int num, String inputstr, String outstr);

	//连接线的宽度
	void setWidth(int num);	
	int conWidth = 3;

	//每个label的周长记录
	std::vector<int> perimeter;
	//记录图片尺寸信息；
	int width;
	int height;


	//min表示components的编号，即label_id。min_id记录是该components的boudary的哪一个点
	int min1, min1_id;
	int min2, min2_id;


	//一组记录所有components最小距离的点的变量

	//g_min记录components的编号 g_min1记录components编号,g_min_id记录所有boundary上的某一点
	int g_min1, g_min1_id;
	int g_min2, g_min2_id;
};

