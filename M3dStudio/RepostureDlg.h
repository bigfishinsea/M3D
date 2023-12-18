#pragma once
#ifndef DIALOG_H__
#define DIALOG_H__

#include <QDialog>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <gp_Trsf.hxx>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QLabel;
class QDoubleSpinBox;
class QComboBox;
class QPushButton;
class QGroupBox;
QT_END_NAMESPACE

class Command;

//constexpr double PI = 3.1415926535;

class reposturedialog :public QDialog
{
	Q_OBJECT
public:
	reposturedialog(QWidget*, Command*, std::string);
	~reposturedialog();
	void init();
public:
	//获得选中组件的名称
	QString GetSelectedname();
	//获得当前调姿接口的坐标
	double GetSelectedConnectorOrigin_X();
	double GetSelectedConnectorOrigin_Y();
	double GetSelectedConnectorOrigin_Z();
	double GetSelectedConnectorAxisZ_X();
	double GetSelectedConnectorAxisZ_Y();
	double GetSelectedConnectorAxisZ_Z();
	double GetSelectedConnectorAxisX_X();
	double GetSelectedConnectorAxisX_Y();
	double GetSelectedConnectorAxisX_Z();

	//重写关闭事件
	void closeEvent(QCloseEvent*) override;
	
	void CommonSettings();
	//自由体时对窗口的设置,采用操作器,不再需要了
	/*void SetFreePattern();*/
	//平移时对窗口的设置
	void SetRotatePattarn();
	//旋转时对窗口的设置
	void SetSlidePattarn();
	//球副
	void SetSpherePattern();
	//其它接口时对窗口的设置
	void SetOtherPattern();
	void getConDisp();
private slots:
	//调整坐标原点和方向变化反映到这上面
	void On_LineEdit_ValidChanged(const QString&);
	//移动或者旋转的变化均反映到这上面
	void My_DoubleSpinBox_ValidChanged1(double val);
	//应用按钮响应
	void On_savebutton_clicked();
	//恢复按钮响应
	void On_restorebutton_clicked();

	//解决QDoubleSpinBox响应两次槽函数的问题

private:
	void tempChange(gp_Trsf);

	Command* _p_Cmd;
	//bool _Free;//是否是自由体
	std::string _mtype;
	std::vector<double> v15_record;
	std::vector<double> v9_read{ 0,0,0,0,0,1,1,0,0 };

	//自由体调姿记录每次的变换矩阵
	gp_Trsf x_slide_trsf;
	gp_Trsf y_slide_trsf;
	gp_Trsf z_slide_trsf;
	gp_Trsf x_rotate_trsf;
	gp_Trsf y_rotate_trsf;
	gp_Trsf z_rotate_trsf;
	//上一次读取的记录，仅在恢复操作使用
	std::vector<double> last_record;
	//记录上一次的改动，主要用在自由体和球副等，只保存三个平移量和三个旋转量
	std::vector<double> last_change;
	//记录所有的改动过程，在关闭时保存到文档使用
	std::vector<double> change_all;
	//第一次读取的时候如果不为0也会对组件进行操作，设置bool变量区分
	std::vector<bool> first_read;
	// 改变consComboBox得到的参数可能也会影响
	bool change_consComboBox;
	std::unordered_map<std::string, std::unordered_set<std::string>> name_map;
	std::unordered_map<std::string, std::string> assembly_graph;

	//所有的控件
	QLabel* title_label;
	QLineEdit* title_line;
	// 加一个选择约束接口的下拉列表
	QLabel* consConn_label;
	QComboBox* consConnBox;

	QLabel* origin_label;
	QLineEdit* origin_line;
	QLabel* AxisZ_label;
	QLineEdit* Axis_Z;
	QLabel* AxisX_label;
	QLineEdit* Axis_X;

	QLabel* adjust_label;
	QLabel* offset_label;
	QLabel* delta_label;
	QLabel* slide_label_x;
	QDoubleSpinBox* slide_spin_x;
	QLineEdit* spin_line_xdelta;
	QLabel* slide_label_y;
	QDoubleSpinBox* slide_spin_y;
	QLineEdit* spin_line_ydelta;
	QLabel* slide_label_z;
	QDoubleSpinBox* slide_spin_z;
	QLineEdit* spin_line_zdelta;
	QLabel* rotate_label_x;
	QDoubleSpinBox* rotate_spin_x;
	QLineEdit* rotate_line_xdelta;
	QLabel* rotate_label_y;
	QDoubleSpinBox* rotate_spin_y;
	QLineEdit* rotate_line_ydelta;
	QLabel* rotate_label_z;
	QDoubleSpinBox* rotate_spin_z;
	QLineEdit* rotate_line_zdelta;
};

#endif
