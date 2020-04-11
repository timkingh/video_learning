#pragma once

#include <QtWidgets/QDialog>
#include "ui_samp.h"

class samp : public QDialog
{
	Q_OBJECT

public:
	samp(QWidget *parent = Q_NULLPTR);

private:
	Ui::sampClass ui;
};
