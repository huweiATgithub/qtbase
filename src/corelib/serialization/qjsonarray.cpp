// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qstringlist.h>
#include <qcborarray.h>
#include <qvariant.h>
#include <qdebug.h>

#include <private/qcborvalue_p.h>
#include <private/qjson_p.h>

#include "qjsonwriter_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QJsonArray
    \inmodule QtCore
    \ingroup json
    \ingroup shared
    \ingroup qtserialization
    \reentrant
    \since 5.0

    \brief The QJsonArray class encapsulates a JSON array.

    \compares equality
    \compareswith equality QJsonValue
    \endcompareswith

    A JSON array is a list of values. The list can be manipulated by inserting and
    removing QJsonValue's from the array.

    A QJsonArray can be converted to and from a QVariantList. You can query the
    number of entries with size(), insert(), and removeAt() entries from it
    and iterate over its content using the standard C++ iterator pattern.

    QJsonArray is an implicitly shared class and shares the data with the document
    it has been created from as long as it is not being modified.

    You can convert the array to and from text based JSON through QJsonDocument.

    \sa {JSON Support in Qt}, {Saving and Loading a Game}
*/

/*!
    \typedef QJsonArray::Iterator

    Qt-style synonym for QJsonArray::iterator.
*/

/*!
    \typedef QJsonArray::ConstIterator

    Qt-style synonym for QJsonArray::const_iterator.
*/

/*!
    \typedef QJsonArray::size_type

    Typedef for qsizetype. Provided for STL compatibility.
*/

/*!
    \typedef QJsonArray::value_type

    Typedef for QJsonValue. Provided for STL compatibility.
*/

/*!
    \typedef QJsonArray::difference_type

    Typedef for qsizetype. Provided for STL compatibility.
*/

/*!
    \typedef QJsonArray::pointer

    Typedef for QJsonValue *. Provided for STL compatibility.
*/

/*!
    \typedef QJsonArray::const_pointer

    Typedef for const QJsonValue *. Provided for STL compatibility.
*/

/*!
    \typedef QJsonArray::reference

    Typedef for QJsonValue &. Provided for STL compatibility.
*/

/*!
    \typedef QJsonArray::const_reference

    Typedef for const QJsonValue &. Provided for STL compatibility.
*/

/*!
    Creates an empty array.
 */
QJsonArray::QJsonArray() = default;

/*!
    \fn QJsonArray::QJsonArray(std::initializer_list<QJsonValue> args)
    \since 5.4
    Creates an array initialized from \a args initialization list.

    QJsonArray can be constructed in a way similar to JSON notation,
    for example:
    \code
    QJsonArray array = { 1, 2.2, QString() };
    \endcode
 */

/*!
    \internal
 */
QJsonArray::QJsonArray(QCborContainerPrivate *array)
    : a(array)
{
}

/*!
    Deletes the array.
 */
QJsonArray::~QJsonArray() = default;

QJsonArray::QJsonArray(std::initializer_list<QJsonValue> args)
{
    for (const auto & arg : args)
        append(arg);
}

/*!
    Creates a copy of \a other.

    Since QJsonArray is implicitly shared, the copy is shallow
    as long as the object doesn't get modified.
 */
QJsonArray::QJsonArray(const QJsonArray &other) noexcept = default;

/*!
    \since 5.10

    Move-constructs a QJsonArray from \a other.
*/
QJsonArray::QJsonArray(QJsonArray &&other) noexcept
    : a(other.a)
{
    other.a = nullptr;
}

/*!
    Assigns \a other to this array.
 */
QJsonArray &QJsonArray::operator =(const QJsonArray &other) noexcept = default;

/*!
    \fn QJsonArray &QJsonArray::operator =(QJsonArray &&other)
    \since 5.10

    Move-assigns \a other to this array.
*/

/*!
    \fn void QJsonArray::swap(QJsonArray &other)
    \since 5.10

    Swaps the array \a other with this. This operation is very fast and never fails.
*/

