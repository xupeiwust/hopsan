#include "PlotTab.h"

#include "PlotWindow.h"
#include "Configuration.h"
#include "PlotCurve.h"

#include "GUIObjects/GUIContainerObject.h"

#include "Utilities/GUIUtilities.h"
#include <limits>

#include "qwt_scale_engine.h"
#include "qwt_symbol.h"
#include "qwt_text_label.h"
#include "qwt_plot_renderer.h"
#include "qwt_scale_map.h"
#include "qwt_plot.h"
//#include "qwt_legend_item.h"
#include "qwt_legend.h"
//#include "q_layout.h"
#include "qwt_scale_draw.h"
#include "qwt_scale_widget.h"
#include <qwt_dyngrid_layout.h>

#include "MainWindow.h"
#include "Widgets/ProjectTabWidget.h"
#include "Widgets/HcomWidget.h"

#include "version_gui.h"

#include <cmath>

const double DBLMAX = std::numeric_limits<double>::max();







//! @brief Constructor for plot tabs.
//! @param parent Pointer to the plot window the tab belongs to
PlotTab::PlotTab(PlotWindow *parent)
    : QWidget(parent)
{
    mpParentPlotWindow = parent;
    setAcceptDrops(true);
    setMouseTracking(true);
    mHasSpecialXAxis=false;
    mSpecialXVectorLabel = QString("Time [s]");
    mLeftAxisLogarithmic = false;
    mRightAxisLogarithmic = false;
    mBottomAxisLogarithmic = false;
    mIsSpecialPlot = false;

    //Initiate default values for left y-axis
    mCurrentUnitsLeft.insert("Value", gConfig.getDefaultUnit("Value"));
    mCurrentUnitsLeft.insert("Pressure", gConfig.getDefaultUnit("Pressure"));
    mCurrentUnitsLeft.insert("Flow", gConfig.getDefaultUnit("Flow"));
    mCurrentUnitsLeft.insert("Position", gConfig.getDefaultUnit("Position"));
    mCurrentUnitsLeft.insert("Velocity", gConfig.getDefaultUnit("Velocity"));
    mCurrentUnitsLeft.insert("Force", gConfig.getDefaultUnit("Force"));
    mCurrentUnitsLeft.insert("Torque", gConfig.getDefaultUnit("Torque"));
    mCurrentUnitsLeft.insert("Angle", gConfig.getDefaultUnit("Angle"));
    mCurrentUnitsLeft.insert("Angular Velocity", gConfig.getDefaultUnit("Angular Velocity"));

    //Initiate default values for right y-axis
    mCurrentUnitsRight.insert("Value", gConfig.getDefaultUnit("Value"));
    mCurrentUnitsRight.insert("Pressure", gConfig.getDefaultUnit("Pressure"));
    mCurrentUnitsRight.insert("Flow", gConfig.getDefaultUnit("Flow"));
    mCurrentUnitsRight.insert("Position", gConfig.getDefaultUnit("Position"));
    mCurrentUnitsRight.insert("Velocity", gConfig.getDefaultUnit("Velocity"));
    mCurrentUnitsRight.insert("Force", gConfig.getDefaultUnit("Force"));
    mCurrentUnitsRight.insert("Torque", gConfig.getDefaultUnit("Torque"));
    mCurrentUnitsRight.insert("Angle", gConfig.getDefaultUnit("Angle"));
    mCurrentUnitsRight.insert("Angular Velocity", gConfig.getDefaultUnit("Angular Velocity"));

    mCurveColors << "Blue" << "Red" << "Green" << "Orange" << "Pink" << "Brown" << "Purple" << "Gray";

    for(int plotID=0; plotID<2; ++plotID)
    {
        //Plots
        mpQwtPlots[plotID] = new QwtPlot(mpParentPlotWindow);
        mpQwtPlots[plotID]->setMouseTracking(true);
        mpQwtPlots[plotID]->setAcceptDrops(false);
        mpQwtPlots[plotID]->setCanvasBackground(QColor(Qt::white));
        mpQwtPlots[plotID]->setAutoReplot(true);

        //Panning Tool
        mpPanner[plotID] = new QwtPlotPanner(mpQwtPlots[plotID]->canvas());
        mpPanner[plotID]->setMouseButton(Qt::LeftButton);
        mpPanner[plotID]->setEnabled(false);

        //Rubber Band Zoom
        mpZoomer[plotID] = new QwtPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, mpQwtPlots[plotID]->canvas());      //Zoomer for left y axis
        mpZoomer[plotID]->setMaxStackDepth(10000);
        mpZoomer[plotID]->setRubberBand(QwtPicker::NoRubberBand);
        mpZoomer[plotID]->setRubberBandPen(QColor(Qt::green));
        mpZoomer[plotID]->setTrackerMode(QwtPicker::ActiveOnly);
        mpZoomer[plotID]->setTrackerPen(QColor(Qt::white));
        mpZoomer[plotID]->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
        mpZoomer[plotID]->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
        mpZoomer[plotID]->setZoomBase(QRectF());
        mpZoomer[plotID]->setEnabled(false);

        mpZoomerRight[plotID] = new QwtPlotZoomer( QwtPlot::xTop, QwtPlot::yRight, mpQwtPlots[plotID]->canvas());   //Zoomer for right y axis
        mpZoomerRight[plotID]->setMaxStackDepth(10000);
        mpZoomerRight[plotID]->setRubberBand(QwtPicker::NoRubberBand);
        mpZoomerRight[plotID]->setRubberBandPen(QColor(Qt::green));
        mpZoomerRight[plotID]->setTrackerMode(QwtPicker::ActiveOnly);
        mpZoomerRight[plotID]->setTrackerPen(QColor(Qt::white));
        mpZoomerRight[plotID]->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
        mpZoomerRight[plotID]->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
        mpZoomerRight[plotID]->setEnabled(false);

        //Wheel Zoom
        mpMagnifier[plotID] = new QwtPlotMagnifier(mpQwtPlots[plotID]->canvas());
        mpMagnifier[plotID]->setAxisEnabled(QwtPlot::yLeft, true);
        mpMagnifier[plotID]->setAxisEnabled(QwtPlot::yRight, true);
        mpMagnifier[plotID]->setZoomInKey(Qt::Key_Plus, Qt::ControlModifier);
        mpMagnifier[plotID]->setWheelFactor(1.1);
        mpMagnifier[plotID]->setMouseButton(Qt::NoButton, Qt::NoModifier);
        mpMagnifier[plotID]->setEnabled(true);

        mpGrid[plotID] = new QwtPlotGrid;
        mpGrid[plotID]->enableXMin(true);
        mpGrid[plotID]->enableYMin(true);
        mpGrid[plotID]->setMajorPen(QPen(Qt::black, 0, Qt::DotLine));
        mpGrid[plotID]->setMinorPen(QPen(Qt::gray, 0 , Qt::DotLine));
        mpGrid[plotID]->attach(mpQwtPlots[plotID]);
    }

    mpBarPlot = new QSint::BarChartPlotter(this);

    //Curve Marker Symbol
    mpMarkerSymbol = new QwtSymbol();
    mpMarkerSymbol->setStyle(QwtSymbol::XCross);
    mpMarkerSymbol->setSize(10,10);

    // Legend Stuff
    constructLegendSettingsDialog();

    mpExternalLegend = 0; //No external legend by default

    mpRightPlotLegend = new PlotLegend(QwtPlot::yRight);
    mpRightPlotLegend->attach(this->getPlot());
    mpRightPlotLegend->setAlignment(Qt::AlignRight);

    mpLeftPlotLegend = new PlotLegend(QwtPlot::yLeft);
    mpLeftPlotLegend->attach(this->getPlot());
    mpLeftPlotLegend->setAlignment(Qt::AlignLeft);


    // Create the lock axis dialog
    constructAxisSettingsDialog();


    mpLayouta = new QGridLayout(this);



    for(int plotID=0; plotID<2; ++plotID)
    {
        mpQwtPlots[plotID]->setAutoFillBackground(true);
        mpQwtPlots[plotID]->setPalette(gConfig.getPalette());
        mpLayouta->addWidget(mpQwtPlots[plotID]);

    }

    connect(mpQwtPlots[FIRSTPLOT], SIGNAL(legendClicked(QwtPlotItem*)), this, SLOT(updateLegend(QwtPlotItem *)));//QwtPlotItem *, bool)));

    for(int plotID=1; plotID<2; ++plotID)       //Hide all plots except first one by default
    {
        showPlot(HopsanPlotID(plotID), false);
    }
    mpBarPlot->setVisible(false);

    mpQwtPlots[FIRSTPLOT]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}


//! @brief Destructor for plot tab. Removes all curves before tab is deleted.
PlotTab::~PlotTab()
{
    for(int plotID=0; plotID<2; ++plotID)
    {
        while(!mPlotCurvePtrs[plotID].empty())
        {
            removeCurve(mPlotCurvePtrs[plotID].last());
        }
    }
}

void PlotTab::applyLegendSettings()
{
    // Show/change internal legneds
    if(mpLegendsInternalEnabledCheckBox->isChecked())
    {
        mpRightPlotLegend->show();
        mpLeftPlotLegend->show();

        mpRightPlotLegend->setMaxColumns(mpLegendCol->value());
        mpLeftPlotLegend->setMaxColumns(mpLegendCol->value());

        QString symStyle = mpLegendSym->currentText();

        setLegendSymbol(symStyle);

        mpRightPlotLegend->setBackgroundMode(HopQwtPlotLegendItem::BackgroundMode(mpLegendBg->currentIndex()));
        mpLeftPlotLegend->setBackgroundMode(HopQwtPlotLegendItem::BackgroundMode(mpLegendBg->currentIndex()));

        int alignL = mpLegendLPosition->currentIndex();
        int alignR = mpLegendRPosition->currentIndex();

        if ( alignL == 0 )
        {
            mpLeftPlotLegend->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        }
        else if ( alignL == 1 )
        {
            mpLeftPlotLegend->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
        }
        else
        {
            mpLeftPlotLegend->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        }

        if ( alignR == 0 )
        {
            mpRightPlotLegend->setAlignment(Qt::AlignTop | Qt::AlignRight);
        }
        else if ( alignR == 1 )
        {
            mpRightPlotLegend->setAlignment(Qt::AlignBottom | Qt::AlignRight);
        }
        else
        {
            mpRightPlotLegend->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
        }

        QColor LegColBlob = QColor(mpLegendBlob->currentText());
        mpRightPlotLegend->setBackgroundBrush(LegColBlob);
        mpLeftPlotLegend->setBackgroundBrush(LegColBlob);

        QFont fontl = mpLeftPlotLegend->font();
        fontl.setPointSize(mpLegendSize->value());
        mpLeftPlotLegend->setFont(fontl);

        QFont fontr = mpRightPlotLegend->font();
        fontr.setPointSize(mpLegendSize->value());
        mpRightPlotLegend->setFont(fontr);
    }
    else
    {
        mpRightPlotLegend->hide();
        mpLeftPlotLegend->hide();
    }

    // Handle external legend
    if (mpLegendsExternalEnabledCheckBox->isChecked())
    {
        if (mpExternalLegend == 0)
        {
            mpExternalLegend = new QwtLegend();
            mpExternalLegend->setAutoFillBackground(false);
            mpExternalLegend->setFrameStyle(QFrame::NoFrame | QFrame::Sunken);
            mpExternalLegend->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            mpQwtPlots[FIRSTPLOT]->insertLegend(mpExternalLegend, QwtPlot::TopLegend);
        }

        QFont font = mpExternalLegend->font();
        font.setPointSize(mpLegendSize->value());
        mpExternalLegend->setFont(font);

        QString symStyle = mpLegendSym->currentText();
        setLegendSymbol(symStyle);
    }
    else
    {
        mpQwtPlots[FIRSTPLOT]->insertLegend(NULL, QwtPlot::TopLegend);
        // Since it is deleted set ptr to NULL
        mpExternalLegend = 0;
    }

    // Decide if we should hide or show manual legend offset controls
    if (mpLegendsAutoOffsetCheckBox->isChecked())
    {
        mpLegendLeftOffset->setDisabled(true);
        mpLegendRightOffset->setDisabled(true);
    }
    else
    {
        mpLegendLeftOffset->setDisabled(false);
        mpLegendRightOffset->setDisabled(false);
    }

    rescaleToCurves();
}

void PlotTab::openLegendSettingsDialog()
{
    mpLegendSettingsDialog->exec();
}


void PlotTab::setTabName(QString name)
{
    mpParentPlotWindow->mpPlotTabWidget->setTabText(mpParentPlotWindow->mpPlotTabWidget->indexOf(this), name);
}


void PlotTab::addBarChart(QStandardItemModel *pItemModel)
{
    mIsSpecialPlot = true;
    mpParentPlotWindow->mpZoomButton->setDisabled(true);
    mpParentPlotWindow->mpImportClassicData->setDisabled(true);
    mpParentPlotWindow->mpArrowButton->setDisabled(true); // Arrow
    mpParentPlotWindow->mpPanButton->setDisabled(true);
    mpParentPlotWindow->mpSaveButton->setDisabled(true);
    //mpParentPlotWindow->mpExportButton->setDisabled(true);
    mpParentPlotWindow->mpExportToCsvAction->setDisabled(true);
    mpParentPlotWindow->mpExportToGnuplotAction->setDisabled(true);
    mpParentPlotWindow->mpExportToOldHopAction->setDisabled(true);
    mpParentPlotWindow->mpExportToMatlabAction->setDisabled(true);
    mpParentPlotWindow->mpLoadFromXmlButton->setDisabled(true);
    mpParentPlotWindow->mpGridButton->setDisabled(true);
    mpParentPlotWindow->mpBackgroundColorButton->setDisabled(true);
    mpParentPlotWindow->mpNewWindowFromTabButton->setDisabled(true);
    mpParentPlotWindow->mpResetXVectorButton->setDisabled(true);
    mpParentPlotWindow->mpBodePlotButton->setDisabled(true);
//    mpParentPlotWindow->mpExportPdfAction->setDisabled(true);
    //mpParentPlotWindow->mpExportToGraphicsAction->setDisabled(true);

    for(int i=0; i<2; ++i)
    {
        mpQwtPlots[i]->setVisible(false);
    }
    mpBarPlot->setVisible(true);

    double min=0;
    double max=0;
    for(int c=0; c<pItemModel->columnCount(); ++c)
    {
        double componentMin = 0;
        double componentMax = 0;
        for(int r=0; r<pItemModel->rowCount(); ++r)
        {
            double data = pItemModel->data(pItemModel->index(r, c)).toDouble();
            if(data > 0)
            {
                componentMax += data;
            }
            if(data < 0)
            {
                componentMin += data;
            }
        }

        min=std::min(min, componentMin);
        max=std::max(max, componentMax);
    }

    mpBarPlot->axisY()->setRanges(min, max);

    mpBarPlot->axisY()->setTicks(max/50, max/10);                     //Minor & major
    mpBarPlot->axisY()->setPen(QPen(Qt::darkGray));
    mpBarPlot->axisY()->setMinorTicksPen(QPen(Qt::gray));
    mpBarPlot->axisY()->setMajorTicksPen(QPen(Qt::darkGray));
    //mpBarPlot->axisY()->setMinorGridPen(QPen(Qt::gray));
    mpBarPlot->axisY()->setMajorGridPen(QPen(Qt::lightGray));
    mpBarPlot->axisY()->setTextColor(Qt::black);
    mpBarPlot->axisY()->setOffset(int(log10(max)+1)*10);
    //qDebug() << "Max = " << max << ", offset = " << mpBarPlot->axisY()->offset();

    mpBarPlot->axisX()->setPen(QPen(Qt::darkGray));
    mpBarPlot->axisX()->setMinorTicksPen(QPen(Qt::gray));
    mpBarPlot->axisX()->setMajorTicksPen(QPen(Qt::darkGray));
    mpBarPlot->axisX()->setMajorGridPen(QPen(Qt::lightGray));
    mpBarPlot->axisX()->setTextColor(Qt::black);
    mpBarPlot->axisX()->setFont(QFont("Calibri", 9));

    mpBarPlot->setBarSize(32, 128);
    mpBarPlot->setBarOpacity(0.9);

    QLinearGradient bg(0,0,0,1);
    bg.setCoordinateMode(QGradient::ObjectBoundingMode);
    bg.setColorAt(1, QColor(0xccccff));
    bg.setColorAt(0.0, Qt::white);
    mpBarPlot->setBackground(QBrush(bg));
    //mpBarPlot->setBackground(QColor("White"));

    mpBarPlot->setModel(pItemModel);

    mpLayouta->addWidget(mpBarPlot);
}


