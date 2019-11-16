#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtGui/QFontDatabase>

#include "zdbg/Driver.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

    QFontDatabase::addApplicationFont("qrc:/fonts/MajorMonoDisplay-Regular.ttf");
    QFontDatabase::addApplicationFont("qrc:/fonts/RobotoMono-Bold.ttf");
    QFontDatabase::addApplicationFont("qrc:/fonts/RobotoMono-BoldItalic.ttf");
    QFontDatabase::addApplicationFont("qrc:/fonts/RobotoMono-Italic.ttf");
    QFontDatabase::addApplicationFont("qrc:/fonts/RobotoMono-Light.ttf");
    QFontDatabase::addApplicationFont("qrc:/fonts/RobotoMono-LightItalic.ttf");
    QFontDatabase::addApplicationFont("qrc:/fonts/RobotoMono-Medium.ttf");
    QFontDatabase::addApplicationFont("qrc:/fonts/RobotoMono-MediumItalic.ttf");
    QFontDatabase::addApplicationFont("qrc:/fonts/RobotoMono-Regular.ttf");
    QFontDatabase::addApplicationFont("qrc:/fonts/RobotoMono-Thin.ttf");
    QFontDatabase::addApplicationFont("qrc:/fonts/RobotoMono-ThinItalic.ttf");

	QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/");

    zdbg::Driver driver;
    driver.setEngine(&engine);
    engine.rootContext()->setContextProperty("driver", &driver);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
					 &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);
	engine.load(url);

	return app.exec();
}
