/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kcmidentitymodel.h"
#include "networkmodel.h"
#include "networkmodelitem.h"
#include "uiutils.h"

#include <NetworkManagerQt/Settings>

#include <QIcon>
#include <QFont>

#include <KLocalizedString>

KcmIdentityModel::KcmIdentityModel(QObject* parent)
    : QIdentityProxyModel(parent)
{
    NetworkModel * baseModel = new NetworkModel(this);
    setSourceModel(baseModel);
}

KcmIdentityModel::~KcmIdentityModel()
{
}

Qt::ItemFlags KcmIdentityModel::flags(const QModelIndex& index) const
{
    const QModelIndex mappedProxyIndex = index.sibling(index.row(), 0);
    return QIdentityProxyModel::flags(mappedProxyIndex) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int KcmIdentityModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return 3;
}

QHash<int, QByteArray> KcmIdentityModel::roleNames() const
{
    QHash<int, QByteArray> roles = QIdentityProxyModel::roleNames();
    roles[KcmConnectionIconRole] = "KcmConnectionIcon";
    roles[KcmConnectionTypeRole] = "KcmConnectionType";
    return roles;
}

QVariant KcmIdentityModel::data(const QModelIndex& index, int role) const
{
    const QModelIndex sourceIndex = sourceModel()->index(index.row(), 0);
    NetworkManager::ConnectionSettings::ConnectionType type = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(sourceModel()->data(sourceIndex, NetworkModel::TypeRole).toInt());

    NetworkManager::ConnectionSettings::Ptr settings;
    NetworkManager::VpnSetting::Ptr vpnSetting ;
    if (type == NetworkManager::ConnectionSettings::Vpn) {
        settings = NetworkManager::findConnection(sourceModel()->data(sourceIndex, NetworkModel::ConnectionPathRole).toString())->settings();
        if (settings) {
            vpnSetting = settings->setting(NetworkManager::Setting::Vpn).staticCast<NetworkManager::VpnSetting>();
        }
    }

    QString tooltip;
    const QString iconName = UiUtils::iconAndTitleForConnectionSettingsType(type, tooltip);

    if (role == KcmConnectionIconRole) {
        return iconName;
    } else if (role == KcmConnectionTypeRole) {
        if (type == NetworkManager::ConnectionSettings::Vpn && vpnSetting) {
            return QString("%1 (%2)").arg(tooltip).arg(vpnSetting->serviceType().section('.', -1));
        }
        return tooltip;
    } else {
        return sourceModel()->data(index, role);
    }

    return QVariant();
}

QModelIndex KcmIdentityModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex KcmIdentityModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if (proxyIndex.column() > 0) {
        return QModelIndex();
    }

    return QIdentityProxyModel::mapToSource(proxyIndex);
}