//! @brief Adds a plot curve to a plot tab
//! @param curve Pointer to the plot curve
//! @param desiredColor Desired color for curve (will override default colors)
void PlotTab::addCurve(PlotCurve *curve, QColor desiredColor, HopsanPlotID plotID)
{
    if(mHasSpecialXAxis)
    {
        curve->getQwtPlotCurvePtr()->setSamples(mSpecialXVector, curve->getDataVector());
    }


    //If all curves on the same axis has the same custom unit, assign this unit to the new curve as well
    QString customUnit;
    for(int i=0; i<mPlotCurvePtrs[plotID].size(); ++i)
    {
        if(mPlotCurvePtrs[plotID].at(i)->getAxisY() == curve->getAxisY())
        {
            if(customUnit == QString())
            {
                customUnit = mPlotCurvePtrs[plotID].at(i)->getDataUnit();
            }
            else if(customUnit != mPlotCurvePtrs[plotID].at(i)->getDataUnit())  //Unit is different between the other curves, so don't use it
            {
                customUnit = QString();
                break;
            }
        }
    }
    if(customUnit != QString())
    {
        curve->setDataUnit(customUnit);
    }


    mPlotCurvePtrs[plotID].append(curve);

    if(desiredColor == QColor())
    {
        int i=0;
        while(mUsedColors.contains(mCurveColors.first()))
        {
            mCurveColors.append(mCurveColors.first());
            mCurveColors.pop_front();
            ++i;
            if(i>mCurveColors.size()) break;
        }
        mUsedColors.append(mCurveColors.first());
        curve->setLineColor(mCurveColors.first());
    }
    else
    {
        curve->setLineColor(desiredColor);
    }

    mpQwtPlots[plotID]->enableAxis(curve->getAxisY());
    rescaleToCurves();
    updateLabels();
    mpQwtPlots[plotID]->replot();
    mpQwtPlots[plotID]->updateGeometry();
    curve->setLineWidth(2);

    setLegendSymbol(mpLegendSym->currentText());

    mpParentPlotWindow->mpBodePlotButton->setEnabled(mPlotCurvePtrs[FIRSTPLOT].size() > 1);
}


//! @brief Rescales the axes and the zommers so that all plot curves will fit
void PlotTab::rescaleToCurves()
{
    //Cycle plots and rescale each of them
    for(int plotID=0; plotID<2; ++plotID)
    {
        double xMin, xMax, yMinLeft, yMaxLeft, yMinRight, yMaxRight, heightLeft, heightRight;

        //Cycle plots
        if(!mPlotCurvePtrs[plotID].empty())
        {
            xMin=0;         //Min value for X axis
            xMax=10;        //Max value for X axis
            yMinLeft=DBLMAX;     //Min value for left Y axis
            yMaxLeft=-DBLMAX;    //Max value for left Y axis
            yMinRight=DBLMAX;    //Min value for right Y axis
            yMaxRight=-DBLMAX;   //Max value for right Y axis

            bool foundFirstLeft = false;        //Tells that first left axis curve was found
            bool foundFirstRight = false;       //Tells that first right axis curve was found

            //Initialize values for X axis by using the first curve
            xMin=mPlotCurvePtrs[plotID].first()->getQwtPlotCurvePtr()->minXValue();
            xMax=mPlotCurvePtrs[plotID].first()->getQwtPlotCurvePtr()->maxXValue();

            for(int i=0; i<mPlotCurvePtrs[plotID].size(); ++i)
            {
                //! @todo we could speed this up by not checking min max values in case an axis is set to be locked
                if(mPlotCurvePtrs[plotID].at(i)->getAxisY() == QwtPlot::yLeft)
                {
                    if(foundFirstLeft == false)     //First left-axis curve, use min and max Y values as initial values
                    {
                        if(mLeftAxisLogarithmic)    //Only consider positive values if logarithmic scaling (negative ones will be discarded by Qwt)
                        {
                            yMinLeft = findSmallestValueGreaterThanZero(mPlotCurvePtrs[plotID].at(i)->getDataVector());
                        }
                        else
                        {
                            yMinLeft=mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->minYValue();
                        }
                        yMaxLeft=mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->maxYValue();
                        foundFirstLeft = true;
                    }
                    else    //Compare min/max Y value with previous and change if the new one is smaller/larger
                    {
                        if(mLeftAxisLogarithmic)    //Only consider positive values if logarithmic scaling (negative ones will be discarded by Qwt)
                        {
                            if(findSmallestValueGreaterThanZero(mPlotCurvePtrs[plotID].at(i)->getDataVector()) < yMinLeft)
                            {
                                yMinLeft=findSmallestValueGreaterThanZero(mPlotCurvePtrs[plotID].at(i)->getDataVector());
                            }
                        }
                        else
                        {
                            if(mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->minYValue() < yMinLeft)
                            {
                                yMinLeft=mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->minYValue();
                            }
                        }
                        if(mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->maxYValue() > yMaxLeft)
                        {
                            yMaxLeft=mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->maxYValue();
                        }
                    }
                }

                if(mPlotCurvePtrs[plotID].at(i)->getAxisY() == QwtPlot::yRight)
                {
                    if(foundFirstRight == false)    //First right-axis curve, use min and max Y values as initial values
                    {
                        if(mRightAxisLogarithmic)   //Only consider positive values if logarithmic scaling (negative ones will be discarded by Qwt)
                        {
                            yMinRight = findSmallestValueGreaterThanZero(mPlotCurvePtrs[plotID].at(i)->getDataVector());
                        }
                        else
                        {
                            yMinRight=mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->minYValue();
                        }
                        yMaxRight=mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->maxYValue();
                        foundFirstRight = true;
                    }
                    else    //Compare min/max Y value with previous and change if the new one is smaller/larger
                    {
                        if(mRightAxisLogarithmic)       //Only consider positive values if logarithmic scaling (negative ones will be discarded by Qwt)
                        {
                            if(findSmallestValueGreaterThanZero(mPlotCurvePtrs[plotID].at(i)->getDataVector()) < yMinRight)
                            {
                                yMinRight=findSmallestValueGreaterThanZero(mPlotCurvePtrs[plotID].at(i)->getDataVector());
                            }
                        }
                        else
                        {
                            if(mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->minYValue() < yMinRight)
                            {
                                yMinRight=mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->minYValue();
                            }
                        }
                        if(mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->maxYValue() > yMaxRight)
                        {
                            yMaxRight=mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->maxYValue();
                        }
                    }
                }

                //Compare min/max X value with previous and change if the new one is smaller/larger
                if(mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->minXValue() < xMin)
                    xMin=mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->minXValue();
                if(mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->maxXValue() > xMax)
                    xMax=mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->maxXValue();
            }
        }
        else    //No curves
        {
            xMin=0;         //Min value for X axis
            xMax=10;        //Max value for X axis
            yMinLeft=0;     //Min value for left Y axis
            yMaxLeft=10;    //Max value for left Y axis
            yMinRight=0;    //Min value for right Y axis
            yMaxRight=10;   //Max value for right Y axis
        }

        if(yMinLeft > yMaxLeft)
        {
            yMinLeft = 0;
            yMaxLeft = 10;
        }
        else if(yMinRight > yMaxRight)
        {
            yMinRight = 0;
            yMaxRight = 10;
        }

        //Max and min must not be same value; if they are, decrease/increase
        if(yMaxLeft == 0 && yMinLeft == 0)
        {
            yMaxLeft = 1;
            yMinLeft = -1;
        }
        else if(yMaxLeft == yMinLeft && yMaxLeft > 0)
        {
            yMaxLeft = yMaxLeft*2;
            yMinLeft = 0;
        }
        else if(yMaxLeft == yMinLeft && yMaxLeft < 0)
        {
            yMaxLeft = 0;
            yMinLeft = yMinLeft*2;
        }
        if(yMaxRight == 0 && yMinRight == 0)
        {
            yMaxRight = 1;
            yMinRight = -1;
        }
        else if(yMaxRight == yMinRight && yMaxRight > 0)
        {
            yMaxRight = yMaxRight*2;
            yMinRight = 0;
        }
        else if(yMaxRight == yMinRight && yMaxRight < 0)
        {
            yMaxRight = 0;
            yMinRight = yMinRight*2;
        }

        //Calculate heights (used for calculating margins at top and bottom
        heightLeft = yMaxLeft-yMinLeft;
        heightRight = yMaxRight-yMinRight;

        //If plot has log scale, we use a different approach for calculating margins
        //(fixed margins would not make sense with a log scale)

        //! @todo In new qwt the type in the transform has been removed, Trying with dynamic cast instead
        if(dynamic_cast<QwtLogScaleEngine*>(mpQwtPlots[plotID]->axisScaleEngine(QwtPlot::yLeft)))
        {
            heightLeft = 0;
            yMaxLeft = yMaxLeft*2.0;
            yMinLeft = yMinLeft/2.0;
        }
        if(dynamic_cast<QwtLogScaleEngine*>(mpQwtPlots[plotID]->axisScaleEngine(QwtPlot::yRight)))
        {
            heightRight = 0;
            yMaxRight = yMaxRight*2.0;
            yMinRight = yMinRight/2.0;
        }


        //Scale the axes
        if (!mpXbSetLockCheckBox->isChecked())
        {
            mpQwtPlots[plotID]->setAxisScale(QwtPlot::xBottom, xMin, xMax);
            mAxisLimits[plotID].xbMin = xMin;
            mAxisLimits[plotID].xbMax = xMax;
        }

        if (!mpYLSetLockCheckBox->isChecked())
        {
            mpQwtPlots[plotID]->setAxisScale(QwtPlot::yLeft, yMinLeft-0.05*heightLeft, yMaxLeft+0.05*heightLeft);
            mAxisLimits[plotID].yLMin = yMinLeft-0.05*heightLeft;
            mAxisLimits[plotID].yLMax =  yMaxLeft+0.05*heightLeft;
        }

        if (!mpYRSetLockCheckBox->isChecked())
        {
            mpQwtPlots[plotID]->setAxisScale(QwtPlot::yRight, yMinRight-0.05*heightRight, yMaxRight+0.05*heightRight);
            mAxisLimits[plotID].yRMin = yMinRight-0.05*heightRight;
            mAxisLimits[plotID].yRMax = yMaxRight+0.05*heightRight;
        }
        //! @todo will these Locks be overridden by ybuffer ? below

        // Auto calculate mLegendYBufferOffset
        //! @todo only works for linear scale right now, need to check for log scale also
        double leftLegendHeight = mpLeftPlotLegend->geometry(mpQwtPlots[plotID]->geometry()).height();
        double rightLegendHeight = mpRightPlotLegend->geometry(mpQwtPlots[plotID]->geometry()).height();
        double leftTopBufferOffset, leftBottomBufferOffset, rightTopBufferOffset, rightBottomBufferOffset;
        if(mpLegendsAutoOffsetCheckBox->isChecked())
        {
            if ((mpLegendLPosition->currentText() == mpLegendRPosition->currentText()) && (mpLegendRPosition->currentText() == "Top"))
            {
                leftTopBufferOffset = rightTopBufferOffset = qMax(leftLegendHeight,rightLegendHeight);
                leftBottomBufferOffset = rightBottomBufferOffset = 0;
            }
            if ((mpLegendLPosition->currentText() == mpLegendRPosition->currentText()) && (mpLegendRPosition->currentText() == "Bottom"))
            {
                leftTopBufferOffset = rightTopBufferOffset = 0;
                leftBottomBufferOffset = rightBottomBufferOffset = qMax(leftLegendHeight,rightLegendHeight);
            }
            else if (mpLegendLPosition->currentText() == "Bottom")
            {
                leftBottomBufferOffset = leftLegendHeight;
                rightTopBufferOffset = rightLegendHeight;
                leftTopBufferOffset = rightBottomBufferOffset = 0;
            }
            else if (mpLegendRPosition->currentText() == "Bottom")
            {
                rightBottomBufferOffset = rightLegendHeight;
                leftTopBufferOffset = leftLegendHeight;
                rightTopBufferOffset = leftBottomBufferOffset = 0;
            }
        }
        else
        {
            if (mpLegendLPosition->currentText() == "Top")
            {
                leftTopBufferOffset = mpLegendLeftOffset->value()*leftLegendHeight;
                leftBottomBufferOffset = 0;
            }
            else if (mpLegendLPosition->currentText() == "Bottom")
            {
                leftTopBufferOffset = 0;
                leftBottomBufferOffset = mpLegendLeftOffset->value()*leftLegendHeight;
            }
            //! @todo Center? than what to do

            if (mpLegendRPosition->currentText() == "Top")
            {
                rightTopBufferOffset = mpLegendRightOffset->value()*rightLegendHeight;
                rightBottomBufferOffset = 0;
            }
            else if (mpLegendRPosition->currentText() == "Bottom")
            {
                rightTopBufferOffset = 0;
                rightBottomBufferOffset = mpLegendRightOffset->value()*rightLegendHeight;
            }
            //! @todo Center? than what to do
        }

        // Rescale axis to include mLegendYBufferOffset
        //! @todo only works for top buffer right now
        if(dynamic_cast<QwtLogScaleEngine*>(mpQwtPlots[plotID]->axisScaleEngine(QwtPlot::yLeft)))
        {
            //! @todo what shoul happen here ?
            //                double leftlegendheigh = mpLeftPlotLegend->geometry(mpQwtPlots[plotID]->geometry()).height();
            //                double rightlegendheigh = mpRightPlotLegend->geometry(mpQwtPlots[plotID]->geometry()).height();
            //                bufferoffset = max(leftlegendheigh,rightlegendheigh);
            //                double rheight = mpQwtPlots[plotID]->axisWidget(QwtPlot::yRight)->size().height();
            //                double rinterval = mpQwtPlots[plotID]->axisInterval(QwtPlot::yRight).width();
            //                //                double rscale = rinterval/rheight; //change
            //                double lheight = mpQwtPlots[plotID]->axisWidget(QwtPlot::yLeft)->size().height();
            //                double linterval = mpQwtPlots[plotID]->axisInterval(QwtPlot::yLeft).width();
            //                double lscale = linterval/lheight;
            //                heightLeft = 0;
            //                yMaxLeft = yMaxLeft*2.0;
            //                yMinLeft = yMinLeft/2.0;
            //                heightRight = 0;
            //                yMaxRight = yMaxRight*2.0;
            //                yMinRight = yMinRight/2.0;
            //                double bufferoffsetL =  bufferoffset*lscale;//marginss1;
            //                //                double bufferoffsetR =  bufferoffset*rscale;//marginss2;
            //                mpQwtPlots[plotID]->setAxisScale(QwtPlot::yLeft, yMinLeft-0.05*heightLeft, yMaxLeft+0.05*heightLeft+bufferoffsetL);
            //                mAxisLimits[plotID].yLMin = yMinLeft-0.05*heightLeft;
            //                mAxisLimits[plotID].yLMax =  yMaxLeft+0.05*heightLeft+bufferoffsetL;
            //                //                mpQwtPlots[plotID]->setAxisScale(QwtPlot::yRight, yMinRight-0.05*heightRight, yMaxRight+0.05*heightRight+bufferoffsetR);
            //                //                mAxisLimits[plotID].yRMin = yMinRight-0.05*heightRight;
            //                //                mAxisLimits[plotID].yRMax = yMaxRight+0.05*heightRight+bufferoffsetR;
        }
        else
        {
            const double lheight = mpQwtPlots[plotID]->axisWidget(QwtPlot::yLeft)->size().height();
            const double linterval = mpQwtPlots[plotID]->axisInterval(QwtPlot::yLeft).width();
            const double lscale = linterval/lheight;

            mAxisLimits[plotID].yLMin = yMinLeft-0.05*heightLeft-leftBottomBufferOffset*lscale;
            mAxisLimits[plotID].yLMax =  yMaxLeft+0.05*heightLeft+leftTopBufferOffset*lscale;
            mpQwtPlots[plotID]->setAxisScale(QwtPlot::yLeft, mAxisLimits[plotID].yLMin, mAxisLimits[plotID].yLMax);
        }
        if(dynamic_cast<QwtLogScaleEngine*>(mpQwtPlots[plotID]->axisScaleEngine(QwtPlot::yRight)))
        {
            //! @todo what shoul happen here ?
            //                double leftlegendheigh = mpLeftPlotLegend->geometry(mpQwtPlots[plotID]->geometry()).height();
            //                double rightlegendheigh = mpRightPlotLegend->geometry(mpQwtPlots[plotID]->geometry()).height();
            //                bufferoffset = max(leftlegendheigh,rightlegendheigh);

            //                double rheight = mpQwtPlots[plotID]->axisWidget(QwtPlot::yRight)->size().height();
            //                double rinterval = mpQwtPlots[plotID]->axisInterval(QwtPlot::yRight).width();
            //                double rscale = rinterval/rheight; //change

            //                double lheight = mpQwtPlots[plotID]->axisWidget(QwtPlot::yLeft)->size().height();
            //                double linterval = mpQwtPlots[plotID]->axisInterval(QwtPlot::yLeft).width();
            //                //                double lscale = linterval/lheight;
            //                heightLeft = 0;
            //                yMaxLeft = yMaxLeft*2.0;
            //                yMinLeft = yMinLeft/2.0;
            //                heightRight = 0;
            //                yMaxRight = yMaxRight*2.0;
            //                yMinRight = yMinRight/2.0;
            //                //                double bufferoffsetL =  bufferoffset*lscale;
            //                double bufferoffsetR =  bufferoffset*rscale;
            //                //                mpQwtPlots[plotID]->setAxisScale(QwtPlot::yLeft, yMinLeft-0.05*heightLeft, yMaxLeft+0.05*heightLeft+bufferoffsetL);
            //                //                mAxisLimits[plotID].yLMin = yMinLeft-0.05*heightLeft;
            //                //                mAxisLimits[plotID].yLMax =  yMaxLeft+0.05*heightLeft+bufferoffsetL;
            //                mpQwtPlots[plotID]->setAxisScale(QwtPlot::yRight, yMinRight-0.05*heightRight, yMaxRight+0.05*heightRight+bufferoffsetR);
            //                mAxisLimits[plotID].yRMin = yMinRight-0.05*heightRight;
            //                mAxisLimits[plotID].yRMax = yMaxRight+0.05*heightRight+bufferoffsetR;
        }
        else
        {
            const double rheight = mpQwtPlots[plotID]->axisWidget(QwtPlot::yRight)->size().height();
            const double rinterval = mpQwtPlots[plotID]->axisInterval(QwtPlot::yRight).width();
            const double rscale = rinterval/rheight;

            mAxisLimits[plotID].yRMin = yMinRight-0.05*heightRight-rightBottomBufferOffset*rscale;
            mAxisLimits[plotID].yRMax = yMaxRight+0.05*heightRight+rightTopBufferOffset*rscale;
            mpQwtPlots[plotID]->setAxisScale(QwtPlot::yRight, mAxisLimits[plotID].yRMin, mAxisLimits[plotID].yRMax);
        }
        mpQwtPlots[plotID]->updateAxes();
        //! @todo left only applies to left even if the right is overshadowed, problem is that if left, right are bottom and top calculated buffers will be different on each axis

        //Scale the zoom base (maximum zoom)
        QRectF tempDoubleRect;
        tempDoubleRect.setX(xMin);
        tempDoubleRect.setY(yMinLeft-0.05*heightLeft);
        tempDoubleRect.setWidth(xMax-xMin);
        tempDoubleRect.setHeight(yMaxLeft-yMinLeft+0.1*heightLeft);
        mpZoomer[plotID]->setZoomBase(tempDoubleRect);

        QRectF tempDoubleRect2;
        tempDoubleRect2.setX(xMin);
        tempDoubleRect2.setY(yMinRight-0.05*heightRight);
        tempDoubleRect2.setHeight(yMaxRight-yMinRight+0.1*heightRight);
        tempDoubleRect2.setWidth(xMax-xMin);
        mpZoomerRight[plotID]->setZoomBase(tempDoubleRect2);
    }

    //Curve Marker
    //! @todo hmmm memmmory leek? create new markr every time
    mpMarkerSymbol = new QwtSymbol();
    mpMarkerSymbol->setStyle(QwtSymbol::XCross);
    mpMarkerSymbol->setSize(10,10);
}


