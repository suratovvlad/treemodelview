#include "TreeModel.h"

TreeModel::TreeModel(const QStringList& headers, QObject *parent)
	: QAbstractItemModel(parent)
{
	QVector<QVariant> rootData;
	foreach (QString header, headers) {
		rootData << header;
	}
	m_root = new TreeItem(rootData);
}

TreeModel::~TreeModel()
{
	delete m_root;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal
			&& role == Qt::DisplayRole) {
		return m_root->data(section);
	}

	return QVariant();
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	if (orientation != Qt::Horizontal
			|| role != Qt::EditRole) {
		return false;
	}

	bool result = m_root->setData(section, value);

	if (result) {
		emit headerDataChanged(orientation, section, section);
	}

	return result;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	// TODO
	if (parent.isValid() && parent.column() != 0) {
		return QModelIndex();
	}

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	TreeItem *parentItem = getItem(parent);

	TreeItem *childItem = parentItem->child(row);
	if (childItem) {
		return createIndex(row, column, childItem);
	}
	else {
		return QModelIndex();
	}
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	TreeItem* childItem = getItem(index);
	TreeItem* parentItem = childItem->parent();

	if (parentItem == m_root) {
		return QModelIndex();
	}

	return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{	
	TreeItem* parentItem = getItem(parent);
	return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return m_root->columnCount();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	TreeItem *item = getItem(index);

	switch (role) {
		case Qt::EditRole:
		case Qt::DisplayRole:
			return item->data(index.column());
		default:
			break;
	}

	return QVariant();
}

bool TreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role != Qt::EditRole) {
		return false;
	}

	TreeItem *item = getItem(index);
	bool result = item->setData(index.column(), value);

	if (result) {
		emit dataChanged(index, index);
	}

	return result;
}

bool TreeModel::insertColumns(int position, int columns, const QModelIndex& parent)
{
	bool success;

	beginInsertColumns(parent, position, position + columns - 1);
	success = m_root->insertColumns(position, columns);
	endInsertColumns();

	return success;
}

bool TreeModel::TreeModel::insertRows(int position, int rows, const QModelIndex& parent)
{
	TreeItem *parentItem = getItem(parent);
	bool success;

	beginInsertRows(parent, position, position + rows - 1);
	success = parentItem->insertChildren(position, rows, m_root->columnCount());
	endInsertRows();

	return success;
}

