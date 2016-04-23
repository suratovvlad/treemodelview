#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "TreeModel.h"

class FileParser : public QObject
{
	Q_OBJECT

public:
	typedef enum ParserErrors {
		Successful = 0,
		FileNameIsEmpty,
		FileOpenError,
		FileIsNotCorrect
	} ParserErrors ;

	explicit FileParser(QObject *parent = 0);
	
	void setFilename(const QString& filename);
	ParserErrors parse(TreeModel* model);
	QString errorString(const ParserErrors& error) const;

private:
	ParserErrors processLine(const QString& line, TreeModel* model);

private:
	QString m_filename;
};

#endif // FILEPARSER_H