//! @brief Removes a curve from the plot tab
//! @param curve Pointer to curve to remove
void PlotTab::removeCurve(PlotCurve *curve)
{
    int plotID = getPlotIDFromCurve(curve);

    for(int i=0; i<mMarkerPtrs[plotID].size(); ++i)
    {
        if(mMarkerPtrs[plotID].at(i)->getCurve() == curve)
        {
            mpQwtPlots[plotID]->canvas()->removeEventFilter(mMarkerPtrs[plotID].at(i));
            mMarkerPtrs[plotID].at(i)->detach();
            mMarkerPtrs[plotID].removeAt(i);
            --i;
        }
    }

    for(int i=0; i<mUsedColors.size(); ++i)
    {
        if(curve->getQwtPlotCurvePtr()->pen().color() == mUsedColors.at(i))
        {
            mUsedColors.removeAt(i);
            break;
        }
    }

    curve->getQwtPlotCurvePtr()->detach();
    for(int plotID=0; plotID<2; ++plotID)
    {
        mPlotCurvePtrs[plotID].removeAll(curve);
    }
    delete(curve);
    rescaleToCurves();
    updateLabels();
    update();

}


//! @brief Changes the X vector of current plot tab to specified variable
//! @param xArray New data for X-axis
//! @param componentName Name of component from which new data origins
//! @param portName Name of port form which new data origins
//! @param dataName Data name (physical quantity) of new data
//! @param dataUnit Unit of new data
void PlotTab::changeXVector(QVector<double> xArray, const VariableDescription &rVarDesc, HopsanPlotID plotID)
{
    //! @todo maybe create a LogVariableData object and use that instead of maunal hack
    mHasSpecialXAxis = true;
    mSpecialXVector = xArray;

    for(int i=0; i<mPlotCurvePtrs[plotID].size(); ++i)
    {
        mPlotCurvePtrs[plotID].at(i)->getQwtPlotCurvePtr()->setSamples(mSpecialXVector, mPlotCurvePtrs[plotID].at(i)->getDataVector());
        mPlotCurvePtrs[plotID].at(i)->setDataUnit(mPlotCurvePtrs[plotID].at(i)->getDataUnit());
    }

    rescaleToCurves();

    mSpecialXVectorModelPath = gpMainWindow->mpProjectTabs->getCurrentContainer()->getModelFileInfo().filePath();
    mSpecialXVectorDescription = rVarDesc;

    mSpecialXVectorLabel = QString(rVarDesc.mDataName + " [" + rVarDesc.mDataUnit + "]");
    updateLabels();
    update();
    mpParentPlotWindow->mpResetXVectorButton->setEnabled(true);
}


//! @brief Updates labels on plot axes
void PlotTab::updateLabels()
{
    for(int plotID=0; plotID<2; ++plotID)
    {
        mpQwtPlots[plotID]->setAxisTitle(QwtPlot::yLeft, QwtText());
        mpQwtPlots[plotID]->setAxisTitle(QwtPlot::yRight, QwtText());

        if(mPlotCurvePtrs[plotID].size()>0 && mPlotCurvePtrs[plotID][0]->getCurveType() == PORTVARIABLE)
        {
            QStringList leftUnits, rightUnits;
            for(int i=0; i<mPlotCurvePtrs[plotID].size(); ++i)
            {
                QString newUnit = QString(mPlotCurvePtrs[plotID].at(i)->getDataName() + " [" + mPlotCurvePtrs[plotID].at(i)->getDataUnit() + "]");
                if( !(mPlotCurvePtrs[plotID].at(i)->getAxisY() == QwtPlot::yLeft && leftUnits.contains(newUnit)) && !(mPlotCurvePtrs[plotID].at(i)->getAxisY() == QwtPlot::yRight && rightUnits.contains(newUnit)) )
                {
                    if(!mpQwtPlots[plotID]->axisTitle(mPlotCurvePtrs[plotID].at(i)->getAxisY()).isEmpty())
                    {
                        mpQwtPlots[plotID]->setAxisTitle(mPlotCurvePtrs[plotID].at(i)->getAxisY(), QwtText(QString(mpQwtPlots[plotID]->axisTitle(mPlotCurvePtrs[plotID].at(i)->getAxisY()).text().append(", "))));
                    }
                    mpQwtPlots[plotID]->setAxisTitle(mPlotCurvePtrs[plotID].at(i)->getAxisY(), QwtText(QString(mpQwtPlots[plotID]->axisTitle(mPlotCurvePtrs[plotID].at(i)->getAxisY()).text().append(newUnit))));
                    if(mPlotCurvePtrs[plotID].at(i)->getAxisY() == QwtPlot::yLeft)
                    {
                        leftUnits.append(newUnit);
                    }
                    if(mPlotCurvePtrs[plotID].at(i)->getAxisY() == QwtPlot::yRight)
                    {
                        rightUnits.append(newUnit);
                    }
                }
            }
            mpQwtPlots[plotID]->setAxisTitle(QwtPlot::xBottom, QwtText(mSpecialXVectorLabel));
        }
        else if(mPlotCurvePtrs[plotID].size()>0 && mPlotCurvePtrs[plotID][0]->getCurveType() == FREQUENCYANALYSIS)
        {
            for(int i=0; i<mPlotCurvePtrs[plotID].size(); ++i)
            {
                mpQwtPlots[plotID]->setAxisTitle(mPlotCurvePtrs[plotID].at(i)->getAxisY(), "Relative Magnitude [-]");
                mpQwtPlots[plotID]->setAxisTitle(QwtPlot::xBottom, "Frequency [Hz]");
            }
        }
        else if(mPlotCurvePtrs[plotID].size()>0 && mPlotCurvePtrs[plotID][0]->getCurveType() == NYQUIST)
        {
            for(int i=0; i<mPlotCurvePtrs[plotID].size(); ++i)
            {
                mpQwtPlots[plotID]->setAxisTitle(mPlotCurvePtrs[plotID].at(i)->getAxisY(), "Im");
                mpQwtPlots[plotID]->setAxisTitle(QwtPlot::xBottom, "Re");
            }
        }
        else if(mPlotCurvePtrs[plotID].size()>0 && mPlotCurvePtrs[plotID][0]->getCurveType() == BODEGAIN)
        {
            for(int i=0; i<mPlotCurvePtrs[plotID].size(); ++i)
            {
                mpQwtPlots[plotID]->setAxisTitle(mPlotCurvePtrs[plotID].at(i)->getAxisY(), "Magnitude [dB]");
                mpQwtPlots[plotID]->setAxisTitle(QwtPlot::xBottom, QwtText());      //No label, because there will be a phase plot bellow with same label
            }
        }
        else if(mPlotCurvePtrs[plotID].size()>0 && mPlotCurvePtrs[plotID][0]->getCurveType() == BODEPHASE)
        {
            for(int i=0; i<mPlotCurvePtrs[plotID].size(); ++i)
            {
                mpQwtPlots[plotID]->setAxisTitle(mPlotCurvePtrs[plotID].at(i)->getAxisY(), "Phase [deg]");
                mpQwtPlots[plotID]->setAxisTitle(QwtPlot::xBottom, "Frequency [Hz]");
            }
        }
    }
}

bool PlotTab::isGridVisible()
{
    return mpGrid[FIRSTPLOT]->isVisible();
}


void PlotTab::resetXVector()
{
    mHasSpecialXAxis = false;

    for(int i=0; i<mPlotCurvePtrs[FIRSTPLOT].size(); ++i)
    {
        mPlotCurvePtrs[FIRSTPLOT].at(i)->getQwtPlotCurvePtr()->setSamples(mPlotCurvePtrs[FIRSTPLOT].at(i)->getTimeVector(), mPlotCurvePtrs[FIRSTPLOT].at(i)->getDataVector());
        mPlotCurvePtrs[FIRSTPLOT].at(i)->setDataUnit(mPlotCurvePtrs[FIRSTPLOT].at(i)->getDataUnit());
    }

    mSpecialXVectorLabel = QString("Time [s]");
    updateLabels();
    update();

    rescaleToCurves();
    mpQwtPlots[FIRSTPLOT]->replot();
    mpQwtPlots[FIRSTPLOT]->updateGeometry();

    mpParentPlotWindow->mpResetXVectorButton->setEnabled(false);
}


