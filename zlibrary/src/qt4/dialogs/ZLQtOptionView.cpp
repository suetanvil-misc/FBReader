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

#include <cctype>

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QButtonGroup>
#include <QtGui/QRadioButton>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QLineEdit>
#include <QtGui/QSlider>
#include <QtGui/QLayout>

#include <ZLStringUtil.h>
#include "../util/ZLQtKeyUtil.h"

#include "ZLQtOptionView.h"
#include "ZLQtDialogContent.h"

void BooleanOptionView::_createItem() {
	myCheckBox = new QCheckBox(myOption->name().c_str(), myTab->widget());
	myCheckBox->setChecked(((ZLBooleanOptionEntry*)myOption)->initialState());
	myTab->addItem(myCheckBox, myRow, myFromColumn, myToColumn);
	connect(myCheckBox, SIGNAL(toggled(bool)), this, SLOT(onStateChanged(bool)));
}

void BooleanOptionView::_show() {
	myCheckBox->show();
}

void BooleanOptionView::_hide() {
	myCheckBox->hide();
}

void BooleanOptionView::_onAccept() const {
	((ZLBooleanOptionEntry*)myOption)->onAccept(myCheckBox->isChecked());
}

void BooleanOptionView::onStateChanged(bool state) const {
	((ZLBooleanOptionEntry*)myOption)->onStateChanged(state);
}

void ChoiceOptionView::_createItem() {
	//myGroup = new QWidget(myOption->name().c_str(), myTab->widget());
	myGroup = new QFrame(myTab->widget());
	((QFrame*)myGroup)->setFrameShape(QFrame::StyledPanel);
	QVBoxLayout *layout = new QVBoxLayout(myGroup);
	myButtons = new QRadioButton*[((ZLChoiceOptionEntry*)myOption)->choiceNumber()];
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); ++i) {
		myButtons[i] = new QRadioButton(myGroup);
		myButtons[i]->setText(((ZLChoiceOptionEntry*)myOption)->text(i).c_str());
		layout->addWidget(myButtons[i]);
	}
	myButtons[((ZLChoiceOptionEntry*)myOption)->initialCheckedIndex()]->setChecked(true);
	myTab->addItem(myGroup, myRow, myFromColumn, myToColumn);
}

void ChoiceOptionView::_show() {
	myGroup->show();
}

void ChoiceOptionView::_hide() {
	myGroup->hide();
}

void ChoiceOptionView::_onAccept() const {
	for (int i = 0; i < ((ZLChoiceOptionEntry*)myOption)->choiceNumber(); ++i) {
		if (myButtons[i]->isChecked()) {
			((ZLChoiceOptionEntry*)myOption)->onAccept(i);
			return;
		}
	}
}

void ComboOptionView::_createItem() {
	const ZLComboOptionEntry &comboOption = *(ZLComboOptionEntry*)myOption;
	myLabel = new QLabel(myOption->name().c_str(), myTab->widget());
	myComboBox = new QComboBox(myTab->widget());
	myComboBox->setEditable(comboOption.isEditable());

	connect(myComboBox, SIGNAL(activated(int)), this, SLOT(onValueSelected(int)));
	connect(myComboBox, SIGNAL(textChanged(const QString&)), this, SLOT(onValueEdited(const QString&)));

	int width = myToColumn - myFromColumn + 1;
	myTab->addItem(myLabel, myRow, myFromColumn, myFromColumn + width / 2 - 1);
	myTab->addItem(myComboBox, myRow, myToColumn - width / 2 + 1, myToColumn);

	reset();
}

void ComboOptionView::reset() {
	if (myComboBox == 0) {
		return;
	}

	const int count = myComboBox->count();
	for (int i = 0; i < count; ++i) {
		myComboBox->removeItem(0);
	}

	const ZLComboOptionEntry &comboOption = *(ZLComboOptionEntry*)myOption;
	const std::vector<std::string> &values = comboOption.values();
	const std::string &initial = comboOption.initialValue();
	int selectedIndex = -1;
	int index = 0;
	for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); ++it, ++index) {
		myComboBox->insertItem(it - values.begin(), QString::fromUtf8(it->c_str()));
		if (*it == initial) {
			selectedIndex = index;
		}
	}
	if (selectedIndex >= 0) {
		myComboBox->setCurrentIndex(selectedIndex);
	}
}

void ComboOptionView::onTabResized(const QSize &size) {
	myComboBox->setMaximumWidth(size.width() / 2 - 30);
}

void ComboOptionView::_show() {
	myLabel->show();
	myComboBox->show();
}

void ComboOptionView::_hide() {
	myLabel->hide();
	myComboBox->hide();
}

void ComboOptionView::_setActive(bool active) {
	myComboBox->setEnabled(active);
}