/*! \fn QJsonArray &QJsonArray::operator+=(const QJsonValue &value)

    Appends \a value to the array, and returns a reference to the array itself.

    \since 5.3
    \sa append(), operator<<()
*/

/*! \fn QJsonArray QJsonArray::operator+(const QJsonValue &value) const

    Returns an array that contains all the items in this array followed
    by the provided \a value.

    \since 5.3
    \sa operator+=()
*/

/*! \fn QJsonArray &QJsonArray::operator<<(const QJsonValue &value)

    Appends \a value to the array, and returns a reference to the array itself.

    \since 5.3
    \sa operator+=(), append()
*/

/*!
    Converts the string list \a list to a QJsonArray.

    The values in \a list will be converted to JSON values.

    \sa toVariantList(), QJsonValue::fromVariant()
 */
QJsonArray QJsonArray::fromStringList(const QStringList &list)
{
    QJsonArray array;
    for (QStringList::const_iterator it = list.constBegin(); it != list.constEnd(); ++it)
        array.append(QJsonValue(*it));
    return array;
}

#ifndef QT_NO_VARIANT
/*!
    Converts the variant list \a list to a QJsonArray.

    The QVariant values in \a list will be converted to JSON values.

    \note Conversion from \l QVariant is not completely lossless. Please see
    the documentation in QJsonValue::fromVariant() for more information.

    \sa toVariantList(), QJsonValue::fromVariant()
 */
QJsonArray QJsonArray::fromVariantList(const QVariantList &list)
{
    return QJsonPrivate::Variant::toJsonArray(list);
}

/*!
    Converts this object to a QVariantList.

    Returns the created map.
 */
QVariantList QJsonArray::toVariantList() const
{
    return QCborArray::fromJsonArray(*this).toVariantList();
}
#endif // !QT_NO_VARIANT


/*!
    Returns the number of values stored in the array.
 */
qsizetype QJsonArray::size() const
{
    return a ? a->elements.size() : 0;
}

/*!
    \fn QJsonArray::count() const

    Same as size().

    \sa size()
*/

/*!
    Returns \c true if the object is empty. This is the same as size() == 0.

    \sa size()
 */
bool QJsonArray::isEmpty() const
{
    return a == nullptr || a->elements.isEmpty();
}

/*!
    Returns a QJsonValue representing the value for index \a i.

    The returned QJsonValue is \c Undefined, if \a i is out of bounds.

 */
QJsonValue QJsonArray::at(qsizetype i) const
{
    if (!a || i < 0 || i >= a->elements.size())
        return QJsonValue(QJsonValue::Undefined);

    return QJsonPrivate::Value::fromTrustedCbor(a->valueAt(i));
}

/*!
    Returns the first value stored in the array.

    Same as \c at(0).

    \sa at()
 */
QJsonValue QJsonArray::first() const
{
    return at(0);
}

/*!
    Returns the last value stored in the array.

    Same as \c{at(size() - 1)}.

    \sa at()
 */
QJsonValue QJsonArray::last() const
{
    return at(a ? (a->elements.size() - 1) : 0);
}

/*!
    Inserts \a value at the beginning of the array.

    This is the same as \c{insert(0, value)} and will prepend \a value to the array.

    \sa append(), insert()
 */
void QJsonArray::prepend(const QJsonValue &value)
{
    insert(0, value);
}

/*!
    Inserts \a value at the end of the array.

    \sa prepend(), insert()
 */
void QJsonArray::append(const QJsonValue &value)
{
    insert(a ? a->elements.size() : 0, value);
}

/*!
    Removes the value at index position \a i. \a i must be a valid
    index position in the array (i.e., \c{0 <= i < size()}).

    \sa insert(), replace()
 */
void QJsonArray::removeAt(qsizetype i)
{
    if (!a || i < 0 || i >= a->elements.size())
        return;
    detach();
    a->removeAt(i);
}

