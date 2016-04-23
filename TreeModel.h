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

	QVariant headerData(int section, Qt::Orientation orientation,
						int role = Qt::DisplayRole) const override;

	QModelIndex index(int row, int column,
					  const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	QVariant data(const QModelIndex &index,
				  int role = Qt::DisplayRole) const override;

	/**
	  * Данная функция целиком взята из оригинальных исходников QtCore
	  * Оригинальная функция не может сортировать рекурсивно по дереву,
	  * находясь в другой колонке (column). Для того, чтобы это исправить
	  * в было изменено поведенение функции для осуществления соответствующего
	  * поиска.
	  *
	  * Был добавлен дополнительный индекс hierarh_idx, относительно которого
	  * стал строиться рекурсивный обход.
	  *
	  * Комментарии на английском языке - оригинал.
	**/
	QModelIndexList match(const QModelIndex &start, int role,
						  const QVariant &value, int hits,
						  Qt::MatchFlags flags) const override;


	void addOperation(quint32 idHex, const QStringList& treePath,
					  const qreal& value);

	/**
	  * Исходная задача не предполагает какого либо изменения модели.
	  * В связи с этим функции, предназначенные для этого, не были реализованы.
	  *
	**/

private:
	void setupModelData(const QStringList &lines, TreeItem *parent);
	TreeItem* getItem(const QModelIndex &index) const;

private:
	TreeItem* m_root;
};

#endif // TREEMODEL_H
