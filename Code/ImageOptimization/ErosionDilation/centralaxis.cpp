#include "stdafx.h"
#include "centralaxis.h"


centralaxis::centralaxis()
{
}


centralaxis::~centralaxis()
{
}

void centralaxis::disboufield(const char* inputImage, const char* inputImage2, const char* outImage) {

	Mat src = imread(inputImage,CV_8U);
	//cv::bitwise_xor(src, cv::Scalar(255, 255, 255), src);
	vector<vector<Point> > contours; 
	vector<Vec4i> hierarchy;

	//Mat drawing = imread(inputImage2, CV_8UC3);
	Mat out = imread(inputImage2);
	D_B = out.clone();
	//bitwise_xor(out, Scalar(255, 255, 255), out);

	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	Mat Laplacian;
	cv::cvtColor(drawing, Laplacian, CV_BGR2GRAY);
	Mat Laplacianed;
	cv::cvtColor(drawing, Laplacianed, CV_BGR2GRAY);
	vector<uchar> colors(2);
	colors[0] = uchar(0);

	findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	Mat raw_dist_l_all(src.size(), CV_32FC1);
	Mat raw_dist_all(src.size(), CV_32FC1);
	for (int j = 0; j < src.rows; j++)
	{
		for (int i = 0; i < src.cols; i++)
		{
			raw_dist_l_all.at<float>(j, i) = -1;
			raw_dist_all.at<float>(j, i) = -1;
		}
	}

	for (int index = 0; index < contours.size(); index++)
	{
		//计算顶点到轮廓的距离
		Mat raw_dist(src.size(), CV_32FC1);
		Mat raw_dist_l(src.size(), CV_32FC1);
		
		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				raw_dist.at<float>(j, i) = pointPolygonTest(contours[index], Point2f(i, j), true);
				raw_dist_l.at<float>(j, i) = pointPolygonTest(contours[index], Point2f(i, j), true);
			}
		}

		double minVal; double maxVal;
		minMaxLoc(raw_dist, &minVal, &maxVal, 0, 0, Mat());
		minVal = abs(minVal); maxVal = abs(maxVal);

		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				//在外部
				if (raw_dist.at<float>(j, i) < 0)
				{
				}
				//在内部
				else if (raw_dist.at<float>(j, i) >= 0)
				{

					float k = raw_dist.at<float>(j, i);
					raw_dist_all.at<float>(j, i) = k / maxVal;

					float temp = (4 * k - (raw_dist.at<float>(j + 1, i) + raw_dist.at<float>(j - 1, i) + raw_dist.at<float>(j, i + 1) + raw_dist.at<float>(j, i - 1))) / maxVal;

					raw_dist_l.at<float>(j, i) = temp;
					raw_dist_l_all.at<float>(j, i) = temp;

				}
				else
				{
					raw_dist_l_all.at<float>(j, i) = 0;
					raw_dist_all.at<float>(j, i) = 0;
				}
			}
		}

		//用户型化的方式显示距离

		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				//在外部
				if (raw_dist.at<float>(j, i) < 0)
				{
					//drawing.at<Vec3b>(j, i)[0] = 255 - (int)abs(raw_dist.at<float>(j, i)) * 255 / minVal;
				}
				//在内部
				else if (raw_dist.at<float>(j, i) >= 0)
				{
					drawing.at<Vec3b>(j, i)[2] = 255 - (int)raw_dist.at<float>(j, i) * 255 / maxVal;

					out.at<Vec3b>(j, i)[0] = 0;
					out.at<Vec3b>(j, i)[1] = 0;
					out.at<Vec3b>(j, i)[2] = 255 - (int)raw_dist.at<float>(j, i) * 255 / maxVal;
					
					float gary = (int)raw_dist.at<float>(j, i) * 255 / maxVal;

					colors[1] = uchar(gary);
					//Laplacian.at<uchar>(j, i) = colors[1];
					uchar &pixel = Laplacian.at<uchar>(j, i);
					pixel = colors[1];
				}
				else
					// 在边上
				{
					drawing.at<Vec3b>(j, i)[0] = 255; drawing.at<Vec3b>(j, i)[1] = 255; drawing.at<Vec3b>(j, i)[2] = 255;
					out.at<Vec3b>(j, i)[0] = 0;
					out.at<Vec3b>(j, i)[1] = 0;
					out.at<Vec3b>(j, i)[2] = 255;
					uchar &pixel = Laplacian.at<uchar>(j, i);
					pixel = colors[0];
				}
			}
		}

		double minVal_; double maxVal_;
		minMaxLoc(raw_dist_l, &minVal_, &maxVal_, 0, 0, Mat());
		minVal_ = abs(minVal_); maxVal_ = abs(maxVal_);

		//cout << "maxVal_" << maxVal_ << endl;

		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				//在外部
				if (raw_dist_l.at<float>(j, i) < 0)
				{
					//drawing.at<Vec3b>(j, i)[0] = 255 - (int)abs(raw_dist.at<float>(j, i)) * 255 / minVal;
				}
				//在内部
				else if (raw_dist_l.at<float>(j, i) > 0)
				{
					//灰度
					int gray = raw_dist_l.at<float>(j, i) * 255 / maxVal_;
					colors[1] = uchar(gray);
					//Laplacian.at<uchar>(j, i) = colors[1];
					uchar &pixel = Laplacianed.at<uchar>(j, i);
					pixel = colors[1];
				}
				else
				// 在边上
				{
					uchar &pixel = Laplacianed.at<uchar>(j, i);
					pixel = colors[0];
				}
			}
		}
	}

	//imwrite("widen/diff_field.png", drawing);
	//灰度图像
	//imwrite("widen/diff_Lfield.png", Laplacian);
	//laplacianed图像 
	imwrite(outImage, Laplacianed);
	//imwrite("widen/visual_field.png", out);

	////area
	//Mat bin = imread(inputImage, CV_8U);
	//for (int i = 0; i < bin.rows; i++)
	//{
	//	for (int j = 0; j < bin.cols; j++)
	//	{
	//			bin.at<Vec3b>(i, j)[0] = 255;
	//			bin.at<Vec3b>(i, j)[1] = 255;
	//			bin.at<Vec3b>(i, j)[2] = 255;
	//	}
	//}
	//Ledfield = bin.clone();
	
}
void centralaxis::visual_field(const char* inputImage) {
	//距离场可视化
	Mat src = imread(inputImage);
	Mat field = imread("widen/diff_field.png");
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (field.at<Vec3b>(i,j)[0] != 0)
			{
				src.at<Vec3b>(i, j)[0] = field.at<Vec3b>(i, j)[0];
				src.at<Vec3b>(i, j)[1] = field.at<Vec3b>(i, j)[1];
				src.at<Vec3b>(i, j)[2] = field.at<Vec3b>(i, j)[2];
			}
		}
	}
	imwrite("widen/visual_field.png", src);
}
void centralaxis::visual_field_Laplacian(const char* inputImage, const char* inputImage2) {
	
	Mat out_Lap = imread(inputImage);
	Mat out_axis = imread(inputImage);
	Mat img_Ledfield = imread(inputImage2);
	//Laplacianed可视化
	//Mat img_diff_f = D_B.clone();
	/*for (int i = 0; i < img_diff_f.rows; i++)
	{
		for (int j = 0; j < img_diff_f.cols; j++)
		{
			if (img_diff_f.at<Vec3b>(i, j)[0] != 0)
			{
				out_Lap.at<Vec3b>(i, j)[0] = img_Ledfield.at<Vec3b>(i, j)[0];
				out_Lap.at<Vec3b>(i, j)[1] = img_Ledfield.at<Vec3b>(i, j)[1];
				out_Lap.at<Vec3b>(i, j)[2] = img_Ledfield.at<Vec3b>(i, j)[2];
			}
		}
	}
	imwrite("widen/out_Laplacian.png", out_Lap);*/

	//中轴可视化

	cvtColor(img_Ledfield, img_Ledfield, CV_BGR2GRAY);
	int threshval = 60;
	cv::Mat bin_ = threshval < 65 ? (img_Ledfield < threshval) : (img_Ledfield > threshval);
	Axis = bin_;
	imwrite("data/adaption/axis_threshold.png", bin_);

	/*Mat bin = imread("widen/axis_threshold.png");
	for (int i = 0; i < bin.rows; i++)
	{
		for (int j = 0; j < bin.cols; j++)
		{
			if (bin.at<Vec3b>(i, j)[0] == 255)
			{
				out_axis.at<Vec3b>(i, j)[0] = 255;
				out_axis.at<Vec3b>(i, j)[1] = 255;
				out_axis.at<Vec3b>(i, j)[2] = 255;
			}
		}
	}*/

	//imwrite("data/adaption/out_axis.png", out_axis);

	//area
	//imwrite("data/adaption/axis_threshold.png", Ledfield);
}
void centralaxis::visual_classify_BC(const char* inputImage,  const char* inputImage3, const char* outImage, int num) {
	Mat out_ABC = imread(inputImage);
	Mat labels = imread(inputImage3);
	Mat labelBC = imread(inputImage3);
	Mat axis = imread("data/adaption/axis_threshold.png");
	//找到 BC的 label
	cv::Mat gray;
	cv::cvtColor(labelBC, gray, CV_BGR2GRAY);
	//2値化
	int threshval = 100;
	cv::Mat bin = threshval < 128 ? (gray < threshval) : (gray > threshval);
	//反相处理：
	cv::bitwise_xor(bin, cv::Scalar(255, 255, 255), bin);
	//求联通区域
	cv::Mat labelImage(bin.size(), CV_32S);
	int nLabels = connectedComponents(bin, labelImage, 4);
	
	//cout << "nLabels： " << nLabels << endl;
	//labelBC_weight 摊大饼
	for (int i = 0; i < nLabels; i++)
	{
		labelBC_weight.push_back(0);
	}
	//加酱料
	for (int i = 0; i < labelBC.rows; i++)
	{
		for (int j = 0; j < labelBC.cols; j++)
		{
			// 255 area 0 axis
			if (labelBC.at<Vec3b>(i, j)[0] == 255)
			{
				int lable_id = labelImage.at<int>(i, j);
				if (axis.at<Vec3b>(i, j)[0] == 255){
					labelBC_weight[lable_id] += 1;
				}
			}
		}
	}

	/*for (int i = 0; i < labelBC_weight.size(); i++)
	{
		cout << labelBC_weight[i] << endl;
	}*/

	for (int i = 0; i < out_ABC.rows; i++)
	{
		for (int j = 0; j < out_ABC.cols; j++)
		{
			int lable_id = labelImage.at<int>(i, j);
			if (lable_id > 0)
			{
				if (labelBC_weight[lable_id] <= num)
				{
					out_ABC.at<Vec3b>(i, j)[0] = 0;
					out_ABC.at<Vec3b>(i, j)[1] = 255;
					out_ABC.at<Vec3b>(i, j)[2] = 0;
				}
			}
		}
	}
	imwrite(outImage, out_ABC);
}
void centralaxis::visual_classify_ABC(const char* inputImage, const char* inputImage2, const char* outImage) {
	Mat labelBC_w = imread(inputImage);
	Mat labelBC_b = imread(inputImage2);
	Mat labelABC = imread(inputImage2);
	Mat labelAB = imread(inputImage2);

	////为了可视化改了一下颜色 将TypeB由 红色变成 紫色
	for (int i = 0; i < labelBC_w.rows; i++)
	{
		for (int j = 0; j < labelBC_w.cols; j++){
			if (labelBC_w.at<Vec3b>(i, j)[0] == 0 && labelBC_w.at<Vec3b>(i, j)[1] == 0 && labelBC_w.at<Vec3b>(i, j)[2] == 255) {

				//cout << "get red" << endl;
				labelBC_w.at<Vec3b>(i, j)[0] = 255;
				labelBC_w.at<Vec3b>(i, j)[1] = 0;
				labelBC_w.at<Vec3b>(i, j)[2] = 255;

			}
		}
	}
	for (int i = 0; i < labelBC_b.rows; i++)
	{
		for (int j = 0; j < labelBC_b.cols; j++){
			if (labelBC_b.at<Vec3b>(i, j)[0] == 0 && labelBC_b.at<Vec3b>(i, j)[1] == 0 && labelBC_b.at<Vec3b>(i, j)[2] == 255) {

				labelABC.at<Vec3b>(i, j)[0] = 255;
				labelABC.at<Vec3b>(i, j)[1] = 0;
				labelABC.at<Vec3b>(i, j)[2] = 255;

				labelAB.at<Vec3b>(i, j)[0] = 255;
				labelAB.at<Vec3b>(i, j)[1] = 0;
				labelAB.at<Vec3b>(i, j)[2] = 255;

			}
		}
	}
	//cv::imwrite("widen/out_classifyABC_vw.png", labelBC_w);
	//cv::imwrite("widen/out_classifyABC_vb.png", labelAB);

	for (int i = 0; i < labelBC_w.rows; i++)
	{
		for (int j = 0; j < labelBC_w.cols; j++)
		{
			if (labelBC_b.at<Vec3b>(i, j)[0] != 0){
				labelABC.at<Vec3b>(i, j)[0] = labelBC_w.at<Vec3b>(i, j)[0];
				labelABC.at<Vec3b>(i, j)[1] = labelBC_w.at<Vec3b>(i, j)[1];
				labelABC.at<Vec3b>(i, j)[2] = labelBC_w.at<Vec3b>(i, j)[2];

				labelAB.at<Vec3b>(i, j)[0] = labelBC_w.at<Vec3b>(i, j)[0];
				labelAB.at<Vec3b>(i, j)[1] = labelBC_w.at<Vec3b>(i, j)[1];
				labelAB.at<Vec3b>(i, j)[2] = labelBC_w.at<Vec3b>(i, j)[2];

				if (labelBC_w.at<Vec3b>(i, j)[0] == 0 && labelBC_w.at<Vec3b>(i, j)[1] == 255 && labelBC_w.at<Vec3b>(i, j)[2] == 0)
				{
					labelAB.at<Vec3b>(i, j)[0] = 255;
					labelAB.at<Vec3b>(i, j)[1] = 255;
					labelAB.at<Vec3b>(i, j)[2] = 255;
				}
			}
			if (labelBC_b.at<Vec3b>(i, j)[0] == 0 && labelBC_b.at<Vec3b>(i, j)[1] == 255 && labelBC_b.at<Vec3b>(i, j)[2] == 0)
			{
			labelAB.at<Vec3b>(i, j)[0] = 0;
			labelAB.at<Vec3b>(i, j)[1] = 0;
			labelAB.at<Vec3b>(i, j)[2] = 0;
			}
		}
	}

	//imwrite("widen/out_classifyABC_all.png", labelABC);
	imwrite(outImage, labelAB);
}
void centralaxis::setClassifyThreshold(int num) {
	classify_BC = num;
}