/*! \fn void QJsonArray::removeFirst()

    Removes the first item in the array. Calling this function is
    equivalent to calling \c{removeAt(0)}. The array must not be empty. If
    the array can be empty, call isEmpty() before calling this
    function.

    \sa removeAt(), removeLast()
*/

/*! \fn void QJsonArray::removeLast()

    Removes the last item in the array. Calling this function is
    equivalent to calling \c{removeAt(size() - 1)}. The array must not be
    empty. If the array can be empty, call isEmpty() before calling
    this function.

    \sa removeAt(), removeFirst()
*/

/*!
    Removes the item at index position \a i and returns it. \a i must
    be a valid index position in the array (i.e., \c{0 <= i < size()}).

    If you don't use the return value, removeAt() is more efficient.

    \sa removeAt()
 */
QJsonValue QJsonArray::takeAt(qsizetype i)
{
    if (!a || i < 0 || i >= a->elements.size())
        return QJsonValue(QJsonValue::Undefined);

    detach();
    const QJsonValue v = QJsonPrivate::Value::fromTrustedCbor(a->extractAt(i));
    a->removeAt(i);
    return v;
}

/*!
    Inserts \a value at index position \a i in the array. If \a i
    is \c 0, the value is prepended to the array. If \a i is size(), the
    value is appended to the array.

    \sa append(), prepend(), replace(), removeAt()
 */
void QJsonArray::insert(qsizetype i, const QJsonValue &value)
{
    if (a)
        detach(a->elements.size() + 1);
    else
        a = new QCborContainerPrivate;

    Q_ASSERT (i >= 0 && i <= a->elements.size());
    a->insertAt(i, value.type() == QJsonValue::Undefined ? QCborValue(nullptr)
                                                         : QCborValue::fromJsonValue(value));
}

/*!
    \fn QJsonArray::iterator QJsonArray::insert(iterator before, const QJsonValue &value)

    Inserts \a value before the position pointed to by \a before, and returns an iterator
    pointing to the newly inserted item.

    \sa erase(), insert()
*/

/*!
    \fn QJsonArray::iterator QJsonArray::erase(iterator it)

    Removes the item pointed to by \a it, and returns an iterator pointing to the
    next item.

    \sa removeAt()
*/

/*!
    Replaces the item at index position \a i with \a value. \a i must
    be a valid index position in the array (i.e., \c{0 <= i < size()}).

    \sa operator[](), removeAt()
 */
void QJsonArray::replace(qsizetype i, const QJsonValue &value)
{
    Q_ASSERT (a && i >= 0 && i < a->elements.size());
    detach();
    a->replaceAt(i, QCborValue::fromJsonValue(value));
}

/*!
    Returns \c true if the array contains an occurrence of \a value, otherwise \c false.

    \sa count()
 */
bool QJsonArray::contains(const QJsonValue &value) const
{
    for (qsizetype i = 0; i < size(); i++) {
        if (at(i) == value)
            return true;
    }
    return false;
}

/*!
    Returns the value at index position \a i as a modifiable reference.
    \a i must be a valid index position in the array (i.e., \c{0 <= i <
    size()}).

    The return value is of type QJsonValueRef, a helper class for QJsonArray
    and QJsonObject. When you get an object of type QJsonValueRef, you can
    use it as if it were a reference to a QJsonValue. If you assign to it,
    the assignment will apply to the character in the QJsonArray of QJsonObject
    from which you got the reference.

    \sa at()
 */
QJsonValueRef QJsonArray::operator [](qsizetype i)
{
    Q_ASSERT(a && i >= 0 && i < a->elements.size());
    return QJsonValueRef(this, i);
}

/*!
    \overload

    Same as at().
 */
QJsonValue QJsonArray::operator[](qsizetype i) const
{
    return at(i);
}

