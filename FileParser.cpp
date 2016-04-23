#include "FileParser.h"

#include <QFile>
#include <QTextStream>

FileParser::FileParser(QObject *parent) : QObject(parent)
{
	m_filename = QString();
}

void FileParser::setFilename(const QString& filename)
{
	m_filename = filename;
}

FileParser::ParserErrors FileParser::parse(TreeModel* model)
{
	if (m_filename.isEmpty()) {
		return FileNameIsEmpty;
	}
	
	QFile file(m_filename);
	
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return FileOpenError;
	}
	
	QTextStream in(&file);
	QString line = in.readLine();
	while (!line.isNull()) {

		ParserErrors error = processLine(line, model);

		if (error != Successful) {
			file.close();
			return error;
		}
		
		line = in.readLine();
	}
	
	file.close();
	return Successful;
}

QString FileParser::errorString(const FileParser::ParserErrors& error) const
{
	switch (error) {
		case FileNameIsEmpty:
			return tr("File name is empty");
		case FileOpenError:
			return tr("File open error");
		case FileIsNotCorrect:
			return tr("File isn't correct");
		default:
			return tr("Successful");
	}
}

FileParser::ParserErrors FileParser::processLine(const QString& line, TreeModel* model)
{
	QStringList rawData = line.split(";");

	if (rawData.count() < 3) {
		return FileIsNotCorrect;
	}

	bool ok;
	quint32 idHex = rawData.at(0).toInt(&ok, 16); // идентификатор записи

	if (!ok) {
		return FileIsNotCorrect;
	}

	QString path = rawData.at(1); // текстовое описание
	QStringList nodesList = path.split("/");

	if (nodesList.count() < 3) {
		return FileIsNotCorrect;
	}

	QString value = rawData.at(2); // значение

	model->addOperation(idHex, nodesList, value.toDouble());

	return Successful;
}
