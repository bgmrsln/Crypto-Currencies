#ifndef MYCLASS_H
#define MYCLASS_H
#include <QtGui>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLabel>

class MyClass : public QTableWidget
{
    Q_OBJECT
public:
    MyClass(QWidget *parent=0);

public slots:
    void replyFinished(QNetworkReply *reply);
    void secondreplyFinished(QNetworkReply *reply2);
private:
    QNetworkAccessManager *manager;
    QTableWidget *table;
    QString cryptoListName;
    QVector<QString> names;
    QVector<QString> ids;

};

#endif // MYCLASS_H
