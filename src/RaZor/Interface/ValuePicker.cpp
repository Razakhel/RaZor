#include "RaZor/Interface/ValuePicker.hpp"

#include <QMouseEvent>

ValuePicker::ValuePicker(QWidget* parent) : QDoubleSpinBox(parent) {
  setAccelerated(true);
  setCursor(Qt::CursorShape::SizeHorCursor);

  setDecimals(3);
  setFixedWidth(70);
  setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
  setFont(QFont(font().family(), 7, QFont::Weight::Bold));
}

void ValuePicker::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::MouseButton::LeftButton) {
    m_leftClickPressed = true;
    m_initMousePos     = event->pos().x();
    m_initValue        = value();
  }

  QDoubleSpinBox::mousePressEvent(event);
}

void ValuePicker::mouseMoveEvent(QMouseEvent* event) {
  if (m_leftClickPressed) {
    const int moveDiff = event->pos().x() - m_initMousePos;
    setValue(m_initValue + moveDiff / 50.0);
  }

  QDoubleSpinBox::mouseMoveEvent(event);
}

void ValuePicker::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::MouseButton::LeftButton)
    m_leftClickPressed = false;

  QDoubleSpinBox::mouseReleaseEvent(event);
}
