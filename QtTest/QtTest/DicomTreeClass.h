#pragma once
#include <qabstractitemmodel.h>
#include <QList>
#include <QVariant>
#include "DicomSorterClass.h"



class TreeItem
{
public:
    explicit TreeItem(const QList<QVariant> &data, int level, void * internalData = nullptr, TreeItem *parentItem = 0);
	//explicit TreeItem(const QVariant &data, TreeItem *parentItem = 0);
	~TreeItem();
	int level;
	void * internalData;

	void appendChild(TreeItem *child);

	TreeItem *child(int row);
	int childCount() const;
	int columnCount() const;
	
	QVariant data(int column) const;
	int row() const;
	TreeItem *parentItem();

private:
	QList<TreeItem*> m_childItems;
	QList<QVariant> m_itemData;
	//QVariant m_itemData;
	TreeItem *m_parentItem;
};

class DicomTreeClass :	public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit DicomTreeClass(QObject *parent);
	virtual ~DicomTreeClass();

	
	QVariant data(const QModelIndex &index, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column,
		const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	void setupModelData(list<DicomPatient*> PatientsList, TreeItem *parent);

	void addPatient(DicomPatient * patient, TreeItem * parent);
	void addPatientStydu(DicomPatient * patient,TreeItem * parent);
	void addStyduSeries(DicomStudy * study, TreeItem * parent);

	TreeItem *rootItem;
};

