#ifndef QTRESTCLIENT_RESTCLIENT_H
#define QTRESTCLIENT_RESTCLIENT_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/requestbuilder.h"

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>
#include <QtCore/qversionnumber.h>

#include <QtNetwork/qnetworkrequest.h>
#include <QHttpMultiPart>

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
class QJsonSerializer;
#endif

namespace QtRestClient {

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED

class RestClass;
class PagingFactory;

class RestClientPrivate;
//! A class to define access to an API, with general settings
class Q_RESTCLIENT_EXPORT RestClient : public QObject
{
	Q_OBJECT
	friend class RestClientPrivate;

	//! The base URL to be used for every request to that api
	Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
	//! The version number to be appended to the path
	Q_PROPERTY(QVersionNumber apiVersion READ apiVersion WRITE setApiVersion NOTIFY apiVersionChanged)
	//! A collection of headers to be added to every request
	Q_PROPERTY(HeaderHash globalHeaders READ globalHeaders WRITE setGlobalHeaders NOTIFY globalHeadersChanged)
	//! A URL Query with parameters to be added to every request
	Q_PROPERTY(QUrlQuery globalParameters READ globalParameters WRITE setGlobalParameters NOTIFY globalParametersChanged)
	//! A collection of attributes to be set on every request
	Q_PROPERTY(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes READ requestAttributes WRITE setRequestAttributes NOTIFY requestAttributesChanged)

#ifndef QT_NO_SSL
	//! The SSL configuration to be used for HTTPS
	Q_PROPERTY(QSslConfiguration sslConfiguration READ sslConfiguration WRITE setSslConfiguration NOTIFY sslConfigurationChanged)
#endif

public:
	//! Constructor
	explicit RestClient(QObject *parent = nullptr);
	~RestClient() override;

	//! Creates a new rest class for the given path and parent
	RestClass *createClass(const QString &path, QObject *parent = nullptr);
	//! Returns the rest class with the root path
	RestClass *rootClass() const;

	//! Returns the network access manager used by the restclient
	QNetworkAccessManager *manager() const;
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	//! Returns the json serializer used by the restclient
	QJsonSerializer *serializer() const;
#endif
	//! Returns the paging factory used by the restclient
	PagingFactory *pagingFactory() const;

	//! @readAcFn{RestClient::baseUrl}
	QUrl baseUrl() const;
	//! @readAcFn{RestClient::apiVersion}
	QVersionNumber apiVersion() const;
	//! @readAcFn{RestClient::globalHeaders}
	HeaderHash globalHeaders() const;
	//! @readAcFn{RestClient::globalParameters}
	QUrlQuery globalParameters() const;
	//! @readAcFn{RestClient::requestAttributes}
	QHash<QNetworkRequest::Attribute, QVariant> requestAttributes() const;
#ifndef QT_NO_SSL
	//! @readAcFn{RestClient::sslConfiguration}
	QSslConfiguration sslConfiguration() const;
#endif

	//! Creates a request builder with all the settings of this client
	virtual RequestBuilder builder() const;

public Q_SLOTS:
	//! Sets the network access manager to be used by all requests for this client
	void setManager(QNetworkAccessManager *manager);
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	//! Sets the json serializer to be used by all requests for this client
	void setSerializer(QJsonSerializer *serializer);
#endif

	//! Sets the paging factory to be used by all paging requests for this client
	void setPagingFactory(PagingFactory *factory);

	//! @writeAcFn{RestClient::baseUrl}
	void setBaseUrl(QUrl baseUrl);
	//! @writeAcFn{RestClient::apiVersion}
	void setApiVersion(QVersionNumber apiVersion);
	//! @writeAcFn{RestClient::globalHeaders}
	void setGlobalHeaders(HeaderHash globalHeaders);
	//! @writeAcFn{RestClient::globalParameters}
	void setGlobalParameters(QUrlQuery globalParameters);
	//! @writeAcFn{RestClient::requestAttributes}
	void setRequestAttributes(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes);
	//! Sets modern attributes in RestClient::requestAttributes
	void setModernAttributes();
#ifndef QT_NO_SSL
	//! @writeAcFn{RestClient::sslConfiguration}
	void setSslConfiguration(QSslConfiguration sslConfiguration);
#endif

	//! @writeAcFn{RestClient::globalHeaders}
	void addGlobalHeader(const QByteArray &name, const QByteArray &value);
	//! @writeAcFn{RestClient::globalHeaders}
	void removeGlobalHeader(const QByteArray &name);

	//! @writeAcFn{RestClient::globalParameters}
	void addGlobalParameter(const QString &name, const QString &value);
	//! @writeAcFn{RestClient::globalParameters}
	void removeGlobalParameter(const QString &name);

	//! @writeAcFn{RestClient::requestAttributes}
	void addRequestAttribute(QNetworkRequest::Attribute attribute, const QVariant &value);
	//! @writeAcFn{RestClient::requestAttributes}
	void removeRequestAttribute(QNetworkRequest::Attribute attribute);

Q_SIGNALS:
	//! @notifyAcFn{RestClient::baseUrl}
	void baseUrlChanged(QUrl baseUrl, QPrivateSignal);
	//! @notifyAcFn{RestClient::apiVersion}
	void apiVersionChanged(QVersionNumber apiVersion, QPrivateSignal);
	//! @notifyAcFn{RestClient::globalHeaders}
	void globalHeadersChanged(HeaderHash globalHeaders, QPrivateSignal);
	//! @notifyAcFn{RestClient::globalParameters}
	void globalParametersChanged(QUrlQuery globalParameters, QPrivateSignal);
	//! @notifyAcFn{RestClient::requestAttributes}
	void requestAttributesChanged(QHash<QNetworkRequest::Attribute, QVariant> requestAttributes, QPrivateSignal);
#ifndef QT_NO_SSL
	//! @notifyAcFn{RestClient::sslConfiguration}
	void sslConfigurationChanged(QSslConfiguration sslConfiguration, QPrivateSignal);
#endif

protected:
	//! @private
	RestClient(QObject *parent, RestClientPrivate *d_ptr, bool skipNam);
	//! @private
	RestClientPrivate *d_ptr();
	//! @private
	const RestClientPrivate *d_ptr() const;

private:
	QScopedPointer<RestClientPrivate> d;
};

QT_WARNING_POP

}

#endif // QTRESTCLIENT_RESTCLIENT_H
