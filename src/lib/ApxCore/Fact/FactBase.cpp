﻿/*
 * Copyright (C) 2011 Aliaksei Stratsilatau <sa@uavos.com>
 *
 * This file is part of the UAV Open System Project
 *  http://www.uavos.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "FactBase.h"
#include "Fact.h"
#include "FactPropertyBinding.h"

FactBase::FactBase(QObject *parent, const QString &name, FactBase::Flags flags)
    : QObject(parent)
{
    setName(makeNameUnique(name));
    setTreeType(Flag(uint(flags) & TypeMask));
    setOptions(flags & OptsMask);
}
FactBase::~FactBase()
{
    //qDebug() << path() << parent();
    //removed();
    /*if (parentFact()) {
        parentFact()->m_children.removeAll(static_cast<Fact *>(this));
    }*/
    //setParentFact(nullptr);
    //unbindProperties();
    //emit destroyed(this);
    for (auto i : m_actions) {
        delete i;
    }
    for (auto i : m_children) {
        delete i;
    }
    //qDeleteAll(m_children);
    //removeAll();
}

void FactBase::bindProperty(Fact *src, QString name, bool oneway)
{
    unbindProperties(src, name);

    if (src == this) {
        qWarning() << "recursive bind:" << path() << name;
        return;
    }

    FactPropertyBinding *b = new FactPropertyBinding(static_cast<Fact *>(this), src, name);
    _property_binds.append(b);

    if (oneway)
        return;

    src->bindProperty(static_cast<Fact *>(this), name, true);
}
void FactBase::unbindProperty(QString name)
{
    unbindProperties(nullptr, name);
}
void FactBase::unbindProperties(Fact *src, const QString &name)
{
    foreach (auto i, _property_binds) {
        if (!i->match(src, name))
            continue;
        _property_binds.removeOne(i);
        delete i;
    }
}

