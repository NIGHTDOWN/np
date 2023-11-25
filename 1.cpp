#include <QCoreApplication>  
#include <QDesktopServices>  
  
int main(int argc, char *argv[])  
{  
    QCoreApplication a(argc, argv);  
  
    QString url = "https://www.huyintong.com";  
    QDesktopServices::openUrl(QUrl(url));  
  
    return a.exec();  
}