#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTextCodec>
#include "ImageViewModel.h"
#include "ImageView.h"

/**
 * @brief 返回当前时间字符串，格式：2024-04-23-17.13.06
 *
 * @return 当前时间字符串
 */
inline std::string calcCurrTime()
{
	using namespace std::chrono;

	std::string strTime;
	char buffer[1024] = { 0 };

	// 保存记录时间
	system_clock::time_point now = system_clock::now();
	time_t tt = system_clock::to_time_t(now); //利用chrono库自带的方法转换为ctime中的time_t, 会导致精度降低
	// 格式化时间为：2024-04-23-17.13.06
	if (std::strftime(buffer, sizeof(buffer), "%F-%H.%M.%S", std::localtime(&tt)))
	{
		strTime = buffer;
	}

	return strTime;
}

#ifdef _WIN32
#include <windows.h>
#include <Dbghelp.h>

#pragma comment(lib, "Dbghelp.lib")

void CreateDumpFile(const char* lpstrDumpFilePathName, EXCEPTION_POINTERS* pException) {
	wchar_t wchar[2048] = { 0 };
	swprintf(wchar, 128, L"%S", lpstrDumpFilePathName);
	HANDLE hDumpFile = CreateFileW(wchar, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// Dump信息
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;
	// 写入Dump文件内容
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
	CloseHandle(hDumpFile);
}
// 处理Unhandled Exception的回调函数
LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException) {
	CreateDirectory("dump", NULL);
	auto time_str = calcCurrTime();
	CreateDumpFile(("./dump/" + time_str + ".dmp").c_str(), pException);
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

int main(int argc, char* argv[])
{
#ifdef _WIN32
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#endif

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

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
	QTextCodec* codec = QTextCodec::codecForName("gbk");
#else
	QTextCodec* codec = QTextCodec::codecForName("utf-8");
#endif
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForTr(codec);
#else
	QTextCodec* codec = QTextCodec::codecForName("utf-8");
	QTextCodec::setCodecForLocale(codec);
#endif

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