//=============================================================================
const FactList &FactBase::children() const
{
    return m_children;
}
const FactList &FactBase::actions() const
{
    return m_actions;
}
//=============================================================================
void FactBase::addChild(Fact *item)
{
    //item->Qobject::setParent(this);
    if (item->treeType() == Action) {
        if (!m_actions.contains(item)) {
            m_actions.append(item);
            emit actionsUpdated();
        }
        return;
    }
    if (m_children.contains(item))
        return;
    if (property(item->name().toUtf8()).isValid()) {
        qWarning() << "Property override:" << path() << item->name();
    }
    emit itemToBeInserted(m_children.count(), item);
    m_children.append(item);
    updateChildrenNums();
    updateSize();
    emit itemInserted(item);
}
void FactBase::removeChild(Fact *item)
{
    int i = m_actions.indexOf(item);
    if (i >= 0) {
        m_actions.removeAll(item);
        emit actionsUpdated();
    }
    i = m_children.indexOf(item);
    if (i < 0)
        return;
    emit itemToBeRemoved(i, item);
    m_children.removeAt(i);
    updateChildrenNums();
    updateSize();
    emit itemRemoved(item);
}
void FactBase::removeAll()
{
    if (m_children.count() <= 0)
        return;
    for (int i = 0; i < m_children.count(); i++) {
        FactBase *item = child(i);
        disconnect(this, nullptr, item, nullptr);
        item->removeAll();
    }
    //qDebug()<<"removeAll"<<this;
    while (m_children.count() > 0) {
        FactBase *item = child(m_children.count() - 1);
        disconnect(this, nullptr, item, nullptr);
        emit itemToBeRemoved(m_children.count() - 1, item);
        m_children.takeLast();
        m_size = m_children.count();
        item->removed();
        emit itemRemoved(item);
        item->deleteLater();
    }
    emit sizeChanged();
}
void FactBase::moveChild(Fact *item, int n, bool safeMode)
{
    int i = m_children.indexOf(item);
    if (i < 0 || i == n)
        return;
    if (safeMode) {
        emit itemToBeRemoved(i, item);
        m_children.removeAt(i);
        emit itemRemoved(item);
        emit itemToBeInserted(n, item);
        m_children.insert(n, item);
        emit itemInserted(item);
        updateChildrenNums();
    } else {
        if (n > i)
            n++;
        emit itemToBeMoved(i, n, item);
        m_children.removeAt(i);
        m_children.insert(n, item);
        emit itemMoved(item);
    }
    for (int i = 0; i < m_children.count(); ++i)
        child(i)->updateNum();
}
//=============================================================================
void FactBase::remove()
{
    setParentFact(nullptr);
    emit removed();
    deleteLater();
}
void FactBase::move(int n, bool safeMode)
{
    FactBase *p = parentFact();
    if (p)
        p->moveChild(static_cast<Fact *>(this), n, safeMode);
}
//=============================================================================
QString FactBase::makeNameUnique(const QString &s)
{
    QString sr = s.simplified()
                     .replace(' ', '_')
                     .replace('.', '_')
                     .replace(':', '_')
                     .replace('/', '_')
                     .replace('\\', '_')
                     .replace('?', '_')
                     .replace('-', '_')
                     .replace('+', '_');

    if (!parentFact())
        return sr;

    int i = 0;
    nameSuffix = QString();
    QString suffix;
    while (1) {
        FactBase *dup = nullptr;
        for (int i = 0; i < parentFact()->size(); ++i) {
            FactBase *item = parentFact()->child(i);
            if (item == this)
                continue;
            if (item->name() == (sr + suffix)) {
                dup = item;
                break;
            }
        }
        if (!dup)
            break;
        suffix = QString("_%1").arg(++i, 3, 10, QChar('0'));
    }
    nameSuffix = suffix;
    return sr;
}
//=============================================================================
int FactBase::num() const
{
    return m_num;
}
//=============================================================================
Fact *FactBase::child(int n) const
{
    return m_children.value(n, nullptr);
}
//=============================================================================
int FactBase::indexOfChild(Fact *item) const
{
    return m_children.indexOf(item);
}
int FactBase::indexInParent() const
{
    return parentFact()
               ? parentFact()->indexOfChild(static_cast<Fact *>(const_cast<FactBase *>(this)))
               : -1;
}
//=============================================================================
Fact *FactBase::child(const QString &name, Qt::CaseSensitivity cs) const
{
    for (auto i : m_children) {
        if (i->objectName().compare(name, cs) == 0 || i->name().compare(name, cs) == 0)
            return i;
    }
    return nullptr;
}
//=============================================================================
QStringList FactBase::pathStringList(int maxLevel) const
{
    QStringList st;
    for (const FactBase *i = this; i; i = i->parentFact()) {
        st.insert(0, i->name());
        if (i->treeType() == Root)
            break;
        if (maxLevel-- == 0)
            break;
    }
    return st;
}
QString FactBase::path(int maxLevel, const QChar pathDelimiter) const
{
    return pathStringList(maxLevel).join(pathDelimiter);
}
FactList FactBase::pathList() const
{
    FactList list;
    for (const FactBase *i = this; i;) {
        list.append(static_cast<Fact *>(const_cast<FactBase *>(i)));
        if (i->treeType() == Root)
            break;
        i = i->parentFact();
    }
    return list;
}
QString FactBase::path(const FactBase *root) const
{
    int level = -1;
    for (const FactBase *i = this; i && i != root; i = i->parentFact()) {
        level++;
    }
    return path(level);
}
//=============================================================================
void FactBase::updateNum()
{
    int v = indexInParent();
    if (v < 0)
        return;
    if (m_num == v)
        return;
    m_num = v;
    emit numChanged();
}
void FactBase::updateSize()
{
    int v = m_children.count();
    if (m_size == v)
        return;
    m_size = v;
    emit sizeChanged();
}
void FactBase::updateChildrenNums()
{
    for (int i = 0; i < m_children.count(); ++i)
        child(i)->updateNum();
}
//=============================================================================
FactBase::Flag FactBase::treeType(void) const
{
    return m_treeType;
}
void FactBase::setTreeType(FactBase::Flag v)
{
    v = static_cast<Flag>(v & TypeMask);
    if (m_treeType == v)
        return;
    m_treeType = v;
    emit treeTypeChanged();
    if (v == Action && parentFact()) {
        parentFact()->removeChild(static_cast<Fact *>(this));
        parentFact()->addChild(static_cast<Fact *>(this));
    }
}
FactBase::Flags FactBase::options(void) const
{
    return m_options;
}
void FactBase::setOptions(FactBase::Flags v)
{
    v &= OptsMask;
    if (m_options == v)
        return;
    m_options = v;
    emit optionsChanged();
    if (m_options & Section)
        setTreeType(Group);
}
void FactBase::setOption(FactBase::Flag opt, bool v)
{
    if (v)
        setOptions(options() | opt);
    else
        setOptions(options() & (~opt));
}
int FactBase::size(void) const
{
    return m_size;
}
QString FactBase::name(void) const
{
    if (m_name.contains('#'))
        return QString(m_name).replace('#', QString::number(num() + 1)) + nameSuffix;
    return m_name + nameSuffix;
}
void FactBase::setName(const QString &v)
{
    QString s = makeNameUnique(v);
    if (m_name == s && nameSuffix.isEmpty())
        return;
    //emit itemRemoved(this);
    m_name = s;
    setObjectName(name());
    //emit itemAdded(this);
    emit nameChanged();
}
Fact *FactBase::parentFact() const
{
    return qobject_cast<Fact *>(m_parentFact);
}
void FactBase::setParentFact(Fact *v)
{
    FactBase *prevParent = parentFact();
    if (prevParent == v)
        return;
    if (prevParent) {
        prevParent->removeChild(static_cast<Fact *>(this));
    }

    //QObject::setParent(v);
    m_parentFact = v;
    emit parentFactChanged();
    if (!v)
        return;
    v->addChild(static_cast<Fact *>(this));
    updatePath();
}
void FactBase::updatePath()
{
    for (auto f : m_children) {
        f->updatePath();
    }
    emit pathChanged();
}
