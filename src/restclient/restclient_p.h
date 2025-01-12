#ifndef QTRESTCLIENT_QRESTCLIENT_P_H
#define QTRESTCLIENT_QRESTCLIENT_P_H

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
#include <QtJsonSerializer/QJsonSerializer>
#endif

#include "restclient.h"
#include "standardpaging_p.h"

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT RestClientPrivate
{
	Q_DISABLE_COPY(RestClientPrivate)
	friend class RestClient;

public:
	static QHash<QString, RestClient*> globalApis;

	QUrl baseUrl;
	QVersionNumber apiVersion;
	HeaderHash headers;
	QUrlQuery query;
	QHash<QNetworkRequest::Attribute, QVariant> attribs;
#ifndef QT_NO_SSL
	QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
#endif

	QNetworkAccessManager *nam = nullptr;
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	QJsonSerializer *serializer = nullptr;
#endif

	QT_WARNING_PUSH
	QT_WARNING_DISABLE_DEPRECATED
	QScopedPointer<PagingFactory> pagingFactory {};
	QT_WARNING_POP

	RestClass *rootClass = nullptr;

	RestClientPrivate();

	void setupBuilder(RequestBuilder &builder) const;
};

}

#endif // QTRESTCLIENT_QRESTCLIENT_P_H
