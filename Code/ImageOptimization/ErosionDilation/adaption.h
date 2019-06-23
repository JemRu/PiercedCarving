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
	////ͨ��ԭͼ�õ� diff out �������Ԥ�����type
	void detection(const char* inputImage, const char* OutputImage, int num);
	////�õ�border��ǰһ��
	int get_lable(const char* inputImage);
	void boundary_class_define(int label_id);
	bool treatment(const char* inputImage, const char* inputImage2, const char* out);

	void get_boundary(int n);
	////ͨ��diff ����typeA �� bc  �õ�boundary �߽������ �Ŀ��ӻ�ͼ��
	void get_diff_boundary(int n, const char* outImage);
	void get_out_boundary(const char* inputImage);

	void visual_detection(const char* inputImage, const char* OutputImage);
	void visual_classify_2(const char* inputImage, const char* OutputImage, int  area, float ratio);//���������ȿ��ӻ�Ϊ������
	////���ӻ�label��С ��color��ɫ
	////img.visual_classify_jar("widen/diff.png", "widen/test_dvv.p ng", 1, 0.6);
	void visual_classify_jar(const char* inputImage, const char* OutputImage, int  area, float ratio);//����������color bar���ӻ�
	void visual_classify_diff(const char* inputImage, const char* OutputImage, int  area, float ratio);//����������
	////ͨ��diff �õ�diff_c(a �� bc ��) diff_f�����ڼ��� ���볡 disboufield�е��ã�
	void visual_detection_diff(const char* inputImage, const char* OutputImage, const char* OutputImage2);//���ӻ� ä�� ����

	//�ҵ���ͻ���� ���س�ͻ�����Ƿ񻹴��� �����ͻ����Ȩ�� ��Ҫ diff  inputImage���ڿ��ӻ� ����
	bool get_conflict(const char* inputImage, const char* inputImage2, const char* inputImage3, const char* DW, const char* DB, int n);
	//������ֵ�� ����ͻ����
	void get_conflict_kernel(const char* inputImage, const char* inputImage2, int n);
	//�����ͻ�����Ȩ�� ͨ�� out-conflict.png label_b&w ����ÿ�� ��ͻ����� Ȩ�� 
	void get_conflict_weight();
	//�����ͻ���� ͨ�� diff_conflict diff_w �õ�����ĳ�ͻ�����out-treat_con diff_w_uc diff_b_uc
	void treatment_conflict(const char* inputImage, int num1, int num2);
	//����ǳ�ͻ���� ͨ�� diff_w_uc diff_b_uc �õ�һ�� adaption �Ľ�� out-adaption
	void treatment_unconflict(const char* inputImage, const char* inputImage2, const char* out);

	//����ѳ�ͻ���� ͨ��ȥϸС�� diff_w_uc diff_b_uc �õ�һ�� adaption �Ľ�� out-adaption
	void treatment_unconflict_x(const char* out);

	//��� 
	void treatment_optimize();

	void setKernel(int num);

	std::vector < std::vector<cv::Vec2i>> boundary_pixel;//real boundary
	std::vector < std::vector<cv::Vec2i>> boundary_pixel_l;//label_boundary
	std::vector<std::vector<int>> label;

	std::vector<int> perimeter;
	
	//�洢tunnels�������ǵ�label
	std::vector<std::vector<int>> label3;
	//�洢���tunnelǰ�������ǵ�label
	std::vector<std::vector<int>> label2;
	std::vector<std::vector<int>> label_pw;
	std::vector<vector<cv::Vec2i>> lebeled_pixels;
	std::vector<vector<cv::Vec2i>> tunnel_seed_list_x;
	std::vector<vector<cv::Vec2i>> tunnel_seed_list_y;
	//���ʱ�����ӵ�
	std::set<std::tuple<int, int>> tunnel_seed_x;
	std::set<std::tuple<int, int>> tunnel_seed_y;
	//std::vector <set<cv::Vec2i>> tunnel_area_border;
	std::vector<set<std::tuple<int, int>>> tunnel_area_border;
	std::vector<set<std::tuple<int, int>>> tunnel_area_border_copy;
	//�洢��ͬtunnels����Ӧ��path
	std::vector<queue<std::tuple<int, int>>> paths; 
	std::vector<std::vector<int>> label_boundary;//not real boundary
	std::vector<std::vector<int>> label_boundary_id;//type of boundary 0 not boundary BW red  1 BB blue 2
	std::vector<int> label_id;//type of label[*] 0 narrow 1 blind

	std::vector<std::vector<int>> label_in;//ԭͼ 0Ϊ�� 1Ϊ��
	std::vector<std::vector<int>> label_w;//��ɫtypeAB1
	std::vector<std::vector<int>> label_b;//��ɫtypeAB
	std::vector<std::vector<int>> label_wc;//��ɫ ��ͻ���� //1��ͻ 0ͨ�� 2�߽��
	std::vector<std::vector<int>> label_bc;//��ɫ ��ͻ����
	std::vector<std::vector<int>> label_c;//all ��ͻ����
	std::vector<int> label_weight;//��ͻ����Ȩ��

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