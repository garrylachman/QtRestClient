#include "requestbuilder.h"
#include "requestbuilder_p.h"
#include "restreply_p.h"
#include "restclass.h"

#include <QtCore/QBuffer>
#include <QtCore/QJsonDocument>
using namespace QtRestClient;

RequestBuilder::RequestBuilder(const QUrl &baseUrl, QNetworkAccessManager *nam) :
    d{new RequestBuilderPrivate{baseUrl, nam}}
{}

RequestBuilder::RequestBuilder(const RequestBuilder &other) = default;

RequestBuilder::RequestBuilder(RequestBuilder &&other) noexcept = default;

RequestBuilder &RequestBuilder::operator=(const RequestBuilder &other) = default;

RequestBuilder &RequestBuilder::operator=(RequestBuilder &&other) noexcept = default;

RequestBuilder::~RequestBuilder() = default;

RequestBuilder &RequestBuilder::setNetworkAccessManager(QNetworkAccessManager *nam)
{
	d->nam = nam;
	return *this;
}

RequestBuilder &RequestBuilder::setCredentials(QString user, QString password)
{
	d->user = std::move(user);
	d->pass = std::move(password);
	return *this;
}

RequestBuilder &RequestBuilder::setVersion(QVersionNumber version)
{
	d->version = std::move(version);
	return *this;
}

RequestBuilder &RequestBuilder::addHeader(const QByteArray &name, const QByteArray &value)
{
	d->headers.insert(name, value);
	return *this;
}

RequestBuilder &RequestBuilder::addHeaders(const HeaderHash &headers)
{
	for(auto it = headers.constBegin(); it != headers.constEnd(); it++)
		d->headers.insert(it.key(), it.value());
	return *this;
}

RequestBuilder &RequestBuilder::updateFromRelativeUrl(const QUrl &url, bool mergeQuery, bool keepFragment)
{
	auto cUrl = buildUrl();
	d->base = cUrl.resolved(url);
	if(d->base.host() != cUrl.host()) {
		qWarning() << "URL host changed from"
				   << cUrl.host()
				   << "to"
				   << d->base.host();
	}
	//clear all the rest
	d->version = QVersionNumber();
	d->user.clear();
	d->pass.clear();
	d->path.clear();
	if(mergeQuery) {
		QUrlQuery query(url.query());
		for(const auto &item : query.queryItems(QUrl::FullyDecoded)) // clazy:exclude=range-loop
			d->query.addQueryItem(item.first, item.second);
	} else
		d->query = QUrlQuery(url.query());
	if(!keepFragment)
		d->fragment.clear();
	return *this;
}

RequestBuilder &RequestBuilder::addParameter(const QString &name, const QString &value)
{
	d->query.addQueryItem(name, value);
	return *this;
}

RequestBuilder &RequestBuilder::addParameters(const QUrlQuery &parameters)
{
	for(const auto &param : parameters.queryItems(QUrl::FullyDecoded)) // clazy:exclude=range-loop
		d->query.addQueryItem(param.first, param.second);
	return *this;
}

RequestBuilder &RequestBuilder::setFragment(QString fragment)
{
	d->fragment = std::move(fragment);
	return *this;
}

RequestBuilder &RequestBuilder::addPath(const QString &pathSegment)
{
	d->path.append(pathSegment.split(QLatin1Char('/'), QString::SkipEmptyParts));
	return *this;
}

RequestBuilder &RequestBuilder::addPath(const QStringList &pathSegment)
{
	d->path.append(pathSegment);
	return *this;
}

RequestBuilder &RequestBuilder::trailingSlash(bool enable)
{
	d->trailingSlash = enable;
	return *this;
}

RequestBuilder &RequestBuilder::setAttribute(QNetworkRequest::Attribute attribute, const QVariant &value)
{
	d->attributes.insert(attribute, value);
	return *this;
}

RequestBuilder &RequestBuilder::setAttributes(const QHash<QNetworkRequest::Attribute, QVariant> &attributes)
{
	for(auto it = attributes.constBegin(); it != attributes.constEnd(); it++)
		d->attributes.insert(it.key(), it.value());
	return *this;
}

#ifndef QT_NO_SSL
RequestBuilder &RequestBuilder::setSslConfig(QSslConfiguration sslConfig)
{
	d->sslConfig = std::move(sslConfig);
	return *this;
}
#endif

RequestBuilder &RequestBuilder::setBody(QByteArray body, const QByteArray &contentType)
{
	d->body = std::move(body);
	d->postQuery.clear();
	d->headers.insert(RequestBuilderPrivate::ContentType, contentType);
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const QJsonObject &body)
{
	d->body = QJsonDocument(body).toJson(QJsonDocument::Compact);
	d->postQuery.clear();
	d->headers.insert(RequestBuilderPrivate::ContentType, RequestBuilderPrivate::ContentTypeJson);
	return *this;
}

