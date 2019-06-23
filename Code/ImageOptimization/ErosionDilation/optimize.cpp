#include "stdafx.h"
#include "optimize.h"

optimize::optimize()
{
}


optimize::~optimize()
{
}
int optimize::get_black_lable(const char* inputImage){

	cv::Mat srcImg = imread(inputImage, cv::IMREAD_COLOR);
	cv::bitwise_xor(srcImg, cv::Scalar(255, 255, 255), srcImg);
	cv::Mat gray;

	cv::cvtColor(srcImg, gray, CV_BGR2GRAY);

	//2値化
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (gray < threshval) : (gray > threshval);
	//
	//求联通区域
	cv::Mat labelImage(bin.size(), CV_32S);

	//int nLabels = cv::connectedComponents(bin, labelImage, 8);
	int nLabels = connectedComponents(bin, labelImage, 4);

	std::vector<cv::Vec3b> colors(nLabels);
	colors[0] = cv::Vec3b(0, 0, 0);

	//output_info << "Number of connected components = " << nLabels - 1 << std::endl << std::endl;
	//std::output_info << "Number of connected components = " << nLabels << std::endl << std::endl;

	for (int label = 1; label < nLabels; label++)
	{
		colors[label] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
		//std::output_info << "Component " << label << std::endl;

	}

	cv::Mat dst(srcImg.size(), CV_8UC3);

	for (int r = 0; r < dst.rows; r++)
	{
		std::vector<int> cur_vector;
		for (int c = 0; c < dst.cols; c++)
		{
			int cur_lable = labelImage.at<int>(r, c);
			cv::Vec3b &pixel = dst.at<cv::Vec3b>(r, c);
			pixel = colors[cur_lable];

			cur_vector.push_back(cur_lable);
		}
		label.push_back(cur_vector);
	}

	//std::ofstream file("C:/Users/Cauchy/Documents/Visual Studio 2013/Projects/MAT2/data/labled_image.txt");
	std::ofstream file("data_info/labled_image.txt");
	for (int i = 0; i < label.size(); i++)
	{
		for (int j = 0; j < label[i].size(); j++)
		{
			file << " " << label[i][j];
		}
		file << "" << std::endl;
		//cout << label[i].size() << endl;
	}
	//cout << label.size()<<endl;

	imwrite("colores_lable.png", dst);

	return nLabels;
}
void optimize::get_all_boundary(int n){
	for (int i = 1; i < n; i++){
		boundary_class_define(i);
	}
	std::ofstream file2("boundary_info.txt");
	perimeter.clear();
	for (int i = 0; i < boundary_pixel.size(); i++)
	{
		/*******************jem**********************/
		perimeter.push_back(boundary_pixel[i].size());
		/*******************jem**********************/
		for (int j = 0; j < boundary_pixel[i].size(); j++){
			//file2 << i + 1 << " ";
			file2 << boundary_pixel[i][j][0] << " " << boundary_pixel[i][j][1] << std::endl;

		}
	}
}
void optimize::boundary_class_define(int label_id){
	int current_lable = label_id;
	//传入实参（给定lable编号）时，获取boundary的像素值
	std::ofstream out_boundary("boundary_pixel.txt");
	//  label.size()是列数，相当于y

	std::vector < cv::Vec2i> point;
	for (int i = 1; i < label.size() - 1; i++)
	{
		//label[[i].size()是行数，相当于y
		for (int j = 1; j < label[i].size() - 1; j++)
		{
			if (label[i][j] == current_lable)
			{
				if ((label[i - 1][j] == 0 || label[i][j - 1] == 0) || (label[i + 1][j] == 0 || label[i][j + 1] == 0))
				{

					//boundary.push_back(j);
					//boundary_pixel.push_back(boundary);
					out_boundary << "(" << i << "," << j << ")" << std::endl;
					cv::Vec2i v(i, j);
					point.push_back(v);

				}
			}
		}
	};
	boundary_pixel.push_back(point);
}
void optimize::remove_lable(string src, string style, int size){
	string imgname = src;
	string imgstyple = style;
	cv::Mat srcImg = imread(imgname + "." + style);
	if (srcImg.empty())
	{
		cout << "Could not read input image file" << endl;
		system("pause");
	}
	cv::bitwise_xor(srcImg, cv::Scalar(255, 255, 255), srcImg);
	cv::Mat gray;
	cv::cvtColor(srcImg, gray, CV_BGR2GRAY);
	//2値化
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (gray < threshval) : (gray > threshval);
	//求联通区域
	cv::Mat labelImage(bin.size(), CV_32S);
	Mat stats(bin.size(), CV_32S);
	Mat centroids(bin.size(), CV_32S);
	//int nLabels = cv::connectedComponents(bw, labelImage, 8);
	int nLabels = connectedComponentsWithStats(bin, labelImage, stats, centroids, 4);
	//声明vector,类型为opencv 中的vec3b类型，存放不同联通区域的着色 
	vector<Vec3b> colors(nLabels);
	//背景白色
	colors[0] = Vec3b(255, 255, 255);
	std::cout << "nLabels" << nLabels << endl;

	for (int label = 1; label < nLabels; label++)
	{
		float area = stats.at<int>(label, cv::CC_STAT_AREA);
		float ratios = perimeter[label - 1] / area;
		//std::cout << "perimeter:" << perimeter[label - 1] << "area:" << area << "ratios:" << ratios << endl;
		if (area < size)
		{
			colors[label] = Vec3b(255, 255, 255);
		}
		else
		{
			if (ratios == 1)
			{
				colors[label] = Vec3b(255, 255, 255);
			}
		}
	}

	//std::cout << "add color sucess" << endl;
	//dst存储最终上色的标记图像
	Mat dst(bin.size(), CV_8UC3);
	for (int r = 0; r < dst.rows; r++)
	{
		for (int c = 0; c < dst.cols; c++)
		{
			int label = labelImage.at<int>(r, c);
			Vec3b &pixel = dst.at<Vec3b>(r, c);
			pixel = colors[label];
		}
	}

	std::cout << "paint sucess" << endl;
	//反相处理：
	cv::bitwise_xor(bin, cv::Scalar(255, 255, 255), bin);

	
	cv::bitwise_xor(bin, cv::Scalar(255, 255, 255), bin);
	cv::bitwise_xor(dst, cv::Scalar(255, 255, 255), dst);
	imwrite(imgname + "_." + style, bin);
	imwrite(imgname + "_r." + style, dst);
}

