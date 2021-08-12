#pragma once

#ifndef RAZOR_VALUEPICKER_HPP
#define RAZOR_VALUEPICKER_HPP

#include <QDoubleSpinBox>

class ValuePicker : public QDoubleSpinBox {
public:
  explicit ValuePicker(QWidget* parent);

protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  bool m_leftClickPressed {};
  double m_initValue {};
  int m_initMousePos {};
};

#endif // RAZOR_VALUEPICKER_HPP
