/***************************************************************************
 *   Copyright © 2013 Lukas Appelhans <l.appelhans@gmx.de>                 *
 *   Copyright © 2017 Jan Grulich <jgrulich@redhat.com>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/
#ifndef FLATPAKNOTIFIER_H
#define FLATPAKNOTIFIER_H

#include <BackendNotifierModule.h>

extern "C" {
#include <flatpak.h>
}

class FlatpakNotifier : public BackendNotifierModule
{
Q_OBJECT
Q_PLUGIN_METADATA(IID "org.kde.discover.BackendNotifierModule")
Q_INTERFACES(BackendNotifierModule)
public:
    explicit FlatpakNotifier(QObject* parent = nullptr);
    ~FlatpakNotifier() override;

    bool hasUpdates() override;
    bool hasSecurityUpdates() override { return false; }
    void recheckSystemUpdateNeeded() override;
    bool needsReboot() const override { return false; }

private:
    void onFetchUpdatesFinished(FlatpakInstallation *flatpakInstallation, GPtrArray *updates);
    void loadRemoteUpdates(FlatpakInstallation *flatpakInstallation);
    bool setupFlatpakInstallations(GError **error);

    struct Installation {
        ~Installation();

        bool m_hasUpdates = false;
        GFileMonitor *m_monitor = nullptr;
        FlatpakInstallation *m_installation = nullptr;
    };
    Installation m_user;
    Installation m_system;
    GCancellable * const m_cancellable;
};

#endif