//! @brief Slot that opens a dialog from where user can export current plot tab to a XML file
void PlotTab::exportToXml()
{

    //Open a dialog where text and font can be selected
    mpExportXmlDialog = new QDialog(gpMainWindow);
    mpExportXmlDialog->setWindowTitle("Export Plot Tab To XML");

    QLabel *pXmlIndentationLabel = new QLabel("Indentation: ");

    mpXmlIndentationSpinBox = new QSpinBox(this);
    mpXmlIndentationSpinBox->setRange(0,100);
    mpXmlIndentationSpinBox->setValue(2);

    mpIncludeTimeCheckBox = new QCheckBox("Include date && time");
    mpIncludeTimeCheckBox->setChecked(true);

    mpIncludeDescriptionsCheckBox = new QCheckBox("Include variable descriptions");
    mpIncludeDescriptionsCheckBox->setChecked(true);

    QLabel *pOutputLabel = new QLabel("Output data:");

    mpXmlOutputTextBox = new QTextEdit();
    mpXmlOutputTextBox->toHtml();
    mpXmlOutputTextBox->setReadOnly(true);
    mpXmlOutputTextBox->setMinimumSize(700, 210);

    QPushButton *pDoneInDialogButton = new QPushButton("Export");
    QPushButton *pCancelInDialogButton = new QPushButton("Cancel");
    QDialogButtonBox *pButtonBox = new QDialogButtonBox(Qt::Horizontal);
    pButtonBox->addButton(pDoneInDialogButton, QDialogButtonBox::ActionRole);
    pButtonBox->addButton(pCancelInDialogButton, QDialogButtonBox::ActionRole);

    QGridLayout *pDialogLayout = new QGridLayout();
    pDialogLayout->addWidget(pXmlIndentationLabel,          0, 0);
    pDialogLayout->addWidget(mpXmlIndentationSpinBox,       0, 1);
    pDialogLayout->addWidget(mpIncludeTimeCheckBox,         2, 0, 1, 2);
    pDialogLayout->addWidget(mpIncludeDescriptionsCheckBox, 3, 0, 1, 2);
    pDialogLayout->addWidget(pOutputLabel,                  4, 0, 1, 2);
    pDialogLayout->addWidget(mpXmlOutputTextBox,            5, 0, 1, 4);
    pDialogLayout->addWidget(pButtonBox,                    6, 2, 1, 2);

    mpExportXmlDialog->setLayout(pDialogLayout);

    connect(mpXmlIndentationSpinBox,        SIGNAL(valueChanged(int)),  this,               SLOT(updateXmlOutputTextInDialog()));
    connect(mpIncludeTimeCheckBox,          SIGNAL(toggled(bool)),      this,               SLOT(updateXmlOutputTextInDialog()));
    connect(mpIncludeDescriptionsCheckBox,  SIGNAL(toggled(bool)),      this,               SLOT(updateXmlOutputTextInDialog()));
    connect(pDoneInDialogButton,            SIGNAL(clicked()),          this,               SLOT(saveToXml()));
    connect(pCancelInDialogButton,          SIGNAL(clicked()),          mpExportXmlDialog,  SLOT(close()));

    updateXmlOutputTextInDialog();
    mpExportXmlDialog->open();
}


//! @brief Slot that exports plot tab to a specified comma-separated value file (.csv)
void PlotTab::exportToCsv()
{
    //Open file dialog and initialize the file stream
    QString filePath;
    QFileInfo fileInfo;
    filePath = QFileDialog::getSaveFileName(this, tr("Export Plot Tab To CSV File"),
                                            gConfig.getPlotDataDir(),
                                            tr("Comma-separated values (*.csv)"));
    if(filePath.isEmpty()) return;    //Don't save anything if user presses cancel
    fileInfo.setFile(filePath);
    gConfig.setPlotDataDir(fileInfo.absolutePath());

    exportToCsv(filePath);
}


//! @brief Exports plot tab to comma-separated value file with specified filename
//! @param fileName File name
void PlotTab::exportToCsv(QString fileName)
{
    QFile file;
    file.setFileName(fileName);   //Create a QFile object
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gpMainWindow->mpTerminalWidget->mpConsole->printErrorMessage("Failed to open file for writing: " + fileName);
        return;
    }

    QTextStream fileStream(&file);  //Create a QTextStream object to stream the content of file


    //Cycle plot curves
    if(mHasSpecialXAxis)
    {
        for(int i=0; i<mSpecialXVector.size(); ++i)
        {
            fileStream << mSpecialXVector[i];
            for(int j=0; j<mPlotCurvePtrs[FIRSTPLOT].size(); ++j)
            {
                fileStream << ", " << mPlotCurvePtrs[FIRSTPLOT][j]->getDataVector()[i];
            }
            fileStream << "\n";
        }
    }
    else
    {
        for(int i=0; i<mPlotCurvePtrs[FIRSTPLOT][0]->getTimeVector().size(); ++i)
        {
            fileStream << mPlotCurvePtrs[FIRSTPLOT][0]->getTimeVector()[i];
            for(int j=0; j<mPlotCurvePtrs[FIRSTPLOT].size(); ++j)
            {
                fileStream << ", " << mPlotCurvePtrs[FIRSTPLOT][j]->getDataVector()[i];
            }
            fileStream << "\n";
        }
    }


    //        //Cycle plot curves
    //    for(int i=0; i<mPlotCurvePtrs[FIRSTPLOT].size(); ++i)
    //    {
    //        fileStream << "x" << i;                                         //Write time/X vector
    //        if(mHasSpecialXAxis)
    //        {
    //            for(int j=0; j<mVectorX.size(); ++j)
    //            {
    //                fileStream << "," << mVectorX[j];
    //            }
    //        }
    //        else
    //        {
    //            for(int j=0; j<mPlotCurvePtrs[FIRSTPLOT][i]->getTimeVector().size(); ++j)
    //            {
    //                fileStream << "," << mPlotCurvePtrs[FIRSTPLOT][i]->getTimeVector()[j];
    //            }
    //        }
    //        fileStream << "\n";

    //        fileStream << "y" << i;                                             //Write data vector
    //        for(int k=0; k<mPlotCurvePtrs[FIRSTPLOT][i]->getDataVector().size(); ++k)
    //        {
    //            fileStream << "," << mPlotCurvePtrs[FIRSTPLOT][i]->getDataVector()[k];
    //        }
    //        fileStream << "\n";
    //    }

    file.close();
}

void PlotTab::exportToHvc(QString fileName)
{
    if (mPlotCurvePtrs[FIRSTPLOT].size() < 1)
    {
        return;
    }

    QFileInfo fileInfo;
    if (fileName.isEmpty())
    {
        //Open file dialog and initialize the file stream

        QString filePath = QFileDialog::getSaveFileName(this, tr("Export Plot Tab To CSV File"),
                                                        gConfig.getPlotDataDir(),
                                                        tr("HopsanValidationCfg (*.hvc)"));
        if(filePath.isEmpty()) return;    //Don't save anything if user presses cancel
        fileInfo.setFile(filePath);
    }

    QFile file(fileInfo.absoluteFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gpMainWindow->mpTerminalWidget->mpConsole->printErrorMessage("Failed to open file for writing: " + fileName);
        return;
    }

    // Save the csv data
    QString csvFileName=fileInfo.baseName()+".csv";
    this->exportToCsv(fileInfo.absolutePath()+"/"+csvFileName);

    qDebug() << fileInfo.absoluteFilePath();
    qDebug() << fileInfo.absolutePath()+"/"+csvFileName;


    // Save HVC xml data
    QDomDocument doc;
    QDomElement hvcroot = doc.createElement("hopsanvalidationconfiguration");
    doc.appendChild(hvcroot);
    hvcroot.setAttribute("hvcversion", "0.1");

    QString modelPath = relativePath(mPlotCurvePtrs[FIRSTPLOT][0]->getContainerObjectPtr()->getModelFileInfo(), QDir(fileInfo.absolutePath()));
    QDomElement validation = appendDomElement(hvcroot, "validation");
    appendDomTextNode(validation, "modelfile", modelPath);
    appendDomTextNode(validation, "parameterset", "");

    //Cycle plot curves
    for (int i=0; i<mPlotCurvePtrs[FIRSTPLOT].size(); ++i)
    {
        PlotCurve *pPlotCurve = mPlotCurvePtrs[FIRSTPLOT][i];

        QDomElement component = appendDomElement(validation, "component");
        component.setAttribute("name", pPlotCurve->getComponentName());

        QDomElement port = appendDomElement(component, "port");
        port.setAttribute("name", pPlotCurve->getPortName());

        QDomElement variable = appendDomElement(port, "variable");
        variable.setAttribute("name", pPlotCurve->getDataName());

        appendDomTextNode(variable, "csvfile", csvFileName);
        appendDomIntegerNode(variable, "column", i+1);

        appendDomValueNode(variable, "tolerance", 0.01);
    }

    QTextStream hvcFileStream(&file);
    appendRootXMLProcessingInstruction(doc); //The xml "comment" on the first line
    doc.save(hvcFileStream, 2);
    file.close();
}


//! @brief Slot that exports plot tab to a specified matlab script file (.m)
void PlotTab::exportToMatlab()
{
    //Open file dialog and initialize the file stream
    QDir fileDialogSaveDir;
    QString filePath;
    QFileInfo fileInfo;
    QFile file;
    filePath = QFileDialog::getSaveFileName(this, tr("Export Plot Tab To MATLAB File"),
                                            gConfig.getPlotDataDir(),
                                            tr("MATLAB script file (*.m)"));
    if(filePath.isEmpty()) return;    //Don't save anything if user presses cancel
    fileInfo.setFile(filePath);
    gConfig.setPlotDataDir(fileInfo.absolutePath());
    file.setFileName(fileInfo.filePath());   //Create a QFile object
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gpMainWindow->mpTerminalWidget->mpConsole->printErrorMessage("Failed to open file for writing: " + filePath);
        return;
    }
    QTextStream fileStream(&file);  //Create a QTextStream object to stream the content of file
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString();

    //Write initial comment
    fileStream << "% MATLAB File Exported From Hopsan " << QString(HOPSANGUIVERSION) << " " << dateTimeString << "\n";

    //Cycle plot curves
    for(int i=0; i<mPlotCurvePtrs[FIRSTPLOT].size(); ++i)
    {
        fileStream << "x" << i << "=[";                                         //Write time/X vector
        if(mHasSpecialXAxis)
        {
            for(int j=0; j<mSpecialXVector.size(); ++j)
            {
                if(j>0) fileStream << ",";
                fileStream << mSpecialXVector[j];
            }
        }
        else
        {
            for(int j=0; j<mPlotCurvePtrs[FIRSTPLOT][i]->getTimeVector().size(); ++j)
            {
                if(j>0) fileStream << ",";
                fileStream << mPlotCurvePtrs[FIRSTPLOT][i]->getTimeVector()[j];
            }
        }
        fileStream << "];\n";

        fileStream << "y" << i << "=[";                                             //Write data vector
        for(int k=0; k<mPlotCurvePtrs[FIRSTPLOT][i]->getDataVector().size(); ++k)
        {
            if(k>0) fileStream << ",";
            fileStream << mPlotCurvePtrs[FIRSTPLOT][i]->getDataVector()[k];
        }
        fileStream << "];\n";
    }

    //Cycle plot curves
    for(int i=0; i<mPlotCurvePtrs[SECONDPLOT].size(); ++i)
    {
        fileStream << "x" << i+mPlotCurvePtrs[FIRSTPLOT].size() << "=[";                                         //Write time/X vector
        if(mHasSpecialXAxis)
        {
            for(int j=0; j<mSpecialXVector.size(); ++j)
            {
                if(j>0) fileStream << ",";
                fileStream << mSpecialXVector[j];
            }
        }
        else
        {
            for(int j=0; j<mPlotCurvePtrs[SECONDPLOT][i]->getTimeVector().size(); ++j)
            {
                if(j>0) fileStream << ",";
                fileStream << mPlotCurvePtrs[SECONDPLOT][i]->getTimeVector()[j];
            }
        }
        fileStream << "];\n";

        fileStream << "y" << i+mPlotCurvePtrs[FIRSTPLOT].size() << "=[";                                             //Write data vector
        for(int k=0; k<mPlotCurvePtrs[SECONDPLOT][i]->getDataVector().size(); ++k)
        {
            if(k>0) fileStream << ",";
            fileStream << mPlotCurvePtrs[SECONDPLOT][i]->getDataVector()[k];
        }
        fileStream << "];\n";
    }

    //Write plot functions
    QStringList matlabColors;
    matlabColors << "r" << "g" << "b" << "c" << "m" << "y";
    fileStream << "hold on\n";
    fileStream << "subplot(2,1,1)\n";
    for(int i=0; i<mPlotCurvePtrs[FIRSTPLOT].size(); ++i)
    {
        if((mPlotCurvePtrs[FIRSTPLOT][i]->getAxisY() == QwtPlot::yLeft && mLeftAxisLogarithmic) || (mPlotCurvePtrs[FIRSTPLOT][i]->getAxisY() == QwtPlot::yRight && mRightAxisLogarithmic))
        {
            if(mBottomAxisLogarithmic)
                fileStream << "loglog";
            else
                fileStream << "semilogy";
        }
        else
        {
            if(mBottomAxisLogarithmic)
                fileStream << "semilogx";
            else
                fileStream << "plot";
        }
        fileStream << "(x" << i << ",y" << i << ",'-" << matlabColors[i%6] << "','linewidth'," << mPlotCurvePtrs[FIRSTPLOT][i]->getQwtPlotCurvePtr()->pen().width() << ")\n";
    }
    if(mPlotCurvePtrs[SECONDPLOT].size() > 0)
    {
        fileStream << "subplot(2,1,2)\n";
        for(int i=0; i<mPlotCurvePtrs[SECONDPLOT].size(); ++i)
        {
            if((mPlotCurvePtrs[SECONDPLOT][i]->getAxisY() == QwtPlot::yLeft && mLeftAxisLogarithmic) || (mPlotCurvePtrs[SECONDPLOT][i]->getAxisY() == QwtPlot::yRight && mRightAxisLogarithmic))
            {
                if(mBottomAxisLogarithmic)
                    fileStream << "loglog";
                else
                    fileStream << "semilogy";
            }
            else
            {
                if(mBottomAxisLogarithmic)
                    fileStream << "semilogx";
                else
                    fileStream << "plot";
            }
            fileStream << "(x" << i+mPlotCurvePtrs[FIRSTPLOT].size() << ",y" << i+mPlotCurvePtrs[FIRSTPLOT].size() << ",'-" << matlabColors[i%6] << "','linewidth'," << mPlotCurvePtrs[SECONDPLOT][i]->getQwtPlotCurvePtr()->pen().width() << ")\n";
        }
    }

    file.close();
}