int optimize::get_size(const char* inputImage){
	
	cv::Mat src_input = imread(inputImage, cv::IMREAD_COLOR);
	width = src_input.size().width;
	height = src_input.size().height;
	return 0;
}
void optimize::merge(const char* inputImage, const char* outputImage){
	cv::Mat srcImg = imread(inputImage, cv::IMREAD_COLOR);
	cv::Vec2i start_point = boundary_pixel[g_min1][g_min1_id];
	cv::Vec2i end_point = boundary_pixel[g_min2][g_min2_id];
	cv::Point sp(start_point[1], start_point[0]), ep(end_point[1], end_point[0]);

	//如果采用白色像素全部相连。
	cv::line(srcImg, sp, ep, cv::Scalar(255, 255, 255), conWidth);


	cv::imwrite(outputImage, srcImg);
}
double optimize::distance_all()
{
	double global_min = INFINITY;
	for (int i = 0; i < boundary_pixel.size() - 1; i++)
	{
		double min_all = distance_components(i);
		if (min_all < global_min)
		{
			global_min = min_all;
			g_min1 = min1;
			g_min1_id = min1_id;
			g_min2 = min2;
			g_min2_id = min2_id;
		}

	}
	//output_info << g_min1 << "," << g_min1_id << std::endl;
	//output_info << g_min2 << "," << g_min2_id << std::endl;

	return global_min;
}
int optimize::get_lable(const char* inputImage){

	cv::Mat srcImg = imread(inputImage, cv::IMREAD_COLOR);
	//cv::bitwise_xor(srcImg, cv::Scalar(255, 255, 255), srcImg);
	cv::Mat gray;

	cv::cvtColor(srcImg, gray, CV_BGR2GRAY);

	//2値化
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (gray < threshval) : (gray > threshval);

	//反相处理：
	cv::bitwise_xor(bin, cv::Scalar(255, 255, 255), bin);
	//求联通区域
	cv::Mat labelImage(bin.size(), CV_32S);

	//int nLabels = cv::connectedComponents(bin, labelImage, 8);
	int nLabels = connectedComponents(bin, labelImage, 4);


	std::vector<cv::Vec3b> colors(nLabels);
	colors[0] = cv::Vec3b(0, 0, 0);

	//output_info << "Number of connected components = " << nLabels - 1 << std::endl << std::endl;
	//output_info << "Number of connected components = " << nLabels << std::endl << std::endl;

	for (int label = 1; label < nLabels; label++)
	{
		colors[label] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
		//output_info << "Component " << label << std::endl;

	}

	cv::Mat dst(srcImg.size(), CV_8UC3);

	for (int r = 0; r < dst.rows; r++)
	{
		std::vector<int> cur_vector;
		for (int c = 0; c < dst.cols; c++)
		{
			int cur_lable = labelImage.at<int>(r, c);
			cv::Vec3b &pixel = dst.at<cv::Vec3b>(r, c);
			pixel = colors[cur_lable];

			cur_vector.push_back(cur_lable);
		}
		label.push_back(cur_vector);
	}

	std::ofstream file("data_info/labled_image.txt");
	for (int i = 0; i < label.size(); i++)
	{
		for (int j = 0; j < label[i].size(); j++)
		{
			file << " " << label[i][j];
		}
		file << "" << std::endl;
		//cout << label[i].size() << endl;
	}
	//cout << label.size()<<endl;

	imwrite("colores_lable.png", dst);

	return nLabels;
}
double optimize::distance_components(int c_candidate){

	double min_distance = INFINITY;
	int c_min = boundary_pixel.size() - 1;
	for (int i = 0; i < boundary_pixel[c_min].size(); i++){
		for (int j = 0; j < boundary_pixel[c_candidate].size(); j++){
			double temp_d = distance_between(
				boundary_pixel[c_min][i],
				boundary_pixel[c_candidate][j]);
			if (temp_d < min_distance){
				min_distance = temp_d;
				min1 = c_min;
				min1_id = i;
				min2 = c_candidate;
				min2_id = j;
			}
		}
	}
	return min_distance;
}
double optimize::distance_between(cv::Vec2i boundary_pixel1, cv::Vec2i boundary_pixel2)
{
	int y1 = boundary_pixel1[1];
	int x1 = boundary_pixel1[0];

	int y2 = boundary_pixel2[1];
	int x2 = boundary_pixel2[0];

	double distance = sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
	return distance;
}
void optimize::remove_noise(int num, String inputstr, String outstr)
{
	cv::Mat srcImg = imread(inputstr);
	if (srcImg.empty())
	{
		cout << "Could not read input image file" << endl;
		system("pause");
	}
	cv::bitwise_xor(srcImg, cv::Scalar(255, 255, 255), srcImg);
	cv::Mat gray;
	cv::cvtColor(srcImg, gray, CV_BGR2GRAY);
	//2値化
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (gray < threshval) : (gray > threshval);
	//求联通区域
	cv::Mat labelImage(bin.size(), CV_32S);
	Mat stats(bin.size(), CV_32S);
	Mat centroids(bin.size(), CV_32S);
	//int nLabels = cv::connectedComponents(bw, labelImage, 8);
	int nLabels = connectedComponentsWithStats(bin, labelImage, stats, centroids, 4);
	//声明vector,类型为opencv 中的vec3b类型，存放不同联通区域的着色 
	vector<Vec3b> colors(nLabels);
	//背景白色
	colors[0] = Vec3b(255, 255, 255);
	//std::cout << "nLabels" << nLabels << endl;

	for (int label = 1; label < nLabels; label++)
	{
		float area = stats.at<int>(label, cv::CC_STAT_AREA);
		if (area < num)
		{
			colors[label] = Vec3b(255, 255, 255);
		}
	}
	//dst存储最终上色的标记图像
	Mat dst(bin.size(), CV_8UC3);
	for (int r = 0; r < dst.rows; r++)
	{
		for (int c = 0; c < dst.cols; c++)
		{
			int label = labelImage.at<int>(r, c);
			Vec3b &pixel = dst.at<Vec3b>(r, c);
			pixel = colors[label];
		}
	}
	imwrite(outstr, dst);
}
//set
void optimize::setWidth(int num){
	conWidth = num;
}