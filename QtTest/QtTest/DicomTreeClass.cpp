#include "DicomTreeClass.h"

TreeItem::TreeItem(const QList<QVariant> &data, int level, void * internalData, TreeItem *parent)
{
	m_parentItem = parent;
	m_itemData = data;
	this->level = level;
	this->internalData = internalData;
}

TreeItem::~TreeItem()
{
	qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
	m_childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
	return m_childItems.value(row);
}

int TreeItem::childCount() const
{
	return m_childItems.count();
}

int TreeItem::columnCount() const
{
	return m_itemData.count();
}

QVariant TreeItem::data(int column) const
{
	return m_itemData.value(column);
}

TreeItem *TreeItem::parentItem()
{
	return m_parentItem;
}

int TreeItem::row() const
{
	if (m_parentItem)
		return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

	return 0;
}


DicomTreeClass::DicomTreeClass( QObject *parent): QAbstractItemModel(parent)
{
	QList<QVariant> rootData;
	rootData << "Patient";// << "Summary";
	rootItem = new TreeItem(rootData,0);
	//setupModelData(data, rootItem);
}

void DicomTreeClass::setupModelData(list<DicomPatient*> PatientsList, TreeItem * parent)
{
	
	
	for (auto const& patient : PatientsList) {
		addPatient(patient, parent);
				
	}
	
	
}

void DicomTreeClass::addPatient(DicomPatient * patient, TreeItem * parent)
{
	
	QString tmp = QString::fromStdString(patient->name);
	QList<QVariant> columnData;
	columnData << tmp;

	TreeItem *patientNode= new TreeItem(columnData,1,patient,parent);
	parent->appendChild(patientNode);
	addPatientStydu(patient, patientNode);

	
}

void DicomTreeClass::addPatientStydu(DicomPatient * patient, TreeItem * parent)
{
	for (auto const& study : patient->StudyList) {
		
		string DisplayName = "Study: " + study->StudyDescription + " " + study->StudyDate + " " + study->StudyID;
		QString tmp = QString::fromStdString(DisplayName);
		QList<QVariant> columnData;
		columnData << tmp;
		TreeItem *studyNode = new TreeItem(columnData,2,study, parent);
		parent->appendChild(studyNode);
		addStyduSeries(study, studyNode);
	}
	
}

void DicomTreeClass::addStyduSeries(DicomStudy * study, TreeItem * parent)
{
	for (auto const& series : study->SeriesList) {
		string DisplayName = "Series: " + series->SeriesNumber +
										": " + series->SeriesModality + ": " + series->ScanOptions + ": " + series->SeriesDescription + " ("+series->RecordCount + " records)";
		QString tmp = QString::fromStdString(DisplayName);
		QList<QVariant> columnData;
		columnData << tmp;
		TreeItem *seriesNode = new TreeItem(columnData,3, series,parent);
		parent->appendChild(seriesNode);
	}
}


DicomTreeClass::~DicomTreeClass()
{
	delete rootItem;
}

QVariant DicomTreeClass::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

	return item->data(index.column());
}

Qt::ItemFlags DicomTreeClass::flags(const QModelIndex & index) const
{
	if (!index.isValid())
		return 0;

	return QAbstractItemModel::flags(index);
}

QVariant DicomTreeClass::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}

QModelIndex DicomTreeClass::index(int row, int column, const QModelIndex & parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	TreeItem *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	TreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex DicomTreeClass::parent(const QModelIndex & index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
	TreeItem *parentItem = childItem->parentItem();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int DicomTreeClass::rowCount(const QModelIndex & parent) const
{
	TreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}

int DicomTreeClass::columnCount(const QModelIndex & parent) const
{
	if (parent.isValid())
		return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
	else
		return rootItem->columnCount();
}