//! @brief Slot that exports plot tab to specified gnuplot file  (.dat)
void PlotTab::exportToGnuplot()
{
    //Open file dialog and initialize the file stream
    QDir fileDialogSaveDir;
    QString filePath;
    QFileInfo fileInfo;
    QFile file;
    filePath = QFileDialog::getSaveFileName(this, tr("Export Plot Tab To gnuplot File"),
                                            gConfig.getPlotDataDir(),
                                            tr("gnuplot file (*.dat)"));
    if(filePath.isEmpty()) return;    //Don't save anything if user presses cancel
    fileInfo.setFile(filePath);
    gConfig.setPlotDataDir(fileInfo.absolutePath());
    file.setFileName(fileInfo.filePath());   //Create a QFile object
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gpMainWindow->mpTerminalWidget->mpConsole->printErrorMessage("Failed to open file for writing: " + filePath);
        return;
    }

    QTextStream fileStream(&file);  //Create a QTextStream object to stream the content of file
    QDateTime dateTime = QDateTime::currentDateTime();
    QString dateTimeString = dateTime.toString();

    //Write initial comment
    fileStream << "# gnuplot File Exported From Hopsan " << QString(HOPSANGUIVERSION) << " " << dateTimeString << "\n";
    fileStream << "# T";
    for(int i=0; i<mPlotCurvePtrs[FIRSTPLOT].size(); ++i)
    {
        fileStream << "                  Y" << i;
    }
    fileStream << "\n";

    //Write time and data vectors
    QString dummy;
    for(int i=0; i<mPlotCurvePtrs[FIRSTPLOT].first()->getTimeVector().size(); ++i)
    {
        dummy.setNum(mPlotCurvePtrs[FIRSTPLOT].first()->getTimeVector()[i]);
        fileStream << dummy;
        for(int j=0; j<20-dummy.size(); ++j) { fileStream << " "; }

        for(int k=0; k<mPlotCurvePtrs[FIRSTPLOT].size(); ++k)
        {
            dummy.setNum(mPlotCurvePtrs[FIRSTPLOT][k]->getDataVector()[i]);
            fileStream << dummy;
            for(int j=0; j<20-dummy.size(); ++j) { fileStream << " "; }
        }
        fileStream << "\n";
    }

    file.close();
}

void PlotTab::exportToGraphics()
{

    mpGraphicsSettingsDialog = new QDialog(this);
    mpGraphicsSettingsDialog->setWindowTitle("Graphic Controls");
    mpGraphicsSettingsDialog->setWindowModality(Qt::WindowModal);

    mpGraphicsSize = new QSpinBox(this);
    mpGraphicsSize->setRange(1,10000);
    mpGraphicsSize->setSingleStep(1);
    mpGraphicsSize->setValue(500);

    mpGraphicsSizeW = new QSpinBox(this);
    mpGraphicsSizeW->setRange(1,10000);
    mpGraphicsSizeW->setSingleStep(1);
    mpGraphicsSizeW->setValue(500);

    mpGraphicsQuality = new QSpinBox(this);
    mpGraphicsQuality->setRange(1,10);
    mpGraphicsQuality->setSingleStep(1);
    mpGraphicsQuality->setValue(1);

    mpGraphicsForm = new QComboBox(this);
    mpGraphicsForm->addItem("PNG");
    mpGraphicsForm->addItem("PDF");
    mpGraphicsForm->addItem("SVG");

    QGroupBox *graphicsBox = new QGroupBox( "Graphics" );
    QGridLayout *graphicsBoxLayout = new QGridLayout( graphicsBox );

    int row = 0;
    graphicsBoxLayout->addWidget( new QLabel( "Export Format" ), row, 0 );
    graphicsBoxLayout->addWidget( mpGraphicsForm, row, 1 );
    row++;
    graphicsBoxLayout->addWidget( new QLabel( "Height" ), row, 0 );
    graphicsBoxLayout->addWidget( mpGraphicsSize, row, 1 );
    row++;
    graphicsBoxLayout->addWidget( new QLabel( "Width" ), row, 0 );
    graphicsBoxLayout->addWidget( mpGraphicsSizeW, row, 1 );
    row++;
    graphicsBoxLayout->addWidget( new QLabel( "Quality" ), row, 0 );
    graphicsBoxLayout->addWidget( mpGraphicsQuality, row, 1 );

    mpGraphicsSettingsDialog->setLayout(graphicsBoxLayout);

    connect(mpGraphicsSize, SIGNAL(valueChanged(int)), this, SLOT(applyGraphicsSettings()));
    connect(mpGraphicsSizeW, SIGNAL(valueChanged(int)), this, SLOT(applyGraphicsSettings()));
    connect(mpGraphicsQuality, SIGNAL(valueChanged(int)), this, SLOT(applyGraphicsSettings()));
    connect(mpGraphicsForm, SIGNAL(indexChanged(int)), this, SLOT(applyGraphicsSettings()));

}

//void PlotTab::applyGraphicsSettings()
//{
//    //        QString fileName = QFileDialog::getSaveFileName(this, "Export File Name", gConfig.getPlotGfxDir(), "Portable Document Format (*.pdf)");
//    //        if ( !fileName.isEmpty() )
//    //        {
//    //            QFileInfo file(fileName);
//    //            gConfig.setPlotGfxDir(file.absolutePath());

//    //            QwtPlotRenderer renderer;

//    //            QPrinter *printer = new QPrinter(QPrinter::HighResolution);
//    //            printer->setPaperSize(QPrinter::Custom);
//    //            printer->setPaperSize(mpQwtPlots[FIRSTPLOT]->size(), QPrinter::Point);
//    //            printer->setOrientation(QPrinter::Landscape);
//    //            printer->setFullPage(false);
//    //            printer->setOutputFormat(QPrinter::PdfFormat);
//    //            printer->setOutputFileName(fileName);
//    //            renderer.renderTo(mpQwtPlots[FIRSTPLOT],*printer);
//    //        }

//    //        QString fileName = QFileDialog::getSaveFileName(this, "Export File Name", gConfig.getPlotGfxDir(), "Portable Document Format (*.pdf)");
//    //        if ( !fileName.isEmpty() )
//    //        {
//    //            QFileInfo file(fileName);
//    //            gConfig.setPlotGfxDir(file.absolutePath());

//    //            QwtPlotRenderer renderer;

//    //            QPrinter *printer = new QPrinter(QPrinter::HighResolution);
//    //            printer->setPaperSize(QPrinter::Custom);
//    //            printer->setPaperSize(mpQwtPlots[FIRSTPLOT]->size(), QPrinter::Point);
//    //            printer->setOrientation(QPrinter::Landscape);
//    //            printer->setFullPage(false);
//    //            printer->setOutputFormat(QPrinter::PdfFormat);
//    //            printer->setOutputFileName(fileName);
//    //            renderer.renderTo(mpQwtPlots[FIRSTPLOT],*printer);
//        //        }
//}


//! @brief Slot that exports plot tab as vector graphics to specified .pdf file
void PlotTab::exportToPdf()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export File Name", gConfig.getPlotGfxDir(), "Portable Document Format (*.pdf)");
    if ( !fileName.isEmpty() )
    {
        QFileInfo file(fileName);
        gConfig.setPlotGfxDir(file.absolutePath());

        QwtPlotRenderer renderer;

        QPrinter *printer = new QPrinter(QPrinter::HighResolution);
        printer->setPaperSize(QPrinter::Custom);
        printer->setPaperSize(mpQwtPlots[FIRSTPLOT]->size(), QPrinter::Point);
        printer->setOrientation(QPrinter::Landscape);
        printer->setFullPage(false);
        printer->setOutputFormat(QPrinter::PdfFormat);
        printer->setOutputFileName(fileName);
        renderer.renderTo(mpQwtPlots[FIRSTPLOT],*printer);
    }
}

void PlotTab::exportToOldHop()
{
    //Open file dialog and initialize the file stream
    QDir fileDialogSaveDir;
    QString filePath;
    QFileInfo fileInfo;
    filePath = QFileDialog::getSaveFileName(this, tr("Export Plot Tab To OldHopsan Format File"),
                                            gConfig.getPlotDataDir(),
                                            tr("Hopsan Classic file (*.PLO)"));
    if(filePath.isEmpty()) return;    //Don't save anything if user presses cancel
    fileInfo.setFile(filePath);
    gConfig.setPlotDataDir(fileInfo.absolutePath());

    QStringList variables;
    for(int c=0; c<mPlotCurvePtrs[FIRSTPLOT].size(); ++c)
    {
        variables.append(mPlotCurvePtrs[FIRSTPLOT][c]->getPlotLogDataVariable()->getFullVariableName());
    }

    mPlotCurvePtrs[FIRSTPLOT].first()->getContainerObjectPtr()->getLogDataHandler()->exportToPlo(filePath, variables);

    //    file.setFileName(fileInfo.filePath());   //Create a QFile object
    //    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    //    {
    //        gpMainWindow->mpHcomWidget->mpConsole->printErrorMessage("Failed to open file for writing: " + filePath);
    //        return;
    //    }

    //    QTextStream fileStream(&file);  //Create a QTextStream object to stream the content of file
    //    QDateTime dateTime = QDateTime::currentDateTime();
    //    QString dateTimeString = dateTime.toString();
    //    QFileInfo fii(filePath);
    //    QString namez = fii.baseName();
    //    QStringList ScalingvaluesList;
    //    QStringList StartvaluesList;
    //    QVector<double> Scalings;
    //    QString ScaleVal;

    //    QString modelPathwayy = gpMainWindow->mpProjectTabs->getCurrentContainer()->getModelFileInfo().filePath();
    //    QFileInfo fiz(modelPathwayy);
    //    QString namemodel = fiz.baseName();

    //        //Write initial comment
    //    fileStream << "    'VERSION' " << QString(HOPSANGUIVERSION) << " " << dateTimeString << "\n";
    //    fileStream << "    1 " << "\n";
    //    fileStream << "    '"<<namez<<".PLO"<<"'"<<"\n";
    //    fileStream << "        " << mPlotCurvePtrs[FIRSTPLOT].size()<<"    "<< mPlotCurvePtrs[FIRSTPLOT].first()->getTimeVector().size()<<"\n";
    //    fileStream << "    'Time      '";
    //    for(int i=0; i<mPlotCurvePtrs[FIRSTPLOT].size(); ++i)
    //    {
    //        fileStream << ",    'Y" << i<<"      '";
    //    }
    //    fileStream <<",    '"<< "\n";

    //        //Write time and data vectors
    //    QString dummy;


    ////    double iMax = Scalings[0]; //set min and max as the first element
    ////    double iMin = Scalings[0];
    ////    for (int xi=0; xi<Scalings.size(); xi++)
    ////    {
    ////        if (Scalings[xi] < iMin)
    ////            iMin = Scalings[xi];
    ////        if (Scalings[xi] > iMax)
    ////            iMax = Scalings[xi];

    ////        double Scale = (iMax-iMin)/(Scalings.size());


    ////    }




    //    for(int kk=0; kk<mPlotCurvePtrs[FIRSTPLOT].size()+1; ++kk)
    //    {

    //        ScalingvaluesList.append(dummy.setNum(1.0,'E',6));
    //        fileStream <<"  "<< dummy;
    //        for(int j=0; j<12-dummy.size(); ++j) { fileStream << " "; }


    //    }
    //    fileStream << "\n";


    //    for(int i=0; i<mPlotCurvePtrs[FIRSTPLOT].first()->getTimeVector().size(); ++i)
    //    {
    //        dummy.setNum(mPlotCurvePtrs[FIRSTPLOT].first()->getTimeVector()[i],'E',6);
    //        fileStream <<"  "<<dummy;
    //        for(int j=0; j<12-dummy.size(); ++j) { fileStream << " "; }

    //        for(int k=0; k<mPlotCurvePtrs[FIRSTPLOT].size(); ++k)
    //        {
    //            dummy.setNum(mPlotCurvePtrs[FIRSTPLOT][k]->getDataVector()[i],'E',6);
    //            Scalings = mPlotCurvePtrs[FIRSTPLOT][k]->getDataVector();
    //            if(i == 0)
    //            {
    //               StartvaluesList.append(dummy.setNum(mPlotCurvePtrs[FIRSTPLOT][k]->getDataVector()[i],'E',6));
    //            }

    //            fileStream <<"  "<< dummy;
    //            for(int j=0; j<12-dummy.size(); ++j) { fileStream << " "; }

    //        }
    //        fileStream << "\n";
    //    }
    //    fileStream << "  "+namez+".PLO.DAT_-1" <<"\n";
    //    fileStream << "  "+namemodel+".for" <<"\n";
    //    fileStream <<"   Variable     Startvalue     Scaling" <<"\n";
    //    fileStream <<"------------------------------------------------------" <<"\n";
    //    for(int ii=0; ii<mPlotCurvePtrs[FIRSTPLOT].size(); ++ii)
    //    {
    //        fileStream << "  Y" << ii << "     " << StartvaluesList[ii]<<"      "<<ScalingvaluesList[ii]<<"\n";
    //    }



    //    file.close();
}

//! @brief Slot that exports plot tab as bitmap to specified .png file
void PlotTab::exportToPng()
{
    QString fileName = QFileDialog::getSaveFileName(
                this, "Export File Name", gConfig.getPlotGfxDir(),
                "Portable Network Graphics (*.png)");

    if(!fileName.isEmpty())
    {
        QFileInfo file(fileName);
        gConfig.setPlotGfxDir(file.absolutePath());

        if(mpBarPlot->isVisible())
        {
            QPixmap pixmap = QPixmap::grabWidget(this);
            pixmap.save(fileName);
        }
        else
        {
            QPixmap pixmap(mpQwtPlots[FIRSTPLOT]->width(), mpQwtPlots[FIRSTPLOT]->height());
            pixmap.fill();
            QwtPlotRenderer renderer;
            renderer.renderTo(mpQwtPlots[FIRSTPLOT], pixmap);
            pixmap.save(fileName);
        }
    }
}


void PlotTab::enableZoom(bool value)
{
    if(mpParentPlotWindow->mpPanButton->isChecked() && value)
    {
        mpParentPlotWindow->mpPanButton->setChecked(false);
        mpPanner[FIRSTPLOT]->setEnabled(false);
        mpPanner[SECONDPLOT]->setEnabled(false);
    }
    if(mpParentPlotWindow->mpArrowButton->isChecked() && value)
    {
        mpParentPlotWindow->mpArrowButton->setChecked(false);
    }
    mpZoomer[FIRSTPLOT]->setEnabled(value);
    if(value)   { mpZoomer[FIRSTPLOT]->setRubberBand(QwtPicker::RectRubberBand); }
    else        { mpZoomer[FIRSTPLOT]->setRubberBand(QwtPicker::NoRubberBand); }
    mpZoomerRight[FIRSTPLOT]->setEnabled(value);
    mpZoomer[SECONDPLOT]->setEnabled(value);
    if(value)   { mpZoomer[SECONDPLOT]->setRubberBand(QwtPicker::RectRubberBand); }
    else        { mpZoomer[SECONDPLOT]->setRubberBand(QwtPicker::NoRubberBand); }
    mpZoomerRight[SECONDPLOT]->setEnabled(value);
    mpParentPlotWindow->mpResetXVectorButton->setEnabled(false);
}

