#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include "TreeItem.h"

class TreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit TreeModel(const QStringList &headers, QObject *parent = 0);
	virtual ~TreeModel();

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

	// Basic functionality:
	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
	bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
	bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
	bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;

	QModelIndexList match(const QModelIndex &start, int role,
						  const QVariant &value, int hits,
						  Qt::MatchFlags flags) const override;


	void addOperation(quint32 idHex, const QStringList& treePath, const qreal& value);

private:
	void setupModelData(const QStringList &lines, TreeItem *parent);
	TreeItem* getItem(const QModelIndex &index) const;

private:
	TreeItem* m_root;
};

#endif // TREEMODEL_H
