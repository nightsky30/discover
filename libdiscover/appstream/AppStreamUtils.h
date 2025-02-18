/***************************************************************************
 *   Copyright © 2017 Aleix Pol Gonzalez <aleixpol@kde.org>                *
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

#ifndef APPSTREAMUTILS_H
#define APPSTREAMUTILS_H

#include <QUrl>
#include <QList>
#include <AppStreamQt/image.h>
#include <AppStreamQt/component.h>

namespace AppStreamUtils
{
Q_DECL_EXPORT QUrl imageOfKind(const QList<AppStream::Image> &images, AppStream::Image::Kind kind);

Q_DECL_EXPORT QString changelogToHtml(const AppStream::Component &appdata);

Q_DECL_EXPORT QPair<QList<QUrl>, QList<QUrl>> fetchScreenshots(const AppStream::Component &appdata);

Q_DECL_EXPORT QJsonArray licenses(const AppStream::Component &appdata);

Q_DECL_EXPORT QString appstreamId(const QUrl &appstreamUrl);

}

#endif
