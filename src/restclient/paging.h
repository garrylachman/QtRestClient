#ifndef QTRESTCLIENT_PAGING_H
#define QTRESTCLIENT_PAGING_H

#include "QtRestClient/paging_fwd.h"
#include "QtRestClient/genericrestreply.h"
#include <QtCore/qsharedpointer.h>

// ------------- Generic Implementation -------------

namespace QtRestClient {

//! @private
template <typename T>
class PagingData : public QSharedData
{
public:
	PagingData() = default;
	PagingData(const PagingData &other) = default;

	QSharedPointer<IPaging> iPaging;
	QList<T> data;
	RestClient *client = nullptr;
};

template<typename T>
Paging<T>::Paging() :
	d(new PagingData<T>())
{}

template<typename T>
Paging<T>::Paging(const Paging<T> &other) = default;

template<typename T>
Paging<T>::Paging(Paging<T> &&other) noexcept = default;

template<typename T>
Paging<T> &Paging<T>::operator=(const Paging<T> &other) = default;

template<typename T>
Paging<T> &Paging<T>::operator=(Paging<T> &&other) noexcept = default;

template<typename T>
Paging<T>::Paging(IPaging *iPaging, const QList<T> &data, RestClient *client) :
	d(new PagingData<T>())
{
	d->iPaging.reset(iPaging);
	d->data = data;
	d->client = client;
}

template<typename T>
bool Paging<T>::isValid() const
{
	return d->iPaging;
}

template<typename T>
IPaging *Paging<T>::iPaging() const
{
	return d->iPaging.data();
}

template<typename T>
QList<T> Paging<T>::items() const
{
	return d->data;
}

template<typename T>
int Paging<T>::total() const
{
	return d->iPaging->total();
}

template<typename T>
int Paging<T>::offset() const
{
	return d->iPaging->offset();
}

template<typename T>
bool Paging<T>::hasNext() const
{
	return d->iPaging->hasNext();
}

template<typename T>
template<typename EO>
GenericRestReply<Paging<T>, EO> *Paging<T>::next() const
{
	if(d->iPaging->hasNext()) {
		auto reply = d->client->builder()
				.updateFromRelativeUrl(d->iPaging->next(), true)
				.send();
		return new GenericRestReply<Paging<T>, EO>(reply, d->client, d->client);
	} else
		return nullptr;
}

template<typename T>
QUrl Paging<T>::nextUrl() const
{
	return d->iPaging->next();
}

template<typename T>
bool Paging<T>::hasPrevious() const
{
	return d->iPaging->hasPrevious();
}

template<typename T>
template<typename EO>
GenericRestReply<Paging<T>, EO> *Paging<T>::previous() const
{
	if(d->iPaging->hasPrevious()) {
		auto reply = d->client->builder()
				.updateFromRelativeUrl(d->iPaging->previous(), true)
				.send();
		return new GenericRestReply<Paging<T>, EO>(reply, d->client, d->client);
	} else
		return nullptr;
}

template<typename T>
QUrl Paging<T>::previousUrl() const
{
	return d->iPaging->previous();
}

template<typename T>
void Paging<T>::iterate(const std::function<bool (T, int)> &iterator, int to, int from) const
{
	iterate(iterator, {}, {}, to, from);
}

template<typename T>
void Paging<T>::iterate(QObject *scope, const std::function<bool (T, int)> &iterator, int to, int from) const
{
	iterate(scope, iterator, {}, {}, to, from);
}

template<typename T>
template<typename EO>
void Paging<T>::iterate(const std::function<bool(T, int)> &iterator, const std::function<void(QString, int, RestReply::ErrorType)> &errorHandler, const std::function<QString (EO, int)> &failureTransformer, int to, int from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if(index < 0)
		return;

	//calc total limit -> only if supports indexes
	int max = INT_MAX;
	if(d->iPaging->offset() >= 0) {
		if(to >= 0)
			max = qMin(to, d->iPaging->total());
		else
			max = d->iPaging->total();
	}

	//continue to the next one
	if(index < max && d->iPaging->hasNext()) {
		next()->onSucceeded([iterator, errorHandler, failureTransformer, to, index](int, Paging<T> paging) {
				  paging.iterate(iterator, errorHandler, failureTransformer, to, index);
			  })
			  ->onAllErrors(errorHandler, failureTransformer);
	}
}

template<typename T>
template<typename EO>
void Paging<T>::iterate(QObject *scope, const std::function<bool(T, int)> &iterator, const std::function<void(QString, int, RestReply::ErrorType)> &errorHandler, const std::function<QString (EO, int)> &failureTransformer, int to, int from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if(index < 0)
		return;

	//calc total limit -> only if supports indexes
	int max = INT_MAX;
	if(d->iPaging->offset() >= 0) {
		if(to >= 0)
			max = qMin(to, d->iPaging->total());
		else
			max = d->iPaging->total();
	}

	//continue to the next one
	if(index < max && d->iPaging->hasNext()) {
		next()->onSucceeded(scope, [iterator, errorHandler, failureTransformer, to, index](int, Paging<T> paging) {
				  paging.iterate(iterator, errorHandler, failureTransformer, to, index);
			  })
			  ->onAllErrors(scope, errorHandler, failureTransformer);
	}
}

template<typename T>
template<typename EO>
void Paging<T>::iterate(const std::function<bool(T, int)> &iterator, const std::function<void(int, EO)> &failureHandler, const std::function<void(QString, int, RestReply::ErrorType)> &errorHandler, const std::function<void(QJsonSerializerException &)> &exceptionHandler, int to, int from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if(index < 0)
		return;

	//calc total limit -> only if supports indexes
	int max = INT_MAX;
	if(d->iPaging->offset() >= 0) {
		if(to >= 0)
			max = qMin(to, d->iPaging->total());
		else
			max = d->iPaging->total();
	}

	//continue to the next one
	if(index < max && d->iPaging->hasNext()) {
		next()->onSucceeded([iterator, failureHandler, errorHandler, exceptionHandler, to, index](int, Paging<T> paging) {
				  paging.iterate(iterator, failureHandler, errorHandler, exceptionHandler, to, index);
			  })
			  ->onFailed(failureHandler)
			  ->onError(errorHandler)
			  ->onSerializeException(exceptionHandler);
	}
}

template<typename T>
template<typename EO>
void Paging<T>::iterate(QObject *scope, const std::function<bool(T, int)> &iterator, const std::function<void(int, EO)> &failureHandler, const std::function<void(QString, int, RestReply::ErrorType)> &errorHandler, const std::function<void(QJsonSerializerException &)> &exceptionHandler, int to, int from) const
{
	Q_ASSERT(from >= d->iPaging->offset());

	auto index = internalIterate(iterator, to ,from);
	if(index < 0)
		return;

	//calc total limit -> only if supports indexes
	int max = INT_MAX;
	if(d->iPaging->offset() >= 0) {
		if(to >= 0)
			max = qMin(to, d->iPaging->total());
		else
			max = d->iPaging->total();
	}

	//continue to the next one
	if(index < max && d->iPaging->hasNext()) {
		next()->onSucceeded(scope, [iterator, failureHandler, errorHandler, exceptionHandler, to, index](int, Paging<T> paging) {
				  paging.iterate(iterator, failureHandler, errorHandler, exceptionHandler, to, index);
			  })
			  ->onFailed(scope, failureHandler)
			  ->onError(scope, errorHandler)
			  ->onSerializeException(exceptionHandler);
	}
}

template<typename T>
QVariantMap Paging<T>::properties() const
{
	return d->iPaging->properties();
}

template<typename T>
void Paging<T>::deleteAllItems() const
{
	MetaComponent<T>::deleteAllLater(d->data);
}

template<typename T>
int Paging<T>::internalIterate(std::function<bool (T, int)> iterator, int to, int from) const
{
	//handle all items in this paging

	auto offset = d->iPaging->offset();
	auto count = d->data.size();
	auto start = 0;
	auto max = count;
	if(offset >= 0) {// has indexes
		//from
		start = qMax(from, offset) - offset;
		//to
		if(to >= 0)
			max = qMin(to, max + offset) - offset;
	}

	//delete unused items caused by from
	for(auto j = 0; j < start; j++)
		MetaComponent<T>::deleteLater(d->data.value(j));

	//iterate over used items
	int i;
	auto canceled = false;
	for(i = start; i < max; i++) {
		auto item = d->data.value(i);
		auto index = offset >= 0 ? i + offset : -1;
		if(!iterator(item, index)) {
			canceled = true;
			break;
		}
	}

	//delete all unused items caused by to
	for(auto j = i; j < count; j++)
		MetaComponent<T>::deleteLater(d->data.value(j));

	if(canceled)
		return -1;
	else if(offset >= 0)
		return offset + i;
	else
		return 0;
}

}

#endif // QTRESTCLIENT_PAGING_H