void PlotTab::enableArrow(bool value)
{
    if(mpParentPlotWindow->mpZoomButton->isChecked() && value)
    {
        mpParentPlotWindow->mpZoomButton->setChecked(false);
        mpZoomer[FIRSTPLOT]->setEnabled(false);
        mpZoomer[FIRSTPLOT]->setRubberBand(QwtPicker::NoRubberBand);
        mpZoomerRight[FIRSTPLOT]->setEnabled(false);
        mpZoomer[SECONDPLOT]->setEnabled(false);
        mpZoomer[SECONDPLOT]->setRubberBand(QwtPicker::NoRubberBand);
        mpZoomerRight[SECONDPLOT]->setEnabled(false);
    }
    if(mpParentPlotWindow->mpPanButton->isChecked() && value)
    {
        mpParentPlotWindow->mpPanButton->setChecked(false);
        mpPanner[FIRSTPLOT]->setEnabled(false);
        mpPanner[SECONDPLOT]->setEnabled(false);
    }

}


void PlotTab::enablePan(bool value)
{
    if(mpParentPlotWindow->mpZoomButton->isChecked() && value)
    {
        mpParentPlotWindow->mpZoomButton->setChecked(false);
        //mpParentPlotWindow->mpArrowButton->setChecked(false);
        mpZoomer[FIRSTPLOT]->setEnabled(false);
        mpZoomer[FIRSTPLOT]->setRubberBand(QwtPicker::NoRubberBand);
        mpZoomerRight[FIRSTPLOT]->setEnabled(false);
        mpZoomer[SECONDPLOT]->setEnabled(false);
        mpZoomer[SECONDPLOT]->setRubberBand(QwtPicker::NoRubberBand);
        mpZoomerRight[SECONDPLOT]->setEnabled(false);
    }
    if(mpParentPlotWindow->mpArrowButton->isChecked() && value)
    {
        mpParentPlotWindow->mpArrowButton->setChecked(false);
    }
    mpPanner[FIRSTPLOT]->setEnabled(value);
    mpPanner[SECONDPLOT]->setEnabled(value);
}


void PlotTab::enableGrid(bool value)
{
    for(int plotID=0; plotID<2; ++plotID)
    {
        mpGrid[plotID]->setVisible(value);
    }
}


void PlotTab::setBackgroundColor()
{
    QColor color = QColorDialog::getColor(mpQwtPlots[FIRSTPLOT]->canvasBackground().color(), this);
    if (color.isValid())
    {
        mpQwtPlots[FIRSTPLOT]->setCanvasBackground(color);
        mpQwtPlots[FIRSTPLOT]->replot();
        mpQwtPlots[FIRSTPLOT]->updateGeometry();
        mpQwtPlots[SECONDPLOT]->setCanvasBackground(color);
        mpQwtPlots[SECONDPLOT]->replot();
        mpQwtPlots[SECONDPLOT]->updateGeometry();
    }
}


QList<PlotCurve *> PlotTab::getCurves(HopsanPlotID plotID)
{
    return mPlotCurvePtrs[plotID];
}


void PlotTab::setActivePlotCurve(PlotCurve *pCurve)
{
    // Mark deactive all others
    //! @todo if only one can be active it should be enough to deactivate that one
    for(int i=0; i<mPlotCurvePtrs[FIRSTPLOT].size(); ++i)
    {
        if(mPlotCurvePtrs[FIRSTPLOT].at(i) != pCurve)
        {
            mPlotCurvePtrs[FIRSTPLOT].at(i)->markActive(false);
        }
    }
    // Mark active the one
    if (pCurve!=0)
    {
        pCurve->markActive(true);
    }

    // Remember active curve
    mpActivePlotCurve = pCurve;
}


PlotCurve *PlotTab::getActivePlotCurve()
{
    return mpActivePlotCurve;
}


QwtPlot *PlotTab::getPlot(HopsanPlotID plotID)
{
    return mpQwtPlots[plotID];
}


void PlotTab::showPlot(HopsanPlotID plotID, bool visible)
{
    mpQwtPlots[plotID]->setVisible(visible);
}


int PlotTab::getNumberOfCurves(HopsanPlotID plotID)
{
    return mPlotCurvePtrs[plotID].size();
}



void PlotTab::update()
{
    for(int plotID=0; plotID<1; ++plotID)
    {
        mpQwtPlots[plotID]->enableAxis(QwtPlot::yLeft, false);
        mpQwtPlots[plotID]->enableAxis(QwtPlot::yRight, false);
        QList<PlotCurve *>::iterator cit;
        for(cit=mPlotCurvePtrs[plotID].begin(); cit!=mPlotCurvePtrs[plotID].end(); ++cit)
        {
            if(!mpQwtPlots[plotID]->axisEnabled((*cit)->getAxisY())) { mpQwtPlots[plotID]->enableAxis((*cit)->getAxisY()); }
            (*cit)->getQwtPlotCurvePtr()->attach(mpQwtPlots[plotID]);
        }

        for(int i=0; i<mMarkerPtrs[plotID].size(); ++i)
        {
            QPointF posF = mMarkerPtrs[plotID].at(i)->value();
            double x = mpQwtPlots[plotID]->transform(QwtPlot::xBottom, posF.x());
            double y = mpQwtPlots[plotID]->transform(QwtPlot::yLeft, posF.y());
            QPoint pos = QPoint(x,y);
            HopQwtPlotCurve *pCurve = mMarkerPtrs[plotID].at(i)->getCurve()->getQwtPlotCurvePtr();
            mMarkerPtrs[plotID].at(i)->setXValue(pCurve->sample(pCurve->closestPoint(pos)).x());
            mMarkerPtrs[plotID].at(i)->setYValue(mpQwtPlots[plotID]->invTransform(QwtPlot::yLeft, mpQwtPlots[plotID]->transform(pCurve->yAxis(), pCurve->sample(pCurve->closestPoint(pos)).y())));
        }
        mpQwtPlots[plotID]->replot();
        mpQwtPlots[plotID]->updateGeometry();
    }
}


void PlotTab::insertMarker(PlotCurve *pCurve, double x, double y, QString altLabel, bool movable)
{
    qDebug() << "x and y = " << x << ", " << y;

    int plotID = getPlotIDFromCurve(pCurve);

    mpMarkerSymbol->setPen(QPen(pCurve->getQwtPlotCurvePtr()->pen().brush().color(), 3));
    PlotMarker *tempMarker = new PlotMarker(pCurve, this, mpMarkerSymbol);
    mMarkerPtrs[plotID].append(tempMarker);

    tempMarker->attach(mpQwtPlots[plotID]);
    QCursor cursor;
    tempMarker->setXValue(x);
    tempMarker->setYValue(y);

    QString xString;
    QString yString;
    xString.setNum(x);
    yString.setNum(y);
    QwtText tempLabel;
    if(altLabel != QString())
    {
        tempLabel.setText(altLabel);
    }
    else
    {
        tempLabel.setText("("+xString+", "+yString+")");
    }
    tempLabel.setColor(pCurve->getQwtPlotCurvePtr()->pen().brush().color());
    tempLabel.setBackgroundBrush(QColor(255,255,255,220));
    tempLabel.setFont(QFont("Calibri", 12, QFont::Normal));
    tempMarker->setLabel(tempLabel);
    tempMarker->setLabelAlignment(Qt::AlignTop);

    mpQwtPlots[plotID]->canvas()->installEventFilter(tempMarker);
    mpQwtPlots[plotID]->canvas()->setMouseTracking(true);

    tempMarker->setMovable(movable);
}


//! @brief Inserts a curve marker at the specified curve
//! @param curve is a pointer to the specified curve
void PlotTab::insertMarker(PlotCurve *pCurve, QPoint pos, bool movable)
{
    int plotID = getPlotIDFromCurve(pCurve);

    mpMarkerSymbol->setPen(QPen(pCurve->getQwtPlotCurvePtr()->pen().brush().color(), 3));
    PlotMarker *tempMarker = new PlotMarker(pCurve, this, mpMarkerSymbol);
    mMarkerPtrs[plotID].append(tempMarker);

    tempMarker->attach(mpQwtPlots[plotID]);
    QCursor cursor;
    tempMarker->setXValue(pCurve->getQwtPlotCurvePtr()->sample(pCurve->getQwtPlotCurvePtr()->closestPoint(pos)).x());
    tempMarker->setYValue(mpQwtPlots[plotID]->invTransform(QwtPlot::yLeft, mpQwtPlots[plotID]->transform(pCurve->getQwtPlotCurvePtr()->yAxis(), pCurve->getQwtPlotCurvePtr()->sample(pCurve->getQwtPlotCurvePtr()->closestPoint(pos)).y())));

    QString xString;
    QString yString;
    double x = pCurve->getQwtPlotCurvePtr()->sample(pCurve->getQwtPlotCurvePtr()->closestPoint(pos)).x();
    double y = pCurve->getQwtPlotCurvePtr()->sample(pCurve->getQwtPlotCurvePtr()->closestPoint(mpQwtPlots[plotID]->canvas()->mapFromGlobal(cursor.pos()))).y();
    xString.setNum(x);
    yString.setNum(y);
    QwtText tempLabel;
    tempLabel.setText("("+xString+", "+yString+")");
    tempLabel.setColor(pCurve->getQwtPlotCurvePtr()->pen().brush().color());
    tempLabel.setBackgroundBrush(QColor(255,255,255,220));
    tempLabel.setFont(QFont("Calibri", 12, QFont::Normal));
    tempMarker->setLabel(tempLabel);
    tempMarker->setLabelAlignment(Qt::AlignTop);

    mpQwtPlots[plotID]->canvas()->installEventFilter(tempMarker);
    mpQwtPlots[plotID]->canvas()->setMouseTracking(true);

    tempMarker->setMovable(movable);
}


//! @brief Saves the current tab to a DOM element (XML)
//! @param rDomElement Reference to the dom element to save to
//! @param dateTime Tells whether or not date and time should be included
//! @param descriptions Tells whether or not variable descriptions shall be included
void PlotTab::saveToDomElement(QDomElement &rDomElement, bool dateTime, bool descriptions)
{
    if(dateTime)
    {
        QDateTime datetime;
        rDomElement.setAttribute("datetime", datetime.currentDateTime().toString(Qt::ISODate));
    }

    if(mpBarPlot->isVisible())
    {
        QAbstractItemModel *model = mpBarPlot->model();

        for(int c=0; c<model->columnCount(); ++c)
        {
            double losses = model->data(model->index(0, c)).toInt() - model->data(model->index(1, c)).toInt();;

            QDomElement dataTag = appendDomElement(rDomElement, "data");
            QDomElement varTag = appendDomElement(dataTag, "losses");
            QString valueString;
            valueString.setNum(losses);
            QDomText value = varTag.ownerDocument().createTextNode(valueString);
            varTag.appendChild(value);

            if(descriptions)
            {
                varTag.setAttribute("component", model->headerData(c, Qt::Horizontal).toString());
            }
        }
    }
    else
    {

        //Cycle plot curves and write data tags
        for(int j=0; j<mPlotCurvePtrs[FIRSTPLOT][0]->getTimeVector().size(); ++j)
        {
            QDomElement dataTag = appendDomElement(rDomElement, "data");

            if(mHasSpecialXAxis)        //Special x-axis, replace time with x-data
            {
                setQrealAttribute(dataTag, mSpecialXVectorDescription.mDataName, mSpecialXVector[j], 10, 'g');
            }
            else                        //X-axis = time
            {
                setQrealAttribute(dataTag, "time", mPlotCurvePtrs[FIRSTPLOT][0]->getTimeVector()[j], 10, 'g');
            }

            //Write variable tags for each variable
            for(int i=0; i<mPlotCurvePtrs[FIRSTPLOT].size(); ++i)
            {
                QString numTemp;
                numTemp.setNum(i);
                QDomElement varTag = appendDomElement(dataTag, mPlotCurvePtrs[FIRSTPLOT][i]->getDataName()+numTemp);
                QString valueString;
                valueString.setNum(mPlotCurvePtrs[FIRSTPLOT][i]->getDataVector()[j]);
                QDomText value = varTag.ownerDocument().createTextNode(valueString);
                varTag.appendChild(value);

                if(descriptions)
                {
                    varTag.setAttribute("component", mPlotCurvePtrs[FIRSTPLOT][i]->getComponentName());
                    varTag.setAttribute("port", mPlotCurvePtrs[FIRSTPLOT][i]->getPortName());
                    varTag.setAttribute("type", mPlotCurvePtrs[FIRSTPLOT][i]->getDataName());
                    varTag.setAttribute("unit", mPlotCurvePtrs[FIRSTPLOT][i]->getDataUnit());
                }
            }
        }
    }
}


bool PlotTab::isSpecialPlot()
{
    return mIsSpecialPlot;
}


void PlotTab::setBottomAxisLogarithmic(bool value)
{
    mBottomAxisLogarithmic = value;
    if(value)
    {
        getPlot(FIRSTPLOT)->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine(10));
        getPlot(SECONDPLOT)->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine(10));
    }
    else
    {
        getPlot(FIRSTPLOT)->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
        getPlot(SECONDPLOT)->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
    }

}


bool PlotTab::hasLogarithmicBottomAxis()
{
    return mBottomAxisLogarithmic;
}


//! @todo this only tunrs on internal legend automatically, maybe need an otehr version with two arguments
void PlotTab::setLegendsVisible(bool value)
{
    if (value)
    {
        //Only turn on internal automatically
        mpLegendsInternalEnabledCheckBox->setChecked(true);
    }
    else
    {
        mpLegendsInternalEnabledCheckBox->setChecked(false);
        mpLegendsExternalEnabledCheckBox->setChecked(false);
    }
    applyLegendSettings();
}


