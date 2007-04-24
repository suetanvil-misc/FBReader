/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <qpainter.h>
#include <qpixmap.h>
#include <qfontmetrics.h>
#include <qfontdatabase.h>
#include <qimage.h>

#include <ZLUnicodeUtil.h>
#include <ZLImage.h>

#include "ZLQtPaintContext.h"
#include "../../qt/image/ZLQtImageManager.h"
#ifdef USE_ZAURUS_FONTHACK
  #include "ZLZaurusFontHack.h"
#endif /* USE_ZAURUS_FONTHACK */

ZLQtPaintContext::ZLQtPaintContext() {
	myPainter = new QPainter();
	myPixmap = NULL;
	mySpaceWidth = -1;
	myDescent = 0;
	myFontIsStored = false;
}

ZLQtPaintContext::~ZLQtPaintContext() {
	if (myPixmap != NULL) {
		myPainter->end();
		delete myPixmap;
	}
	delete myPainter;
}

void ZLQtPaintContext::setSize(int w, int h) {
	if (myPixmap != NULL) {
		if ((myPixmap->width() != w) || (myPixmap->height() != h)) {
			myPainter->end();
			delete myPixmap;
			myPixmap = NULL;
		}
	}
	if ((myPixmap == NULL) && (w > 0) && (h > 0)) {
		myPixmap = new QPixmap(w, h);
		myPainter->begin(myPixmap);
		if (myFontIsStored) {
			myFontIsStored = false;
			setFont(myStoredFamily, myStoredSize, myStoredBold, myStoredItalic);
		}
	}
}

static const std::string HELVETICA = "helvetica";

void ZLQtPaintContext::fillFamiliesList(std::vector<std::string> &families) const {
	bool helveticaFlag = false;
#ifdef USE_ZAURUS_FONTHACK
	std::set<std::string> famSet = ZLZaurusFontHack::families();
	for (std::set<std::string>::const_iterator it = famSet.begin(); it != famSet.end(); ++it) {
		if (*it == HELVETICA) {
			helveticaFlag = true;
		}
		families.push_back(*it);
	}
#else /* USE_ZAURUS_FONTHACK */
	QFontDatabase db;
	QStringList qFamilies = db.families();
	for (QStringList::Iterator it = qFamilies.begin(); it != qFamilies.end(); ++it) {
		std::string family = (*it).ascii();
		if (family == HELVETICA) {
			helveticaFlag = true;
		}
		families.push_back(family);
	}
#endif /* USE_ZAURUS_FONTHACK */
	if (!helveticaFlag) {
		families.push_back(HELVETICA);
	}
}

const std::string ZLQtPaintContext::realFontFamilyName(std::string &fontFamily) const {
#ifdef USE_ZAURUS_FONTHACK
	std::string family = ZLUnicodeUtil::toLower(fontFamily);
	std::set<std::string> famSet = ZLZaurusFontHack::families();
	for (std::set<std::string>::const_iterator it = famSet.begin(); it != famSet.end(); ++it) {
		if (*it == family) {
			return family;
		}
	}
	return HELVETICA;
#else /* USE_ZAURUS_FONTHACK */
	QString fullName = QFontInfo(QFont(fontFamily.c_str())).family();
	if (fullName.isNull() || fullName.isEmpty()) {
		fullName = QFontInfo(QFont::defaultFont()).family();
		if (fullName.isNull() || fullName.isEmpty()) {
			return HELVETICA;
		}
	}
	return fullName.left(fullName.find(" [")).ascii();
#endif /* USE_ZAURUS_FONTHACK */
}

void ZLQtPaintContext::setFont(const std::string &family, int size, bool bold, bool italic) {
	if (myPainter->device() == 0) {
		myFontIsStored = true;
		myStoredFamily = family;
		myStoredSize = size;
		myStoredBold = bold;
		myStoredItalic= italic;
	} else {
		QFont font = myPainter->font();
		bool fontChanged = false;

		if (font.family() != family.c_str()) {
			font.setFamily(family.c_str());
			fontChanged = true;
		}

		if (font.pointSize() != size) {
			font.setPointSize(size);
			fontChanged = true;
		}

		if ((font.weight() != (bold ? QFont::Bold : QFont::Normal))) {
			font.setWeight(bold ? QFont::Bold : QFont::Normal);
			fontChanged = true;
		}

		if (font.italic() != italic) {
			font.setItalic(italic);
			fontChanged = true;
		}

		if (fontChanged) {
			myPainter->setFont(font);
			mySpaceWidth = -1;
			myDescent = myPainter->fontMetrics().descent();
		}
	}
}

void ZLQtPaintContext::setColor(ZLColor color, LineStyle style) {
	myPainter->setPen(QPen(
		QColor(color.Red, color.Green, color.Blue),
		1,
		(style == SOLID_LINE) ? QPainter::SolidLine : QPainter::DashLine
	));
}

void ZLQtPaintContext::setFillColor(ZLColor color, FillStyle style) {
	myPainter->setBrush(QBrush(
		QColor(color.Red, color.Green, color.Blue),
		(style == SOLID_FILL) ? QPainter::SolidPattern : QPainter::NoBrush
	));
}

int ZLQtPaintContext::stringWidth(const char *str, int len) const {
	return myPainter->fontMetrics().width(QString::fromUtf8(str, len));
}

int ZLQtPaintContext::spaceWidth() const {
	if (mySpaceWidth == -1) {
		mySpaceWidth = myPainter->fontMetrics().width(" ");
	}
	return mySpaceWidth;
}

int ZLQtPaintContext::descent() const {
	return myDescent;
}

int ZLQtPaintContext::stringHeight() const {
	return myPainter->font().pointSize() + 2;
}

void ZLQtPaintContext::drawString(int x, int y, const char *str, int len) {
	QString qStr = QString::fromUtf8(str, len);
	myPainter->drawText(x + leftMargin(), y + topMargin(), qStr);
}

void ZLQtPaintContext::drawImage(int x, int y, const ZLImageData &image) {
	const QImage &qImage = (ZLQtImageData&)image;
	myPainter->drawImage(x + leftMargin(), y + topMargin() - qImage.height(), qImage);
}

void ZLQtPaintContext::drawLine(int x0, int y0, int x1, int y1) {
	myPainter->drawLine(x0 + leftMargin(), y0 + topMargin(),
											x1 + leftMargin(), y1 + topMargin());
}

void ZLQtPaintContext::fillRectangle(int x0, int y0, int x1, int y1) {
	if (x1 < x0) {
		int tmp = x1;
		x1 = x0;
		x0 = tmp;
	}
	if (y1 < y0) {
		int tmp = y1;
		y1 = y0;
		y0 = tmp;
	}
	myPainter->fillRect(x0 + leftMargin(), y0 + topMargin(),
											x1 - x0 + 1, y1 - y0 + 1,
											myPainter->brush());
}

void ZLQtPaintContext::drawFilledCircle(int x, int y, int r) {
	x += leftMargin();
	y += topMargin();
	myPainter->drawEllipse(x - r, y - r, 2 * r + 1, 2 * r + 1);
}

void ZLQtPaintContext::clear(ZLColor color) {
	if (myPixmap != NULL) {
		myPixmap->fill(QColor(color.Red, color.Green, color.Blue));
	}
}

int ZLQtPaintContext::width() const {
	if (myPixmap == NULL) {
		return 0;
	}
	return myPixmap->width() - leftMargin() - rightMargin();
}

int ZLQtPaintContext::height() const {
	if (myPixmap == NULL) {
		return 0;
	}
	return myPixmap->height() - bottomMargin() - topMargin();
}