bool comparesEqual(const QJsonArray &lhs, const QJsonArray &rhs) noexcept
{
    if (lhs.a == rhs.a)
        return true;

    if (!lhs.a)
        return !rhs.a->elements.size();
    if (!rhs.a)
        return !lhs.a->elements.size();
    if (lhs.a->elements.size() != rhs.a->elements.size())
        return false;

    for (qsizetype i = 0; i < lhs.a->elements.size(); ++i) {
        if (lhs.a->valueAt(i) != rhs.a->valueAt(i))
            return false;
    }
    return true;
}

bool comparesEqual(const QJsonArray &lhs, const QJsonValue &rhs) noexcept
{
    return lhs == rhs.toArray();
}

/*! \fn bool QJsonArray::operator==(const QJsonArray &lhs, const QJsonArray &rhs)

    Returns \c true if \a lhs array is equal to \a rhs, \c false otherwise.
*/

/*! \fn bool QJsonArray::operator!=(const QJsonArray &lhs, const QJsonArray &rhs)

    Returns \c true if \a lhs array is not equal to \a rhs, \c false otherwise.
*/

/*! \fn QJsonArray::iterator QJsonArray::begin()

    Returns an \l{STL-style iterators}{STL-style iterator} pointing to the first item in
    the array.

    \sa constBegin(), end()
*/

/*! \fn QJsonArray::const_iterator QJsonArray::begin() const

    \overload
*/

/*! \fn QJsonArray::const_iterator QJsonArray::constBegin() const

    Returns a const \l{STL-style iterators}{STL-style iterator} pointing to the first item
    in the array.

    \sa begin(), constEnd()
*/

/*! \fn QJsonArray::const_iterator QJsonArray::cbegin() const

    Returns a const \l{STL-style iterators}{STL-style iterator} pointing to the first item
    in the array.

    \sa begin(), cend()
*/

/*! \fn QJsonArray::iterator QJsonArray::end()

    Returns an \l{STL-style iterators}{STL-style iterator} pointing to the imaginary item
    after the last item in the array.

    \sa begin(), constEnd()
*/

/*! \fn const_iterator QJsonArray::end() const

    \overload
*/

/*! \fn QJsonArray::const_iterator QJsonArray::constEnd() const

    Returns a const \l{STL-style iterators}{STL-style iterator} pointing to the imaginary
    item after the last item in the array.

    \sa constBegin(), end()
*/

/*! \fn QJsonArray::const_iterator QJsonArray::cend() const

    Returns a const \l{STL-style iterators}{STL-style iterator} pointing to the imaginary
    item after the last item in the array.

    \sa cbegin(), end()
*/

/*! \fn void QJsonArray::push_back(const QJsonValue &value)

    This function is provided for STL compatibility. It is equivalent
    to \l{QJsonArray::append()}{append(value)} and will append \a value to the array.
*/

/*! \fn void QJsonArray::push_front(const QJsonValue &value)

    This function is provided for STL compatibility. It is equivalent
    to \l{QJsonArray::prepend()}{prepend(value)} and will prepend \a value to the array.
*/

/*! \fn void QJsonArray::pop_front()

    This function is provided for STL compatibility. It is equivalent
    to removeFirst(). The array must not be empty. If the array can be
    empty, call isEmpty() before calling this function.
*/

/*! \fn void QJsonArray::pop_back()

    This function is provided for STL compatibility. It is equivalent
    to removeLast(). The array must not be empty. If the array can be
    empty, call isEmpty() before calling this function.
*/

/*! \fn bool QJsonArray::empty() const

    This function is provided for STL compatibility. It is equivalent
    to isEmpty() and returns \c true if the array is empty.
*/