void ComboOptionView::_onAccept() const {
	((ZLComboOptionEntry*)myOption)->onAccept((const char*)myComboBox->currentText().toUtf8());
}

void ComboOptionView::onValueSelected(int index) {
	ZLComboOptionEntry *o = (ZLComboOptionEntry*)myOption;
	if ((index >= 0) && (index < (int)o->values().size())) {
		o->onValueSelected(index);
	}
}

void ComboOptionView::onValueEdited(const QString &value) {
	ZLComboOptionEntry &o = (ZLComboOptionEntry&)*myOption;
	if (o.useOnValueEdited()) {
		o.onValueEdited((const char*)value.toUtf8());
	}
}

void SpinOptionView::_createItem() {
	ZLSpinOptionEntry &entry = *(ZLSpinOptionEntry*)myOption;
	myLabel = new QLabel(myOption->name().c_str(), myTab->widget());
	mySpinBox = new QSpinBox(myTab->widget());
	mySpinBox->setMinimum(entry.minValue());
	mySpinBox->setMaximum(entry.maxValue());
	mySpinBox->setSingleStep(entry.step());
	mySpinBox->setValue(entry.initialValue());
	int width = myToColumn - myFromColumn + 1;
	myTab->addItem(myLabel, myRow, myFromColumn, myFromColumn + width * 2 / 3 - 1);
	myTab->addItem(mySpinBox, myRow, myFromColumn + width * 2 / 3, myToColumn);
}

void SpinOptionView::_show() {
	myLabel->show();
	mySpinBox->show();
}

void SpinOptionView::_hide() {
	myLabel->hide();
	mySpinBox->hide();
}

void SpinOptionView::_onAccept() const {
	((ZLSpinOptionEntry*)myOption)->onAccept(mySpinBox->value());
}

void StringOptionView::_createItem() {
	myLineEdit = new QLineEdit(myTab->widget());
	connect(myLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onValueEdited(const QString&)));
	if (!myOption->name().empty()) {
		myLabel = new QLabel(myOption->name().c_str(), myTab->widget());
		int width = myToColumn - myFromColumn + 1;
		myTab->addItem(myLabel, myRow, myFromColumn, myFromColumn + width / 4 - 1);
		myTab->addItem(myLineEdit, myRow, myFromColumn + width / 4, myToColumn);
	} else {
		myLabel = 0;
		myTab->addItem(myLineEdit, myRow, myFromColumn, myToColumn);
	}
	reset();
}

void StringOptionView::_show() {
	if (myLabel != 0) {
		myLabel->show();
	}
	myLineEdit->show();
}

void StringOptionView::_hide() {
	if (myLabel != 0) {
		myLabel->hide();
	}
	myLineEdit->hide();
}

void StringOptionView::_setActive(bool active) {
	myLineEdit->setReadOnly(!active);
}

void StringOptionView::_onAccept() const {
	((ZLStringOptionEntry*)myOption)->onAccept((const char*)myLineEdit->text().toUtf8());
}

void StringOptionView::reset() {
	if (myLineEdit == 0) {
		return;
	}
	myLineEdit->setText(QString::fromUtf8(((ZLStringOptionEntry*)myOption)->initialValue().c_str()));
	myLineEdit->cursorForward(false, -myLineEdit->text().length());
}

void StringOptionView::onValueEdited(const QString &value) {
	ZLStringOptionEntry &o = (ZLStringOptionEntry&)*myOption;
	if (o.useOnValueEdited()) {
		o.onValueEdited((const char*)value.toUtf8());
	}
}

class KeyButton : public QPushButton {

public:
	KeyButton(KeyOptionView &keyView);

protected:
	void focusInEvent(QFocusEvent*);
	void focusOutEvent(QFocusEvent*);
	void mousePressEvent(QMouseEvent *);
	void keyPressEvent(QKeyEvent *keyEvent);

private:
	KeyOptionView &myKeyView;
};

KeyButton::KeyButton(KeyOptionView &keyView) : QPushButton(keyView.myWidget), myKeyView(keyView) {
	focusOutEvent(0);
}

void KeyButton::focusInEvent(QFocusEvent*) {
	setText("Press key to set action");
	grabKeyboard();
}

void KeyButton::focusOutEvent(QFocusEvent*) {
	releaseKeyboard();
	setText("Press this button to select key");
}

void KeyButton::mousePressEvent(QMouseEvent*) {
	setFocus();
}

void KeyButton::keyPressEvent(QKeyEvent *keyEvent) {
	std::string keyText = ZLQtKeyUtil::keyName(keyEvent);
	if (!keyText.empty()) {
		myKeyView.myCurrentKey = keyText;
		myKeyView.myLabel->setText("Action For " + QString::fromUtf8(keyText.c_str()));
		myKeyView.myLabel->show();
		myKeyView.myComboBox->setCurrentIndex(((ZLKeyOptionEntry*)myKeyView.myOption)->actionIndex(keyText));
		myKeyView.myComboBox->show();
	}
}

