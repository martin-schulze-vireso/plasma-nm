/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#include "bssidcombobox.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>
#include <kicon.h>

#include <QtAlgorithms>

bool signalCompare(const NetworkManager::AccessPoint::Ptr & one, const NetworkManager::AccessPoint::Ptr & two) {
    return one->signalStrength() > two->signalStrength();
}

BssidComboBox::BssidComboBox(QWidget *parent) :
    KComboBox(parent), m_dirty(false)
{
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);

    connect(this, SIGNAL(editTextChanged(QString)), SLOT(editTextChanged(QString)));
    connect(this, SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChanged(int)));
}

QString BssidComboBox::bssid() const
{
    QString result;
    if (!m_dirty)
        result = itemData(currentIndex()).toString();
    else
        result = currentText(); // FIXME validate

    //qDebug() << "Result:" << currentIndex() << result;

    return result;
}

void BssidComboBox::editTextChanged(const QString &)
{
    m_dirty = true;
}

void BssidComboBox::currentIndexChanged(int)
{
    m_dirty = false;
}

void BssidComboBox::init(const QString & bssid, const QString &ssid)
{
    m_initialBssid = bssid;

    //qDebug() << "Initial ssid:" << m_initialBssid;

    QList<NetworkManager::AccessPoint::Ptr> aps;

    foreach(const NetworkManager::Device::Ptr & device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
            NetworkManager::WirelessNetwork::Ptr wifiNetwork = wifiDevice->findNetwork(ssid);

            if (!wifiNetwork) {
                continue;
            }

            foreach (const NetworkManager::AccessPoint::Ptr & newAp, wifiNetwork->accessPoints()) {
                bool found = false;;
                foreach (const NetworkManager::AccessPoint::Ptr & existingAp, aps) {
                    if (newAp->hardwareAddress() == existingAp->hardwareAddress()) {
                        if (newAp->signalStrength() > existingAp->signalStrength()) {
                            aps.removeOne(existingAp);
                            break;
                        } else {
                            found = true;
                            break;
                        }
                    }
                }

                if (!found) {
                    aps << newAp;
                }
            }
        }
    }

    qSort(aps.begin(), aps.end(), signalCompare);
    addBssidsToCombo(aps);

    int index = findData(m_initialBssid);
    if (index == -1) {
        insertItem(0, m_initialBssid, m_initialBssid);
        setCurrentIndex(0);
    } else {
        setCurrentIndex(index);
    }
}

void BssidComboBox::addBssidsToCombo(const QList<NetworkManager::AccessPoint::Ptr> & aps)
{
    clear();

    foreach (const NetworkManager::AccessPoint::Ptr & ap, aps) {
        if (!ap) {
            continue;
        }

        QString text = QString("%1 (%2%)").arg(ap->hardwareAddress()).arg(ap->signalStrength());
        addItem(text, QVariant::fromValue(ap->hardwareAddress()));
    }
}
