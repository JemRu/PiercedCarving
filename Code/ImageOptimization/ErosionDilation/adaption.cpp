#include "stdafx.h"
#include "adaption.h"


adaption::adaption()
{
}


adaption::~adaption()
{
}

void adaption::detection(const char* inputImage, const char* Out, int num) {
	//载入原图   
	Mat image = imread(inputImage);
	Mat inter, out;

	//获取自定义核  
	//进行腐蚀操作  
	Mat elementd = getStructuringElement(MORPH_RECT, Size(num, num));
	erode(image, inter, elementd);
	dilate(inter, out, elementd);
	tempout_detection = out;
	cvtColor(out, out, CV_BGR2GRAY);
	threshold(out, out, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	//cv::imwrite("widen/out.png", out);

	//图像相减
	String filename = "";
	String imgname1 = "";
	String imgname2 = "";

	IplImage* img = cvLoadImage(inputImage);
	IplImage imgTmp = tempout_detection;
	IplImage* img1 = cvCloneImage(&imgTmp);
	IplImage* diff = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
	cvAbsDiff(img, img1, diff);
	cvSaveImage(Out, diff);//Dw&b.png

	/*Mat diffimg = cv::imread(Out);
	Mat gray, bin;
	cvtColor(diffimg, gray, CV_BGR2GRAY);
	threshold(gray, bin, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	diff_detection = diffimg;*/
}
int adaption::get_lable(const char* inputImage){

	Mat diffimg = cv::imread(inputImage);
	diff_detection = diffimg;

	cv::Mat srcImg = imread(inputImage);
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

	std::ofstream file("data_info/labled_image.txt");
	for (int i = 0; i < label.size(); i++)
	{
		for (int j = 0; j < label[i].size(); j++)
		{
			file << " " << label[i][j];
		}
		file << "" << std::endl;
		//std::cout << label[i].size() << endl;
	}
	//std::cout << label.size()<<endl;

	cv::imwrite("temp/color_labels.png", dst);

	return nLabels;
}
void adaption::boundary_class_define(int label_id){
	int current_lable = label_id;
	//传入实参（给定lable编号）时，获取boundary的像素值
	std::ofstream out_boundary("boundary_pixel.txt");
	//  label.size()是列数，相当于y

	std::vector < cv::Vec2i> point;
	std::vector < cv::Vec2i> point_l;
	for (int i = 1; i < label.size() - 1; i++)
	{
		//label[[i].size()是行数，相当于y
		for (int j = 1; j < label[i].size() - 1; j++)
		{
			if (label[i][j] == current_lable)
			{
				if ((label[i - 1][j] == 0 || label[i][j - 1] == 0) || (label[i + 1][j] == 0 || label[i][j + 1] == 0 || label[i - 1][j - 1] == 0 || label[i + 1][j - 1] == 0) || (label[i + 1][j + 1] == 0 || label[i - 1][j + 1] == 0))
				{

					//boundary.push_back(j);
					//boundary_pixel.push_back(boundary);
					out_boundary << "(" << i << "," << j << ")" << std::endl;
					cv::Vec2i v(i, j);
					point_l.push_back(v);

				}
				if ((label[i - 1][j] == 0 || label[i][j - 1] == 0) || (label[i + 1][j] == 0 || label[i][j + 1] == 0 ))
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
	boundary_pixel_l.push_back(point_l);
	boundary_pixel.push_back(point);
}
void adaption::get_boundary(int n){

	for (int i = 1; i < n; i++){
		boundary_class_define(i);
	}
	//std::ofstream file2("widen/boundary_info.txt");

	for (int i = 0; i < boundary_pixel.size(); i++)
	{
		perimeter.push_back(boundary_pixel[i].size());
		std::cout << perimeter[i] << endl;
		for (int j = 0; j < boundary_pixel[i].size(); j++){
			//file2 << boundary_pixel[i][j][0] << " " << boundary_pixel[i][j][1] << std::endl;
			//img.at<Vec3b>(boundary_pixel[i][j][0], boundary_pixel[i][j][1]) = colors[0];
		}
	}

	//imwrite("widen/out_.png",img);
}
void adaption::get_diff_boundary(int n, const char* outImage){
	//x 和 y 是相反的
	
	Mat img = tempout_detection.clone();
	Mat img_copy = tempout_detection.clone();
	bitwise_xor(img, Scalar(255, 255, 255), img);
	bitwise_xor(img_copy, Scalar(255, 255, 255), img_copy);


	Mat diff = diff_detection.clone();
	std::vector<cv::Vec3b> colors(5);
	colors[0] = cv::Vec3b(0, 0, 0);
	colors[1] = cv::Vec3b(0, 0, 255);//可视化接触边界的颜色
	colors[2] = cv::Vec3b(255, 0, 0);//可视化不接触边界的颜色
	colors[3] = cv::Vec3b(0, 255, 255);//可视化不接触边界的颜色
	colors[4] = cv::Vec3b(255, 255, 255);

	//造盒子
	std::vector<int> temp2;
	for (int r = 0; r < img.rows; r++)
	{
		for (int c = 0; c < img.cols; c++)
		{
			temp2.push_back(0);
		}
		label_boundary.push_back(temp2);
		temp2.clear();
	}
	std::vector<int> temp2_;
	for (int r = 0; r < img.rows; r++)
	{
		for (int c = 0; c < img.cols; c++)
		{
			temp2_.push_back(0);
		}
		label_boundary_id.push_back(temp2_);
		temp2_.clear();
	}

	for (int i = 1; i < n; i++){
		boundary_class_define(i);
	}
	//std::ofstream file2("widen/boundary_info.txt");
	//std::cout << boundary_pixel.size() << endl;
	for (int i = 0; i < boundary_pixel.size(); i++)
	{
		perimeter.push_back(boundary_pixel[i].size());
		//std::cout << perimeter[i] << endl;
		bool aa = false;
		for (int j = 0; j < boundary_pixel[i].size(); j++){
			int x = boundary_pixel[i][j][0];
			int y = boundary_pixel[i][j][1];

			//file2 << boundary_pixel[i][j][0] << " " << boundary_pixel[i][j][1] << std::endl;

			label_boundary_id[x][y] = 1;
			
			img.at<Vec3b>(x, y)[0] = 0;
			img.at<Vec3b>(x, y)[1] = 0;
			img.at<Vec3b>(x, y)[2] = 255;
			//8领域检测 标出 接触边 可视化为蓝色
			if (0 == img_copy.at<Vec3b>(x + 1, y)[0] || 0 == img_copy.at<Vec3b>(x - 1, y)[0] || 0 == img_copy.at<Vec3b>(x, y + 1)[0] || 0 == img_copy.at<Vec3b>(x, y - 1)[0] || 0 == img_copy.at<Vec3b>(x + 1, y + 1)[0] || 0 == img_copy.at<Vec3b>(x - 1, y - 1)[0] || 0 == img_copy.at<Vec3b>(x - 1, y + 1)[0] || 0 == img_copy.at<Vec3b>(x + 1, y - 1)[0])
			{
				img.at<Vec3b>(x, y) = colors[2];
				label_boundary_id[x][y] = 2;
			}
		}
	}

	//cv::imwrite(outImage, img);


	for (int i = 0; i < boundary_pixel.size(); i++)
	{
		Mat tempimg = img.clone();
		for (int i = 0; i <tempimg.rows; i++)
		{
			for (int j = 0; j < tempimg.cols; j++)
			{
				tempimg.at<Vec3b>(i, j) = colors[0];
			}
		}
		for (int j = 0; j < boundary_pixel[i].size(); j++){
			int x = boundary_pixel[i][j][0];
			int y = boundary_pixel[i][j][1];
			if (label_boundary_id[x][y] == 2)
			{
				tempimg.at<Vec3b>(x, y) = colors[4];
			}
		}

		cv::bitwise_xor(tempimg, cv::Scalar(255, 255, 255), tempimg);
		cv::Mat gray;
		cv::cvtColor(tempimg, gray, CV_BGR2GRAY);
		int threshval = 100;
		cv::Mat bin = threshval < 128 ? (gray < threshval) : (gray > threshval);
		cv::Mat labelImage(bin.size(), CV_32S);
		int nLabels = cv::connectedComponents(bin, labelImage, 8);
		//std::cout << nLabels << "eihei" << endl;
		if (nLabels == 2)
		{
			label_id.push_back(1);
		}
		else
		{
			label_id.push_back(0); //narrow
		}
	}
}
void adaption::get_out_boundary(const char* inputImage){
	Mat img = cv::imread(inputImage, CV_8UC3);

	Mat img_copy(img.size(), CV_8UC3); 

	std::vector<cv::Vec3b> colors(5);
	colors[0] = cv::Vec3b(0, 0, 0);
	colors[1] = cv::Vec3b(0, 255, 255);
	colors[2] = cv::Vec3b(0, 0, 255);
	colors[3] = cv::Vec3b(255, 255, 255);

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			img_copy.at<Vec3b>(i, j) == colors[2];

			//if (i != 0 && j != 0 && i != img.rows - 1 && j != img.cols - 1) {
			//	if (img.at<Vec3b>(i, j)[0] == 0)
			//	{
			//		
			//		if (img.at<Vec3b>(i + 1, j)[0] == 255 || img.at<Vec3b>(i - 1, j)[0] == 255 || img.at<Vec3b>(i, j + 1)[0] == 255 || img.at<Vec3b>(i, j - 1)[0] == 255 || img.at<Vec3b>(i + 1, j - 1)[0] == 255 || img.at<Vec3b>(i - 1, j - 1)[0] == 255 || img.at<Vec3b>(i - 1, j + 1)[0] == 255 || img.at<Vec3b>(i + 1, j + 1)[0] == 255){
			//			//std::cout << "dedeed" << endl;

			//			img_copy.at<Vec3b>(i, j)[0] == 255;
			//			img_copy.at<Vec3b>(i, j)[1] == 255;
			//			img_copy.at<Vec3b>(i, j)[2] == 255;
			//		}
			//	}
			//}
		}
	}

	//imwrite("widen/diff_1f.png", img_copy);
	
}
void adaption::visual_classify_2(const char* inputImage, const char* OutputImage, int  area, float ratio){
	cv::Mat srcImg = imread(inputImage);
	cv::bitwise_xor(srcImg, cv::Scalar(255, 255, 255), srcImg);
	cv::Mat grayy;
	cv::cvtColor(srcImg, grayy, CV_BGR2GRAY);
	//2値化
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (grayy < threshval) : (grayy > threshval);
	//求联通区域
	cv::Mat labelImage(bin.size(), CV_32S);
	Mat stats(bin.size(), CV_32S);
	Mat centroids(bin.size(), CV_32S);
	//int nLabels = cv::connectedComponents(bw, labelImage, 8);
	int nLabels = connectedComponentsWithStats(bin, labelImage, stats, centroids, 4);
	//声明vector,类型为opencv 中的vec3b类型，存放不同联通区域的着色 
	//vector<uchar> colors(nLabels);
	//背景黑色
	//colors[0] = uchar(0);
	std::vector<cv::Vec3b> colors(nLabels);
	colors[0] = cv::Vec3b(0, 0, 0);
	std::cout << "nLabels" << nLabels << endl;
	
	for (int i = 1; i < nLabels; i++)
	{
		float larea = stats.at<int>(i, cv::CC_STAT_AREA);
		float ss = perimeter[0];
		float ratios = perimeter[i - 1] / larea;
		//std::std::cout << "perimeter:" << perimeter[i - 1] << "   area:" << larea << "   ratios:" << ratios << endl;
		int temp = ratios * 255;
		//colors[i] = uchar(temp);
		//std::cout << "color: " << colors[0] << endl;
		if (ratios >= 0.5) {
			colors[i] = Vec3b(0, 0, 255);
		}
		else
		{
			colors[i] = Vec3b(255, 0, 0);
		}
	}

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
	cv::imwrite("widen/diff_c.png", dst);

	Mat output = cv::imread(OutputImage);
	Mat temp = cv::imread("widen/diff_c.png");
	for (int r = 0; r < temp.rows; r++)
	{
		for (int c = 0; c < temp.cols; c++)
		{
			if (temp.at<Vec3b>(r, c)[0] == 0 && temp.at<Vec3b>(r, c)[1] == 0 && temp.at<Vec3b>(r, c)[2] == 0){

			}
			else
			{
				output.at<Vec3b>(r, c)[0] = temp.at<Vec3b>(r, c)[0];
				output.at<Vec3b>(r, c)[1] = temp.at<Vec3b>(r, c)[1];
				output.at<Vec3b>(r, c)[2] = temp.at<Vec3b>(r, c)[2];
			}

		}
	}

	cv::imwrite("widen/visual_classify.png", output);

}
void adaption::visual_classify_diff(const char* inputImage, const char* OutputImage, int  area, float ratio){
	cv::Mat srcImg = imread(inputImage);
	cv::bitwise_xor(srcImg, cv::Scalar(255, 255, 255), srcImg);
	cv::Mat grayy;
	cv::cvtColor(srcImg, grayy, CV_BGR2GRAY);
	//2値化
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (grayy < threshval) : (grayy > threshval);
	//求联通区域
	cv::Mat labelImage(bin.size(), CV_32S);
	Mat stats(bin.size(), CV_32S);
	Mat centroids(bin.size(), CV_32S);
	//int nLabels = cv::connectedComponents(bw, labelImage, 8);
	int nLabels = connectedComponentsWithStats(bin, labelImage, stats, centroids, 4);
	//声明vector,类型为opencv 中的vec3b类型，存放不同联通区域的着色 
	//vector<uchar> colors(nLabels);
	//背景黑色
	//colors[0] = uchar(0);
	std::vector<cv::Vec3b> colors(nLabels);
	colors[0] = cv::Vec3b(0, 0, 0);
	std::cout << "nLabels" << nLabels << endl;

	for (int i = 1; i < nLabels; i++)
	{
		float larea = stats.at<int>(i, cv::CC_STAT_AREA);
		float ss = perimeter[0];
		float ratios = perimeter[i - 1] / larea;
		//std::std::cout << "perimeter:" << perimeter[i - 1] << "   area:" << larea << "   ratios:" << ratios << endl;
		int temp = ratios * 255;
		colors[i] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
	}

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
	cv::imwrite("widen/diff_c.png", dst);

	Mat output = cv::imread(OutputImage);
	Mat temp = cv::imread("widen/diff_c.png");
	for (int r = 0; r < temp.rows; r++)
	{
		for (int c = 0; c < temp.cols; c++)
		{
			if (temp.at<Vec3b>(r, c)[0] == 0 && temp.at<Vec3b>(r, c)[1] == 0 && temp.at<Vec3b>(r, c)[2] == 0){

			}
			else
			{
				output.at<Vec3b>(r, c)[0] = temp.at<Vec3b>(r, c)[0];
				output.at<Vec3b>(r, c)[1] = temp.at<Vec3b>(r, c)[1];
				output.at<Vec3b>(r, c)[2] = temp.at<Vec3b>(r, c)[2];
			}

		}
	}

	cv::imwrite("widen/visual_classify.png", output);

}
void adaption::visual_classify_jar(const char* inputImage, const char* OutputImage, int  area, float ratio){
	cv::Mat srcImg = imread(inputImage);
	cv::bitwise_xor(srcImg, cv::Scalar(255, 255, 255), srcImg);
	cv::Mat grayy;
	cv::cvtColor(srcImg, grayy, CV_BGR2GRAY);
	//2値化
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (grayy < threshval) : (grayy > threshval);
	//求联通区域
	cv::Mat labelImage(bin.size(), CV_32S);
	Mat stats(bin.size(), CV_32S);
	Mat centroids(bin.size(), CV_32S);
	//int nLabels = cv::connectedComponents(bw, labelImage, 8);
	int nLabels = connectedComponentsWithStats(bin, labelImage, stats, centroids, 4);
	//声明vector,类型为opencv 中的vec3b类型，存放不同联通区域的着色 
	vector<uchar> colors(nLabels);
	//背景黑色
	colors[0] = uchar(0);
	std::cout << "nLabels" << nLabels << endl;

	for (int i = 1; i < nLabels; i++)
	{
		float larea = stats.at<int>(i, cv::CC_STAT_AREA);
		float ss = perimeter[0];
		float ratios = perimeter[i - 1] / larea;
		//std::std::cout << "perimeter:" << perimeter[i - 1] << "   area:" << larea << "   ratios:" << ratios << endl;
		int temp = ratios * 255;
		colors[i] = uchar(temp);
		//std::cout << "color: " << colors[0] << endl;
		
	}

	Mat gray;
	Mat dst(bin.size(), CV_8UC3);
	cv::cvtColor(dst, gray, CV_BGR2GRAY);
	for (int r = 0; r < gray.rows; r++)
	{
		for (int c = 0; c < gray.cols; c++)
		{
			int label = labelImage.at<int>(r, c);
			uchar &pixel = gray.at<uchar>(r, c);
			pixel = colors[label];
		}
	}
	applyColorMap(gray, gray, COLORMAP_JET);
	cv::imwrite("widen/diff_c.png", gray);

	Mat output = cv::imread(OutputImage);
	Mat temp = cv::imread("widen/diff_c.png");
	for (int r = 0; r < temp.rows; r++)
	{
		for (int c = 0; c < temp.cols; c++)
		{
			if (int label = labelImage.at<int>(r, c) == 0){

			}
			else
			{
				output.at<Vec3b>(r, c)[0] = temp.at<Vec3b>(r, c)[0];
				output.at<Vec3b>(r, c)[1] = temp.at<Vec3b>(r, c)[1];
				output.at<Vec3b>(r, c)[2] = temp.at<Vec3b>(r, c)[2];
			}

		}
	}

	cv::imwrite("widen/visual_classify.png", output);

}
void adaption::visual_detection(const char* inputImage, const char* Out) {
	
	Mat input_v = cv::imread(inputImage);
	Mat out = cv::imread("widen/diff.png");
	bool flag = false;

	//加宽
	for (int r = 0; r < out.rows; r++)
	{
		for (int c = 0; c < out.cols; c++)
		{
			if (out.at<Vec3b>(r, c)[1] != 0){
				input_v.at<Vec3b>(r, c)[0] = 255;
				input_v.at<Vec3b>(r, c)[1] = 0;
				input_v.at<Vec3b>(r, c)[2] = 0;
				flag = true;
			}
		}
	}
	imwrite(Out, input_v);
}
void adaption::visual_detection_diff(const char* inputImage, const char* OutputImage, const char* OutputImage2){
	
	cv::Mat srcImg = diff_detection.clone();
	cv::bitwise_xor(srcImg, cv::Scalar(255, 255, 255), srcImg);
	cv::Mat grayy;
	cv::cvtColor(srcImg, grayy, CV_BGR2GRAY);
	//2値化
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (grayy < threshval) : (grayy > threshval);
	//求联通区域
	cv::Mat labelImage(bin.size(), CV_32S);
	Mat stats(bin.size(), CV_32S);
	Mat centroids(bin.size(), CV_32S);
	//int nLabels = cv::connectedComponents(bw, labelImage, 8);
	int nLabels = connectedComponentsWithStats(bin, labelImage, stats, centroids, 4);
	//声明vector,类型为opencv 中的vec3b类型，存放不同联通区域的着色 
	std::vector<cv::Vec3b> colors(nLabels);
	colors[0] = cv::Vec3b(0, 0, 0);
	colors[1] = cv::Vec3b(0, 255, 255);
	colors[2] = cv::Vec3b(0, 0, 255);
	colors[3] = cv::Vec3b(255, 255, 255);

	for (int i = 1; i < nLabels; i++)
	{
		float larea = stats.at<int>(i, cv::CC_STAT_AREA);
		float ss = perimeter[0];
		float ratios = perimeter[i - 1] / larea;
		//std::std::cout << "perimeter:" << perimeter[i - 1] << "   area:" << larea << "   ratios:" << ratios << endl;
		int temp = ratios * 255;
		//colors[i] = cv::Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
	}

	Mat dst(bin.size(), CV_8UC3);
	Mat dst2(bin.size(), CV_8UC3);
	for (int r = 0; r < dst.rows; r++)
	{
		for (int c = 0; c < dst.cols; c++)
		{
			int label = labelImage.at<int>(r, c);
			//std::cout << "label" << label << endl;
			Vec3b &pixel = dst.at<Vec3b>(r, c);
			pixel = colors[0];
			dst2.at<Vec3b>(r, c) = colors[0];
			if (label > 0)
			{
				if (label_id[label - 1l] == 1)
				{
					pixel = colors[2];
					dst2.at<Vec3b>(r, c) = colors[3];
				}
				if (label_id[label - 1l] == 0)
				{
					pixel = colors[1];
				}
			}
		}
	}
	//cv::imwrite("widen/diff_c.png", dst);
	cv::imwrite(OutputImage2, dst2);

	Mat output = cv::imread(inputImage);
	for (int r = 0; r < dst.rows; r++)
	{
		for (int c = 0; c < dst.cols; c++)
		{
			if (dst.at<Vec3b>(r, c)[0] == 0 && dst.at<Vec3b>(r, c)[1] == 0 && dst.at<Vec3b>(r, c)[2] == 0){

			}
			else
			{
				output.at<Vec3b>(r, c)[0] = dst.at<Vec3b>(r, c)[0];
				output.at<Vec3b>(r, c)[1] = dst.at<Vec3b>(r, c)[1];
				output.at<Vec3b>(r, c)[2] = dst.at<Vec3b>(r, c)[2];
			}

		}
	}

	cv::imwrite(OutputImage, output);

	//边界可视化
	//Mat output2 = cv::imread("widen/diff_f.png");
	//Mat temp2 = cv::imread("widen/diff_f.png");
	//for (int r = 0; r < temp2.rows; r++)
	//{
	//	for (int c = 0; c < temp2.cols; c++)
	//	{
	//		output2.at<Vec3b>(r, c)[0] = 0;
	//		output2.at<Vec3b>(r, c)[1] = 0;
	//		output2.at<Vec3b>(r, c)[2] = 0;
	//		if (temp2.at<Vec3b>(r, c)[0] == 0 && temp2.at<Vec3b>(r, c)[1] == 0 && temp2.at<Vec3b>(r, c)[2] == 0){
	//			if (r != 0 && c != 0 && r != temp2.rows - 1 && c != temp2.cols - 1) {
	//				if (temp2.at<Vec3b>(r + 1, c)[0] == 255 || temp2.at<Vec3b>(r - 1, c)[0] == 255 || temp2.at<Vec3b>(r, c + 1)[0] == 255 || temp2.at<Vec3b>(r, c - 1)[0] == 255 || temp2.at<Vec3b>(r + 1, c - 1)[0] == 255 || temp2.at<Vec3b>(r - 1, c - 1)[0] == 255 || temp2.at<Vec3b>(r - 1, c + 1)[0] == 255 || temp2.at<Vec3b>(r + 1, c + 1)[0] == 255){
	//					output2.at<Vec3b>(r, c)[0] = 255;
	//					output2.at<Vec3b>(r, c)[1] = 255;
	//					output2.at<Vec3b>(r, c)[2] = 255;
	//				}
	//			}
	//		}

	//	}
	//}
	//cv::imwrite("widen/diff_of.png", output2);

}
bool adaption::treatment(const char* inputImage, const char* inputImage2, const char* OutputImage) {


	Mat diffimg = cv::imread("widen/diff.png");
	Mat input, inter, out;
	Mat element2 = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(diffimg, inter, element2);
	cvtColor(inter, inter, CV_BGR2GRAY);
	threshold(inter, inter, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cv::imwrite("widen/diff_dilate.png", inter);

	input = cv::imread(inputImage);
	Mat input_v = cv::imread(inputImage);
	Mat wide = cv::imread("widen/diff_dilate.png");
	bool flag = false;

	//加宽
	for (int r = 0; r < wide.rows; r++)
	{
		for (int c = 0; c < wide.cols; c++)
		{
			if (wide.at<Vec3b>(r, c)[1] != 0){

				input.at<Vec3b>(r, c)[0] = 255;
				input.at<Vec3b>(r, c)[1] = 255;
				input.at<Vec3b>(r, c)[2] = 255;

				input_v.at<Vec3b>(r, c)[0] = 0;
				input_v.at<Vec3b>(r, c)[1] = 0;
				input_v.at<Vec3b>(r, c)[2] = 255;
				flag = true;
			}
		}
	}
	stringstream ss_v;
	ss_v.str("");
	string visualimage = ss_v.str();
	cv::imwrite(OutputImage, input);
	cv::imwrite(visualimage, input_v);
	return flag;
}
bool adaption::get_conflict(const char* inputImage, const char* inputImage2, const char* inputImage3, const char* DW, const char* DB, int n){

	Mat imgw_type = imread(inputImage2);
	Mat imgb_type = imread(inputImage3);
	Mat out_conflict = imread(inputImage);
	std::vector<cv::Vec3b> colors(7);
	colors[0] = cv::Vec3b(0, 0, 0);
	colors[1] = cv::Vec3b(255, 255, 255);
	colors[2] = cv::Vec3b(0, 0, 255); //R
	colors[3] = cv::Vec3b(0, 255, 0); //G
	colors[4] = cv::Vec3b(255, 0, 0); //B
	colors[5] = cv::Vec3b(0, 255, 255); //Y
	colors[6] = cv::Vec3b(0, 255, 255); //Y
	//get label_w
    //std::cout << "开始装弹" << endl;

	for (int i = 0; i < imgw_type.rows; i++)
	{
		vector<int> temp;
		for (int j = 0; j < imgw_type.cols; j++){
			temp.push_back(0);
		}
		label_w.push_back(temp);
		label_b.push_back(temp);
		label_wc.push_back(temp);
		label_bc.push_back(temp);
		label_c.push_back(temp);
		temp.clear();
	}

	bool havediff = false;
	for (int i = 0; i < imgw_type.rows; i++)
	{
		vector<int> temp;
		for (int j = 0; j < imgw_type.cols; j++)
		{
			//std::cout << imgw_type.at<Vec3b>(i, j) << endl;
			if (imgw_type.at<Vec3b>(i, j)[0] == 0 && imgw_type.at<Vec3b>(i, j)[1] == 0 && imgw_type.at<Vec3b>(i, j)[2] == 255)
			{
				label_w[i][j] = 1;
				havediff = true;
				//std::cout << "get B" << endl;
			}
			if (imgw_type.at<Vec3b>(i, j)[0] == 0 && imgw_type.at<Vec3b>(i, j)[1] == 255 && imgw_type.at<Vec3b>(i, j)[2] == 255)
			{
				label_w[i][j] = 1;
				havediff = true;
				//std::cout << "get A" << endl;
			}
		}
	}

	for (int i = 0; i < imgb_type.rows; i++)
	{
		vector<int> temp;
		for (int j = 0; j < imgb_type.cols; j++)
		{
			if (imgb_type.at<Vec3b>(i, j)[0] == 0 && imgb_type.at<Vec3b>(i, j)[1] == 0 && imgb_type.at<Vec3b>(i, j)[2] == 255)
			{
				label_b[i][j] = 1;
				havediff = true;
			}
			if (imgb_type.at<Vec3b>(i, j)[0] == 0 && imgb_type.at<Vec3b>(i, j)[1] == 255 && imgb_type.at<Vec3b>(i, j)[2] == 255)
			{
				label_b[i][j] = 1;
				havediff = true;
			}
		}
	}

	//std::cout << "装弹完成" << endl;
	////先处理白色 垂直检测 和 水平检测 
	Mat diff_w = imread(DW);
	Dw = diff_w.clone();
	cv::Mat labelImage(diff_w.size(), CV_32S);
	cv::cvtColor(diff_w, diff_w, CV_BGR2GRAY);
	int threshval = 100;
	diff_w = threshval < 128 ? (diff_w < threshval) : (diff_w > threshval);
	int nLabels = connectedComponents(diff_w, labelImage, 4);

	//std::cout << "初始计数完成" << endl;

	for (int i = 0; i < imgw_type.rows; i++)
	{
		vector<int> temp;
		for (int j = 0; j < imgw_type.cols; j++)
		{
			int cur_lable = labelImage.at<int>(i, j);
			if (label_w[i][j] == 1){
				//垂直向下检测
				if (label_b[i + 1][j] == 1)
				{
					//向上填充 最多填充到阈值大小
					int index = i;
					int mark = 0;
					label_wc[index][j] = 1;
					//std::cout << "get border" << endl;
					//out_conflict.at<Vec3b>(index, j) = colors[2];

					while (label_w[index - 1][j] == 1)
					{
						mark++;
						if (mark > kernel)
							break;

						label_wc[index - 1][j] = 1;
						//out_conflict.at<Vec3b>(index - 1, j) = colors[2];
						index--;
						//std::cout << "get conflict" << endl;
					}
				}
			
				//垂直向上检测
				if (label_b[i - 1][j] == 1)
				{
					//向下填充 最多填充到阈值大小
					int index = i;
					int mark = 0;
					label_wc[index][j] = 1;
					//std::cout << "get border" << endl;
					//out_conflict.at<Vec3b>(index, j) = colors[2];

					while (label_w[index + 1][j] == 1)
					{
						mark++;
						if (mark > kernel)
							break;

						label_wc[index + 1][j] = 1;
						//out_conflict.at<Vec3b>(index + 1, j) = colors[2];
						index ++;
						//std::cout << "get conflict" << endl;
					}
				}
			
				//水平向左检测
				if (label_b[i][j - 1] == 1)
				{
					//向右填充 最多填充到阈值大小
					int index = j;
					int mark = 0;
					label_wc[i][index] = 1;
					//std::cout << "get border" << endl;
					//out_conflict.at<Vec3b>(i, index) = colors[2];

					while (label_w[i][index + 1] == 1)
					{
						mark++;
						if (mark > kernel)
							break;

						label_wc[i][index + 1] = 1;
						//out_conflict.at<Vec3b>(i, index + 1) = colors[2];
						index ++;
						//std::cout << "get conflict" << endl;
					}
				}
			
				//水平向右检测
				if (label_b[i][j + 1] == 1)
				{
					//向左填充 最多填充到阈值大小
					int index = j;
					int mark = 0;
					label_wc[i][index] = 1;
					//std::cout << "get border" << endl;
					//out_conflict.at<Vec3b>(i, index) = colors[2];

					while (label_w[i][index-1] == 1)
					{
						mark++;
						if (mark > kernel)
							break;

						label_wc[i][index-1] = 1;
						//out_conflict.at<Vec3b>(i, index-1) = colors[2];
						index --;
						//std::cout << "get conflict" << endl;
					}
				}
			}
		}
	}

	
	////接着处理黑色 垂直检测 和 水平检测 
	Mat diff_b = imread(DB);
	Db = diff_b.clone();
	cv::Mat labelImage2(diff_b.size(), CV_32S);
	cv::cvtColor(diff_b, diff_b, CV_BGR2GRAY);
	int threshvalb = 100;
	diff_b = threshvalb < 128 ? (diff_b < threshvalb) : (diff_b > threshvalb);
	int nLabels2 = connectedComponents(diff_b, labelImage2, 4);

	//std::cout << "初始计数完成" << endl;

	for (int i = 0; i < imgb_type.rows; i++)
	{
		vector<int> temp;
		for (int j = 0; j < imgb_type.cols; j++)
		{
			int cur_lable = labelImage2.at<int>(i, j);
			//确定 需要水平填充
			if (label_b[i][j] == 1){

				//垂直向下检测
				if (label_w[i + 1][j] == 1)
				{
					//向上填充
					int index = i;
					int mark = 0;
					label_bc[index][j] = 1;
					//std::cout << "get border" << endl;
					//out_conflict.at<Vec3b>(index, j) = colors[2];

					while (label_b[index - 1][j] == 1)
					{
						mark++;
						if (mark > kernel)
							break;

						label_bc[index - 1][j] = 1;
						//out_conflict.at<Vec3b>(index - 1, j) = colors[2];
						index--;
						//std::cout << "get conflict" << endl;
					}
				}

				//垂直向上检测
				if (label_w[i - 1][j] == 1)
				{
					//向下填充
					int index = i;
					int mark = 0;
					label_bc[index][j] = 1;
					//std::cout << "get border" << endl;
					//out_conflict.at<Vec3b>(index, j) = colors[2];

					while (label_b[index + 1][j] == 1)
					{
						mark++;
						if (mark > kernel)
							break;

						label_bc[index + 1][j] = 1;
						//out_conflict.at<Vec3b>(index + 1, j) = colors[2];
						index++;
						//std::cout << "get conflict" << endl;
					}
				}

				//水平向左检测
				if (label_w[i][j - 1] == 1)
				{
					//向上填充
					int index = j;
					int mark = 0;
					label_bc[i][index] = 1;
					//std::cout << "get border" << endl;
					//out_conflict.at<Vec3b>(i, index) = colors[2];

					while (label_b[i][index + 1] == 1)
					{
						mark++;
						if (mark > kernel)
							break;

						label_bc[i][index + 1] = 1;
						//out_conflict.at<Vec3b>(i, index + 1) = colors[2];
						index++;
						//std::cout << "get conflict" << endl;
					}
				}

				//水平检测
				if (label_w[i][j + 1] == 1)
				{

					int index = j;
					int mark = 0;
					label_bc[i][index] = 1;
					//std::cout << "get border" << endl;
					//out_conflict.at<Vec3b>(i, index) = colors[2];

					while (label_b[i][index - 1] == 1)
					{
						mark++;
						if (mark > kernel)
							break;

						label_bc[i][index - 1] = 1;
						//out_conflict.at<Vec3b>(i, index-1) = colors[2];
						index--;
						//std::cout << "get conflict" << endl;
					}
				}
			}
		}
	}

	//std::cout << "检测完成" << endl;
	//visual
	for (int i = 0; i < imgw_type.rows; i++)
	{
		for (int j = 0; j < imgw_type.cols; j++)
		{
			/*if (label_b[i][j] == 1)
			{
				out_conflict.at<Vec3b>(i, j) = colors[4];
			}
			if (label_w[i][j] == 1)
			{
				out_conflict.at<Vec3b>(i, j) = colors[5];
			}*/
			if (label_wc[i][j] == 1)
			{
				out_conflict.at<Vec3b>(i, j) = colors[2];
			}
			if (label_bc[i][j] == 1)
			{
				out_conflict.at<Vec3b>(i, j) = colors[2];
			}
		}
	}
	//std::cout << "冲突区域可视化完成" << endl;
	visualconfliction = out_conflict;
	cv::imwrite("data/adaption/out-conflict.png", out_conflict);
	return havediff;
}
void adaption::get_conflict_weight(){

	Mat conflict = visualconfliction.clone();
	Mat output(conflict.size(), CV_8UC3);

	//造盒子
	std::vector<cv::Vec3b> colors(7);
	colors[0] = cv::Vec3b(0, 0, 0);
	colors[1] = cv::Vec3b(255, 255, 255);
	colors[2] = cv::Vec3b(0, 0, 255); //R
	colors[3] = cv::Vec3b(0, 255, 0); //G
	colors[4] = cv::Vec3b(255, 0, 0); //B
	colors[5] = cv::Vec3b(0, 255, 255); //Y
	colors[6] = cv::Vec3b(0, 255, 255); //Y
	for (int i = 0; i < conflict.rows; i++)
	{
		vector<int> temp;
		for (int j = 0; j < conflict.cols; j++){
			temp.push_back(0);
		}
		label_c.push_back(temp);
		temp.clear();
	}

	for (int i = 0; i < conflict.rows; i++)
	{
		for (int j = 0; j < conflict.cols; j++)
		{
			if (0 == conflict.at<Vec3b>(i, j)[0] && 0 == conflict.at<Vec3b>(i, j)[1] && 255 == conflict.at<Vec3b>(i, j)[2]){
				output.at<Vec3b>(i, j) = colors[1];
				label_c[i][j] = 1;
			}
			else
			{
				output.at<Vec3b>(i, j) = colors[0];
			}
		}
	}

	Confliction = output;
	imwrite("temp/diff_conflict.png", Confliction);

	//std::cout << "初始化冲突区域完成" << endl;

	cv::bitwise_xor(output, cv::Scalar(255, 255, 255), output);
	cv::Mat gray;

	cv::cvtColor(output, gray, CV_BGR2GRAY);

	//2値化
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (gray < threshval) : (gray > threshval);
	//
	//求联通区域
	cv::Mat labelImage(bin.size(), CV_32S);

	int nLabels = connectedComponents(bin, labelImage, 4);

	for (int i = 0; i < nLabels; i++)
	{ 
		label_weight.push_back(0);
	}
	for (int i = 0; i < output.rows; i++)
	{
		for (int j = 0; j < output.cols; j++)
		{
			int cur_lable = labelImage.at<int>(i, j);
			if (label_b[i][j] == 1)
			{
				label_weight[cur_lable] --;
			}
			if (label_w[i][j] == 1)
			{
				label_weight[cur_lable] ++;
			}
		}
	}
	//for (int i = 0; i < label_weight.size(); i++)
	//{
	//	cout <<i<< " ： " << label_weight[i] << endl;
	//}
}
void adaption::treatment_conflict(const char* inputImage, int num1, int num2) {
	
	Mat conflict_label = Confliction.clone();
	std::vector<cv::Vec3b> colors(7);
	colors[0] = cv::Vec3b(0, 0, 0);
	colors[1] = cv::Vec3b(255, 255, 255);
	colors[2] = cv::Vec3b(0, 0, 255); //R
	colors[3] = cv::Vec3b(0, 255, 0); //G
	colors[4] = cv::Vec3b(255, 0, 0); //B
	colors[5] = cv::Vec3b(0, 255, 255); //Y
	colors[6] = cv::Vec3b(0, 255, 255); //Y
	
	cv::Mat grayy;
	cv::cvtColor(conflict_label, grayy, CV_BGR2GRAY);
	
	//2値化
	int threshval = 100;
	cv::Mat binn = threshval < 128 ? (grayy < threshval) : (grayy > threshval);
	//求联通区域
	cv::Mat labelImage(binn.size(), CV_32S);
	int nLabels = connectedComponents(binn, labelImage, 4);
	Mat out = imread(inputImage);
	//cv::imwrite("temp/inputImage.png", out);

	//处理冲突区域
	for (int i = 0; i < out.rows; i++)
	{
		for (int j = 0; j < out.cols; j++)
		{
			int cur_lable = labelImage.at<int>(i, j);
			if (cur_lable >= 1)
			{
				if (label_weight[cur_lable]>0){
					out.at<Vec3b>(i, j) = colors[1];
				}
				if (label_weight[cur_lable]<0)
				{
					out.at<Vec3b>(i, j) = colors[0];
				}
				if (label_weight[cur_lable] == 0){
					out.at<Vec3b>(i, j) = colors[0];
					//out.at<Vec3b>(i, j) = rand() % 2 == 1 ? colors[0] : colors[1];
				}
			}
		}
	}
	//处理非冲突区域
	treat_con = out;
	//cv::imwrite("temp/treat_con.png", treat_con);

	//visaul diff not conflict type A B
	Mat diff_w_ucAB = Dw.clone();
	Mat diff_b_ucAB = Db.clone();

	for (int i = 0; i < diff_b_ucAB.rows; i++)
	{
		for (int j = 0; j < diff_b_ucAB.cols; j++)
		{
			diff_w_ucAB.at<Vec3b>(i, j) = colors[0];
			diff_b_ucAB.at<Vec3b>(i, j) = colors[0];

			if (label_b[i][j] == 1)
			{
				diff_b_ucAB.at<Vec3b>(i, j) = colors[1];
				if (label_c[i][j] == 1)
				{
					diff_b_ucAB.at<Vec3b>(i, j) = colors[0];
				}
			}
			

			if (label_w[i][j] == 1)
			{
				diff_w_ucAB.at<Vec3b>(i, j) = colors[1];
				if (label_c[i][j] == 1)
				{
					diff_w_ucAB.at<Vec3b>(i, j) = colors[0];
				}
			}
			
		}
	}
	//imwrite("temp/diff_w_uc.png", diff_w_ucAB);
	//imwrite("temp/diff_b_uc.png", diff_b_ucAB);

	//remove nosie in Dw_uc
	cv::bitwise_xor(diff_w_ucAB, cv::Scalar(255, 255, 255), diff_w_ucAB);
	cv::Mat gray;
	cv::cvtColor(diff_w_ucAB, gray, CV_BGR2GRAY);
	//2値化
	cv::Mat bin = threshval < 128 ? (gray < threshval) : (gray > threshval);
	//求联通区域
	cv::Mat labelImagew(bin.size(), CV_32S);
	Mat stats(bin.size(), CV_32S);
	Mat centroids(bin.size(), CV_32S);
	//int nLabels = cv::connectedComponents(bw, labelImage, 8);
	int nLabelsw = connectedComponentsWithStats(bin, labelImagew, stats, centroids, 4);
	//声明vector,类型为opencv 中的vec3b类型，存放不同联通区域的着色 
	vector<Vec3b> colorsw(nLabelsw);
	//背景白色
	colorsw[0] = Vec3b(255, 255, 255);
	//std::cout << "nLabels" << nLabels << endl;

	for (int label = 1; label < nLabelsw; label++)
	{
		float area = stats.at<int>(label, cv::CC_STAT_AREA);
		if (area < num1)
		{
			colorsw[label] = Vec3b(255, 255, 255);
		}
	}
	//dst存储最终上色的标记图像
	Mat dst(bin.size(), CV_8UC3);
	for (int r = 0; r < dst.rows; r++)
	{
		for (int c = 0; c < dst.cols; c++)
		{
			int label = labelImagew.at<int>(r, c);
			Vec3b &pixel = dst.at<Vec3b>(r, c);
			pixel = colorsw[label];
		}
	}
	cv::bitwise_xor(dst, cv::Scalar(255, 255, 255), dst);
	
	//remove nosie in Db_uc
	cv::bitwise_xor(diff_b_ucAB, cv::Scalar(255, 255, 255), diff_b_ucAB);
	cv::Mat grayb;
	cv::cvtColor(diff_b_ucAB, grayb, CV_BGR2GRAY);
	//2値化
	cv::Mat binb = threshval < 128 ? (grayb < threshval) : (grayb > threshval);
	//求联通区域
	cv::Mat labelImageb(binb.size(), CV_32S);
	Mat statsb(binb.size(), CV_32S);
	Mat centroidsb(binb.size(), CV_32S);
	//int nLabels = cv::connectedComponents(bw, labelImage, 8);
	int nLabelsb = connectedComponentsWithStats(binb, labelImageb, statsb, centroidsb, 4);
	//声明vector,类型为opencv 中的vec3b类型，存放不同联通区域的着色 
	vector<Vec3b> colorsb(nLabelsb);
	//背景白色
	colorsb[0] = Vec3b(255, 255, 255);
	//std::cout << "nLabels" << nLabels << endl;

	for (int label = 1; label < nLabelsb; label++)
	{
		float area = statsb.at<int>(label, cv::CC_STAT_AREA);
		if (area < num2)
		{
			colorsb[label] = Vec3b(255, 255, 255);
		}
	}
	//dst存储最终上色的标记图像
	Mat dstb(binb.size(), CV_8UC3);
	for (int r = 0; r < dstb.rows; r++)
	{
		for (int c = 0; c < dstb.cols; c++)
		{
			int label = labelImageb.at<int>(r, c);
			Vec3b &pixel = dstb.at<Vec3b>(r, c);
			pixel = colorsb[label];
		}
	}
	cv::bitwise_xor(dstb, cv::Scalar(255, 255, 255), dstb);

	//imwrite("temp/diff_w_uc_r.png", dst);
	//imwrite("temp/diff_b_uc_r.png", dstb);

	Dw_uc = dst;
	Db_uc = dstb;
}
void adaption::treatment_unconflict(const char* inputImage, const char* inputImage2, const char* OutputImage) {

	//去掉非常小的区域

	//处理白色
	Mat diffimg = cv::imread("widen/diff_w_uc.png");
	Mat input, inter, out;
	Mat element2 = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(diffimg, inter, element2);
	cvtColor(inter, inter, CV_BGR2GRAY);
	threshold(inter, inter, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cv::imwrite("widen/diff_w_uc_dilate.png", inter);

	input = cv::imread(inputImage);
	Mat input_v = cv::imread(inputImage);
	Mat wide = cv::imread("widen/diff_w_uc_dilate.png");
	bool flag = false;

	//加宽
	for (int r = 0; r < wide.rows; r++)
	{
		for (int c = 0; c < wide.cols; c++)
		{
			if (wide.at<Vec3b>(r, c)[1] != 0){

				input.at<Vec3b>(r, c)[0] = 255;
				input.at<Vec3b>(r, c)[1] = 255;
				input.at<Vec3b>(r, c)[2] = 255;

				//input_v.at<Vec3b>(r, c)[0] = 0;
				//input_v.at<Vec3b>(r, c)[1] = 0;
				//input_v.at<Vec3b>(r, c)[2] = 255;
				flag = true;
			}
		}
	}
	stringstream ss_v;
	ss_v.str("");
	string visualimage = ss_v.str();
	cv::imwrite(OutputImage, input);
	//cv::imwrite(visualimage, input_v);

	//处理黑色
	Mat diffimg_b = cv::imread("widen/diff_b_uc.png");
	Mat input_b, inter_b, out_b;
	Mat element2_b = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(diffimg_b, inter_b, element2_b);
	cvtColor(inter_b, inter_b, CV_BGR2GRAY);
	threshold(inter_b, inter_b, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cv::imwrite("widen/diff_b_uc_dilate.png", inter_b);

	input_b = cv::imread(inputImage);
	Mat wide_b = cv::imread("widen/diff_b_uc_dilate.png");
	bool flag_b = false;

	//加宽
	for (int r = 0; r < wide_b.rows; r++)
	{
		for (int c = 0; c < wide_b.cols; c++)
		{
			if (wide_b.at<Vec3b>(r, c)[1] != 0){

				input.at<Vec3b>(r, c)[0] = 0;
				input.at<Vec3b>(r, c)[1] = 0;
				input.at<Vec3b>(r, c)[2] = 0;
				flag_b = true;
			}
		}
	}

	/*stringstream ss_v;
	ss_v.str("");
	string visualimage = ss_v.str();*/

	cv::imwrite(OutputImage, input);
}

void adaption::treatment_unconflict_x(const char* OutputImage) {

	//处理白色
	Mat diffimg = Dw_uc.clone();
	Mat input, inter, out;
	Mat element2 = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(diffimg, inter, element2);
	input = treat_con.clone();

	//加宽
	for (int r = 0; r < inter.rows; r++)		
	{
		for (int c = 0; c < inter.cols; c++)
		{
			if (inter.at<Vec3b>(r, c)[0] == 255){
				input.at<Vec3b>(r, c)[0] = 255;
				input.at<Vec3b>(r, c)[1] = 255;
				input.at<Vec3b>(r, c)[2] = 255;
			}
		}
	}
	//处理黑色
	Mat diffimg_b = Db_uc.clone();
	Mat inter_b;
	Mat element2_b = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(diffimg_b, inter_b, element2_b);
	//加宽
	for (int r = 0; r < inter_b.rows; r++)
	{
		for (int c = 0; c < inter_b.cols; c++)
		{
			if (inter_b.at<Vec3b>(r, c)[0] == 255){
				input.at<Vec3b>(r, c)[0] = 0;
				input.at<Vec3b>(r, c)[1] = 0;
				input.at<Vec3b>(r, c)[2] = 0;
			}
		}
	}
	cv::imwrite(OutputImage, input);
}

void adaption::treatment_optimize(){
	string img1 = "widen/out-adaption";
	string style = "png";
	//accuracy
	int tiny_r = 150;
	//inaccuracy!!!
	float ratio_r = 0.8;
	stringstream ss11;
	ss11.str("");
	string filename11("widen/out-adaption");
	string fileformat11(".png");
	ss11 << filename11 << fileformat11;
	string inputimage11 = ss11.str();
	optimize pixel2;
	int n_ = pixel2.get_black_lable(inputimage11.c_str());
	pixel2.get_all_boundary(n_);// add perimeter
	std::cout << "n: " << n_ << endl;
	pixel2.remove_lable(img1, style, tiny_r);
	cv::Mat result = imread("widen/out-adaption_r.png", cv::IMREAD_COLOR);
	cv::bitwise_xor(result, cv::Scalar(255, 255, 255), result);
	cv::imwrite("widen/out-adaption_r.png", result);
	string img2 = "widen/out-adaption_r";
	string style2 = "png";
	//accuracy
	int tiny2 = 150;
	//inaccuracy!!!
	float ratio2 = 0.8;
	stringstream ss12;
	ss12.str("");
	string filename12("widen/out-adaption_r");
	string fileformat12(".png");
	ss12 << filename12 << fileformat11;
	string inputimage12 = ss12.str();
	optimize pixel;
	int nn = pixel.get_black_lable(inputimage12.c_str());
	pixel.get_all_boundary(nn);// add perimeter
	std::cout << "n: " << nn << endl;
	pixel.remove_lable(img2, style2, tiny2);
	cv::Mat result_out = imread("widen/out-adaption_r_r.png", cv::IMREAD_COLOR);
	cv::bitwise_xor(result_out, cv::Scalar(255, 255, 255), result_out);
	cv::imwrite("widen/out-adaption_0.png", result_out);
}
