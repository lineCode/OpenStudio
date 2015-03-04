/**********************************************************************
*  Copyright (c) 2008-2015, Alliance for Sustainable Energy.  
*  All rights reserved.
*  
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*  
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*  
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**********************************************************************/

#ifndef UTILITIES_PLOT_LINEPLOT_HPP
#define UTILITIES_PLOT_LINEPLOT_HPP

#include "../UtilitiesAPI.hpp"

#include "Plot2D.hpp"
#include "../data/TimeSeries.hpp"
#include "../data/Vector.hpp"
#include "../data/Matrix.hpp"
 
#include <QWidget>
#include <QPushButton>
#include <QAction>
#include <QColor>
 
#include <qwt/qwt_legend.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_legend_label.h>
#include <qwt/qwt_text.h>
#include <qwt/qwt_color_map.h>
#include <qwt/qwt_series_data.h>
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_plot_zoomer.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_layout.h>
#include <qwt/qwt_point_data.h>

#include <cmath>

namespace openstudio{

/** LinePlotData is abstract class for data that can be used in a line plot.
 *  Derive from this class to plot your data.
 *  \deprecated { Qwt drawing widgets are deprecated in favor of Javascript }
 */
class UTILITIES_API LinePlotData : public QwtSeriesData<QPointF>
{
public:

  /// virtual destructor
  virtual ~LinePlotData() {}

  /// must provide copy
  virtual LinePlotData* copy() const = 0;

  /// get the value at point x
// virtual double value(double x) const = 0;

  /// minX
  virtual double minX() const = 0;

  /// maxX
  virtual double maxX() const = 0;

  /// minY
  virtual double minY() const = 0;

  /// maxY
  virtual double maxY() const = 0;

  /// minValue
  virtual double minValue() const = 0;

  /// maxValue
  virtual double maxValue() const = 0;

  /// sumValue
  virtual double sumValue() const = 0;

  /// meanValue
  virtual double meanValue() const = 0;

  /// stdDevValue
  virtual double stdDevValue() const = 0;

  /// units for plotting on axes or scaling
  virtual void units(const std::string &unit) = 0;

  /// units for plotting on axes or scaling
  virtual std::string units() const = 0;

  virtual size_t size() const = 0;
  
  virtual QPointF sample(size_t i) const = 0;

  virtual QRectF boundingRect() const = 0;

protected:
  LinePlotData() {}
};

/** TimeSeriesLinePlotData converts a time series into Line plot data
*   \deprecated { Qwt drawing widgets are deprecated in favor of Javascript }
*/
class UTILITIES_API TimeSeriesLinePlotData: public LinePlotData
{
public:

  /// constructor
  TimeSeriesLinePlotData(TimeSeries timeSeries);
  
  /// constructor
  TimeSeriesLinePlotData(TimeSeries timeSeries, double fracDaysOffset);

  /// virtual destructor
  virtual ~TimeSeriesLinePlotData();

  /// must provide copy
  virtual TimeSeriesLinePlotData* copy() const;

  /// value at fractional day - used by qwtPlot engine
//  double value(double fractionalDay) const;

  /// minX
  double minX() const;

  /// maxX
  double maxX() const;

  /// minY
  double minY() const;

  /// maxY
  double maxY() const;

  /// minValue
  double minValue() const;

  /// maxValue
  double maxValue() const;

  /// sumValue
  double sumValue() const;

  /// meanValue
  double meanValue() const;

  /// stdDevValue
  double stdDevValue() const;

  /// reimplement bounding rect for speed
  QRectF boundingRect() const;

  /// reimplement sample
  QPointF sample(size_t i) const;

  /// reimplement abstract function size
  size_t size(void) const;

  /// reimplement abstract function x
  double x(size_t pos) const;

  /// reimplement abstract function y
  double y(size_t pos) const;

  /// units for plotting on axes or scaling
  void units(const std::string &unit);

  /// units for plotting on axes or scaling
  std::string units() const;

private:
  TimeSeries m_timeSeries;
  double m_minValue;
  double m_maxValue;
  double m_minX;
  double m_maxX;
  double m_minY;
  double m_maxY;
  size_t m_size;
  QRectF m_boundingRect;
  std::string m_units;
  double m_fracDaysOffset;
  // testing Vector class
  Vector m_x;
  Vector m_y;
};

/** VectorLinePlotData converts two Vectors into Line plot data
*   \deprecated { Qwt drawing widgets are deprecated in favor of Javascript }
*/
class UTILITIES_API VectorLinePlotData: public LinePlotData
{
public:

