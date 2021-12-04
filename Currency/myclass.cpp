#include "myclass.h"
#include <QRegExp>
#include <QHBoxLayout>
#include <QFile>
#include <QVector>



MyClass::MyClass(QWidget *parent) : QTableWidget(parent)
{
    //list name is read from env variable: MYCRYPTOCONVERT
    cryptoListName=qgetenv("MYCRYPTOCONVERT");
    manager = new QNetworkAccessManager(this) ;
    connect(manager, SIGNAL(finished(QNetworkReply *)),this, SLOT(replyFinished(QNetworkReply *)));
    //this url has the information about crypto-currency names and symbols.
    manager->get(QNetworkRequest(QUrl("https://api.coingecko.com/api/v3/coins/list?include_platform=false")));
    QHBoxLayout *layout = new QHBoxLayout;
    //table is created
    table = new QTableWidget();
    layout->addWidget(table);

    //table's write permission is taken
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setWindowTitle("Crypto Currency");
    setLayout(layout);
}

//this function does the symbol-name translation. Then creates the url and makes another connection.
void MyClass::replyFinished(QNetworkReply *reply)  {
    //currency names are read from the file
     QFile cryptoFile(cryptoListName);
     if (!cryptoFile.open(QIODevice::ReadOnly | QIODevice::Text))
             return;
     int pos = 0;
     int noflines=0;
     // read the data fetched from the web site
     QString data = (QString) reply->readAll();

    //if symbol is given, id is taken instead.
     while(!cryptoFile.atEnd()){
         QString line= cryptoFile.readLine();
         line=line.simplified();
         QRegExp rx("\"id\":\"([^\"]+)\",\"symbol\":\"([^\"]+)\",\"name\":\""+line+"\"");
         QRegExp rx2("\"id\":\"([^\"]+)\",\"symbol\":\""+line+"\",\"name\":\"([^\"]+)\"");

         if(rx.indexIn(data,pos) != -1){
            names.append(line);
            ids.append(rx.cap(1));
            noflines++;
         }else if(rx2.indexIn(data,pos) != -1){
             ids.append(rx2.cap(1));
             names.append(rx2.cap(2));
             noflines++;
         }


     }
     //url is created
     QString link= "https://api.coingecko.com/api/v3/simple/price?ids=";
     for(QString id: ids ){
         link += id+"%2C";
     }
     link+= "&vs_currencies=usd%2Ceur%2Cgbp";

     //first row of the table
     table->setRowCount(noflines+1);

     table->setColumnCount(4);
     QTableWidgetItem *eitem = new QTableWidgetItem();
     table-> setItem(0,0,eitem);

     QTableWidgetItem *itemusd = new QTableWidgetItem();
     table-> setItem(0,1,itemusd);
     itemusd->setText("USD");

     QTableWidgetItem *itemeur = new QTableWidgetItem();
     table-> setItem(0,2,itemeur);
     itemeur->setText("EUR");

     QTableWidgetItem *itemgbp = new QTableWidgetItem();
     table-> setItem(0,3,itemgbp);
     itemgbp->setText("GBP");

     manager = new QNetworkAccessManager(this) ;
     //second connection to take data
     connect(manager, SIGNAL(finished(QNetworkReply *)),this, SLOT(secondreplyFinished(QNetworkReply *)));    
     manager->get(QNetworkRequest(QUrl(link)));

}

void MyClass::secondreplyFinished(QNetworkReply *reply2){
    QString data2 = (QString) reply2->readAll();

    //create table with noflines+1 rows and 4 columns.
    for(int i=0;i< names.size();i++){
        QTableWidgetItem *item = new QTableWidgetItem();
        table-> setItem(i+1,0,item);      
        item->setText(names[i]);
        int pos=0;
        QRegExp rx("\""+ids[i]+"\":\\{\"usd\":([^\"]+),\"eur\":([^\"]+),\"gbp\":([^\"\\}]+)\\}");
        //if crypto id is found, its eur,gbp,usd equivalents are put to the table
        if(rx.indexIn(data2,pos) != -1){
            for(int j=1;j<4;j++){
                QTableWidgetItem *itemn = new QTableWidgetItem();
                table-> setItem(i+1,j,itemn);
                itemn->setText(rx.cap(j));
            }
        }
    }
    //column widths are set
    table->resizeColumnsToContents();
    //initial table size is set
    table->setMinimumSize((table->columnWidth(0)+table->columnWidth(1)+table->columnWidth(2)+table->columnWidth(3)+table->columnWidth(3)),(names.size()+2)*(table->rowHeight(0)));
}