//! @brief Private slot that updates the xml preview field in the export to xml dialog
QString PlotTab::updateXmlOutputTextInDialog()
{
    QDomDocument domDocument;
    QDomElement element = domDocument.createElement("hopsanplotdata");
    domDocument.appendChild(element);
    this->saveToDomElement(element, mpIncludeTimeCheckBox->isChecked(), mpIncludeDescriptionsCheckBox->isChecked());
    QString output = domDocument.toString(mpXmlIndentationSpinBox->value());

    QStringList lines = output.split("\n");

    //We want the first 10 lines and the last 2 from the xml output
    QString display;
    for(int i=0; i<10 && i<lines.size(); ++i)
    {
        display.append(lines[i]);
        display.append("\n");
    }
    for(int k=0; k<mpXmlIndentationSpinBox->value(); ++k) display.append(" ");
    if(lines.size() > 9)
    {
        display.append("...\n");
        display.append(lines[lines.size()-2]);
        display.append(lines[lines.size()-1]);
    }


    display.replace(" ", "&nbsp;");
    display.replace(">", "!!!GT!!!");
    display.replace("<", "<font color=\"saddlebrown\">&lt;");
    display.replace("!!!GT!!!","</font><font color=\"saddlebrown\">&gt;</font>");
    display.replace("\n", "<br>\n");
    display.replace("&lt;?xml", "&lt;?xml</font>");
    display.replace("&lt;data", "&lt;data</font>");

    display.replace("0&nbsp;", "0</font>&nbsp;");
    display.replace("1&nbsp;", "1</font>&nbsp;");
    display.replace("2&nbsp;", "2</font>&nbsp;");
    display.replace("3&nbsp;", "3</font>&nbsp;");
    display.replace("4&nbsp;", "4</font>&nbsp;");
    display.replace("5&nbsp;", "5</font>&nbsp;");
    display.replace("6&nbsp;", "6</font>&nbsp;");
    display.replace("7&nbsp;", "7</font>&nbsp;");
    display.replace("8&nbsp;", "8</font>&nbsp;");
    display.replace("9&nbsp;", "9</font>&nbsp;");

    display.replace("&lt;hopsanplotdata", "&lt;hopsanplotdata</font>");
    display.replace("&lt;losses", "&lt;losses</font>");
    display.replace("&nbsp;version=", "&nbsp;version=<font face=\"Consolas\" color=\"darkred\">");
    display.replace("&nbsp;encoding=", "&nbsp;encoding=<font face=\"Consolas\" color=\"darkred\">");
    display.replace("&nbsp;component=", "&nbsp;component=<font face=\"Consolas\" color=\"darkred\">");
    display.replace("&nbsp;port=", "&nbsp;port=<font face=\"Consolas\" color=\"darkred\">");
    display.replace("&nbsp;type=", "&nbsp;type=<font face=\"Consolas\" color=\"darkred\">");
    display.replace("&nbsp;unit=", "&nbsp;unit=<font face=\"Consolas\" color=\"darkred\">");
    display.replace("&nbsp;time=", "&nbsp;time=<font face=\"Consolas\" color=\"darkred\">");
    display.replace("&nbsp;datetime=", "&nbsp;datetime=<font face=\"Consolas\" color=\"darkred\">");
    display.replace("\"&nbsp;", "\"</font>&nbsp;");

    display.replace("&nbsp;", "<font face=\"Consolas\">&nbsp;</font>");
    display.replace("</font></font>", "</font>");

    mpXmlOutputTextBox->setText(display);

    return output;
}


//! @brief Private slot that opens a file dialog and saves the current tab to a specified XML file
//! @note Don't call this directly, call exportToXml() first and it will subsequently call this slot
void PlotTab::saveToXml()
{
    //Open file dialog and initialize the file stream
    QDir fileDialogSaveDir;
    QString filePath;
    QFileInfo fileInfo;
    QFile file;
    filePath = QFileDialog::getSaveFileName(this, tr("Export Plot Tab To XML File"),
                                            gConfig.getPlotDataDir(),
                                            tr("Extensible Markup Language (*.xml)"));
    if(filePath.isEmpty()) return;    //Don't save anything if user presses cancel
    fileInfo.setFile(filePath);
    gConfig.setPlotDataDir(fileInfo.absolutePath());
    file.setFileName(fileInfo.filePath());   //Create a QFile object
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gpMainWindow->mpTerminalWidget->mpConsole->printErrorMessage("Failed to open file for writing: " + filePath);
        return;
    }

    QDomDocument domDocument;
    QDomElement element = domDocument.createElement("hopsanplotdata");
    domDocument.appendChild(element);
    this->saveToDomElement(element, mpIncludeTimeCheckBox->isChecked(), mpIncludeDescriptionsCheckBox->isChecked());
    appendRootXMLProcessingInstruction(domDocument);

    QTextStream fileStream(&file);
    domDocument.save(fileStream, mpXmlIndentationSpinBox->value());
    file.close();

    mpExportXmlDialog->close();
}


int PlotTab::getPlotIDFromCurve(PlotCurve *pCurve)
{
    for(int plotID=0; plotID<2; ++plotID)
    {
        if(mPlotCurvePtrs[plotID].contains(pCurve))
            return plotID;
    }
    qFatal("Plot curve has no plot ID (should never happen)");
    Q_ASSERT(false);      //Plot curve has no plot ID (should never happen)
    return -1;
}

