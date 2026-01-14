#include "gettimestampswork.h"
#include "globalVariables.h"
#include "utf8.h"

#include <QDateTime>
#include <QThread>
#include <QDebug>

getTimestampsWork::getTimestampsWork(QObject *parent) : QObject(parent)
{
    
}

void getTimestampsWork::startSending(QString url)
{
    client = new httpClient();
    connect(client, &httpClient::requestSuccess, this, &getTimestampsWork::handleSuccess);
    connect(client, &httpClient::requestError, this, &getTimestampsWork::handleError);
    connect(client, &httpClient::sendFail, this, &getTimestampsWork::handleFail);
    
    client->get(url);
}

void getTimestampsWork::handleSuccess(const QByteArray &response)
{
//    qint64 clientTimestamps = QDateTime::currentSecsSinceEpoch();
    qint64 clientTimestamps = QDateTime::currentMSecsSinceEpoch();
    qDebug()<<"clientTimestamps "<<clientTimestamps;
    qint64 serverTimestamps = response.toLongLong();
    qDebug()<<"serverTimestamps "<<serverTimestamps;
    timeDifference = serverTimestamps - clientTimestamps;
    qDebug()<<"timeDifference: "<<timeDifference;
    
    emit calculateSuccess();
    QThread::msleep(500);
    emit workFinished();
}

void getTimestampsWork::handleError(const QString &errorString)
{
    qDebug()<<"send error: "<<errorString;
}

void getTimestampsWork::handleFail()
{
    qDebug()<<"重传次数过多";
    
    emit getTimeStampsFailed();
    emit workFinished();
}
