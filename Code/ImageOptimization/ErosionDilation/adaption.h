#pragma once
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <set>
#include <queue>
#include "optimize.h"
using namespace  cv;
using namespace std;
class adaption
{
public:
	adaption();
	~adaption();
	////通过原图得到 diff out 检测所有预处理的type
	void detection(const char* inputImage, const char* OutputImage, int num);
	////得到border的前一步
	int get_lable(const char* inputImage);
	void boundary_class_define(int label_id);
	bool treatment(const char* inputImage, const char* inputImage2, const char* out);

	void get_boundary(int n);
	////通过diff 区分typeA 与 bc  得到boundary 边界的区分 的可视化图像
	void get_diff_boundary(int n, const char* outImage);
	void get_out_boundary(const char* inputImage);

	void visual_detection(const char* inputImage, const char* OutputImage);
	void visual_classify_2(const char* inputImage, const char* OutputImage, int  area, float ratio);//根据狭长比可视化为红与蓝
	////可视化label大小 用color着色
	////img.visual_classify_jar("widen/diff.png", "widen/test_dvv.p ng", 1, 0.6);
	void visual_classify_jar(const char* inputImage, const char* OutputImage, int  area, float ratio);//根据狭长比color bar可视化
	void visual_classify_diff(const char* inputImage, const char* OutputImage, int  area, float ratio);//根据狭长比
	////通过diff 得到diff_c(a 黄 bc 红) diff_f（用于计算 距离场 disboufield中调用）
	void visual_detection_diff(const char* inputImage, const char* OutputImage, const char* OutputImage2);//可视化 盲区 狭区

	//找到冲突区域 返回冲突区域是否还存在 计算冲突区域权重 需要 diff  inputImage用于可视化 画板
	bool get_conflict(const char* inputImage, const char* inputImage2, const char* inputImage3, const char* DW, const char* DB, int n);
	//根据阈值来 填充冲突区域
	void get_conflict_kernel(const char* inputImage, const char* inputImage2, int n);
	//计算冲突区域的权重 通过 out-conflict.png label_b&w 计算每个 冲突区域的 权重 
	void get_conflict_weight();
	//处理冲突区域 通过 diff_conflict diff_w 得到处理的冲突区域的out-treat_con diff_w_uc diff_b_uc
	void treatment_conflict(const char* inputImage, int num1, int num2);
	//处理非冲突区域 通过 diff_w_uc diff_b_uc 得到一次 adaption 的结果 out-adaption
	void treatment_unconflict(const char* inputImage, const char* inputImage2, const char* out);

	//处理费冲突区域 通过去细小的 diff_w_uc diff_b_uc 得到一次 adaption 的结果 out-adaption
	void treatment_unconflict_x(const char* out);

	//结果 
	void treatment_optimize();

	void setKernel(int num);

	std::vector < std::vector<cv::Vec2i>> boundary_pixel;//real boundary
	std::vector < std::vector<cv::Vec2i>> boundary_pixel_l;//label_boundary
	std::vector<std::vector<int>> label;

	std::vector<int> perimeter;
	
	//存储tunnels的像素们的label
	std::vector<std::vector<int>> label3;
	//存储检测tunnel前的像素们的label
	std::vector<std::vector<int>> label2;
	std::vector<std::vector<int>> label_pw;
	std::vector<vector<cv::Vec2i>> lebeled_pixels;
	std::vector<vector<cv::Vec2i>> tunnel_seed_list_x;
	std::vector<vector<cv::Vec2i>> tunnel_seed_list_y;
	//检测时的种子点
	std::set<std::tuple<int, int>> tunnel_seed_x;
	std::set<std::tuple<int, int>> tunnel_seed_y;
	//std::vector <set<cv::Vec2i>> tunnel_area_border;
	std::vector<set<std::tuple<int, int>>> tunnel_area_border;
	std::vector<set<std::tuple<int, int>>> tunnel_area_border_copy;
	//存储不同tunnels所对应的path
	std::vector<queue<std::tuple<int, int>>> paths; 
	std::vector<std::vector<int>> label_boundary;//not real boundary
	std::vector<std::vector<int>> label_boundary_id;//type of boundary 0 not boundary BW red  1 BB blue 2
	std::vector<int> label_id;//type of label[*] 0 narrow 1 blind

	std::vector<std::vector<int>> label_in;//原图 0为白 1为黑
	std::vector<std::vector<int>> label_w;//白色typeAB1
	std::vector<std::vector<int>> label_b;//黑色typeAB
	std::vector<std::vector<int>> label_wc;//白色 冲突区域 //1冲突 0通过 2边界点
	std::vector<std::vector<int>> label_bc;//黑色 冲突区域
	std::vector<std::vector<int>> label_c;//all 冲突区域
	std::vector<int> label_weight;//冲突区域权重

	Mat Confliction, visualconfliction;
	Mat Dw, Db, Dw_uc, Db_uc, treat_con;

	//lion 4x4 13b9w
	//scenery 2000px 8.5x8.5 11 b 9 w
	//scenery 1700px 8.5x8.5 11 b 7 w
	//scenery 2400px 8.5x8.5 11 b 7 w
	//tree 1200px 12x12 5b3w
	//tree 1200px 8.5x8.5 5w7b
	int kernel = 3;
	Mat element = getStructuringElement(MORPH_RECT, Size(kernel, kernel));

	Mat tempout_detection;
	Mat diff_detection;

};