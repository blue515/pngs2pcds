// C++ 标准库
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

// OpenCV 库
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// PCL 库
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>

// 定义点云类型
typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloud;

// 相机内参
const double camera_factor = 1000;
const double camera_cx = 325.5;
const double camera_cy = 253.5;
const double camera_fx = 518.0;
const double camera_fy = 519.0;

void GetFileNames(string path,vector<string>& filenames)
{
    DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(path.c_str()))){
        cout<<"Folder doesn't Exist!"<<endl;
        return;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0){
            filenames.push_back(path + "/" + ptr->d_name);
    }
    }
    closedir(pDir);
}

// 主函数 
int main(int argc, char** argv)
{
    
	vector <string> files1;
	vector <string> files2;
	//需要读取的文件夹路径，使用单右斜杠"/"
	string filePath1 = "/home/blue/blender/train/bowl045/png/rgb";
	string filePath2 = "/home/blue/blender/train/bowl045/png/depth";
	//string filePath = "./datas"; //相对路径也可以
	GetFileNames(filePath1, files1);
	GetFileNames(filePath2, files2);
	for (int i = 0; i < files1.size(); i++) {
    
		// 图像矩阵
		cv::Mat rgb, depth;
		// 使用cv::imread()来读取图像
		// rgb 图像是8UC3的彩色图像
		// depth 是16UC1的单通道图像，注意flags设置-1,表示读取原始数据不做任何修改
		rgb = cv::imread(files1[i]);
		depth = cv::imread(files2[i], -1);

		// 点云变量
		// 使用智能指针，创建一个空点云。这种指针用完会自动释放。
		PointCloud::Ptr cloud(new PointCloud);
		// 遍历深度图
		for (int m = 0; m < depth.rows; m++)
			for (int n = 0; n < depth.cols; n++)
			{
    
				// 获取深度图中(m,n)处的值
				ushort d = depth.ptr<ushort>(m)[n];
				// d 可能没有值，若如此，跳过此点
				if (d == 0)
					continue;
				// d 存在值，则向点云增加一个点
				PointT p;

				// 计算这个点的空间坐标
				p.z = double(d) / camera_factor;
				p.x = (n - camera_cx) * p.z / camera_fx;
				p.y = (m - camera_cy) * p.z / camera_fy;

				// 从rgb图像中获取它的颜色
				// rgb是三通道的BGR格式图，所以按下面的顺序获取颜色
				p.b = rgb.ptr<uchar>(m)[n * 3];
				p.g = rgb.ptr<uchar>(m)[n * 3 + 1];
				p.r = rgb.ptr<uchar>(m)[n * 3 + 2];

				// 把p加入到点云中
				cloud->points.push_back(p);
			}
		int size = cloud->points.size();
		// 设置并保存点云
		cloud->height = 1;
		cloud->width = cloud->points.size();
		cloud->is_dense = false;
		string name = "/home/blue/blender/train/bowl045/pcd/bowl_" + to_string(i);

		pcl::io::savePLYFile(name + ".ply", *cloud);   //将点云数据保存为ply文件
		pcl::io::savePCDFile(name + ".pcd", *cloud);   //将点云数据保存为pcd文件
		// 清除数据并退出
		cloud -> points.clear();
		cout << "point cloud size = " << size << endl;
		cout << "NO." << i << " Done!" << "\n" << endl;
	}
	cout << "End" << endl;
	return 0;
}
