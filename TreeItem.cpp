#include "TreeItem.h"

TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem* parent)
	: QObject(parent)
{
	m_parent = parent;
	m_data = data;
}

TreeItem::~TreeItem()
{
	qDeleteAll(m_children);
}

qint32 TreeItem::childCount() const
{
	return m_children.count();
}

qint32 TreeItem::childNumber() const
{
	if (m_parent) {
		return m_parent->m_children.indexOf(const_cast<TreeItem*>(this));
	}

	return 0;
}

qint32 TreeItem::columnCount() const
{
	return m_data.count();
}

TreeItem* TreeItem::child(qint32 row)
{
	return m_children.value(row);
}

TreeItem* TreeItem::parent()
{
	return m_parent;
}

QVariant TreeItem::data(int column) const
{
	return m_data.value(column);
}

bool TreeItem::setData(int column, const QVariant &value)
{
	if (column < 0 || column >= m_data.size()) {
		return false;
	}

	m_data[column] = value;
	return true;
}

bool TreeItem::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > m_children.size()) {
		return false;
	}

	for (int row = 0; row < count; ++row) {
		QVector<QVariant> data(columns);
		TreeItem *item = new TreeItem(data, this);
		m_children.insert(position, item);
	}

	return true;
}
