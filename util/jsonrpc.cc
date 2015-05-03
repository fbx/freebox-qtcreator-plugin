/*
  Freebox QtCreator plugin for QML application development.

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not see
  http://www.gnu.org/licenses/lgpl-2.1.html.

  Copyright (c) 2014, Freebox SAS, See AUTHORS for details.
*/
#include <QObject>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "jsonrpc.hh"

namespace Freebox {
namespace Util {

uint JsonRpcRequest::s_next_id = 0;

JsonRpcRequest::JsonRpcRequest(const QUrl &endpoint,
                               const QString &method,
                               const QJsonValue &arg,
                               QObject *owner) :
    QObject(owner),
    m_id(s_next_id++)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QJsonObject rpc;
    rpc[QStringLiteral("jsonrpc")] = QStringLiteral("2.0");
    rpc[QStringLiteral("method")] = method;
    rpc[QStringLiteral("params")] = arg;
    rpc[QStringLiteral("id")] = QString::number(m_id);

    QJsonDocument body(rpc);

    QNetworkRequest req(endpoint);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    m_reply = manager->post(req, body.toJson());

    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
}

JsonRpcRequest::~JsonRpcRequest()
{
}

void JsonRpcRequest::replyFinished()
{
    QJsonDocument doc = QJsonDocument::fromJson(m_reply->readAll());
    QJsonObject root = doc.object();

    if (m_reply->error() != QNetworkReply::NoError) {
        QJsonObject e;
        e[QStringLiteral("code")] = -32000;
        e[QStringLiteral("message")] = QString(QLatin1String("Network error: ") + m_reply->errorString());
        emit error(e);
        return;
    }

    if (root[QStringLiteral("jsonrpc")].toString() != QStringLiteral("2.0")) {
        QJsonObject e;
        e[QStringLiteral("message")] = QStringLiteral("Bad JSON RPC version in response");
        emit error(e);
        return;
    }

    if (root[QStringLiteral("id")] != QString::number(m_id)) {
        QJsonObject e;
        e[QStringLiteral("message")] = QStringLiteral("Bad ID in response");
        emit error(e);
        return;
    }

    if (root.contains(QStringLiteral("error"))) {
        emit error(root[QStringLiteral("error")].toObject());
        return;
    }

    emit done(root[QStringLiteral("result")]);
}

} // namespace Util
} // namespace Freebox
