#include "paratreemodel.h"
TreeItem::TreeItem(const QVector<QVariant>& data,bool f,TreeItem* parent)
    : m_itemData(data),flag(f),m_parentItem(parent)
{}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem* item)
{
    m_childItems.append(item);
}

TreeItem* TreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

int TreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeItem::data(int column) const
{
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}


void TreeItem::setData(int column, QVariant value)
{
    m_itemData[column] = value;
}


TreeItem* TreeItem::parentItem()
{
    return m_parentItem;
}

void TreeItem::clearItem() {
    m_childItems.clear();
}



TreeModel::TreeModel(const QString& data, QObject* parent)
    : QAbstractItemModel(parent)
{
    flag = f();
    rootItem = new TreeItem({ tr("Title"), tr("Summary")});
    setupModelData(data.split('\n'), rootItem);
}
TreeModel::TreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    flag=f();
}
TreeModel::~TreeModel()
{
    delete rootItem;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem* parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem* childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem* parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex& parent) const
{
    TreeItem* parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
    
     if (role == Qt::EditRole)
    {
         return item->data(index.column());
    }    else if (role != Qt::DisplayRole)
        return QVariant();
    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    if(flag){
    //设置第四列可编辑，第五列在第四列值为"关注"时可编辑
        if (index.column() == bFocusedcol || (index.column() == sFocusNamecol && (this->data(index.sibling(index.row(), bFocusedcol), Qt::DisplayRole) == "是")))
        {
            Qt::ItemFlags flags = QAbstractItemModel::flags(index);
            flags |= Qt::ItemIsEditable;
            return flags;
        }
    }
    else {
        if (index.column() == 2 || (index.column() == 3 && (this->data(index.sibling(index.row(), 2), Qt::DisplayRole) == "是")))
        {
            Qt::ItemFlags flags = QAbstractItemModel::flags(index);
            flags |= Qt::ItemIsEditable;
            return flags;
        }
    }

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
    int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
bool TreeModel::setData(const QModelIndex& index, const QVariant& value,int role)
{
    if (index.isValid() && role == Qt::EditRole)
    {
        TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
        item->setData(index.column(), value);

        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool TreeModel::buildModel(CyberInfo* pCyberInfo) {
    //if (pCyberInfo == nullptr)return false;
    //QVector<TreeItem*> parents;
    //QVector<int> indentations;
    //rootItem->clearItem();
    //parents << rootItem; 
    //indentations << 0;
    //if(flag){

    //    vector<myParameter*> myParameter = pCyberInfo->GetParameters();
    //    int size = myParameter.size();

    //    for (int i = 0; i<size; i++) {

    //        int position = count(myParameter[i]->sFullName.begin(), myParameter[i]->sFullName.end(), '.');

    //        QVector<QVariant>columnData;
    //        
    //        int pos = (myParameter[i]->sFullName).find_last_of('.');
    //        
    //        string name;

    //        if (pos == -1)
    //        {
    //            name = myParameter[i]->sFullName;
    //        }
    //        else {
    //            name = (myParameter[i]->sFullName).substr(pos+1);
    //        }

    //        bool con = (stricmp(name.c_str(), myParameter[i]->sType.c_str()) == 0);

    //        columnData << QString::fromStdString(name)
    //            << (con?"":QString::fromStdString(myParameter[i]->sType))
    //            << QString::fromStdString(myParameter[i]->defaultValue)
    //            << QString::fromStdString(myParameter[i]->sUnit)
    //            << QString::fromStdString(myParameter[i]->sDimension);
    //        
    //        if (myParameter[i]->bFocused&&!con)columnData << "是";
    //        else columnData << "";
    //        
    //        columnData 
    //            << QString::fromStdString(myParameter[i]->sFocusName);

    //        buildhelp(parents, indentations, position,columnData);
    //    }
    //}
    //else {
    //    vector<myOutputVar*> m_OutputVars = pCyberInfo->GetOutputVars();
    //    int size = m_OutputVars.size();

    //    for (int i = 0; i<size; i++) {
    //        //void* ptr = &(m_OutputVars[i]->sFullName);
    //        int position = count(m_OutputVars[i]->sFullName.begin(), m_OutputVars[i]->sFullName.end(), '.');
    //        
    //        
    //        //0507增加，对parent的判断及处理，尽量不改动buildhelp()函数，在此对parent处理完毕
    //        int position_before = 0;
    //        if (i >= 1)//为防止i-1取不到数据
    //        {
    //            position_before = count(m_OutputVars[i - 1]->sFullName.begin(), m_OutputVars[i - 1]->sFullName.end(), '.');
    //        }
    //        if (position > position_before)
    //        {
    //            int namepos = (m_OutputVars[i]->sFullName).find_last_of('.');
    //            string now_parentname = (m_OutputVars[i]->sFullName).substr(0, namepos);
    //            int parentname_pos = now_parentname.find_last_of('.');
    //            string name;
    //            if (parentname_pos == -1)name = now_parentname;
    //            else name = now_parentname.substr(parentname_pos + 1);
    //            if (now_parentname != m_OutputVars[i - 1]->sFullName)
    //            {
    //                QVector<QVariant>columnData;
    //                for (int j = 0; j < OutputtreeMaxCols; j++) {
    //                    columnData << QString::fromStdString(name) << "" << "" << "";
    //                }
    //                parents.last()->appendChild(new TreeItem(columnData, flag, parents.last()));
    //            }
    //        }
    //        else if (position == position_before && position != 0)
    //        {
    //            int namepos = (m_OutputVars[i]->sFullName).find_last_of('.');
    //            string now_parentname = (m_OutputVars[i]->sFullName).substr(0, namepos);
    //            int before_namepos = (m_OutputVars[i - 1]->sFullName).find_last_of('.');
    //            string before_parentname = (m_OutputVars[i - 1]->sFullName).substr(0, before_namepos);
    //            if (now_parentname != before_parentname)
    //            {
    //                int parentname_pos = now_parentname.find_last_of('.');
    //                string name;
    //                if (parentname_pos == -1)name = now_parentname;
    //                else name = now_parentname.substr(parentname_pos + 1);
    //                QVector<QVariant>columnData;
    //                for (int j = 0; j < OutputtreeMaxCols; j++) {
    //                    columnData << QString::fromStdString(name) << "" << "" << "";
    //                }
    //                parents.pop_back();
    //                parents.last()->appendChild(new TreeItem(columnData, flag, parents.last()));
    //                parents << parents.last()->child(parents.last()->childCount() - 1);
    //            }
    //        }
    //        else if (position_before > position && position != 0)
    //        {
    //            string before_fullname = m_OutputVars[i - 1]->sFullName;
    //            while (position_before != position)
    //            {
    //                int before_fullname_pos;
    //                before_fullname_pos = before_fullname.find_last_of('.');
    //                before_fullname = before_fullname.substr(0, before_fullname_pos);
    //                parents.pop_back();
    //                indentations.pop_back();
    //                position_before--;
    //            }

    //            //再判断前缀是否相等，不相等的话再做处理
    //            //现在的before_fullname和现在的名字应该已经等长了，可以仿照相等时的情况做处理
    //            int namepos = (m_OutputVars[i]->sFullName).find_last_of('.');
    //            string now_parentname = (m_OutputVars[i]->sFullName).substr(0, namepos);
    //            int before_namepos = before_fullname.find_last_of('.');
    //            string before_parentname = before_fullname.substr(0, before_namepos);
    //            if (now_parentname != before_parentname)
    //            {
    //                int parentname_pos = now_parentname.find_last_of('.');
    //                string name;
    //                if (parentname_pos == -1)name = now_parentname;
    //                else name = now_parentname.substr(parentname_pos + 1);
    //                QVector<QVariant>columnData;
    //                for (int j = 0; j < OutputtreeMaxCols; j++) {
    //                    columnData << QString::fromStdString(name) << "" << "" << "";
    //                }
    //                parents.pop_back();
    //                parents.last()->appendChild(new TreeItem(columnData, flag, parents.last()));
    //                parents << parents.last()->child(parents.last()->childCount() - 1);
    //            }
    //        }


    //        QVector<QVariant>columnData;
    //        int pos = (m_OutputVars[i]->sFullName).find_last_of('.');
    //        string name;
    //        if (pos == -1)name = m_OutputVars[i]->sFullName;
    //        else name = (m_OutputVars[i]->sFullName).substr(pos+1);
    //        bool con = (stricmp(name.c_str(), m_OutputVars[i]->sType.c_str()) == 0);
    //        //for (int j = 0; j < OutputtreeMaxCols; j++) {
    //            columnData << QString::fromStdString(name)
    //                << (con ? "" : QString::fromStdString(m_OutputVars[i]->sType));
    //            if (m_OutputVars[i]->bFocused&&!con)columnData << "是";
    //            else columnData << "";
    //            columnData << QString::fromStdString(m_OutputVars[i]->sFocusName);
    //        //}
    //        buildhelp(parents, indentations, position,columnData);
    //    }
    //}

    if (pCyberInfo == nullptr)return false;
    rootItem->clearItem();
    if (flag) {
        vector<myParameter*> myParameter = pCyberInfo->GetParameters();
        int size = myParameter.size();

        for (int i = 0; i < size; i++) {
            string sFullName = myParameter[i]->sFullName;
            vector<string> ParentNames;
            string parentname;
            for (int i = 0; i < sFullName.length(); i++)
            {
                if (sFullName.at(i) != '.') {
                    parentname += sFullName.at(i);
                }
                else {
                    ParentNames.push_back(parentname);
                    parentname = "";
                }
            }
            if (parentname != "")
            {
                ParentNames.push_back(parentname);
            }

            //对此行数据进行处理
            QVector<QVariant>columnData;
            string name = ParentNames[ParentNames.size() - 1];
            bool con = (stricmp(name.c_str(), myParameter[i]->sType.c_str()) == 0);
            columnData << QString::fromStdString(name)
                << (con ? "" : QString::fromStdString(myParameter[i]->sType))
                << QString::fromStdString(myParameter[i]->defaultValue)
                << QString::fromStdString(myParameter[i]->sUnit)
                << QString::fromStdString(myParameter[i]->sDimension);

            if (myParameter[i]->bFocused && !con)columnData << "是";
            else columnData << "";

            columnData
                << QString::fromStdString(myParameter[i]->sFocusName);

            //从根节点开始遍历找节点
            TreeItem* parent = rootItem;
            int searchdepth = 0;
            while (searchdepth < ParentNames.size() - 1)
            {
                string nowname = ParentNames[searchdepth];
                //找当前的parent的子节点中是否有nowname，有就将parent更新，没有就创建新的item然后更新
                bool changeparent = false;
                int nowparent_childs = parent->childCount();
                for (int j = 0; j < nowparent_childs; j++)
                {
                    TreeItem* child = parent->child(j);
                    QVariant namedata = child->data(0);
                    string snamedata = namedata.toString().toStdString();
                    if (nowname == snamedata)
                    {
                        parent = parent->child(j);
                        changeparent = true;
                        break;
                    }
                }

                //代表没有找到，要自己创建一个节点
                if (changeparent == false)
                {
                    QVector<QVariant>columnData;
                    columnData << QString::fromStdString(nowname) << "" << "" << "" << "" << "" << "";
                    TreeItem* newitem = new TreeItem(columnData, flag, parent);
                    parent->appendChild(newitem);
                    parent = newitem;
                }

                searchdepth++;
            }

            //最后往parent添加节点
            parent->appendChild(new TreeItem(columnData, flag, parent));        
        }   
    }
    else {
        vector<myOutputVar*> m_OutputVars = pCyberInfo->GetOutputVars();
        int size = m_OutputVars.size();

        for (int i = 0; i < size; i++) {
            string sFullName = m_OutputVars[i]->sFullName;
            vector<string> ParentNames;
            string parentname;
            for (int i = 0; i < sFullName.length(); i++)
            {
                if (sFullName.at(i) != '.'){
                    parentname += sFullName.at(i);
                }else {
                    ParentNames.push_back(parentname);
                    parentname = "";
                }
            }
            if (parentname != "")
            {
                ParentNames.push_back(parentname);
            }

            //对此行数据进行处理
            QVector<QVariant>columnData;
            string name = ParentNames[ParentNames.size() - 1];
            bool con = (stricmp(name.c_str(), m_OutputVars[i]->sType.c_str()) == 0);
            columnData << QString::fromStdString(name)
                << (con ? "" : QString::fromStdString(m_OutputVars[i]->sType));
            if (m_OutputVars[i]->bFocused && !con)columnData << "是";
            else columnData << "";
            columnData << QString::fromStdString(m_OutputVars[i]->sFocusName);
            
            //从根节点开始遍历找节点
            TreeItem* parent = rootItem;
            int searchdepth = 0;
            while (searchdepth < ParentNames.size() - 1)
            {
                string nowname = ParentNames[searchdepth];
                //找当前的parent的子节点中是否有nowname，有就将parent更新，没有就创建新的item然后更新
                bool changeparent = false;
                int nowparent_childs = parent->childCount();
                for (int j = 0; j < nowparent_childs; j++)
                {
                    TreeItem* child = parent->child(j);
                    QVariant namedata = child->data(0);
                    string snamedata = namedata.toString().toStdString();
                    if (nowname == snamedata)
                    {
                        parent = parent->child(j);
                        changeparent = true;
                        break;
                    }
                }

                //代表没有找到，要自己创建一个节点
                if (changeparent == false)
                {
                    QVector<QVariant>columnData;
                    columnData << QString::fromStdString(nowname) << "" << "" << "";
                    TreeItem* newitem = new TreeItem(columnData, flag, parent);
                    parent->appendChild(newitem);
                    parent = newitem;
                }

                searchdepth++;
            }

            //最后往parent添加节点
            parent->appendChild(new TreeItem(columnData, flag, parent));
        }
    }

    return true;
}

void TreeModel::buildhelp(QVector<TreeItem*>& parents,QVector<int>& indentations,int position, QVector<QVariant>& columnData) {

    if (position > indentations.last()) {

        if (parents.last()->childCount() > 0) {
            //parent的数据不应该从parents.last()->child()获取，例如前一个数据是phi_dd，后一个数据是cylinder.R，就会将R的父节点误设为phi_dd
            parents << parents.last()->child(parents.last()->childCount() - 1);
            indentations << position;
        }
    }
    //否则根据空格数回溯到真正父节点
    //这个回溯方法好像也有点问题，回溯到的也不一定是真正的父节点,做处理使其尽量不要调用到
    else {
        while (position < indentations.last() && parents.count() > 0) {
            parents.pop_back();
            indentations.pop_back();
        }
    }
    parents.last()->appendChild(new TreeItem(columnData, flag, parents.last()));
}

//根据多行字符串序列构建模型数据
void TreeModel::setupModelData(const QStringList& lines, TreeItem* parent)
{
    rootItem->clearItem();
    QVector<TreeItem*> parents;
    QVector<int> indentations;
    parents << parent;
    indentations << 0;
    int number = 0;
    bool conflag = false;
    /// 一行字符串对应模型中一个节点，对应树视图中一行
    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            position++;
        }
        const QString lineData = lines[number].mid(position).trimmed();
        if (!lineData.isEmpty()) {
            //行内的数据按制表符分割
            const QStringList columnStrings =
                lineData.split(QLatin1Char('\t'), QString::SkipEmptyParts);
            QVector<QVariant> columnData;
            int count = columnStrings.count();
            //处理接口信息
            if (columnStrings.front() == "connector"){
                conflag = true;
                ++number;
                continue;
            }
            //
            if (!conflag)
            {
                //输出变量树
                if (flag == false) {
                    if (count == 1)columnData << columnStrings[0];
                    else {
                        //调整顺序，从参数类型、参数名、参数值、(单位)改为参数名、参数值、(单位)
                            /*for (const QString& columnString : columnStrings)
                                columnData << columnString;
                        */
                        columnData << columnStrings[1];
                        if (count == 5)
                            columnData << columnStrings[3];
                        if (columnData.count() < 2)
                            columnData << "";
                        //if (count==4&&columnStrings[2] == "是" )
                        if (columnStrings[2] == "是")
                            columnData << columnStrings[2];
                    }

                    //小于五列的行用空格补齐
                    while (columnData.count() < 4) columnData << "";
                }
                else {
                //参数树
                    /*if (count == 1)
                        columnData << columnStrings[0];
                    else if{
                        columnData << columnStrings[0] << columnStrings[1];
                    }*/
                    for (int i = 0; i < count; i++)
                    {
                        if (columnStrings[i] == "是" && i != bFocusedcol) {
                            while(columnData.count()<bFocusedcol)columnData << "";
                        }
                        columnData << columnStrings[i];
                    }
                        //while(columnData.count() < sDimensioncol)
                       //columnData << "";
                    
                    /*if (count>4&&columnStrings[4] == "关注")
                        columnData << columnStrings[4];*/
                    //小于四列的行用空格补齐
                    while (columnData.count() < 7) columnData << "";
                }

                //根据行开头的空格数决定树形结构
                //若当前行开头空格数大于前一行的空格数，为子节点
                if (position > indentations.last()) {

                    if (parents.last()->childCount() > 0) {
                        parents << parents.last()->child(parents.last()->childCount() - 1);
                        indentations << position;
                    }
                }
                //否则根据空格数回溯到真正父节点
                else {
                    while (position < indentations.last() && parents.count() > 0) {
                        parents.pop_back();
                        indentations.pop_back();
                    }
                }
                parents.last()->appendChild(new TreeItem(columnData, flag, parents.last()));
            }
            else {
                m_cons.push_back(make_pair(columnStrings[0], columnStrings[1]));
            }
        }
        ++number;
    }
}

void TreeModel::ReSet(TreeModel* model,int sta,int end) {
    for (int itr = 0; itr < model->rootItem->childCount(); ++itr) {
        SetZero(model->rootItem->child(itr),sta,end);
    }
    emit dataChanged(model->index(0, sta), model->index(model->rowCount(), end));
}

void TreeModel::SetZero(TreeItem* root,int sta,int end) {
    root->setData(sta, " ");
    root->setData(end, " ");
    for (int itr = 0; itr < root->childCount(); ++itr) {
        SetZero(root->child(itr),sta,end);
    }
}

ListTreeModel::ListTreeModel(const QString& data, QObject* parent)
{
    setrootItem(new TreeItem({ tr("全名"), tr("参数类型"),tr("缺省值"),tr("单位"),tr("维度"),tr("是否关注"),tr("关注名") })) ;
    setflag(f());
    setupModelData(data.split('\n'), getrootItem());

}

outputTreeModel::outputTreeModel(const QString& data, QObject* parent)
{
    setrootItem(new TreeItem({ tr("全名"),tr("类型"),tr("是否关注"),tr("变型") }));
    setflag( f());
    setupModelData(data.split('\n'), getrootItem());
}

//创建下拉列表
QWidget* CheckBoxDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& /* option */,
    const QModelIndex&  index ) const
{
    if (index.sibling(index.row(), 1).data() == " ") return nullptr;
    QComboBox* editor = new QComboBox(parent);
    editor->addItem("是");
    editor->addItem("否");
    
    return editor;
}

void CheckBoxDelegate::setEditorData(QWidget* editor,
    const QModelIndex& index) const
{
    QString text = index.model()->data(index, Qt::EditRole).toString();
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QComboBox* combobox = static_cast<QComboBox*>(editor);
    combobox->setCurrentIndex(combobox->findText(text));
}

void CheckBoxDelegate::setModelData(QWidget* editor,
    QAbstractItemModel* model,
    const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);

    QString text = comboBox->currentText();

    model->setData(index, text, Qt::EditRole);
}
void CheckBoxDelegate::updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}