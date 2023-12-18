#include "DocumentModel.h"
#include "Command.h"
//#include "DocumentModel.h"
#include "Component.h"
#include "Connector.h"
#include <AIS_InteractiveContext.hxx>

Command::Command(View* view) :m_step(0),curr_x(0.0), curr_y(0.0), view(view), myDocumentCommon(nullptr), m_Comproot(nullptr)
{
	init();
}

void Command::init()
{
	if (view)
	{
		myContext = view->GetContext();
		myDocumentCommon = /*dynamic_cast<DocumentModel*>*/(view->GetDocument());
	}
	if (myDocumentCommon)
		m_Comproot = myDocumentCommon->GetComponent();
}

Connector* Command::GetConnector() const
{
	return nullptr;
}

Component* Command::GetComponent() const
{
	return nullptr;
}

const string Command::getSelectedname()
{
	myContext->InitSelected();
	if (!myContext->HasSelectedShape())
	{
		return "";
	}
	Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
	TopoDS_Shape shape;
	if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
	{
		shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
	}
	if (m_Comproot)
	{
		list<Component*> comChildren = m_Comproot->GetChilds();
		for (auto iter = comChildren.begin(); iter != comChildren.end(); ++iter)
		{
			Component* pComChild = *iter;
			if (pComChild->GetCompound().IsEqual(shape))
				return pComChild->GetCompName();
		}
	}
	return "";
}

Command::~Command()
{

}

void Command::CancelCommand()
{

}
