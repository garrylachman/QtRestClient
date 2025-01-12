#ifndef QTRESTCLIENT_IPAGING_H
#define QTRESTCLIENT_IPAGING_H

#include "QtRestClient/qtrestclient_global.h"

#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
class QJsonSerializer;
#endif

namespace QtRestClient {

//! Interface to parse generic paging objects and operate on them
class Q_RESTCLIENT_EXPORT IPaging  // clazy:exclude=copyable-polymorphic
{
public:
	virtual ~IPaging();

	//! Returns the items of this paging object, i.e. it's data
	virtual QJsonArray items() const = 0;

	//! Returns the total number of objects there are
	virtual int total() const;
	//! Returns the offset this paging begins at
	virtual int offset() const;
	//! Returns true, if there is a next paging object
	virtual bool hasNext() const = 0;
	//! Returns the link to the next paging object
	virtual QUrl next() const = 0;
	//! Returns true, if there is a previous paging object
	virtual bool hasPrevious() const;
	//! Returns the link to the previous paging object
	virtual QUrl previous() const;
	//! Returns a hash containing all properties of the original JSON
	virtual QVariantMap properties() const = 0;
	//! Returns the original JSON element parsed
	virtual QJsonObject originalJson() const = 0;
};

class Q_RESTCLIENT_EXPORT Q_DECL_DEPRECATED_X("Use QtRestClient::IPagingFactory instead") PagingFactory
{
	Q_DISABLE_COPY(PagingFactory)

public:
	PagingFactory();
	virtual ~PagingFactory();

	//! Creates a new paging object of the given data
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	virtual IPaging *createPaging(QJsonSerializer *serializer, const QJsonObject &data) const = 0;
#else
	virtual IPaging *createPaging(const QJsonObject &data) const = 0;
#endif
};

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
//! An interface for a factory to create paging objects
class Q_RESTCLIENT_EXPORT IPagingFactory : public PagingFactory {};
QT_WARNING_POP

}

Q_DECLARE_METATYPE(QtRestClient::IPaging*)

#endif // QTRESTCLIENT_IPAGING_H