/*! \class QJsonArray::iterator
    \inmodule QtCore
    \brief The QJsonArray::iterator class provides an STL-style non-const iterator for QJsonArray.

    \compares strong
    \compareswith strong QJsonArray::const_iterator
    \endcompareswith

    QJsonArray::iterator allows you to iterate over a QJsonArray
    and to modify the array item associated with the
    iterator. If you want to iterate over a const QJsonArray, use
    QJsonArray::const_iterator instead. It is generally a good practice to
    use QJsonArray::const_iterator on a non-const QJsonArray as well, unless
    you need to change the QJsonArray through the iterator. Const
    iterators are slightly faster and improves code readability.

    The default QJsonArray::iterator constructor creates an uninitialized
    iterator. You must initialize it using a QJsonArray function like
    QJsonArray::begin(), QJsonArray::end(), or QJsonArray::insert() before you can
    start iterating.

    Most QJsonArray functions accept an integer index rather than an
    iterator. For that reason, iterators are rarely useful in
    connection with QJsonArray. One place where STL-style iterators do
    make sense is as arguments to \l{generic algorithms}.

    Multiple iterators can be used on the same array. However, be
    aware that any non-const function call performed on the QJsonArray
    will render all existing iterators undefined.

    \sa QJsonArray::const_iterator
*/

/*! \typedef QJsonArray::iterator::iterator_category

  A synonym for \e {std::random_access_iterator_tag} indicating
  this iterator is a random access iterator.
*/

/*! \typedef QJsonArray::iterator::difference_type

    \internal
*/

/*! \typedef QJsonArray::iterator::value_type

    \internal
*/

/*! \typedef QJsonArray::iterator::reference

    \internal
*/

/*! \typedef QJsonArray::iterator::pointer

    \internal
*/

/*! \fn QJsonArray::iterator::iterator()

    Constructs an uninitialized iterator.

    Functions like operator*() and operator++() should not be called
    on an uninitialized iterator. Use operator=() to assign a value
    to it before using it.

    \sa QJsonArray::begin(), QJsonArray::end()
*/

/*! \fn QJsonArray::iterator::iterator(QJsonArray *array, qsizetype index)
    \internal
*/

/*! \fn QJsonValueRef QJsonArray::iterator::operator*() const


    Returns a modifiable reference to the current item.

    You can change the value of an item by using operator*() on the
    left side of an assignment.

    The return value is of type QJsonValueRef, a helper class for QJsonArray
    and QJsonObject. When you get an object of type QJsonValueRef, you can
    use it as if it were a reference to a QJsonValue. If you assign to it,
    the assignment will apply to the character in the QJsonArray of QJsonObject
    from which you got the reference.
*/

/*! \fn QJsonValueRef *QJsonArray::iterator::operator->() const

    Returns a pointer to a modifiable reference to the current item.
*/

/*! \fn QJsonValueRef QJsonArray::iterator::operator[](qsizetype j) const

    Returns a modifiable reference to the item at offset \a j from the
    item pointed to by this iterator (the item at position \c{*this + j}).

    This function is provided to make QJsonArray iterators behave like C++
    pointers.

    The return value is of type QJsonValueRef, a helper class for QJsonArray
    and QJsonObject. When you get an object of type QJsonValueRef, you can
    use it as if it were a reference to a QJsonValue. If you assign to it,
    the assignment will apply to the element in the QJsonArray or QJsonObject
    from which you got the reference.

    \sa operator+()
*/

/*!
    \fn bool QJsonArray::iterator::operator==(const iterator &lhs, const iterator &rhs)
    \fn bool QJsonArray::iterator::operator==(const iterator &lhs, const const_iterator &rhs)

    Returns \c true if \a lhs points to the same item as \a rhs
    iterator; otherwise returns \c false.

    \sa operator!=()
*/

/*!
    \fn bool QJsonArray::iterator::operator!=(const iterator &lhs, const iterator &rhs)
    \fn bool QJsonArray::iterator::operator!=(const iterator &lhs, const const_iterator &rhs)

    Returns \c true if \a lhs points to a different item than \a rhs
    iterator; otherwise returns \c false.

    \sa operator==()
*/

