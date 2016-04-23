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

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0) {
		return QModelIndex();
	}

	TreeItem* parentItem = getItem(parent);

	TreeItem* childItem = parentItem->child(row);
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

	return createIndex(parentItem->childCount(), 0, parentItem);
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

	return QAbstractItemModel::flags(index);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	TreeItem *item = getItem(index);

	switch (role) {
		case Qt::DisplayRole:
			return item->data(index.column());
		default:
			break;
	}

	return QVariant();
}

/**
  * Комментарии на английском языке - оригинал.
 */
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

			// Добавляем дополнительный индекс, относительно которого
			// построим рекурсивный обход
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
			// В следующих двух строках изменяем idx на hierarh_idx
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
	// 0xabcdef - представление числа - уникального индекса idHex

	// Для листов дерева индекс самый маленький
	quint32 ef = idHex & 0x0000ff;
	// Для средних узлов - побольше
	quint32 cd = (idHex) & 0x00ff00;
	// Самый большой индекс для узлов верхнего уровня
	quint32 ab = (idHex) & 0xff0000;

	// Номера колонок
	qint32 titleColumn = 0;
	qint32 valueColumn = 1;
	qint32 indexColumn = 2;

	TreeItem* parentItem = getItem(QModelIndex());

	beginInsertRows(QModelIndex(), 0, parentItem->childCount());

	// Смотрим, есть ли верхний узел с таким же индексом
	QModelIndexList items = match(this->index(0, indexColumn), Qt::DisplayRole, ab, -1, Qt::MatchRecursive);
	if (items.isEmpty()) {

		// Если такого узла нет, то вставляем новый узел
		parentItem->insertChildren(parentItem->childCount(), 1, 3);

		// Вставляем индекс для самого верхнего узла (адрес)
		parentItem->child(parentItem->childCount() - 1)->setData(indexColumn, ab);
		// Вставляем имя узла
		parentItem->child(parentItem->childCount() - 1)->setData(titleColumn, treePath.at(0));

		// Изменяем текущего родителя на только что созданного потомка
		parentItem = parentItem->child(parentItem->childCount() - 1);

	} else {
		// Если такой узел есть, то изменяем родителя на него
		parentItem = parentItem->child(items.at(0).row());
	}

	// Далее аналогичные шаги для узлов среднего уровня
	items = match(this->index(0,indexColumn), Qt::DisplayRole, cd, -1, Qt::MatchRecursive);

	if (items.isEmpty()) {

		// Если такого узла нет, то вставляем новый узел
		parentItem->insertChildren(parentItem->childCount(), 1, 3);

		// Вставляем индекс
		parentItem->child(parentItem->childCount() - 1)->setData(indexColumn, cd);
		// Вставляем имя узла
		parentItem->child(parentItem->childCount() - 1)->setData(titleColumn, treePath.at(1));

		// Изменяем текущего родителя на только что созданного потомка
		parentItem = parentItem->child(parentItem->childCount() - 1);

	} else {
		// Если такой узел есть, то изменяем родителя на него
		parentItem = parentItem->child(items.at(0).row());
	}

	// Если файл составлен корректно и все номера узлов уникальны, то
	// match должен вернуть пустой список индексов.
	// В противном случае, повторный узел мы не вставляем
	items = match(this->index(0,indexColumn), Qt::DisplayRole, ef, -1, Qt::MatchRecursive);

	if (items.isEmpty()) {
		// Вставляем новый узел
		parentItem->insertChildren(parentItem->childCount(), 1, 3);

		// Вставляем индекс
		parentItem->child(parentItem->childCount() - 1)->setData(indexColumn, ef);
		// Вставляем имя узла
		parentItem->child(parentItem->childCount() - 1)->setData(titleColumn, treePath.at(2));
		// Вставляем значение узла узла
		parentItem->child(parentItem->childCount() - 1)->setData(valueColumn, value);
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
