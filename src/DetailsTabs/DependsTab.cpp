/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "DependsTab.h"

// Qt includes
#include <QtGui/QComboBox>

// KDE includes
#include <KDebug>
#include <KLocale>
#include <KTextBrowser>

// LibQApt includes
#include <libqapt/package.h>

DependsTab::DependsTab(QWidget *parent)
    : KVBox(parent)
    , m_package(0)
    , m_comboBox(0)
    , m_dependsBrowser(0)
{
    m_comboBox = new QComboBox(this);
    m_comboBox->addItem(i18nc("@item:inlistbox", "Depenencies of the Current Version"));
    m_comboBox->addItem(i18nc("@item:inlistbox", "Depenencies of the Latest Version"));
    m_comboBox->addItem(i18nc("@item:inlistbox", "Provided Packages"));
    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(populateDepends(int)));
    m_dependsBrowser = new KTextBrowser(this);
}

DependsTab::~DependsTab()
{
}

void DependsTab::setPackage(QApt::Package *package)
{
    m_package = package;
    m_dependsBrowser->setText(QString());
    kDebug() << m_comboBox->currentIndex();
    populateDepends(m_comboBox->currentIndex());
}

void DependsTab::populateDepends(int index)
{
    m_dependsBrowser->setText(QString());
    switch (index) {
        case 0:
            foreach (const QString &string, m_package->dependencyList(false)) {
                m_dependsBrowser->append(string);
            }
            break;
        case 1:
            foreach (const QString &string, m_package->dependencyList(true)) {
                m_dependsBrowser->append(string);
            }
            break;
        case 2:
            foreach (const QString &string, m_package->providesList()) {
                m_dependsBrowser->append(string);
            }
            break;
    }
}

#include "DependsTab.moc"