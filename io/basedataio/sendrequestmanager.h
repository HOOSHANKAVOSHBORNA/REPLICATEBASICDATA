#ifndef SENDREQUESTMANAGER_H
#define SENDREQUESTMANAGER_H

#include <QByteArray>
#include <QObject>
#include <QSerialPort>
#include <QTextStream>
#include <QTimer>

#include "sendthread.h"

class SendRequestManager : public QObject
{
    Q_OBJECT
public:
    explicit SendRequestManager(QObject *parent = nullptr);
private slots:
    void handleResponse(const QString &);
    void handleError(const QString &);
    void handleTimeout(const QString &);
private:
    QTextStream m_standardOutput;
    SendThread m_sendThread;
    QList<QString> m_serialPortNameList;
    int m_curentPort;
};

#endif // SENDREQUESTMANAGER_H