bool TreeModel::removeColumns(int position, int columns, const QModelIndex& parent)
{
	bool success;

	beginRemoveColumns(parent, position, position + columns - 1);
	success = m_root->removeColumns(position, columns);
	endRemoveColumns();

	if (m_root->columnCount() == 0) {
		removeRows(0, rowCount());
	}

	return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex& parent)
{
	TreeItem *parentItem = getItem(parent);
	bool success = true;

	beginRemoveRows(parent, position, position + rows - 1);
	success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

QModelIndexList TreeModel::match(const QModelIndex& start, int role, const QVariant& value, int hits, Qt::MatchFlags flags) const
{
	QModelIndexList result;
	uint matchType = flags & 0x0F;
	Qt::CaseSensitivity cs = flags & Qt::MatchCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
	bool recurse = flags & Qt::MatchRecursive;
	bool wrap = flags & Qt::MatchWrap;
	bool allHits = (hits == -1);
	QString text; // only convert to a string if it is needed
	QModelIndex p = parent(start);
	int from = start.row();
	int to = rowCount(p);

	// iterates twice if wrapping
	for (int i = 0; (wrap && i < 2) || (!wrap && i < 1); ++i) {
		for (int r = from; (r < to) && (allHits || result.count() < hits); ++r) {
			QModelIndex idx = index(r, start.column(), p);
			QModelIndex hierarh_idx = index(r, 0, p);
			if (!idx.isValid())
				continue;
			QVariant v = data(idx, role);
			// QVariant based matching
			if (matchType == Qt::MatchExactly) {
				if (value == v)
					result.append(idx);
			} else { // QString based matching
				if (text.isEmpty()) // lazy conversion
					text = value.toString();
				QString t = v.toString();
				switch (matchType) {
					case Qt::MatchRegExp:
						if (QRegExp(text, cs).exactMatch(t))
							result.append(idx);
						break;
					case Qt::MatchWildcard:
						if (QRegExp(text, cs, QRegExp::Wildcard).exactMatch(t))
							result.append(idx);
						break;
					case Qt::MatchStartsWith:
						if (t.startsWith(text, cs))
							result.append(idx);
						break;
					case Qt::MatchEndsWith:
						if (t.endsWith(text, cs))
							result.append(idx);
						break;
					case Qt::MatchFixedString:
						if (t.compare(text, cs) == 0)
							result.append(idx);
						break;
					case Qt::MatchContains:
					default:
						if (t.contains(text, cs))
							result.append(idx);
				}
			}
			if (recurse && hasChildren(hierarh_idx)) { // search the hierarchy
				result += match(index(0, idx.column(), hierarh_idx), role,
								(text.isEmpty() ? value : text),
								(allHits ? -1 : hits - result.count()), flags);
			}
		}
		// prepare for the next iteration
		from = 0;
		to = start.row();
	}
	return result;
}

void TreeModel::addOperation(quint32 idHex, const QStringList& treePath, const qreal& value)
{
	// 0xabcdef - представление числа
	quint32 ef = idHex & 0x0000ff;
	quint32 cd = (idHex) & 0x00ff00;
	quint32 ab = (idHex) & 0xff0000;

	qint32 titleColumn = 0;
	qint32 valueColumn = 1;
	qint32 indexColumn = 2;


	TreeItem* parentItem = getItem(QModelIndex());
	beginInsertRows(QModelIndex(), 0, parentItem->childCount());

	QModelIndexList items = match(this->index(0, indexColumn), Qt::DisplayRole, ab, -1, Qt::MatchRecursive);
	if (items.isEmpty()) {

		parentItem->insertChildren(parentItem->childCount(), 1, 3);
		parentItem->child(parentItem->childCount() - 1)->setData(indexColumn, ab); // insert id for address
		parentItem->child(parentItem->childCount() - 1)->setData(titleColumn, treePath.at(0)); // insert name for address
		parentItem = parentItem->child(parentItem->childCount() - 1);
	} else {
		parentItem = parentItem->child(items.at(0).row());
	}

	items = match(this->index(0,indexColumn), Qt::DisplayRole, cd, -1, Qt::MatchRecursive);

	if (items.isEmpty()) {
		parentItem->insertChildren(parentItem->childCount(), 1, 3);
		parentItem->child(parentItem->childCount() - 1)->setData(indexColumn, cd); // insert id for node
		parentItem->child(parentItem->childCount() - 1)->setData(titleColumn, treePath.at(1)); // insert name for node
		parentItem = parentItem->child(parentItem->childCount() - 1);
	} else {
		parentItem = parentItem->child(items.at(0).row());
	}

	items = match(this->index(0,indexColumn), Qt::DisplayRole, ef, -1, Qt::MatchRecursive);

	if (items.isEmpty()) {
		parentItem->insertChildren(parentItem->childCount(), 1, 3);
		parentItem->child(parentItem->childCount() - 1)->setData(indexColumn, ef); // insert id for operation
		parentItem->child(parentItem->childCount() - 1)->setData(titleColumn, treePath.at(2)); // insert name for operation
		parentItem->child(parentItem->childCount() - 1)->setData(valueColumn, value); // insert value for operation
	}

	endInsertRows();
}

TreeItem* TreeModel::TreeModel::getItem(const QModelIndex& index) const
{
	if (index.isValid()) {
		TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
		if (item) {
			return item;
		}
	}

	return m_root;
}