//! @brief HelpFunction for constructor
void PlotTab::constructLegendSettingsDialog()
{
    mpLegendSettingsDialog = new QDialog(this);
    mpLegendSettingsDialog->setWindowTitle("Legend Controls");
    mpLegendSettingsDialog->setWindowModality(Qt::WindowModal);

    mpLegendSize = new QSpinBox(this);
    mpLegendSize->setRange(1,20);
    mpLegendSize->setSingleStep(1);
    mpLegendSize->setValue(11);

    mpLegendCol = new QSpinBox(this);
    mpLegendCol->setRange(1,20);
    mpLegendCol->setSingleStep(1);
    mpLegendCol->setValue(1);

    mpLegendLeftOffset = new QDoubleSpinBox(this);
    mpLegendLeftOffset->setRange(-DBLMAX, DBLMAX);
    mpLegendLeftOffset->setDecimals(2);
    mpLegendLeftOffset->setSingleStep(0.1);
    mpLegendLeftOffset->setValue(0);

    mpLegendRightOffset = new QDoubleSpinBox(this);
    mpLegendRightOffset->setRange(-DBLMAX, DBLMAX);
    mpLegendRightOffset->setDecimals(2);
    mpLegendRightOffset->setSingleStep(0.1);
    mpLegendRightOffset->setValue(0);

    mpLegendsAutoOffsetCheckBox = new QCheckBox(this);
    mpLegendsAutoOffsetCheckBox->setCheckable(true);
    mpLegendsAutoOffsetCheckBox->setChecked(true);

//    mpLegendsOffYREnabledCheckBox = new QCheckBox(this);
//    mpLegendsOffYREnabledCheckBox->setCheckable(true);

    mpLegendsInternalEnabledCheckBox = new QCheckBox(this);
    mpLegendsInternalEnabledCheckBox->setCheckable(true);
    mpLegendsInternalEnabledCheckBox->setChecked(true); //Internal on by default

    mpLegendsExternalEnabledCheckBox = new QCheckBox(this);
    mpLegendsExternalEnabledCheckBox->setCheckable(true);

    mpLegendLPosition = new QComboBox(this);
    mpLegendLPosition->addItem("Top");
    mpLegendLPosition->addItem("Bottom");
    mpLegendLPosition->addItem("Centre");

    mpLegendRPosition = new QComboBox(this);
    mpLegendRPosition->addItem("Top");
    mpLegendRPosition->addItem("Bottom");
    mpLegendRPosition->addItem("Centre");

    mpLegendBlob = new QComboBox(this);
    mpLegendBlob->addItem("White");
    mpLegendBlob->addItem("Red");
    mpLegendBlob->addItem("Blue");
    mpLegendBlob->addItem("Black");
    mpLegendBlob->addItem("Maroon");
    mpLegendBlob->addItem("Gray");
    mpLegendBlob->addItem("LightSalmon");
    mpLegendBlob->addItem("SteelBlue");
    mpLegendBlob->addItem("Yellow");
    mpLegendBlob->addItem("Gray");
    mpLegendBlob->addItem("Fuchsia");
    mpLegendBlob->addItem("PaleGreen");
    mpLegendBlob->addItem("PaleTurquoise");
    mpLegendBlob->addItem("Cornsilk");
    mpLegendBlob->addItem("HotPink");
    mpLegendBlob->addItem("Peru");
    mpLegendBlob->addItem("Pink");

    mpLegendBg = new QComboBox(this);
    mpLegendBg->addItem("Legends", HopQwtPlotLegendItem::LegendBackground);
    mpLegendBg->addItem("Items", HopQwtPlotLegendItem::ItemBackground);

    mpLegendSym = new QComboBox(this);
    mpLegendSym->addItem("Line", HopQwtPlotCurve::LegendShowLine ); //Line first (default)
    mpLegendSym->addItem("Rectangle", HopQwtPlotCurve::LegendNoAttribute );
    mpLegendSym->addItem("Default Symbol", HopQwtPlotCurve::LegendShowSymbol );
    mpLegendSym->addItem("Brush", HopQwtPlotCurve::LegendShowBrush );

    QGroupBox *legendBox = new QGroupBox( "Legend" );
    QGridLayout *legendBoxLayout = new QGridLayout( legendBox );

    int row = 0;
    legendBoxLayout->addWidget( new QLabel( "Size" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendSize, row, 1 );

    row++;
    legendBoxLayout->addWidget( new QLabel( "Columns" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendCol, row, 1 );

    row++;
    legendBoxLayout->addWidget( new QLabel( "Left Legend Position" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendLPosition, row, 1 );

    row++;
    legendBoxLayout->addWidget( new QLabel( "Right Legend Position" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendRPosition, row, 1 );

    row++;
    legendBoxLayout->addWidget( new QLabel( "Background" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendBg, row, 1 );

    QPushButton *pFinishedLegButton = new QPushButton("Close", mpLegendSettingsDialog);
    QDialogButtonBox *pFinishedLegButtonBox = new QDialogButtonBox(Qt::Horizontal);
    pFinishedLegButtonBox->addButton(pFinishedLegButton, QDialogButtonBox::ActionRole);

    row++;
    legendBoxLayout->addWidget( new QLabel( "Internal Legends" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendsInternalEnabledCheckBox, row, 1 );

    row++;
    legendBoxLayout->addWidget( new QLabel( "External Legends" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendsExternalEnabledCheckBox, row, 1 );

    row++;
    legendBoxLayout->addWidget( new QLabel( "Legend BG Color" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendBlob, row, 1 );

    row++;
    legendBoxLayout->addWidget( new QLabel( "Legend Symbol" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendSym, row, 1 );

    row++;
    legendBoxLayout->addWidget( new QLabel( "Auto Offset" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendsAutoOffsetCheckBox, row, 1 );

    row++;
    legendBoxLayout->addWidget( new QLabel( "Left Legend Offset" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendLeftOffset, row, 1 );

    row++;
    legendBoxLayout->addWidget( new QLabel( "Right Legend Offset" ), row, 0 );
    legendBoxLayout->addWidget( mpLegendRightOffset, row, 1 );

    row++;
    legendBoxLayout->addWidget( pFinishedLegButton, row, 1 );

    mpLegendSettingsDialog->setLayout(legendBoxLayout);

    connect(mpLegendSize, SIGNAL(valueChanged(int)), this, SLOT(applyLegendSettings()));
    connect(mpLegendCol, SIGNAL(valueChanged(int)), this, SLOT(applyLegendSettings()));
    connect(mpLegendsInternalEnabledCheckBox, SIGNAL(toggled(bool)), this, SLOT(applyLegendSettings()));
    connect(mpLegendsExternalEnabledCheckBox, SIGNAL(toggled(bool)), this, SLOT(applyLegendSettings()));
    connect(mpLegendBg, SIGNAL(currentIndexChanged(int)), this, SLOT(applyLegendSettings()));
    connect(mpLegendSym, SIGNAL(currentIndexChanged(int)), this, SLOT(applyLegendSettings()));
    connect(mpLegendLPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(applyLegendSettings()));
    connect(mpLegendRPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(applyLegendSettings()));
    connect(mpLegendBlob, SIGNAL(currentIndexChanged(int)), this, SLOT(applyLegendSettings()));
    connect(mpLegendLeftOffset, SIGNAL(valueChanged(double)), this, SLOT(applyLegendSettings()));
    connect(mpLegendRightOffset, SIGNAL(valueChanged(double)), this, SLOT(applyLegendSettings()));
    connect(mpLegendsAutoOffsetCheckBox, SIGNAL(toggled(bool)), this, SLOT(applyLegendSettings()));
    connect(pFinishedLegButton, SIGNAL(clicked()), mpLegendSettingsDialog, SLOT(close()));
}

//! @brief HelpFunction for constructor
void PlotTab::constructAxisSettingsDialog()
{
    mpSetAxisDialog = new QDialog(this);
    mpSetAxisDialog->setWindowTitle("Set Lock on Axis");
    //mpSetAxisDialog->setWindowModality(Qt::WindowModal);

    mpXbSetLockCheckBox = new QCheckBox("Lock X-Axis");
    mpXbSetLockCheckBox->setCheckable(true);
    mpYLSetLockCheckBox = new QCheckBox("Lock YL-Axis");
    mpYLSetLockCheckBox->setCheckable(true);
    mpYRSetLockCheckBox = new QCheckBox("Lock YR-Axis");
    mpYRSetLockCheckBox->setCheckable(true);

    mpXbSetLockCheckBox->setChecked(false);
    mpYLSetLockCheckBox->setChecked(false);
    mpYRSetLockCheckBox->setChecked(false);

    QLabel *pXminLabel = new QLabel("X Axis Min: ", mpSetAxisDialog);
    mpXminSpinBox = new QDoubleSpinBox(mpSetAxisDialog);
    mpXminSpinBox->setRange(-DBLMAX, DBLMAX);
    mpXminSpinBox->setDecimals(10);
    mpXminSpinBox->setSingleStep(0.1);

    QLabel *pXmaxLabel = new QLabel("X Axis Max: ", mpSetAxisDialog);
    mpXmaxSpinBox = new QDoubleSpinBox(mpSetAxisDialog);
    mpXmaxSpinBox->setRange(-DBLMAX, DBLMAX);
    mpXmaxSpinBox->setDecimals(10);
    mpXmaxSpinBox->setSingleStep(0.1);

    QLabel *pYLminLabel = new QLabel("YL Axis Min: ", mpSetAxisDialog);
    mpYLminSpinBox = new QDoubleSpinBox(mpSetAxisDialog);
    mpYLminSpinBox->setRange(-DBLMAX, DBLMAX);
    mpYLminSpinBox->setDecimals(10);
    mpYLminSpinBox->setSingleStep(0.1);

    QLabel *pYLmaxLabel = new QLabel("YL Axis Max: ", mpSetAxisDialog);
    mpYLmaxSpinBox = new QDoubleSpinBox(mpSetAxisDialog);
    mpYLmaxSpinBox->setRange(-DBLMAX, DBLMAX);
    mpYLmaxSpinBox->setDecimals(10);
    mpYLmaxSpinBox->setSingleStep(0.1);

    QLabel *pYRminLabel = new QLabel("YR Axis Min: ", mpSetAxisDialog);
    mpYRminSpinBox = new QDoubleSpinBox(mpSetAxisDialog);
    mpYRminSpinBox->setRange(-DBLMAX, DBLMAX);
    mpYRminSpinBox->setDecimals(10);
    mpYRminSpinBox->setSingleStep(0.1);

    QLabel *pYRmaxLabel = new QLabel("YR Axis Max: ", mpSetAxisDialog);
    mpYRmaxSpinBox = new QDoubleSpinBox(mpSetAxisDialog);
    mpYRmaxSpinBox->setRange(-DBLMAX, DBLMAX);
    mpYRmaxSpinBox->setDecimals(10);
    mpYRmaxSpinBox->setSingleStep(0.1);

    QPushButton *pFinishedButton = new QPushButton("Done", mpSetAxisDialog);
    QDialogButtonBox *pFinishedButtonBox = new QDialogButtonBox(Qt::Horizontal);
    pFinishedButtonBox->addButton(pFinishedButton, QDialogButtonBox::ActionRole);

    QGridLayout *pLockAxisDialogLayout = new QGridLayout(mpSetAxisDialog);

    pLockAxisDialogLayout->addWidget(pXminLabel,                0, 0);
    pLockAxisDialogLayout->addWidget(mpXminSpinBox,             0, 1);
    pLockAxisDialogLayout->addWidget(pXmaxLabel,                0, 2);
    pLockAxisDialogLayout->addWidget(mpXmaxSpinBox,             0, 3);
    pLockAxisDialogLayout->addWidget(mpXbSetLockCheckBox,         0, 4);
    pLockAxisDialogLayout->addWidget(pYLminLabel,                1, 0);
    pLockAxisDialogLayout->addWidget(mpYLminSpinBox,             1, 1);
    pLockAxisDialogLayout->addWidget(pYLmaxLabel,                1, 2);
    pLockAxisDialogLayout->addWidget(mpYLmaxSpinBox,             1, 3);
    pLockAxisDialogLayout->addWidget(mpYLSetLockCheckBox,         1, 4);
    pLockAxisDialogLayout->addWidget(pYRminLabel,                2, 0);
    pLockAxisDialogLayout->addWidget(mpYRminSpinBox,             2, 1);
    pLockAxisDialogLayout->addWidget(pYRmaxLabel,                2, 2);
    pLockAxisDialogLayout->addWidget(mpYRmaxSpinBox,             2, 3);
    pLockAxisDialogLayout->addWidget(mpYRSetLockCheckBox,         2, 4);
    //pLockAxisDialogLayout->addWidget(mpSetLockCheckBox,         6, 0, 1, 2);
    pLockAxisDialogLayout->addWidget(pFinishedButtonBox,       6, 1, 1, 2);
    //pLockAxisDialogLayout->addWidget(pNextButton,              3, 1, 1, 1);

    mpSetAxisDialog->setLayout(pLockAxisDialogLayout);

    connect(mpXminSpinBox, SIGNAL(valueChanged(double)), this, SLOT(applyAxisSettings()));
    connect(mpXmaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(applyAxisSettings()));
    connect(mpYLminSpinBox, SIGNAL(valueChanged(double)), this, SLOT(applyAxisSettings()));
    connect(mpYLmaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(applyAxisSettings()));
    connect(mpYRminSpinBox, SIGNAL(valueChanged(double)), this, SLOT(applyAxisSettings()));
    connect(mpYRmaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(applyAxisSettings()));
    connect(mpXbSetLockCheckBox, SIGNAL(toggled(bool)), this, SLOT(applyAxisSettings()));
    connect(mpYLSetLockCheckBox, SIGNAL(toggled(bool)), this, SLOT(applyAxisSettings()));
    connect(mpYRSetLockCheckBox, SIGNAL(toggled(bool)), this, SLOT(applyAxisSettings()));
    connect(pFinishedButton, SIGNAL(clicked()), mpSetAxisDialog, SLOT(close()));

}

//! @brief Help function to set legend symbole style
//! @todo allways sets for all curves, maybe should only set for one
void PlotTab::setLegendSymbol(const QString symStyle)
{
    for(int j=0; j<mPlotCurvePtrs[FIRSTPLOT].size(); ++j)
    {
        mPlotCurvePtrs[FIRSTPLOT].at(j)->getQwtPlotCurvePtr()->setLegendAttribute( HopQwtPlotCurve::LegendNoAttribute, false);
        mPlotCurvePtrs[FIRSTPLOT].at(j)->getQwtPlotCurvePtr()->setLegendAttribute( HopQwtPlotCurve::LegendShowLine, false);
        mPlotCurvePtrs[FIRSTPLOT].at(j)->getQwtPlotCurvePtr()->setLegendAttribute( HopQwtPlotCurve::LegendShowSymbol, false);
        mPlotCurvePtrs[FIRSTPLOT].at(j)->getQwtPlotCurvePtr()->setLegendAttribute( HopQwtPlotCurve::LegendShowBrush, false);

        if( symStyle == "Rectangle")
        {
            mPlotCurvePtrs[FIRSTPLOT].at(j)->getQwtPlotCurvePtr()->setLegendAttribute( HopQwtPlotCurve::LegendNoAttribute, true);
        }
        else if( symStyle == "Line")
        {
            mPlotCurvePtrs[FIRSTPLOT].at(j)->getQwtPlotCurvePtr()->setLegendAttribute( HopQwtPlotCurve::LegendShowLine, true);
        }
        else if( symStyle == "Default Symbol")
        {
            mPlotCurvePtrs[FIRSTPLOT].at(j)->getQwtPlotCurvePtr()->setLegendAttribute( HopQwtPlotCurve::LegendShowSymbol, true);
        }
        else if( symStyle == "Brush")
        {
            mPlotCurvePtrs[FIRSTPLOT].at(j)->getQwtPlotCurvePtr()->setLegendAttribute( HopQwtPlotCurve::LegendShowBrush, true);
        }
    }
}


//! @brief Defines what happens when used drags something into the plot window
void PlotTab::dragEnterEvent(QDragEnterEvent *event)
{
    //Don't accept drag events to FFT and Bode plots
    if(mPlotCurvePtrs[0].size() > 0 && mPlotCurvePtrs[0][0]->getCurveType() != PORTVARIABLE) return;

    if (event->mimeData()->hasText())
    {
        //Create the hover rectangle (size will be changed by dragMoveEvent)
        mpHoverRect = new QRubberBand(QRubberBand::Rectangle,this);
        mpHoverRect->setGeometry(0, 0, this->width(), this->height());
        mpHoverRect->setStyleSheet("selection-background-color: blue");
        mpHoverRect->setWindowOpacity(1);
        mpHoverRect->show();

        event->acceptProposedAction();
    }
}


//! @brief Defines what happens when user is dragging something in the plot window.
void PlotTab::dragMoveEvent(QDragMoveEvent *event)
{
    //Don't accept drag events to FFT and Bode plots
    if(mPlotCurvePtrs[0].size() > 0 && mPlotCurvePtrs[0][0]->getCurveType() != PORTVARIABLE) return;

    QCursor cursor;
    if(this->mapFromGlobal(cursor.pos()).y() > getPlot()->canvas()->height()/2+getPlot()->canvas()->y()+10 && getNumberOfCurves(FIRSTPLOT) >= 1)
    {
        mpHoverRect->setGeometry(getPlot()->canvas()->x()+9, getPlot()->canvas()->height()/2+getPlot()->canvas()->y()+10, getPlot()->canvas()->width(), getPlot()->canvas()->height()/2);
        mpParentPlotWindow->showHelpPopupMessage("Replace X-axis with selected variable.");
    }
    else if(this->mapFromGlobal(cursor.pos()).x() < getPlot()->canvas()->x()+9 + getPlot()->canvas()->width()/2)
    {
        mpHoverRect->setGeometry(getPlot()->canvas()->x()+9, getPlot()->canvas()->y()+9, getPlot()->canvas()->width()/2, getPlot()->canvas()->height());
        mpParentPlotWindow->showHelpPopupMessage("Add selected variable to left Y-axis.");
    }
    else
    {
        mpHoverRect->setGeometry(getPlot()->canvas()->x()+9 + getPlot()->canvas()->width()/2, getPlot()->canvas()->y()+9, getPlot()->canvas()->width()/2, getPlot()->canvas()->height());
        mpParentPlotWindow->showHelpPopupMessage("Add selected variable to right Y-axis.");
    }
    QWidget::dragMoveEvent(event);
}


//! @brief Defines what happens when user drags something out from the plot window.
void PlotTab::dragLeaveEvent(QDragLeaveEvent *event)
{
    //Don't accept drag events to FFT and Bode plots
    if(mPlotCurvePtrs[0].size() > 0 && mPlotCurvePtrs[0][0]->getCurveType() != PORTVARIABLE) return;

    delete(mpHoverRect);
    QWidget::dragLeaveEvent(event);
}


//! @brief Defines what happens when user drops something in the plot window
void PlotTab::dropEvent(QDropEvent *event)
{
    QWidget::dropEvent(event);

    //Don't accept drag events to FFT and Bode plots
    if(mPlotCurvePtrs[0].size() > 0 && mPlotCurvePtrs[0][0]->getCurveType() != PORTVARIABLE) return;

    if (event->mimeData()->hasText())
    {
        delete(mpHoverRect);

        QString mimeText = event->mimeData()->text();
        if(mimeText.startsWith("HOPSANPLOTDATA:"))
        {
            qDebug() << mimeText;
            mimeText.remove("HOPSANPLOTDATA:");

            QCursor cursor;
            //! @todo should not be half heigh should be slightly lower (Peters opinion)
            if(this->mapFromGlobal(cursor.pos()).y() > getPlot()->canvas()->height()/2+getPlot()->canvas()->y()+10 && getNumberOfCurves(FIRSTPLOT) >= 1)
            {
                VariableDescription desc = gpMainWindow->mpProjectTabs->getCurrentContainer()->getLogDataHandler()->getPlotData(mimeText, -1)->getVariableDescription();
                desc.mDataUnit = gConfig.getDefaultUnit(desc.mDataName);
                changeXVector(gpMainWindow->mpProjectTabs->getCurrentContainer()->getLogDataHandler()->getPlotDataValues(desc.getFullName(), -1), desc );
            }
            else if(this->mapFromGlobal(cursor.pos()).x() < getPlot()->canvas()->x()+9 + getPlot()->canvas()->width()/2)
            {
                mpParentPlotWindow->addPlotCurve(gpMainWindow->mpProjectTabs->getCurrentContainer()->getLogDataHandler()->getPlotData(mimeText, -1), QwtPlot::yLeft);
            }
            else
            {
                mpParentPlotWindow->addPlotCurve(gpMainWindow->mpProjectTabs->getCurrentContainer()->getLogDataHandler()->getPlotData(mimeText, -1), QwtPlot::yRight);
            }
        }
    }
}


//! @brief Handles the right-click menu in the plot tab
void PlotTab::contextMenuEvent(QContextMenuEvent *event)
{
    QWidget::contextMenuEvent(event);

    //   return;
    if(this->mpZoomer[FIRSTPLOT]->isEnabled())
    {
        return;
    }

    QMenu menu;

    QMenu *yAxisRightMenu;
    QMenu *yAxisLeftMenu;
    QMenu *changeUnitsMenu;
    QMenu *insertMarkerMenu;

    QAction *setRightAxisLogarithmic = 0;
    QAction *setLeftAxisLogarithmic = 0;


    yAxisLeftMenu = menu.addMenu(QString("Left Y Axis"));
    yAxisRightMenu = menu.addMenu(QString("Right Y Axis"));

    yAxisLeftMenu->setEnabled(mpQwtPlots[FIRSTPLOT]->axisEnabled(QwtPlot::yLeft));
    yAxisRightMenu->setEnabled(mpQwtPlots[FIRSTPLOT]->axisEnabled(QwtPlot::yRight));

    //Create menu and actions for changing units
    changeUnitsMenu = menu.addMenu(QString("Change Units"));
    QMap<QAction *, PlotCurve *> actionToCurveMap;
    QMap<QString, double> unitMap;
    QList<PlotCurve *>::iterator itc;
    QMap<QString, double>::iterator itu;
    for(itc=mPlotCurvePtrs[FIRSTPLOT].begin(); itc!=mPlotCurvePtrs[FIRSTPLOT].end(); ++itc)
    {
        QMenu *pTempMenu = changeUnitsMenu->addMenu(QString((*itc)->getComponentName() + ", " + (*itc)->getPortName() + ", " + (*itc)->getDataName()));
        unitMap = gConfig.getCustomUnits((*itc)->getDataName());
        for(itu=unitMap.begin(); itu!=unitMap.end(); ++itu)
        {
            QAction *pTempAction = pTempMenu->addAction(itu.key());
            actionToCurveMap.insert(pTempAction, (*itc));
        }
    }


    //Create actions for making axis logarithmic
    if(mpQwtPlots[FIRSTPLOT]->axisEnabled(QwtPlot::yLeft))
    {
        setLeftAxisLogarithmic = yAxisLeftMenu->addAction("Logarithmic Scale");
        setLeftAxisLogarithmic->setCheckable(true);
        setLeftAxisLogarithmic->setChecked(mLeftAxisLogarithmic);
    }
    if(mpQwtPlots[FIRSTPLOT]->axisEnabled(QwtPlot::yRight))
    {
        setRightAxisLogarithmic = yAxisRightMenu->addAction("Logarithmic Scale");
        setRightAxisLogarithmic->setCheckable(true);
        setRightAxisLogarithmic->setChecked(mRightAxisLogarithmic);
    }


    //Create menu for insereting curve markers
    insertMarkerMenu = menu.addMenu(QString("Insert Curve Marker"));
    for(int plotID=0; plotID<2; ++plotID)
    {
        for(itc=mPlotCurvePtrs[plotID].begin(); itc!=mPlotCurvePtrs[plotID].end(); ++itc)
        {
            QAction *pTempAction = insertMarkerMenu->addAction((*itc)->getCurveName());
            actionToCurveMap.insert(pTempAction, (*itc));
        }
    }



    // ----- Wait for user to make a selection ----- //

    QCursor *cursor;
    QAction *selectedAction = menu.exec(cursor->pos());

    // ----- User has selected something -----  //



    // Check if user did not click on a menu item
    if(selectedAction == 0)
    {
        return;
    }


    // Change unit on selected curve
    if(selectedAction->parentWidget()->parentWidget() == changeUnitsMenu)
    {
        actionToCurveMap.find(selectedAction).value()->setDataUnit(selectedAction->text());
    }


    //Make axis logarithmic
    if (selectedAction == setRightAxisLogarithmic)
    {
        mRightAxisLogarithmic = !mRightAxisLogarithmic;
        if(mRightAxisLogarithmic)
        {
            mpQwtPlots[FIRSTPLOT]->setAxisScaleEngine(QwtPlot::yRight, new QwtLogScaleEngine(10));
            rescaleToCurves();
            mpQwtPlots[FIRSTPLOT]->replot();
            mpQwtPlots[FIRSTPLOT]->updateGeometry();
        }
        else
        {
            mpQwtPlots[FIRSTPLOT]->setAxisScaleEngine(QwtPlot::yRight, new QwtLinearScaleEngine);
            rescaleToCurves();
            mpQwtPlots[FIRSTPLOT]->replot();
            mpQwtPlots[FIRSTPLOT]->updateGeometry();
        }
    }
    else if (selectedAction == setLeftAxisLogarithmic)
    {
        mLeftAxisLogarithmic = !mLeftAxisLogarithmic;
        if(mLeftAxisLogarithmic)
        {
            qDebug() << "Logarithmic!";
            mpQwtPlots[FIRSTPLOT]->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine(10));
            rescaleToCurves();
            mpQwtPlots[FIRSTPLOT]->replot();
            mpQwtPlots[FIRSTPLOT]->updateGeometry();
        }
        else
        {
            qDebug() << "Linear!";
            mpQwtPlots[FIRSTPLOT]->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
            rescaleToCurves();
            mpQwtPlots[FIRSTPLOT]->replot();
            mpQwtPlots[FIRSTPLOT]->updateGeometry();
        }
    }


    //Insert curve marker
    if(selectedAction->parentWidget() == insertMarkerMenu)
    {
        insertMarker(actionToCurveMap.find(selectedAction).value(), event->pos());
    }

}
