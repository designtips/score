#pragma once
#include <Process/LayerView.hpp>
#include <Media/Step/Model.hpp>
#include <QGraphicsSceneMouseEvent>
#include <Automation/AutomationColors.hpp>
#include <QPainter>

namespace Media
{
namespace Step
{

class View final : public Process::LayerView
{
    Q_OBJECT
  public:
    explicit View(QGraphicsItem* parent)
      : Process::LayerView{parent}
    {
      setFlag(QGraphicsItem::ItemClipsToShape);
    }

    void setBarWidth(double v)
    {
      m_barWidth = v;
      update();
    }
    const Model* m_model{};

  signals:
    void pressed();
    void askContextMenu(const QPoint & t1, const QPointF & t2);
    void change(int, float);
    void released();

  private:
    void paint_impl(QPainter* p) const override
    {
      if(!m_model)
        return;

      if(m_barWidth > 2.)
      {
        p->setRenderHint(QPainter::Antialiasing, true);

        static QPen pen(score::Skin::instance().Tender3.color()); pen.setWidth(2.);
        static QBrush br(score::Skin::instance().Tender3.color().lighter());
        static QPen pen2(score::Skin::instance().Tender3.color().lighter()); pen.setWidth(2.);
        static QBrush br2(score::Skin::instance().Tender3.color().lighter().lighter());
        p->setPen(pen);

        const auto h = boundingRect().height();
        const auto w = boundingRect().width();
        const auto bar_w = m_barWidth;
        auto cur_pos = 0.;

        const auto& steps = m_model->steps();
        std::size_t i = 0;
        while(cur_pos < w)
        {
          auto idx = i % steps.size();
          auto step = steps[idx];
          p->fillRect(QRectF{cur_pos, step * h, (float)bar_w, h - step * h}, br);
          p->drawLine(QPointF{cur_pos, step * h}, QPointF{cur_pos + bar_w, step * h});

          cur_pos += bar_w;
          i++;
          if(i == steps.size())
          {
            break;
          }
        }

        // Now draw the echo
        p->setPen(pen2);
        while(cur_pos < w)
        {
          auto idx = i % steps.size();
          auto step = steps[idx];
          p->fillRect(QRectF{cur_pos, step * h, (float)bar_w, h - step * h}, br2);
          p->drawLine(QPointF{cur_pos, step * h}, QPointF{cur_pos + bar_w, step * h});

          cur_pos += bar_w;
          i++;
        }

        p->setRenderHint(QPainter::Antialiasing, false);
      }
      else
      {
        p->drawText(boundingRect(), Qt::AlignCenter, tr("Zoom in to edit steps"));
      }
    }

    void mousePressEvent(QGraphicsSceneMouseEvent* ev) override
    {
      if(!m_model)
        return;

      ev->accept();
      emit pressed();
      std::size_t pos = std::size_t(ev->pos().x() / m_barWidth) % m_model->steps().size();
      if(pos < m_model->steps().size())
      {
        emit change(pos, ev->pos().y() / boundingRect().height());
      }
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent* ev) override
    {
      if(!m_model)
        return;

      ev->accept();

      std::size_t pos = std::size_t(ev->pos().x() / m_barWidth) % m_model->steps().size();
      if(pos < m_model->steps().size())
      {
        emit change(pos, ev->pos().y() / boundingRect().height());
      }
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent* ev) override
    {
      ev->accept();
      emit released();
    }

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* ev) override
    {
      emit askContextMenu(ev->screenPos(), ev->scenePos());
      ev->accept();
    }

  private:
    double m_barWidth{};
};

}
}