/*!
    \fn bool QJsonArray::iterator::operator<(const iterator &lhs, const iterator &rhs)
    \fn bool QJsonArray::iterator::operator<(const iterator &lhs, const const_iterator &rhs)

    Returns \c true if the item pointed to by \a lhs iterator is less than
    the item pointed to by the \a rhs iterator.
*/

/*!
    \fn bool QJsonArray::iterator::operator<=(const iterator &lhs, const iterator &rhs)
    \fn bool QJsonArray::iterator::operator<=(const iterator &lhs, const const_iterator &rhs)

    Returns \c true if the item pointed to by \a lhs iterator is less than
    or equal to the item pointed to by the \a rhs iterator.
*/

/*!
    \fn bool QJsonArray::iterator::operator>(const iterator &lhs, const iterator &rhs)
    \fn bool QJsonArray::iterator::operator>(const iterator &lhs, const const_iterator &rhs)

    Returns \c true if the item pointed to by \a lhs iterator is greater
    than the item pointed to by the \a rhs iterator.
*/

/*!
    \fn bool QJsonArray::iterator::operator>=(const iterator &lhs, const iterator &rhs)
    \fn bool QJsonArray::iterator::operator>=(const iterator &lhs, const const_iterator &rhs)

    Returns \c true if the item pointed to by \a lhs iterator is greater
    than or equal to the item pointed to by the \a rhs iterator.
*/

/*! \fn QJsonArray::iterator &QJsonArray::iterator::operator++()

    The prefix \c{++} operator, \c{++it}, advances the iterator to the
    next item in the array and returns an iterator to the new current
    item.

    Calling this function on QJsonArray::end() leads to undefined results.

    \sa operator--()
*/

/*! \fn QJsonArray::iterator QJsonArray::iterator::operator++(int)

    \overload

    The postfix \c{++} operator, \c{it++}, advances the iterator to the
    next item in the array and returns an iterator to the previously
    current item.
*/

/*! \fn QJsonArray::iterator &QJsonArray::iterator::operator--()

    The prefix \c{--} operator, \c{--it}, makes the preceding item
    current and returns an iterator to the new current item.

    Calling this function on QJsonArray::begin() leads to undefined results.

    \sa operator++()
*/

/*! \fn QJsonArray::iterator QJsonArray::iterator::operator--(int)

    \overload

    The postfix \c{--} operator, \c{it--}, makes the preceding item
    current and returns an iterator to the previously current item.
*/

/*! \fn QJsonArray::iterator &QJsonArray::iterator::operator+=(qsizetype j)

    Advances the iterator by \a j items. If \a j is negative, the
    iterator goes backward.

    \sa operator-=(), operator+()
*/

/*! \fn QJsonArray::iterator &QJsonArray::iterator::operator-=(qsizetype j)

    Makes the iterator go back by \a j items. If \a j is negative,
    the iterator goes forward.

    \sa operator+=(), operator-()
*/

/*! \fn QJsonArray::iterator QJsonArray::iterator::operator+(qsizetype j) const

    Returns an iterator to the item at \a j positions forward from
    this iterator. If \a j is negative, the iterator goes backward.

    \sa operator-(), operator+=()
*/

/*! \fn QJsonArray::iterator QJsonArray::iterator::operator-(qsizetype j) const

    Returns an iterator to the item at \a j positions backward from
    this iterator. If \a j is negative, the iterator goes forward.

    \sa operator+(), operator-=()
*/

/*! \fn qsizetype QJsonArray::iterator::operator-(iterator other) const

    Returns the number of items between the item pointed to by \a
    other and the item pointed to by this iterator.
*/

