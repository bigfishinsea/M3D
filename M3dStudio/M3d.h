#pragma once
#pragma execution_character_set ("utf-8")
#include <string>
#include <vector>
#include <list>
#include <map>

using namespace std;

#define PI 3.14159

//线性容差
#define LINEAR_TOL 1e-6

//材料的种类：
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
////材料的名称
//extern vector<string> g_sMaterialNames;
//extern vector<string> g_sMaterialChnNames;
//
////材料的密度：单位 Kg/mm3，绘图的单位为 mm
//extern vector<double> g_fMaterialDensity;
//
////材料的颜色
//extern vector<vector<double> > g_fMaterialColor;
//
////材料的透明度
//extern vector<double> g_fMaterialTransparency;

//合并为材料数据结构
struct myMaterial
{
	string sMaterialName;
	string sMaterialChnName;
	double fMaterialDensity;
	vector<double> v3MaterialColor;
	double fMaterialTransparency;
};

//存储全部材料信息
extern vector<myMaterial> g_vMaterials;
//模型库
class ModelicaModel;
extern vector<ModelicaModel*> g_vModelicaModels;
