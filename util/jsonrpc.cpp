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

#include <utils/networkaccessmanager.h>

#include <QObject>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrl>

#include "jsonrpc.h"

namespace Freebox {
namespace Util {

uint JsonRpcRequest::s_next_id = 0;

JsonRpcRequest::JsonRpcRequest(const QUrl &endpoint,
                               const QString &method,
                               const QJsonValue &arg,
                               QObject *parent)
  : QObject(parent),
    m_id(QString::number(s_next_id++))
{
    QJsonObject rpc;
    rpc["jsonrpc"] = "2.0";
    rpc["method"] = method;
    rpc["params"] = arg;
    rpc["id"] = m_id;

    QJsonDocument body(rpc);

    QNetworkRequest req(endpoint);
    req.setHeader(QNetworkRequest::ContentTypeHeader,
                  QByteArray("application/json"));

    m_reply = Utils::NetworkAccessManager::instance()->post(req, body.toJson());
    connect(m_reply, &QNetworkReply::finished,
            this, &JsonRpcRequest::replyFinished);
}

void JsonRpcRequest::replyFinished()
{
    QJsonDocument doc = QJsonDocument::fromJson(m_reply->readAll());
    QJsonObject root = doc.object();

    if (m_reply->error() != QNetworkReply::NoError) {
        QJsonObject e;
        e["code"] = -32000;
        e["message"] = QJsonValue("Network error: " + m_reply->errorString());
        emit error(e);
        return;
    }

    if (root["jsonrpc"] != "2.0") {
        QJsonObject e;
        e["message"] = "Bad JSON RPC version in response";
        emit error(e);
        return;
    }

    if (root["id"] != m_id) {
        QJsonObject e;
        e["message"] = "Bad ID in response";
        emit error(e);
        return;
    }

    if (root.contains("error")) {
        emit error(root["error"].toObject());
        return;
    }

    emit done(root["result"]);
}

} // namespace Util
} // namespace Freebox
