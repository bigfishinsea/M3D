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
	//���ѡ�����������
	QString GetSelectedname();
	//��õ�ǰ���˽ӿڵ�����
	double GetSelectedConnectorOrigin_X();
	double GetSelectedConnectorOrigin_Y();
	double GetSelectedConnectorOrigin_Z();
	double GetSelectedConnectorAxisZ_X();
	double GetSelectedConnectorAxisZ_Y();
	double GetSelectedConnectorAxisZ_Z();
	double GetSelectedConnectorAxisX_X();
	double GetSelectedConnectorAxisX_Y();
	double GetSelectedConnectorAxisX_Z();

	//��д�ر��¼�
	void closeEvent(QCloseEvent*) override;
	
	void CommonSettings();
	//������ʱ�Դ��ڵ�����,���ò�����,������Ҫ��
	/*void SetFreePattern();*/
	//ƽ��ʱ�Դ��ڵ�����
	void SetRotatePattarn();
	//��תʱ�Դ��ڵ�����
	void SetSlidePattarn();
	//��
	void SetSpherePattern();
	//�����ӿ�ʱ�Դ��ڵ�����
	void SetOtherPattern();
	void getConDisp();
private slots:
	//��������ԭ��ͷ���仯��ӳ��������
	void On_LineEdit_ValidChanged(const QString&);
	//�ƶ�������ת�ı仯����ӳ��������
	void My_DoubleSpinBox_ValidChanged1(double val);
	//Ӧ�ð�ť��Ӧ
	void On_savebutton_clicked();
	//�ָ���ť��Ӧ
	void On_restorebutton_clicked();

	//���QDoubleSpinBox��Ӧ���βۺ���������

private:
	void tempChange(gp_Trsf);

	Command* _p_Cmd;
	//bool _Free;//�Ƿ���������
	std::string _mtype;
	std::vector<double> v15_record;
	std::vector<double> v9_read{ 0,0,0,0,0,1,1,0,0 };

	//��������˼�¼ÿ�εı任����
	gp_Trsf x_slide_trsf;
	gp_Trsf y_slide_trsf;
	gp_Trsf z_slide_trsf;
	gp_Trsf x_rotate_trsf;
	gp_Trsf y_rotate_trsf;
	gp_Trsf z_rotate_trsf;
	//��һ�ζ�ȡ�ļ�¼�����ڻָ�����ʹ��
	std::vector<double> last_record;
	//��¼��һ�εĸĶ�����Ҫ������������򸱵ȣ�ֻ��������ƽ������������ת��
	std::vector<double> last_change;
	//��¼���еĸĶ����̣��ڹر�ʱ���浽�ĵ�ʹ��
	std::vector<double> change_all;
	//��һ�ζ�ȡ��ʱ�������Ϊ0Ҳ���������в���������bool��������
	std::vector<bool> first_read;
	// �ı�consComboBox�õ��Ĳ�������Ҳ��Ӱ��
	bool change_consComboBox;
	std::unordered_map<std::string, std::unordered_set<std::string>> name_map;
	std::unordered_map<std::string, std::string> assembly_graph;

	//���еĿؼ�
	QLabel* title_label;
	QLineEdit* title_line;
	// ��һ��ѡ��Լ���ӿڵ������б�
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