/*! \class QJsonArray::const_iterator
    \inmodule QtCore
    \brief The QJsonArray::const_iterator class provides an STL-style const iterator for QJsonArray.

    \compares strong
    \compareswith strong QJsonArray::iterator
    \endcompareswith

    QJsonArray::const_iterator allows you to iterate over a
    QJsonArray. If you want to modify the QJsonArray as
    you iterate over it, use QJsonArray::iterator instead. It is generally a
    good practice to use QJsonArray::const_iterator on a non-const QJsonArray
    as well, unless you need to change the QJsonArray through the
    iterator. Const iterators are slightly faster and improves
    code readability.

    The default QJsonArray::const_iterator constructor creates an
    uninitialized iterator. You must initialize it using a QJsonArray
    function like QJsonArray::constBegin(), QJsonArray::constEnd(), or
    QJsonArray::insert() before you can start iterating.

    Most QJsonArray functions accept an integer index rather than an
    iterator. For that reason, iterators are rarely useful in
    connection with QJsonArray. One place where STL-style iterators do
    make sense is as arguments to \l{generic algorithms}.

    Multiple iterators can be used on the same array. However, be
    aware that any non-const function call performed on the QJsonArray
    will render all existing iterators undefined.

    \sa QJsonArray::iterator
*/

/*! \fn QJsonArray::const_iterator::const_iterator()

    Constructs an uninitialized iterator.

    Functions like operator*() and operator++() should not be called
    on an uninitialized iterator. Use operator=() to assign a value
    to it before using it.

    \sa QJsonArray::constBegin(), QJsonArray::constEnd()
*/

/*! \fn QJsonArray::const_iterator::const_iterator(const QJsonArray *array, qsizetype index)
    \internal
*/

/*! \typedef QJsonArray::const_iterator::iterator_category

  A synonym for \e {std::random_access_iterator_tag} indicating
  this iterator is a random access iterator.
*/

/*! \typedef QJsonArray::const_iterator::difference_type

    \internal
*/

/*! \typedef QJsonArray::const_iterator::value_type

    \internal
*/

/*! \typedef QJsonArray::const_iterator::reference

    \internal
*/

/*! \typedef QJsonArray::const_iterator::pointer

    \internal
*/

/*! \fn QJsonArray::const_iterator::const_iterator(const iterator &other)

    Constructs a copy of \a other.
*/

/*! \fn const QJsonValueRef QJsonArray::const_iterator::operator*() const

    Returns the current item.
*/

/*! \fn const QJsonValueRef *QJsonArray::const_iterator::operator->() const

    Returns a pointer to the current item.
*/

/*! \fn QJsonValue QJsonArray::const_iterator::operator[](qsizetype j) const

    Returns the item at offset \a j from the item pointed to by this iterator (the item at
    position \c{*this + j}).

    This function is provided to make QJsonArray iterators behave like C++
    pointers.

    \sa operator+()
*/

/*! \fn bool QJsonArray::const_iterator::operator==(const const_iterator &lhs, const const_iterator &rhs)

    Returns \c true if \a lhs points to the same item as \a rhs
    iterator; otherwise returns \c false.

    \sa operator!=()
*/

/*! \fn bool QJsonArray::const_iterator::operator!=(const const_iterator &lhs, const const_iterator &rhs)

    Returns \c true if \a lhs points to a different item than \a rhs
    iterator; otherwise returns \c false.

    \sa operator==()
*/

/*!
    \fn bool QJsonArray::const_iterator::operator<(const const_iterator &lhs, const const_iterator &rhs)

    Returns \c true if the item pointed to by \a lhs iterator is less than
    the item pointed to by the \a rhs iterator.
*/

/*!
    \fn bool QJsonArray::const_iterator::operator<=(const const_iterator &lhs, const const_iterator &rhs)

    Returns \c true if the item pointed to by \a lhs iterator is less than
    or equal to the item pointed to by the \a rhs iterator.
*/

/*!
    \fn bool QJsonArray::const_iterator::operator>(const const_iterator &lhs, const const_iterator &rhs)

    Returns \c true if the item pointed to by \a lhs iterator is greater
    than the item pointed to by the \a rhs iterator.
*/

