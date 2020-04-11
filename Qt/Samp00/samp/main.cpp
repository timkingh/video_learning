#include "samp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	samp w;
	w.show();
	return a.exec();
}
