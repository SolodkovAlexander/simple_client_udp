#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHostAddress>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QNetworkDatagram>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    client(new QUdpSocket(this))
{
    ui->setupUi(this);

    //fill combos with addresses
    initCombosAddress();

    //make connections
    connect(ui->buttonConnect, &QPushButton::clicked, this, &MainWindow::onConnect);
    connect(ui->buttonDisconnect, &QPushButton::clicked, this, &MainWindow::stopClient);
    connect(ui->buttonSendMessage, &QPushButton::clicked, this, &MainWindow::onSendMessage);
    connect(client, &QUdpSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(client, &QUdpSocket::readyRead, this, &MainWindow::onReadyRead);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnect()
{
    //stop client
    if (client->isOpen() || client->isValid())
        stopClient();

    //start client
    QString addressStr = ui->comboAddress->currentText();
    quint16 port = ui->editPort->text().toUShort();
    if (client->bind(QHostAddress(addressStr), port))
    {
        log(QString("Client started to %1:%2.")
            .arg(addressStr,
                 QString::number(port)));
    }
    else
    {
        log(QString("Error start client to %1:%2 (%3)!")
            .arg(addressStr,
                 QString::number(port),
                 client->errorString()));
    }
}

void MainWindow::onDisconnected()
{
    log(QString("Client (%1:%2) disconnected.")
        .arg(client->localAddress().toString(),
             QString::number(client->localPort())));
}

void MainWindow::onSendMessage()
{
    QString message = ui->editMessage->toPlainText();
    QString addressStr = ui->comboSendToAddress->currentText();
    quint16 port = ui->editSendToPort->text().toUShort();

    log(QString("Client (%1:%2) started send message (\"%3\") [%4 bytes] to (%5:%6).")
        .arg(client->localAddress().toString(),
             QString::number(client->localPort()),
             message,
             QString::number(message.toUtf8().count()),
             addressStr,
             QString::number(port)));
    qint64 sentBytes = client->writeDatagram(message.toUtf8(),
                                             QHostAddress(addressStr),
                                             port);
    if (sentBytes != -1)
    {
        log(QString("Client finished send message [%1 bytes].")
            .arg(sentBytes));
    }
    else
    {
        log(QString("Error send message to %1:%2 (%3)!")
            .arg(addressStr,
                 QString::number(port),
                 client->errorString()));
    }
}

void MainWindow::onReadyRead()
{
    while (client->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = client->receiveDatagram();
        QByteArray datagramData = datagram.data();
        log(QString("Datagram (\"%1\") [%2 bytes] received from client (%3:%4).")
            .arg(QString::fromUtf8(datagramData),
                 QString::number(datagramData.count()),
                 datagram.senderAddress().toString(),
                 QString::number(datagram.senderPort())));
    }
}

void MainWindow::initCombosAddress()
{
    for (const QHostAddress &address : QNetworkInterface::allAddresses())
    {
        if (address.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv4Protocol)
            continue;

        QString addressStr = address.toString();
        ui->comboAddress->addItem(addressStr);
        ui->comboSendToAddress->addItem(addressStr);
    }
}

void MainWindow::stopClient()
{
    //stop socket
    QString addressStr = client->localAddress().toString();
    quint16 port = client->localPort();
    client->close();

    log(QString("Client (%1:%2) stopped.")
        .arg(addressStr,
             QString::number(port)));
}

void MainWindow::log(const QString &message)
{
    ui->editLog->appendPlainText(message);
}