/*!
    \fn bool QJsonArray::const_iterator::operator>=(const const_iterator &lhs, const const_iterator &rhs)

    Returns \c true if the item pointed to by \a lhs iterator is greater
    than or equal to the item pointed to by the \a rhs iterator.
*/

/*! \fn QJsonArray::const_iterator &QJsonArray::const_iterator::operator++()

    The prefix \c{++} operator, \c{++it}, advances the iterator to the
    next item in the array and returns an iterator to the new current
    item.

    Calling this function on QJsonArray::end() leads to undefined results.

    \sa operator--()
*/

/*! \fn QJsonArray::const_iterator QJsonArray::const_iterator::operator++(int)

    \overload

    The postfix \c{++} operator, \c{it++}, advances the iterator to the
    next item in the array and returns an iterator to the previously
    current item.
*/

/*! \fn QJsonArray::const_iterator &QJsonArray::const_iterator::operator--()

    The prefix \c{--} operator, \c{--it}, makes the preceding item
    current and returns an iterator to the new current item.

    Calling this function on QJsonArray::begin() leads to undefined results.

    \sa operator++()
*/

/*! \fn QJsonArray::const_iterator QJsonArray::const_iterator::operator--(int)

    \overload

    The postfix \c{--} operator, \c{it--}, makes the preceding item
    current and returns an iterator to the previously current item.
*/

/*! \fn QJsonArray::const_iterator &QJsonArray::const_iterator::operator+=(qsizetype j)

    Advances the iterator by \a j items. If \a j is negative, the
    iterator goes backward.

    \sa operator-=(), operator+()
*/

/*! \fn QJsonArray::const_iterator &QJsonArray::const_iterator::operator-=(qsizetype j)

    Makes the iterator go back by \a j items. If \a j is negative,
    the iterator goes forward.

    \sa operator+=(), operator-()
*/

/*! \fn QJsonArray::const_iterator QJsonArray::const_iterator::operator+(qsizetype j) const

    Returns an iterator to the item at \a j positions forward from
    this iterator. If \a j is negative, the iterator goes backward.

    \sa operator-(), operator+=()
*/

/*! \fn QJsonArray::const_iterator QJsonArray::const_iterator::operator-(qsizetype j) const

    Returns an iterator to the item at \a j positions backward from
    this iterator. If \a j is negative, the iterator goes forward.

    \sa operator+(), operator-=()
*/

/*! \fn qsizetype QJsonArray::const_iterator::operator-(const_iterator other) const

    Returns the number of items between the item pointed to by \a
    other and the item pointed to by this iterator.
*/

/*!
    \internal
 */
bool QJsonArray::detach(qsizetype reserve)
{
    if (!a)
        return true;
    a = a->detach(a.data(), reserve ? reserve : size());
    return a;
}

size_t qHash(const QJsonArray &array, size_t seed)
{
    return qHashRange(array.begin(), array.end(), seed);
}

#if !defined(QT_NO_DEBUG_STREAM) && !defined(QT_JSON_READONLY)
QDebug operator<<(QDebug dbg, const QJsonArray &a)
{
    QDebugStateSaver saver(dbg);
    if (!a.a) {
        dbg << "QJsonArray()";
        return dbg;
    }
    QByteArray json;
    QJsonPrivate::Writer::arrayToJson(a.a.data(), json, 0, true);
    dbg.nospace() << "QJsonArray("
                  << json.constData() // print as utf-8 string without extra quotation marks
                  << ")";
    return dbg;
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &stream, const QJsonArray &array)
{
    return stream << QJsonDocument{array};
}

QDataStream &operator>>(QDataStream &stream, QJsonArray &array)
{
    QJsonDocument doc;
    stream >> doc;
    array = doc.array();
    return stream;
}
#endif

QT_END_NAMESPACE

