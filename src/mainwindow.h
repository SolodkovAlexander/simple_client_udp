#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QUdpSocket;

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void onConnect();
    void onDisconnected();
    void onSendMessage();
    void onReadyRead();

private:
    void initCombosAddress();
    void stopClient();
    void log(const QString &message);

private:
    Ui::MainWindow *ui;
    QUdpSocket *client;
};

#endif // MAINWINDOW_H
