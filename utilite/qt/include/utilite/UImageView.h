/*
 * ImageView.h
 *
 *  Created on: 2012-06-20
 *      Author: mathieu
 */

#ifndef IMAGEVIEW_H_
#define IMAGEVIEW_H_

#include <QtGui/QWidget>
#include <QtGui/QPainter>

class UImageView : public QWidget
{
	Q_OBJECT;
public:
	UImageView(QWidget * parent = 0) : QWidget(parent) {}
	~UImageView() {}
	void setBackgroundBrush(const QBrush & brush) {brush_ = brush;}

public slots:
	void setImage(const QImage & image)
	{
		if(pixmap_.width() != image.width() || pixmap_.height() != image.height())
		{
			this->setMinimumSize(image.width(), image.height());
			this->setGeometry(this->geometry().x(), this->geometry().y(), image.width(), image.height());
		}
		pixmap_ = QPixmap::fromImage(image);
		this->update();
	}

private:
	void computeScaleOffsets(float & scale, float & offsetX, float & offsetY)
	{
		scale = 1.0f;
		offsetX = 0.0f;
		offsetY = 0.0f;

		if(!pixmap_.isNull())
		{
			float w = pixmap_.width();
			float h = pixmap_.height();
			float widthRatio = float(this->rect().width()) / w;
			float heightRatio = float(this->rect().height()) / h;

			if(widthRatio < heightRatio)
			{
				scale = widthRatio;
			}
			else
			{
				scale = heightRatio;
			}

			w *= scale;
			h *= scale;

			if(w < this->rect().width())
			{
				offsetX = (this->rect().width() - w)/2.0f;
			}
			if(h < this->rect().height())
			{
				offsetY = (this->rect().height() - h)/2.0f;
			}
		}
	}

protected:
	virtual void paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		if(!pixmap_.isNull())
		{
			//Draw background
			painter.save();
			painter.setBrush(brush_);
			painter.drawRect(this->rect());
			painter.restore();

			painter.save();
			//Scale
			float ratio, offsetX, offsetY;
			this->computeScaleOffsets(ratio, offsetX, offsetY);
			painter.translate(offsetX, offsetY);
			painter.scale(ratio, ratio);
			painter.drawPixmap(QPoint(0,0), pixmap_);
			painter.restore();
		}
	}

private:
	QPixmap pixmap_;
	QBrush brush_;
};


#endif /* IMAGEVIEW_H_ */