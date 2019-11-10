#ifndef QTRESTCLIENT_REQUESTBUILDER_H
#define QTRESTCLIENT_REQUESTBUILDER_H

#include "QtRestClient/qtrestclient_global.h"

#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>
#include <QtCore/qurl.h>
#include <QtCore/qurlquery.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qshareddata.h>
#include <QHttpMultiPart>

namespace QtRestClient {

struct RequestBuilderPrivate;
//! A helper class to build QUrl and QNetworkRequest objects
class Q_RESTCLIENT_EXPORT RequestBuilder
{
public:
	//! Constructs a builder with the given base url
	RequestBuilder(const QUrl &baseUrl, QNetworkAccessManager *nam = nullptr);
	//! Copy constructor
	RequestBuilder(const RequestBuilder &other);
	//! Move constructor
	RequestBuilder(RequestBuilder &&other) noexcept;
	//! Copy assignment operator
	RequestBuilder &operator=(const RequestBuilder &other);
	//! Move assignment operator
	RequestBuilder &operator=(RequestBuilder &&other) noexcept;
	~RequestBuilder();

	//! Sets the network access manager to be used for send()
	RequestBuilder &setNetworkAccessManager(QNetworkAccessManager *nam);

	//! Sets the credentails of the URL
	RequestBuilder &setCredentials(QString user, QString password = {});
	//! Sets the version of the API
	RequestBuilder &setVersion(QVersionNumber version);
	//! appends a path segment to the builders path
	RequestBuilder &addPath(const QString &pathSegment);
	//! @copydoc RequestBuilder::addPath(const QString &)
	RequestBuilder &addPath(const QStringList &pathSegment);
	//! Enables the trailing slash for the generated URL
	RequestBuilder &trailingSlash(bool enable = true);
	//! Adds a parameter to the URL
	RequestBuilder &addParameter(const QString &name, const QString &value);
	//! Adds parameters to the URL
	RequestBuilder &addParameters(const QUrlQuery &parameters);
	//! Sets the fragment part of the URL
	RequestBuilder &setFragment(QString fragment);
	//! Adds a HTTP header to be added to the network request
	RequestBuilder &addHeader(const QByteArray &name, const QByteArray &value);
	//! Adds HTTP headers to be added to the network request
	RequestBuilder &addHeaders(const HeaderHash &headers);

	//! Updates the builder from the (relative) URL and includes all of it's elements
	RequestBuilder &updateFromRelativeUrl(const QUrl &url, bool mergeQuery = false, bool keepFragment = false);

	//! Sets the given attribute on the generated network request
	RequestBuilder &setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value);
	//! Sets the given attributes on the generated network request
	RequestBuilder &setAttributes(const QHash<QNetworkRequest::Attribute, QVariant> &attributes);
#ifndef QT_NO_SSL
	//! Sets the ssl configuration to be used by the network request
	RequestBuilder &setSslConfig(QSslConfiguration sslConfig);
#endif

	//! Sets the content of the generated network request
	RequestBuilder &setBody(QByteArray body, const QByteArray &contentType);
	//! @copybrief RequestBuilder::setBody(QByteArray, const QByteArray &)
	RequestBuilder &setBody(const QJsonObject &body);
	//! @copydoc RequestBuilder::setBody(const QJsonObject &)
	RequestBuilder &setBody(const QJsonArray &body);
	//! Sets the HTTP-Verb to be used by the generated network request
	RequestBuilder &setVerb(QByteArray verb);
    //! Set QHttpMultiPart
    RequestBuilder &setHttpMultiPart(QHttpMultiPart *httpMultiPart);

	//! Adds a post parameter to the body
	RequestBuilder &addPostParameter(const QString &name, const QString &value);
	//! Adds post parameters to the body
	RequestBuilder &addPostParameters(const QUrlQuery &parameters);

	//! Creates a URL from the builder settings
	QUrl buildUrl() const;
	//! Creates a network request from the builder settings
	QNetworkRequest build() const;
	//! reates a network request and sends it with the builder settings
	QNetworkReply *send() const;

protected:
	//! @private
	RequestBuilder(RequestBuilderPrivate *d_ptr);
	//! @private
	RequestBuilderPrivate *d_ptr();
	//! @private
	const RequestBuilderPrivate *d_ptr() const;

private:
	QSharedDataPointer<RequestBuilderPrivate> d;
};

}

#endif // QTRESTCLIENT_REQUESTBUILDER_H
