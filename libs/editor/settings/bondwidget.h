/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#ifndef PLASMA_NM_BOND_WIDGET_H
#define PLASMA_NM_BOND_WIDGET_H

#include <QWidget>
#include <QMenu>
#include <QListWidgetItem>
#include <QDBusPendingCallWatcher>

#include <NetworkManagerQt/BondSetting>

#include "settingwidget.h"

namespace Ui
{
class BondWidget;
}

class Q_DECL_EXPORT BondWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit BondWidget(const QString & masterUuid, const NetworkManager::Setting::Ptr &setting = NetworkManager::Setting::Ptr(),
               QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~BondWidget();

    void loadConfig(const NetworkManager::Setting::Ptr &setting) Q_DECL_OVERRIDE;

    QVariantMap setting() const Q_DECL_OVERRIDE;

    virtual bool isValid() const Q_DECL_OVERRIDE;

private Q_SLOTS:
    void addBond(QAction * action);
    void currentBondChanged(QListWidgetItem * current, QListWidgetItem * previous);
    void bondAddComplete(QDBusPendingCallWatcher * watcher);

    void editBond();
    void deleteBond();

    void populateBonds();

private:
    QString m_uuid;
    Ui::BondWidget * m_ui;
    QMenu * m_menu;
};

#endif // PLASMA_NM_BOND_WIDGET_H
