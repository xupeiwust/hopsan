/*-----------------------------------------------------------------------------
 This source file is a part of Hopsan

 Copyright (c) 2009 to present year, Hopsan Group

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 For license details and information about the Hopsan Group see the files
 GPLv3 and HOPSANGROUP in the Hopsan source code root directory

 For author and contributor information see the AUTHORS file
-----------------------------------------------------------------------------*/

//!
//! @file   GUIConnector.h
//! @author Flumes <flumes@lists.iei.liu.se>
//! @date   2010-01-01
//!
//! @brief Contains the GUIConnector class
//!
//$Id$

#ifndef GUICONNECTOR_H
#define GUICONNECTOR_H

#include <QGraphicsWidget>
#include <QObject>
#include <QPen>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMoveEvent>
#include <QTextStream>
#include <QtXml>

#include "common.h"
#include "GUIConnectorAppearance.h"

class ConnectorLine;
class GraphicsView;
class WorkspaceObject;
class Port;
class SystemContainer;
class ContainerObject;
class Component;

class Connector : public QGraphicsWidget
{
    Q_OBJECT
    friend class ConnectorLine;
    friend class AnimatedConnector;
public:
    Connector(ContainerObject *pParentContainer);
    ~Connector();

    void setParentContainer(ContainerObject *pParentContainer);
    ContainerObject *getParentContainer();

    Port *getStartPort();
    Port *getEndPort();
    QString getStartPortName() const;
    QString getEndPortName() const;
    QString getStartComponentName() const;
    QString getEndComponentName() const;
    void setStartPort(Port *pPort);
    void setEndPort(Port *pPort);

    void finishCreation();

    void addPoint(QPointF point);
    void removePoint(bool deleteIfEmpty = false);
    void setPointsAndGeometries(const QVector<QPointF> &rPoints, const QStringList &rGeometries);
    QPointF getStartPoint();
    QPointF getEndPoint();
    ConnectorLine *getLine(int line);
    ConnectorLine *getLastLine();
    int getNumberOfLines();
    bool isFirstOrLastDiagonal();
    bool isFirstAndLastDiagonal();
    ConnectorGeometryEnumT getGeometry(const int lineNumber);

    void refreshConnectorAppearance();
    void refreshPen();

    bool isConnected();
    bool isMakingDiagonal() const;
    bool isActive() const;
    bool isBroken() const;
    bool isDangling();
    bool isVolunector() const;

    void makeVolunector();
    void makeVolunector(Component *pComponent);
    Component* getVolunectorComponent();

    void saveToDomElement(QDomElement &rDomElement);

public slots:
    void setIsoStyle(GraphicsTypeEnumT gfxType);
    void drawConnector(bool alignOperation=false);
    void updateStartPoint(QPointF point);
    void updateEndPoint(QPointF point);
    void moveAllPoints(double offsetX, double offsetY);
    void updateLine(int);
    void makeDiagonal(bool diagonal);
    void doSelect(bool lineSelected, int lineNumber);
    void selectIfBothComponentsSelected();
    void setColor(const QColor &rColor);
    void setActive();
    void setPassive();
    void setHovered();
    void setUnHovered();
    void deleteMe(UndoStatusEnumT undo=Undo);
    void deleteMeWithNoUndo();
    void deselect();
    void select();
    void setDashed(bool value);

private slots:
    void setVisible(bool visible);

signals:
    void connectionFinished();

private:
    void refreshPen(const QString &type);
    void determineAppearance();
    void refreshConnectedSystemportsGraphics();
    void disconnectPortSigSlots(Port* pPort);
    void connectPortSigSlots(Port* pPort);
    void addLine(ConnectorLine *pLine);
    void removeAllLines();
    void updateStartEndPositions();

    bool mIsActive;
    bool mIsBroken;
    bool mMakingDiagonal;
    bool mIsDashed;

    ContainerObject *mpParentContainerObject;
    ConnectorAppearance *mpConnectorAppearance;
    Port *mpStartPort;
    Port *mpEndPort;

    QVector<ConnectorLine*> mpLines;
    QVector<ConnectorGeometryEnumT> mGeometries;
    QVector<QPointF> mPoints;

    Component *mpVolunectorComponent;
};


class ConnectorLine : public QObject, public QGraphicsLineItem
{
    friend class Connector;
    friend class AnimatedConnector;
    Q_OBJECT
public:
    ConnectorLine(double x1, double y1, double x2, double y2, int lineNumber, Connector *pParentConnector);
    ~ConnectorLine();

    void paint(QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w);
    void addEndArrow();
    void addStartArrow();
    void setLine(QPointF pos1, QPointF pos2);
    int getLineNumber();
    void setPen(const QPen &pen);

public slots:
    void setConnectorFinished();

signals:
    void lineMoved(int);
    void lineHoverEnter();
    void lineHoverLeave();
    void lineSelected(bool isSelected, int lineNumber);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    void clearArrows();

    Connector *mpParentConnector;

    bool mParentConnectorFinished;
    bool mHasStartArrow;
    bool mHasEndArrow;
    int mLineNumber;

    ConnectorGeometryEnumT mGeometry;
    QGraphicsLineItem *mArrowLine1;
    QGraphicsLineItem *mArrowLine2;
    double mArrowSize;
    double mArrowAngle;

    QPointF mStartPos;
    QPointF mEndPos;
    QPointF mOldPos;

    QGraphicsLineItem *mpVolunectorLine;
};


class Volunector : public Connector
{
public:
    Volunector(ContainerObject *pParentContainer);
private:
    Component *mpVolunectorComponent;
};

#endif // GUICONNECTOR_H
