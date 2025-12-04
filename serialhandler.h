#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QTimer>

class SerialHandler : public QObject
{
    Q_OBJECT
public:
    explicit SerialHandler(QObject *parent = nullptr);

private:
    QTimer timer;
    double phase[5];
    qint64 startTime;

    double freqs[5];

signals:
    void newData(int channel, qint64 timestamp, double value);

private slots:
    void generateFakeData();
};

#endif // SERIALHANDLER_H
