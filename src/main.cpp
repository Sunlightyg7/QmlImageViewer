#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "ImageViewModel.h"
#include "ImageView.h"

int main(int argc, char* argv[])
{
	// 1. 全局缩放使能
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	// 2. 适配非整数倍缩放
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

	QGuiApplication app(argc, argv);

	// 3. 适配字体渲染 hinting
	QFont font = app.font();
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setHintingPreference(QFont::PreferFullHinting);
	QGuiApplication::setFont(font);

	// 创建 QML 引擎
	QQmlApplicationEngine engine;

	app.setOrganizationName("Some Company");
	app.setOrganizationDomain("somecompany.com");
	app.setApplicationName("Amazing Application");
	qmlRegisterType<ImageView>("io.root", 1, 0, "ImageView");
	qmlRegisterType<ImageViewModel>("io.root", 1, 0, "ImageViewModel");

	// 加载 QML 文件，显示界面
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

	// 如果 QML 文件没有成功加载，则退出
	if (engine.rootObjects().isEmpty())
		return -1;

	// 启动应用程序
	return app.exec();
}