void KeyOptionView::_createItem() {
	myWidget = new QWidget(myTab->widget());
	QGridLayout *layout = new QGridLayout(myWidget);
	myKeyButton = new KeyButton(*this);
	layout->addWidget(myKeyButton, 0, 0, 1, 2);
	myLabel = new QLabel(myWidget);
	myLabel->setTextFormat(Qt::PlainText);
	layout->addWidget(myLabel, 1, 0);
	myComboBox = new QComboBox(myWidget);
	const std::vector<std::string> &actions = ((ZLKeyOptionEntry*)myOption)->actionNames();
	for (std::vector<std::string>::const_iterator it = actions.begin(); it != actions.end(); ++it) {
		myComboBox->insertItem(it - actions.begin(), it->c_str());
	}
	connect(myComboBox, SIGNAL(activated(int)), this, SLOT(onValueChanged(int)));
	layout->addWidget(myComboBox, 1, 1);
	myTab->addItem(myWidget, myRow, myFromColumn, myToColumn);
}

void KeyOptionView::reset() {
	if (myWidget == 0) {
		return;
	}
	myCurrentKey.erase();
	myLabel->hide();
	myComboBox->hide();
}

void KeyOptionView::_show() {
	myWidget->show();
	myKeyButton->show();
	if (!myCurrentKey.empty()) {
		myLabel->show();
		myComboBox->show();
	} else {
		myLabel->hide();
		myComboBox->hide();
	}
}

void KeyOptionView::_hide() {
	myKeyButton->hide();
	myWidget->hide();
	myLabel->hide();
	myComboBox->hide();
	myCurrentKey.erase();
}

void KeyOptionView::_onAccept() const {
	((ZLKeyOptionEntry*)myOption)->onAccept();
}

void KeyOptionView::onValueChanged(int index) {
	if (!myCurrentKey.empty()) {
		((ZLKeyOptionEntry*)myOption)->onValueChanged(myCurrentKey, index);
	}
}

QSlider *ColorOptionView::createColorSlider(int index, const char *name, int value) {
	myLayout->addWidget(new QLabel(name, myWidget), index, 0);
	QSlider *slider = new QSlider(Qt::Horizontal, myWidget);
	myLayout->addWidget(slider, index, 1);
	slider->setMinimum(0);
	slider->setMaximum(255);
	slider->setSingleStep(5);
	slider->setTracking(true);
	slider->setValue(value);
	connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(onSliderMove(int)));
	return slider;
}

void ColorOptionView::_createItem() {
	myWidget = new QWidget(myTab->widget());
	myLayout = new QGridLayout(myWidget);
	const ZLColor &color = ((ZLColorOptionEntry*)myOption)->color();
	myRSlider = createColorSlider(0, "Red", color.Red);
	myGSlider = createColorSlider(1, "Green", color.Green);
	myBSlider = createColorSlider(2, "Blue", color.Blue);

	myColorBar = new QLabel("                  ", myWidget);
	QPalette palette = myColorBar->palette();
	palette.setColor(myColorBar->backgroundRole(), QColor(color.Red, color.Green, color.Blue));
	myColorBar->setPalette(palette);
	myColorBar->setFrameStyle(QFrame::Panel | QFrame::Plain);
	myLayout->addWidget(myColorBar, 0, 2, 3, 1);
	myColorBar->setAutoFillBackground(true);

	myTab->addItem(myWidget, myRow, myFromColumn, myToColumn);
}

void ColorOptionView::reset() {
	if (myWidget == 0) {
		return;
	}
	ZLColorOptionEntry &colorEntry = *(ZLColorOptionEntry*)myOption;
	colorEntry.onReset(ZLColor(myRSlider->value(), myGSlider->value(), myBSlider->value()));
	const ZLColor &color = colorEntry.color();
	myRSlider->setValue(color.Red);
	myGSlider->setValue(color.Green);
	myBSlider->setValue(color.Blue);
	QPalette palette = myColorBar->palette();
	palette.setColor(myColorBar->backgroundRole(), QColor(color.Red, color.Green, color.Blue));
	myColorBar->setPalette(palette);
}

void ColorOptionView::_show() {
	myWidget->show();
}

void ColorOptionView::_hide() {
	myWidget->hide();
}

void ColorOptionView::onSliderMove(int) {
	QPalette palette = myColorBar->palette();
	palette.setColor(myColorBar->backgroundRole(), QColor(myRSlider->value(), myGSlider->value(), myBSlider->value()));
	myColorBar->setPalette(palette);
}

void ColorOptionView::_onAccept() const {
	((ZLColorOptionEntry*)myOption)->onAccept(ZLColor(myRSlider->value(), myGSlider->value(), myBSlider->value()));
}