RequestBuilder &RequestBuilder::setBody(const QJsonArray &body)
{
	d->body = QJsonDocument(body).toJson(QJsonDocument::Compact);
	d->postQuery.clear();
	d->headers.insert(RequestBuilderPrivate::ContentType, RequestBuilderPrivate::ContentTypeJson);
	return *this;
}

RequestBuilder &RequestBuilder::setVerb(QByteArray verb)
{
	d->verb = std::move(verb);
    return *this;
}

RequestBuilder &RequestBuilder::setHttpMultiPart(QHttpMultiPart *httpMultiPart)
{
    d->httpMultiPart = httpMultiPart;
}

RequestBuilder &RequestBuilder::addPostParameter(const QString &name, const QString &value)
{
	d->postQuery.addQueryItem(name, value);
	d->body.clear();
	d->headers.insert(RequestBuilderPrivate::ContentType, RequestBuilderPrivate::ContentTypeUrlEncoded);
	return *this;
}

RequestBuilder &RequestBuilder::addPostParameters(const QUrlQuery &parameters)
{
	for(const auto &param : parameters.queryItems(QUrl::FullyDecoded)) // clazy:exclude=range-loop
		d->postQuery.addQueryItem(param.first, param.second);
	d->body.clear();
	d->headers.insert(RequestBuilderPrivate::ContentType, RequestBuilderPrivate::ContentTypeUrlEncoded);
	return *this;
}

QUrl RequestBuilder::buildUrl() const
{
	auto url = d->base;

	auto pathList = url.path().split(QLatin1Char('/'), QString::SkipEmptyParts);
	if(!d->version.isNull())
		pathList.append(QLatin1Char('v') + d->version.normalized().toString());
	pathList.append(d->path);
	url.setPath(QLatin1Char('/') + pathList.join(QLatin1Char('/')) + (d->trailingSlash ? QStringLiteral("/") : QString()));

	if(!d->user.isNull())
		url.setUserName(d->user);
	if(!d->pass.isNull())
		url.setPassword(d->pass);
	if(!d->query.isEmpty())
		url.setQuery(d->query);
	if(!d->fragment.isNull())
		url.setFragment(d->fragment);

	return url;
}

QNetworkRequest RequestBuilder::build() const
{
	QNetworkRequest request{buildUrl()};
	d->prepareRequest(request);
	return request;
}

QNetworkReply *RequestBuilder::send() const
{
	QNetworkRequest request{buildUrl()};
	QByteArray body;
	d->prepareRequest(request, &body);
	return RestReplyPrivate::compatSend(d->nam, request, d->verb, body);
}

RequestBuilder::RequestBuilder(RequestBuilderPrivate *d_ptr) :
	d{d_ptr}
{}

RequestBuilderPrivate *RequestBuilder::d_ptr()
{
	return d;
}

const RequestBuilderPrivate *RequestBuilder::d_ptr() const
{
	return d;
}

// ------------- Private Implementation -------------

const QByteArray RequestBuilderPrivate::ContentType = "Content-Type";
const QByteArray RequestBuilderPrivate::ContentTypeJson = "application/json";
const QByteArray RequestBuilderPrivate::ContentTypeUrlEncoded = "application/x-www-form-urlencoded";

RequestBuilderPrivate::RequestBuilderPrivate(const QUrl &baseUrl, QNetworkAccessManager *nam) :
	QSharedData{},
	nam{nam},
	base{baseUrl},
	user{baseUrl.userName()},
	pass{baseUrl.password()},
	query{baseUrl.query()},
	fragment{baseUrl.fragment()},
#ifndef QT_NO_SSL
	sslConfig{QSslConfiguration::defaultConfiguration()},
#endif
	verb{RestClass::GetVerb}
{}

void RequestBuilderPrivate::prepareRequest(QNetworkRequest &request, QByteArray *sBody) const
{
	// add headers etc.
	for(auto it = headers.constBegin(); it != headers.constEnd(); it++)
		request.setRawHeader(it.key(), it.value());
	for(auto it = attributes.constBegin(); it != attributes.constEnd(); it++)
		request.setAttribute(it.key(), it.value());
#ifndef QT_NO_SSL
	request.setSslConfiguration(sslConfig);
#endif

	// create the body
	if (sBody) {
		if(!body.isEmpty())
			*sBody = body;
		else if(headers.value(RequestBuilderPrivate::ContentType) == RequestBuilderPrivate::ContentTypeUrlEncoded &&
				  !postQuery.isEmpty())
			*sBody = postQuery.query().toUtf8();
	}
}
