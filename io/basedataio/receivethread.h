#ifndef RECEVETHREAD_H
#define RECEVETHREAD_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>

class ReceiveThread : public QThread
{
    Q_OBJECT
public:
    explicit ReceiveThread(QObject *parent = nullptr);
    ~ReceiveThread() override;

    void startReceive(const QString &portName, int waitTimeout, const QString &response);

signals:
    void request(const QString &s);
    void error(const QString &s);
    void timeout(const QString &s);

private:
    void run() override;

    QString m_portName;
    QString m_response;
    int m_waitTimeout = 0;
    QMutex m_mutex;
    bool m_quit = false;
};

#endif // RECEVETHREAD_H
