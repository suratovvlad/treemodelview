#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QVariant>

class TreeItem : public QObject
{
	Q_OBJECT

public:

	explicit TreeItem(const QVector<QVariant>& data, TreeItem *parent = 0);
	virtual ~TreeItem();

	qint32 childCount() const;
	qint32 childNumber() const;
	qint32 columnCount() const;
	qint32 row() const;

	TreeItem* child(qint32 row);
	TreeItem* parent();

	QVariant data(int column) const;
	bool setData(int column, const QVariant &value);

	bool insertChildren(int position, int count, int columns);

private:
	TreeItem* m_parent;
	QList<TreeItem*> m_children;
	QVector<QVariant> m_data;
};

#endif // TREEITEM_H
