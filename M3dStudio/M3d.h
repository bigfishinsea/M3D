#pragma once
#pragma execution_character_set ("utf-8")
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

#define PI 3.14159

//�����ݲ�
#define LINEAR_TOL 1e-6

//���ϵ����ࣺ
/*
  Graphic3d_NameOfMaterial_Brass,           //!< Brass        (Physic)
  Graphic3d_NameOfMaterial_Bronze,          //!< Bronze       (Physic)
  Graphic3d_NameOfMaterial_Copper,          //!< Copper       (Physic)
  Graphic3d_NameOfMaterial_Gold,            //!< Gold         (Physic)
  Graphic3d_NameOfMaterial_Pewter,          //!< Pewter       (Physic)
  Graphic3d_NameOfMaterial_Plastered,       //!< Plastered    (Generic)
  Graphic3d_NameOfMaterial_Plastified,      //!< Plastified   (Generic)
  Graphic3d_NameOfMaterial_Silver,          //!< Silver       (Physic)
  Graphic3d_NameOfMaterial_Steel,           //!< Steel        (Physic)
  Graphic3d_NameOfMaterial_Stone,           //!< Stone        (Physic)
  Graphic3d_NameOfMaterial_ShinyPlastified, //!< Shiny Plastified (Generic)
  Graphic3d_NameOfMaterial_Satin,           //!< Satin        (Generic)
  Graphic3d_NameOfMaterial_Metalized,       //!< Metalized    (Generic)
  Graphic3d_NameOfMaterial_Ionized,         //!< Ionized      (Generic)
  Graphic3d_NameOfMaterial_Chrome,          //!< Chrome       (Physic)
  Graphic3d_NameOfMaterial_Aluminum,        //!< Aluminum     (Physic)
  Graphic3d_NameOfMaterial_Obsidian,        //!< Obsidian     (Physic)
  Graphic3d_NameOfMaterial_Neon,            //!< Neon         (Physic)
  Graphic3d_NameOfMaterial_Jade,            //!< Jade         (Physic)
  Graphic3d_NameOfMaterial_Charcoal,        //!< Charcoal     (Physic)
  Graphic3d_NameOfMaterial_Water,           //!< Water        (Physic)
  Graphic3d_NameOfMaterial_Glass,           //!< Glass        (Physic)
  Graphic3d_NameOfMaterial_Diamond,         //!< Diamond      (Physic)
  Graphic3d_NameOfMaterial_Transparent,     //!< Transparent  (Physic)
  Graphic3d_NameOfMaterial_DEFAULT,         //!< Default      (Generic);
  Graphic3d_NameOfMaterial_UserDefined,     //!< User-defined (Physic);
*/
////���ϵ�����
//extern vector<string> g_sMaterialNames;
//extern vector<string> g_sMaterialChnNames;
//
////���ϵ��ܶȣ���λ Kg/mm3����ͼ�ĵ�λΪ mm
//extern vector<double> g_fMaterialDensity;
//
////���ϵ���ɫ
//extern vector<vector<double> > g_fMaterialColor;
//
////���ϵ�͸����
//extern vector<double> g_fMaterialTransparency;

//�ϲ�Ϊ�������ݽṹ
struct myMaterial
{
	string sMaterialName;
	string sMaterialChnName;
	double fMaterialDensity;
	vector<double> v3MaterialColor;
	double fMaterialTransparency;
};

//�洢ȫ��������Ϣ
extern vector<myMaterial> g_vMaterials;
//ģ�Ϳ�
class ModelicaModel;
extern vector<ModelicaModel*> g_vModelicaModels;
