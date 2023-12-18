#include "ImageShowDlg.h"

ImageShowDlg::ImageShowDlg(QWidget* parent)
	: QDialog(parent)
{
	//setGeometry(600, 300, 500, 460);
	//setFixedSize(500, 460);

	//���ؼ�
	myLabel = new QLabel(this);
	myLabel->setGeometry(5, 5, 800, 500);
}

ImageShowDlg::~ImageShowDlg()
{
}

void ImageShowDlg::SetPixmap(QPixmap pm)
{
	//m_Pixmap = pm;
	qreal width = pm.width(); //�����ǰͼƬ�Ŀ�͸�
	qreal height = pm.height();

	m_Pixmap = pm.scaled(800/*width/2*/, 500/*height / 2*/, Qt::KeepAspectRatio);

	myLabel->setPixmap(m_Pixmap);
}
