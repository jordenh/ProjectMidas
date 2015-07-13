/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp org.bluez.obex.Transfer1.xml -p obex_transfer1_bluez5
 *
 * qdbusxml2cpp is Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef OBEX_TRANSFER1_BLUEZ5_H_1400058158
#define OBEX_TRANSFER1_BLUEZ5_H_1400058158

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface org.bluez.obex.Transfer1
 */
class OrgBluezObexTransfer1Interface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.bluez.obex.Transfer1"; }

public:
    OrgBluezObexTransfer1Interface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~OrgBluezObexTransfer1Interface();

    Q_PROPERTY(QString Filename READ filename)
    inline QString filename() const
    { return qvariant_cast< QString >(property("Filename")); }

    Q_PROPERTY(QString Name READ name)
    inline QString name() const
    { return qvariant_cast< QString >(property("Name")); }

    Q_PROPERTY(QDBusObjectPath Session READ session)
    inline QDBusObjectPath session() const
    { return qvariant_cast< QDBusObjectPath >(property("Session")); }

    Q_PROPERTY(qulonglong Size READ size)
    inline qulonglong size() const
    { return qvariant_cast< qulonglong >(property("Size")); }

    Q_PROPERTY(QString Status READ status)
    inline QString status() const
    { return qvariant_cast< QString >(property("Status")); }

    Q_PROPERTY(qulonglong Transferred READ transferred)
    inline qulonglong transferred() const
    { return qvariant_cast< qulonglong >(property("Transferred")); }

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> Cancel()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Cancel"), argumentList);
    }

    inline QDBusPendingReply<> Resume()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Resume"), argumentList);
    }

    inline QDBusPendingReply<> Suspend()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Suspend"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

namespace org {
  namespace bluez {
    namespace obex {
      typedef ::OrgBluezObexTransfer1Interface Transfer1;
    }
  }
}
#endif
