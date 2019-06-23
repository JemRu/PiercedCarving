// ErosionDilation.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "adaption.h"
#include "centralaxis.h"
#include "optimize.h"

using namespace  cv;
using namespace std; 

int _tmain(int argc, _TCHAR* argv[])
{
	//全局参数
	//用户输入
	float size = 8.5;//打印尺寸 用户输入
	float Fw = 0.03, Fb = 0.05;//打印机阈值
	float resolution;//图像分辨率
	//优化阈值
	int Wc = 2;//图像补偿
	int Ww = 3;//实体阈值
	int Wb = 5;//孔洞阈值
	int Classify_BC_w = 0;//预处理区域分类
	int Classify_BC_b = 0;//预处理区域分类
	int Connector;//连通宽度
	int noisew, noiseb;
	//图像
	String imStr = "data/input.png";//输入
	String imoutStr = "data/output.png";//输出
	String imRemoveStr = "data/temp/input_removeNosie.png";
	String imConnectStr = "data/temp/out_connect.png";//after connection
	String imCompensateStr = "data/temp/out_compensate.png";//after compensation
	String imAdaptionStr = "data/temp/out_adaption.png";//after adaption
	
	Mat inputIm = imread(imStr);
	resolution = inputIm.cols > inputIm.rows ? inputIm.cols : inputIm.rows;
	cout << "请输入打印尺寸, 单位cm, 例：4(即 4x4cm)：" << endl;
	cin >> size;
	cout << "请输入打印机 实体 孔洞 的精度阈值, 单位cm, 例：0.03 0.05(一般的SLA打印机阈值)：" << endl;
	cin >> Fw >> Fb ;

	//计算阈值
	Ww = ceil(Fw / (size / resolution));
	if ((Ww % 2) == 0)
		Ww++;
	Wb = ceil(Fb / (size / resolution));
	if ((Wb % 2) == 0)
		Wb++;

	Classify_BC_w = ceil(size*0.004 / (size / resolution));
	Classify_BC_b = ceil(size*0.007 / (size / resolution));
	noisew = Classify_BC_w * Classify_BC_w * 0.8;
	noiseb = Classify_BC_b * Classify_BC_b * 0.8;
	Connector = Ww;

	cout << "图像大小为：" << inputIm.cols <<" x "<< inputIm.rows <<" px"<< endl;
	cout << "图像填充为后为：" << resolution << " x " << resolution << " px" << endl;
	cout << "打印尺寸为" << size << " x " << size << " cm" << endl;
	cout << "建议细节检测阈值设置 实体 >= " << Ww << ", 孔洞 >= " << Wb << " 。" << endl;
	cout << "建议细节分类阈值设置 实体 " << Classify_BC_w << ", 孔洞 " << Classify_BC_b << " 。" << endl;
	cout << "----------------------------------------------------------" << endl;
	cout << "请输入，实体、孔洞 检测阈值(大于3的奇数，如：3 5 )：" << endl;
	cin >> Ww >> Wb;
	Connector = Ww;
	cout << "请输入，实体、孔洞 细节分类阈值(如：5 10)：" << endl;
	cin >> Classify_BC_w >> Classify_BC_b;
	noisew = Classify_BC_w * Classify_BC_w * 0.8;
	noiseb = Classify_BC_b * Classify_BC_b * 0.8;
	


	/*********************************************pretreatment******************************************/
	//remove noise
	cout << "Image Optimization start" << endl;
	cout << "remove noise" << endl;
	optimize remove;
	remove.remove_noise(noisew, imStr, imRemoveStr);
	remove.remove_noise(noiseb, imRemoveStr, imRemoveStr);

	/**************************************Connectivity Enforcement*************************************/
	cout << "Connectivity Enforcement" << endl;
	Mat conin = imread(imRemoveStr);
	imwrite(imConnectStr, conin);
	while (true)
	{
		optimize pixel;
		pixel.setWidth(Connector);
		pixel.get_size(imConnectStr.c_str());
		int n = pixel.get_lable(imConnectStr.c_str());
		//cout << n << endl;
		if (n <= 2)
			break;
		pixel.get_all_boundary(n);
		pixel.distance_all();
		pixel.merge(imConnectStr.c_str(), imConnectStr.c_str());
	}

	/***************************************Compensation Filtering**************************************/
	cout << "Compensation Filtering" << endl;
	Mat image = imread(imConnectStr);
	//获取自定义核
	Mat element = getStructuringElement(MORPH_RECT, Size(Wc, Wc));
	Mat out;
	//进行腐蚀操作
	erode(image, out, element);
	//!
	cv::imwrite(imCompensateStr, image);

	/*********************************************Detail Adaption****************************************/
	Mat Adaptionin = imread(imCompensateStr);
	imwrite(imAdaptionStr, Adaptionin);
	bool iteration = true;
	int numItera = 1;
	while (iteration)
	{
		cout << "iteration " << numItera << endl;
		//cout << "Details Detection" << endl;
		//Dw
		String DwStr = "data/adaption/Dw.png";
		String DwboundaryStr = "data/adaption/Dw_boundary.png";
		String Dw_ABStr = "data/adaption/Dw_AB.png";
		String Dw_BStr = "data/adaption/Dw_B.png";
		String Dw_BFStr = "data/adaption/Dw_B_ledfield.png";
		String Dw_classifyStr = "data/adaption/Dw_classifyABC.png";
		adaption img;
		img.detection(imAdaptionStr.c_str(), DwStr.c_str(), Ww);
		optimize removesmall;
		int nosiesize = Classify_BC_w;
		//cout << nosiesize << endl;
		removesmall.remove_noise(nosiesize, DwStr, Dw_BStr);
		removesmall.remove_noise(nosiesize, Dw_BStr, Dw_BStr);
		int n = img.get_lable(Dw_BStr.c_str());
		img.get_diff_boundary(n, DwboundaryStr.c_str());
		img.visual_detection_diff(imAdaptionStr.c_str(), Dw_ABStr.c_str(), Dw_BStr.c_str());
		centralaxis field;
		field.disboufield(Dw_BStr.c_str(), Dw_ABStr.c_str(), Dw_BFStr.c_str());
		field.visual_field_Laplacian(imAdaptionStr.c_str(), Dw_BFStr.c_str());
		int Classifyw = Classify_BC_w*Classify_BC_w;
		//cout <<"Classifyw: " <<Classifyw << endl;
		field.visual_classify_BC(Dw_ABStr.c_str(), Dw_BStr.c_str(), Dw_classifyStr.c_str(), Classifyw);
		//Db
		Mat Adaptioninb = imread(imAdaptionStr);
		bitwise_xor(Adaptioninb, cv::Scalar(255, 255, 255), Adaptioninb);
		String imAdaptionbStr = "data/adaption/inputb_adaption.png";
		imwrite(imAdaptionbStr, Adaptioninb);
		String DbStr = "data/adaption/Db.png";
		String DbboundaryStr = "data/adaption/Db_boundary.png";
		String Db_ABStr = "data/adaption/Db_AB.png";
		String Db_BStr = "data/adaption/Db_B.png";
		String Db_BFStr = "data/adaption/Db_B_ledfield.png";
		String Db_classifyStr = "data/adaption/Db_classifyABC.png";
		String Dwb_ABCStr = "data/adaption/D_classifyABC.png";//可视化注释
		String Dwb_ABStr = "data/adaption/D_classifyAB.png";
		adaption imgb;
		imgb.detection(imAdaptionbStr.c_str(), DbStr.c_str(), Wb);
		optimize removesmallb;
		int nosiesizeb = Classify_BC_b;
		removesmallb.remove_noise(nosiesizeb, DbStr, Db_BStr);
		removesmallb.remove_noise(nosiesizeb, Db_BStr, Db_BStr);
		int nb = imgb.get_lable(Db_BStr.c_str());
		imgb.get_diff_boundary(nb, DbboundaryStr.c_str());
		imgb.visual_detection_diff(imAdaptionStr.c_str(), Db_ABStr.c_str(), Db_BStr.c_str());
		centralaxis fieldb;
		fieldb.disboufield(Db_BStr.c_str(), Db_ABStr.c_str(), Db_BFStr.c_str());
		fieldb.visual_field_Laplacian(imAdaptionStr.c_str(), Db_BFStr.c_str()); 
		int Classifyb = Classify_BC_b*Classify_BC_b;
		//cout << "Classifyb: " << Classifyb << endl;
		fieldb.visual_classify_BC(Db_ABStr.c_str(), Db_BStr.c_str(), Db_classifyStr.c_str(), Classifyb);
		fieldb.visual_classify_ABC(Dw_classifyStr.c_str(), Db_classifyStr.c_str(), Dwb_ABStr.c_str());

		//cout << "Treat confliction" << endl;
		adaption confliction;
		iteration = confliction.get_conflict(Dwb_ABStr.c_str(), Dw_classifyStr.c_str(), Db_classifyStr.c_str(), DwStr.c_str(), DbStr.c_str(), 0);
		confliction.get_conflict_weight();
		confliction.treatment_conflict(imAdaptionStr.c_str(), nosiesize, nosiesizeb);
		confliction.treatment_unconflict_x(imAdaptionStr.c_str());
		removesmallb.remove_noise(nosiesizeb, imAdaptionStr, imAdaptionStr);
		removesmallb.remove_noise(nosiesizeb, imAdaptionStr, imAdaptionStr);

		//cout << "Connectivity Enforcement" << endl;
		while (true)
		{
			optimize pixel;
			pixel.setWidth(Connector);
			pixel.get_size(imAdaptionStr.c_str());
			int n = pixel.get_lable(imAdaptionStr.c_str());
			//cout << n << endl;
			if (n <= 2)
				break;
			pixel.get_all_boundary(n);
			pixel.distance_all();
			pixel.merge(imAdaptionStr.c_str(), imAdaptionStr.c_str());
		}
		numItera++;
	}
	
	Mat Adaptionout = imread(imAdaptionStr);
	imwrite(imoutStr, Adaptionout);
	cout << "Image Optimization end" << endl;
/*******************************************Image Optimization end***********************************************************/	
	system("pause");
	return 0;
}