  /// constructor with x and y vectors
  VectorLinePlotData(const Vector& xVector, const Vector& yVector);

  /// virtual destructor
  virtual ~VectorLinePlotData() {}

  /// must provide copy
  virtual VectorLinePlotData* copy() const;

  /// get the value at point x
//  double value(double x) const;

  /// set the interp method, defaults to Nearest
  void interpMethod(InterpMethod interpMethod);

  /// minX
  double minX() const;

  /// maxX
  double maxX() const;

  /// minY
  double minY() const;

  /// maxY
  double maxY() const;

  /// minValue
  double minValue() const;

  /// maxValue
  double maxValue() const;

  /// sumValue
  double sumValue() const;

  /// meanValue
  double meanValue() const;

  /// stdDevValue
  double stdDevValue() const;

  /// reimplement bounding rect for speed
  QRectF boundingRect() const;

  /// reimplement sample
  QPointF sample(size_t i) const;

  /// reimplement abstract function size
  size_t size(void) const;

  /// reimplement abstract function x
  double x(size_t pos) const;

  /// reimplement abstract function y
  double y(size_t pos) const;

  /// units for plotting on axes or scaling
  void units(const std::string &unit);

  /// units for plotting on axes or scaling
  std::string units() const;

private:

  // set ranges and bounding box
  void init();

  Vector m_xVector;
  Vector m_yVector;
  InterpMethod m_interpMethod;
  double m_minX, m_maxX, m_minY, m_maxY;
  QRectF m_boundingRect;
  size_t m_size;
  std::string m_units;
};
 

/** Line plots data in a nice image 
*   \deprecated { Qwt drawing widgets are deprecated in favor of Javascript }
*/
class UTILITIES_API LinePlot : public Plot2D
{
  Q_OBJECT
public:

  /// constructor
  LinePlot(QWidget* parent = nullptr, Qt::WindowFlags flags = nullptr);

  /// virtual destructor
  virtual ~LinePlot();

  /// set the data, takes ownership of data and delete's it when not used any more
  void linePlotData(LinePlotData* data, const std::string& name, QColor color = Qt::blue, double offset = 0.0 );

  /// convenience wrapper for timeseries data
  void timeseriesData(TimeSeries tsData, const std::string& name, QColor color = Qt::blue );

  /// set left and right axes data
  void linePlotLeftRightAxesData(LinePlotData* leftAxisData, LinePlotData* rightAxisData, const std::string& leftName, const std::string& rightName, QColor leftColor = Qt::blue , QColor rightColor = Qt::green );

  /// update based on center and span
  void xCenterSpan(double center, double span);

  /// scaling function for more than 3 units
  void scaleCurves(QwtPlotCurve *curve);

  /// set thickness of all curves on a line plot
  void setLineThickness(int width);

  int lineThickness() const;

protected:

  /// drop target support for drag/drop operations
  void dragEnterEvent(QDragEnterEvent *e);

private:
  // grid 
  QwtPlotGrid *m_grid = nullptr;
  /// legend
  QwtLegend *m_legend = nullptr;
  double centerXValue(double center);
  double spanXValue(double span);
  QString m_leftAxisUnits;
  QString m_rightAxisUnits;
  void init();
  QColor m_lastColor; //for color tracking using curveColor
  std::vector<QColor> m_colorVec;
  QColor curveColor(QColor &lastColor);
  // for multiple curves
  double m_xAxisMin;
  double m_xAxisMax;
  Plot2DTimeAxis *m_plot2DTimeAxis = nullptr;
  DateTime m_startDateTime;
  DateTime m_endDateTime;
  double m_duration;

  // number of curves on current plot
  int numberOfCurves();

  // curve thickness for multiple lines trac #170
  int m_lineThickness;

  // set line based on color and number of curves and thickness trac #179
  QPen curvePen(QColor &color);
  
private slots:
  /// legend clicked event
  void showCurve(QwtPlotItem *, bool on);

};

} // openstudio

#endif // UTILITIES_PLOT_LINEPLOT_HPP
