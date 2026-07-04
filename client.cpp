#include "client.h"
#include "ui_client.h"
#include <QTcpSocket>
#include <QCoro/QCoroNetwork> // Required to await QTcpSocket
#include <QMessageBox>        // Required for UI feedback
#include <QDebug>

client::client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::client)
{
    ui->setupUi(this);
}

client::~client()
{
    delete ui;
}

QCoro::Task<void> client::sendNetworkMessage(QString host, quint16 port, QString message)
{
    QTcpSocket socket;

    qDebug() << "Attempting to connect to" << host << ":" << port;

    socket.connectToHost(host, port);

    bool connected = co_await qCoro(socket).waitForConnected();

    if (!connected) {
        QMessageBox::critical(this, "Connection Failed", socket.errorString());
        co_return;
    }

    qDebug() << "Connected successfully.";

    socket.write(message.toUtf8());

    socket.disconnectFromHost();

    co_await qCoro(socket).waitForDisconnected();

    QMessageBox::information(this, "Success", "Message sent successfully!");
}

void client::on_sendButton_clicked()
{
    QString host = "127.0.0.1";
    quint16 port = 54321;
    QString message = ui->messagelineEdit->text();

    sendNetworkMessage(host, port, message);
}
