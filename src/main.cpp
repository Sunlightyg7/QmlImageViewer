#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "ImageViewModel.h"
#include "ImageView.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    // 创建 QGuiApplication 对象
    QGuiApplication app(argc, argv);

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
