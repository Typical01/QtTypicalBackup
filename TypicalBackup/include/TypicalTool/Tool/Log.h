#pragma once

#ifndef _LOG_H
#define _LOG_H


#include <TypicalTool/Tool/pch.h>
#include <TypicalTool/Tool/Tchar_Typical.h>
#include <TypicalTool/Tool/StringManage.h>


namespace Typical_Tool
{
	using namespace StringManage;


	template <typename T, typename = void>
	struct is_streamable : std::false_type {};
	template <typename T>
	struct is_streamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type {};
	template <typename T>
	struct TypeCheck {
		static_assert(is_streamable<T>::value, "Printf: 参数类型不支持流输出");
	};

	
	class Printf {
	private:
		Tstr FormatStr;

	public:
		// 使用一个占位符（%s）来替换格式化字符串中的参数
		template<typename... Args>
		Printf(const Tstr& format, Args... args) 
		{
			std::vector<Tstr> placeholders = { format };
			FormatStr = format_impl(placeholders, args...);
		}

	private:
		// 基础模板：处理最后一个参数
		template<typename T = bool>
		Tstr format_impl(std::vector<Tstr>& placeholders) { return placeholders[0]; }
		// 递归模板：处理其他参数
		template<typename T, typename... Args>
		Tstr format_impl(std::vector<Tstr>& placeholders, T first, Args... args) 
		{
			//static_assert(is_streamable<T>::value, "Printf: 参数类型不支持流输出");
			TypeCheck<T>();

			Tstr formatted_string = placeholders[0];
			size_t pos = formatted_string.find(TEXT("%s")); // 查找第一个占位符

			if (pos != Tstr::npos) {
				Tostringstream oss;
				oss << first;  // 将第一个参数转换为字符串
				formatted_string.replace(pos, 2, oss.str()); // 替换占位符
			}
			placeholders[0] = formatted_string; // 递归替换剩余的占位符
			return format_impl(placeholders, args...);
		}

	public:
		// 获取格式化后的字符串
		template<typename T = bool>
		Tstr str() const { return FormatStr; }
		// 自动转换
		template<typename T = bool>
		operator Tstr() const { return str(); }
	};
	

#define LogMessage_Tips 0		//提示: Tips [INFO] | 绿色
#define LogMessage_Warning 1		//警告: Warning [WARRING] | 黄色
#define LogMessage_Error 2		//错误: Error [ERROR] | 红色
#define LogMessage_LineEnd 3		//结束: 不换行
#define LogMessage_LineFeed 4		//换行: '\n'
#define LogTip LogMessage_Tips
#define LogWar LogMessage_Warning 
#define LogErr LogMessage_Error 
#define LogEnd LogMessage_LineEnd 
#define LogLnf LogMessage_LineFeed


#ifndef MB_OK
#define MB_OK 0
#endif
#ifndef MB_ICONSTOP
#define MB_ICONSTOP 0
#endif
#ifndef MB_ICONWARNING
#define MB_ICONWARNING 0
#endif


	class LogStringBuffer {
	public:
		Tstr AnsiEsc_Color;
		Tstr TimeStr;
		Tstr Text;
		Tstr AnsiEsc_Reset;

	public:
		LogStringBuffer()
			: AnsiEsc_Color(TEXT("")), TimeStr(TEXT("")), Text(TEXT("")), AnsiEsc_Reset(TEXT("")) { }

	public:
		template<class T = bool>
		void SetString(const Tstr& _AnsiEsc_Color, const Tstr& _Text, const Tstr& _AnsiEsc_Reset)
		{
			AnsiEsc_Color = _AnsiEsc_Color;
			TimeStr = TEXT("");
			Text = _Text;
			AnsiEsc_Reset = _AnsiEsc_Reset;
		}
		template<class T = bool>
		void SetString(const Tstr& _AnsiEsc_Color, const Tstr& _TimeStr, const Tstr& _Text, const Tstr& _AnsiEsc_Reset)
		{
			AnsiEsc_Color = _AnsiEsc_Color;
			TimeStr = _TimeStr;
			Text = _Text;
			AnsiEsc_Reset = _AnsiEsc_Reset;
		}
	};
	template<class T = bool>
	Tostream& operator<<(Tostream& Ostream, const LogStringBuffer& LogStringBuffer) {
		return Ostream << LogStringBuffer.AnsiEsc_Color << LogStringBuffer.TimeStr
			<< LogStringBuffer.Text << LogStringBuffer.AnsiEsc_Reset;
	}


#ifndef UNICODE
#define LogOutOstream std::cout
#define LogErrOstream std::cerr
#else
#define LogOutOstream std::wcout
#define LogErrOstream std::wcerr
#endif


	template<class T = bool>
	class LogFile {
	public:
		HWND hConsole = nullptr; //控制台句柄

		std::shared_ptr<Tofstream> LogFileStreamOut;			//日志文件流 输出
		std::mutex MutexLogFileStreamOut;
		std::queue<Tstr> LogFileWriteQueue;					//日志 WriteConfigFile队列
		std::thread LogFileProcessing;						//日志文件处理: 主要是输出到[./Log/时间_程序名.txt]文件
		std::atomic<bool> bIsLogFileWriteThreadStop = false;	//是否 停止日志文件写入线程
	};


	/* 
	* 不需要控制台颜色显示: #undef _ANSIESC_CONSOLE_CHAR_COLOR
	* bDebug: 需要显示指定 | 显示 Log中的 bDebug消息
	* WIN32APP:	对于 Win32app 显示指定控制台分离
	* English: 标准库控制台的地区支持, 默认为中文
	*/
	class TYPICALTOOL_API Log {
	private:
		static std::shared_ptr<LogFile<>> LogFileInstance;

		//static HWND hConsole; //控制台句柄

		//static std::shared_ptr<Tofstream> LogFileStreamOut;			//日志文件流 输出
		//static std::mutex MutexLogFileStreamOut;
		//static std::queue<Tstr> LogFileWriteQueue;					//日志 WriteConfigFile队列
		//static std::thread LogFileProcessing;						//日志文件处理: 主要是输出到[./Log/时间_程序名.txt]文件
		//static std::atomic<bool> bIsLogFileWriteThreadStop;	//是否 停止日志文件写入线程

	private:
		static bool bLogInitialize;			//控制台初始化
		static bool bIsLogFileWrite;		//是否 写入日志文件
		static bool bIsLogAllOutput;		//是否 输出所有日志
		static bool bShowLog;				//显示日志
		static bool bShowTime;				//显示时间
		static bool bDebug;					//#define _Debug(或自定义的bDebug) (bDebug == true)

		bool bConsole;						//控制台
		bool bRelease;						//发行版

	public:
		Log(bool bConsole, bool bRelease)
			: bConsole(bConsole), bRelease(bRelease)
		{
			Initialize();
		}

		~Log()
		{
			StopLogWrite(); //当程序结束时, 退出 日志文件写入线程

			if (LogFileInstance != nullptr) {
				if (LogFileInstance->LogFileProcessing.joinable()) {
					LogFileInstance->LogFileProcessing.join();
				}
			}
		}

	public:
		/* 控制台初始化状态: false(需要初始化), true(跳过初始化)
		*/
		template<typename T = bool>
		void Initialize() {

			//控制台初始化
			if (!bLogInitialize) {
#ifdef WIN32APP
#ifdef _WINDOWS
				//Windows 控制台编码修改: UTF-8
				SetConsoleOutputCP(CP_UTF8);
				SetConsoleCP(CP_UTF8);
#ifndef _CONSOLE
				//分离控制台
				if (FreeConsole() == 0) {
					LogMessageBox(TEXT("Log::Initialize: 分离控制台失败!"), Log_Error, MB_ICONSTOP);
					LogMessageBox(Printf(TEXT("Log::Initialize: 错误代码: [%s]"), ToStr(GetLastError())).str().c_str(), Log_Error, MB_ICONSTOP);
				}
				//分配控制台: 当不是控制台程序时
				if (AllocConsole() == 0) {
					LogMessageBox(TEXT("Log::Initialize: 分配控制台失败!"), Log_Error, MB_ICONSTOP);
					LogMessageBox(Printf(TEXT("Log::Initialize: 错误代码: [%s]"), ToStr(GetLastError())).str().c_str(), Log_Error, MB_ICONSTOP);
				}

				// 获取标准输出流的句柄
				FILE* FilePtr;
				// 重定向标准输出流
				errno_t err = freopen_s(&FilePtr, "CONOUT$", "w", stdout);
				if (err != 0) {
					LogMessageBox(TEXT("Log::Initialize: 重定向标准输出流失败!"), Log_Error, MB_ICONSTOP);
					LogMessageBox(Printf(TEXT("Log::Initialize: 错误代码: [%s]"), err), Log_Error, MB_ICONSTOP);
					return;
				}
				// 重定向标准错误流
				err = freopen_s(&FilePtr, "CONOUT$", "w", stderr);
				if (err != 0) {
					LogMessageBox(TEXT("Log::Initialize: : 重定向标准错误流失败!"), Log_Error, MB_ICONSTOP);
					LogMessageBox(Printf(TEXT("Log::Initialize: 错误代码: [%s]"), err), Log_Error, MB_ICONSTOP);
					return;
				}
				err = freopen_s(&FilePtr, "CONIN$", "r", stdin);
				if (err != 0) {
					LogMessageBox(TEXT("Log::Initialize: : 重定向标准输入流失败!"), Log_Error, MB_ICONSTOP);
					LogMessageBox(Printf(TEXT("Log::Initialize: 错误代码: [%s]"), err), Log_Error, MB_ICONSTOP);
					return;
				}
				// 设置标准句柄
				HANDLE hConOut = CreateFile(TEXT("CONOUT$"), GENERIC_WRITE | GENERIC_READ,
					FILE_SHARE_WRITE | FILE_SHARE_READ,
					NULL, OPEN_EXISTING, 0, NULL);
				SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
				SetStdHandle(STD_ERROR_HANDLE, hConOut);

				HANDLE hConIn = CreateFile(TEXT("CONIN$"), GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL, OPEN_EXISTING, 0, NULL);
				SetStdHandle(STD_INPUT_HANDLE, hConIn);
#endif

				// 获取控制台窗口的句柄
				if (LogFileInstance != nullptr) {
					LogFileInstance->hConsole = GetConsoleWindow();
				}
				EnableAnsiEscape();

				//检查标准输出流
				if (!LogOutOstream.good() || !LogErrOstream.good()) {
					LogMessageBox(TEXT("Log::Initialize: LogOutOstream/LogErrOstream 无效!"), Log_Error, MB_ICONSTOP);
				}
				else {
					// 设置全局地区: 数字表示方法...
#ifdef English
					std::locale::global(std::locale("en_US.UTF-8"));
					Log_Out(ANSIESC_GREEN, LogErrOstream, Printf(TEXT("Log::Initialize: en_US.UTF-8 Successful!\n"), Log_Tips), ANSIESC_RESET, LogErr);
#else
					std::locale::global(std::locale("zh_CN.UTF-8"));
					Log_Out(ANSIESC_GREEN, LogErrOstream, Printf(TEXT("Log::Initialize: zh_CN.UTF-8 成功!\n"), Log_Tips), ANSIESC_RESET, LogErr);
#endif
					// 设置 cout/cerr 使用全局地区
					LogOutOstream.imbue(std::locale(""));
					LogErrOstream.imbue(std::locale(""));
				}
#endif
#else
				//检查标准输出流
				if (!LogOutOstream.good()) {
					LogMessageBox(TEXT("Log::Initialize: LogOutOstream 无效!"), Log_Error, MB_ICONSTOP);
					throw Truntime_error(TEXT("Log::Initialize: LogOutOstream Not good!"));
				}
				if (!LogErrOstream.good()) {
					LogMessageBox(TEXT("Log::Initialize: LogErrOstream 无效!"), Log_Error, MB_ICONSTOP);
					throw Truntime_error(TEXT("Log::Initialize: LogErrOstream Not good!"));
				}

				// 设置全局地区: 数字表示方法...
#ifdef English
				std::locale::global(std::locale("en_US.UTF-8"));
				Log_Out(ANSIESC_GREEN, LogErrOstream, Printf(TEXT("Log::Initialize: en_US.UTF-8 Successful!\n"), Log_Tips), ANSIESC_RESET, LogErr);
#else
				std::locale::global(std::locale("zh_CN.UTF-8"));
				Log_Out(ANSIESC_GREEN, LogErrOstream, Printf(TEXT("Log::Initialize: zh_CN.UTF-8 成功!\n"), Log_Tips), ANSIESC_RESET, LogErr);
#endif
				// 设置 cout/cerr 使用全局地区
				LogOutOstream.imbue(std::locale(""));
				LogErrOstream.imbue(std::locale(""));
#endif

				//完成初始化
				bLogInitialize = true;
			}
		}

	private:
		template<typename T = bool>
		static void Log_Out(const Tstr& _ANSIESC_Color, Tostream& _Ostream, const Tstr& _str, const Tstr& _ANSIESC_RESET, int32_t _LogOutputLevel = LogErr) 
		{
			if (!_Ostream.good()) {
				return;
			}
			
			LogStringBuffer tempLogBuffer;
			if (bShowTime) {
				tempLogBuffer.SetString(_ANSIESC_Color, Log::GetFormatTime(), _str, _ANSIESC_RESET);
			}
			else {
				tempLogBuffer.SetString(_ANSIESC_Color, _str, _ANSIESC_RESET);
			}
			_Ostream << tempLogBuffer;

			//WriteConfigFile log日志
			if (!bIsLogFileWrite) { return; }

			if (bIsLogAllOutput) { //所有级别
				LogWirte(tempLogBuffer);
			}
			else { //错误级别
				if (_LogOutputLevel == LogErr) {
					LogWirte(tempLogBuffer);
				}
			}
		}

		// _WINDOWS || _CONSOLE
		template<typename T = bool>
		void Logs_ustr(const Tstr& _Text, int32_t _Lm) {
			switch (_Lm) {
			case LogMessage_Tips: {
				if (bConsole) {
					Log_Out(ANSIESC_GREEN, LogOutOstream, Printf(TEXT("%s%s%s"), Log_Tips, _Text, Log_LineFeed), ANSIESC_RESET);
				}
				else {
					LogMessageBox(_Text, Log_Tips, MB_OK);
				}

				break;
			}
			case LogMessage_Warning: {
				if (bConsole) {
					Log_Out(ANSIESC_YELLOW, LogOutOstream, Printf(TEXT("%s%s%s"), Log_Warning, _Text, Log_LineFeed), ANSIESC_RESET);
				}
				else {
					LogMessageBox(_Text, Log_Warning, MB_ICONWARNING);
				}

				break;
			}
			case LogMessage_Error: {
				if (bConsole) {
					Log_Out(ANSIESC_RED, LogErrOstream, Printf(TEXT("%s%s%s"), Log_Error, _Text, Log_LineFeed), ANSIESC_RESET, LogErr);
				}
				else {
					LogMessageBox(_Text, Log_Error, MB_ICONSTOP);
				}

				break;
			}
			case LogMessage_LineEnd: {
				if (bConsole) {
					Log_Out(TEXT(""), LogOutOstream, Printf(TEXT("%s%s"), Log_Tab, _Text), TEXT(""));
					return;
				}
				else {
					LogMessageBox(_Text, Log_Text, MB_OK);
				}

				break;
			}
			case LogMessage_LineFeed: {
				if (bConsole) {
					Log_Out(TEXT(""), LogOutOstream, Printf(TEXT("%s%s%s"), Log_Tab, _Text, Log_LineFeed), TEXT(""));
					return;
				}
				else {
					LogMessageBox(_Text, Log_Text, MB_OK);
				}

				break;
			}
			}
		}
		void Logs_lgm()
		{
			if (bConsole) {
				Log_Out(TEXT(""), LogOutOstream, Log_LineFeed, TEXT(""));
				return;
			}
		}


	public:

		//显示/隐藏 Log消息
		template<class T = bool>
		void operator()(const Tstr& _Text, int32_t _Lm = LogLnf)
		{
			if (!bShowLog) { return; }

			if (bDebug) { // bDebug
				Logs_ustr(_Text, _Lm);
			}
			else { // bRelease
				if (bRelease) {
					Logs_ustr(_Text, _Lm);
				}
			}
		}
		template<class T = bool>
		void operator()(int32_t _Lm, const Tstr& _Text)
		{
			if (!bShowLog) { return; }

			if (bDebug) { // bDebug
				Logs_ustr(_Text, _Lm);
			}
			else { // bRelease
				if (bRelease) {
					Logs_ustr(_Text, _Lm);
				}
			}
		}
		/*
		* 输出 _LineNumber 个换行符
		*/
		void operator()(__int64 _LineNumber = 0)
		{
			if (!bShowLog) { return; }

			if (bDebug) { // bDebug
				for (auto i = 0; i < _LineNumber; i++) {
					Logs_lgm();
				}
			}
			else { // bRelease
				if (bRelease) {
					for (auto i = 0; i < _LineNumber; i++) {
						Logs_lgm();
					}
				}
			}
		}

	public:
		//设置 日志显示
		static void SetShowLog(bool _ShowLog) { bShowLog = _ShowLog; }
		//设置 时间显示
		static void SetShowTime(bool _ShowTime) { bShowTime = _ShowTime; }

		//设置 控制台显示: _WINDOWS(Win32 API)
		template<typename T = bool>
		static void SetShowConsole(bool _ShowConsole)
		{
#ifdef _WINDOWS
			if (LogFileInstance == nullptr) return;

			//显示/隐藏 窗口
			if (_ShowConsole) {
				ShowWindow(LogFileInstance->hConsole, SW_SHOWDEFAULT);
			}
			else {
				ShowWindow(LogFileInstance->hConsole, SW_HIDE);
			}
#else
			if (bDebug) {
				Log_Out(ANSIESC_RED, LogErrOstream, 
					Printf(TEXT("%s%s%s"), Log_Error, TEXT("%sLog::SetShowConsole: 显示控制台: 无效(#ifndef _WINDOWS)\n"), Log_LineFeed), ANSIESC_RESET, LogErr);
			}
#endif
		}
		//设置 bDebug 模式
		static void SetDebug(bool _bDebug) { bDebug = _bDebug; }
		//设置 日志文件写入
		static void SetLogFileWrite(bool _bIsLogFileWrite) { bIsLogFileWrite = _bIsLogFileWrite; }

		template<class T = bool>
		//设置 日志写入等级: !=LogMessage_Error(输出所有级别)
		static void SetLogWriteLevel(int32_t LogWriteLevel = LogMessage_Error)
		{
			//输出所有级别
			if (LogWriteLevel != LogMessage_Error) {
				bIsLogAllOutput = true;

				if (bDebug) {
					Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::SetLogWriteLevel: 文件输出级别[All]\n"), Log_Warning), ANSIESC_RESET, LogErr);
				}
			}
			else {
				bIsLogAllOutput = false;

				if (bDebug) {
					Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::SetLogWriteLevel: 文件输出级别[Error]\n"), Log_Warning), ANSIESC_RESET, LogErr);
				}
			}
		}

	public:

		/// <summary>
		/// 设置 日志文件写入的配置
		/// </summary>
		/// <param name="_LogFileName">: 日志文件名</param>
		/// <param name="_LogFolderPath">: 日志文件所在文件夹路径</param>
		/// <param name="bSingleLogFile">: 单日志文件</param>
		template<typename T = bool>
		void SetLogFileWriteConfig(const Tstr& _LogFileName, const Tstr& _LogFolderPath, bool _bSingleLogFile = true)
		{
			//获取 当前路径/Log/文件名_Log.txt 
			//创建文件夹 Log
			Tstr LogFilePath = _LogFileName + TEXT("_Log.txt");
			if (!_bSingleLogFile) {
				Tstr Log_FolderName = _LogFolderPath;
				if (Log_FolderName == TEXT("")) {
					Log_FolderName = TEXT("Log");
				}

				if (std::filesystem::exists(Log_FolderName)) { //路径存在
					if (std::filesystem::is_directory(Log_FolderName)) { // 是目录
						//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
						Tstr LogFileName_Time = Log::GetFormatTime(TEXT("%Y-%m-%d_%H-%M-%S_"), TEXT(""), TEXT(""));
						// ./Log/时间.txt  ||  ./时间.txt
						LogFilePath = Printf(TEXT("%s%s%s%s"), Log_FolderName, PATH_SLASH, LogFileName_Time, LogFilePath);
					}
					else { // 不是目录
					}
				}
				else { //目录不存在
					std::filesystem::create_directories(Log_FolderName); //创建目录
					//Log文件名: 格式化日期时间(年-月-日_时-分-秒) + .txt
					Tstr LogFileName_Time = Log::GetFormatTime(TEXT("%Y-%m-%d_%H-%M-%S_"), TEXT(""), TEXT(""));
					//Log/时间.txt  ||  ./时间.txt
					LogFilePath = Printf(TEXT("%s%s%s%s"), Log_FolderName, PATH_SLASH, LogFileName_Time, LogFilePath);
				}
			}
			else {
				Tstr Log_FolderName = _LogFolderPath;
				if (Log_FolderName == TEXT("")) {
					Log_FolderName = TEXT("Log");
				}

				if (!std::filesystem::exists(Log_FolderName)) { //路径不存在
					std::filesystem::create_directories(Log_FolderName); //创建目录
				}

				if (std::filesystem::is_directory(Log_FolderName)) { // 是目录
					LogFilePath = Printf(TEXT("%s%s%s"), Log_FolderName, PATH_SLASH, LogFilePath);
				}
			}
			Log_Out(ANSIESC_GREEN, LogErrOstream, Printf(TEXT("%sLog::SetLogFileWriteConfig: 日志输出文件名[%s]\n"), Log_Tips, LogFilePath), ANSIESC_RESET, LogErr);

			//打开文件
			bIsLogFileWrite = CreateLogFileManageThread(LogFilePath);
		}

		// 日志写入相关
	private:
		template<typename T = bool>
		static void StopLogWrite()
		{
			if (LogFileInstance == nullptr) {
				Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::StopLogWrite: LogFileInstance无效!\n"), Log_Warning), ANSIESC_RESET, LogErr);
				return;
			}

			if (!LogFileInstance->bIsLogFileWriteThreadStop.load()) {
				LogFileInstance->bIsLogFileWriteThreadStop.store(true); //退出 条件
				LogFileInstance->LogFileProcessing.join(); //主线程等待 后台退出
				Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::StopLogWrite: 日志写入完成! 程序退出...\n"), Log_Warning), ANSIESC_RESET, LogErr);
				//Time::wait_s<time::ms>(10); //主线程等待 后台退出
			}
		}

		template<typename T = bool>
		static void LogWirte(const LogStringBuffer& _LogBuffer)
		{
			if (LogFileInstance == nullptr) {
				Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::LogWirte: LogFileInstance无效!\n"), Log_Warning), ANSIESC_RESET, LogErr);
				return;
			}
			std::lock_guard<std::mutex> QueueGuard(LogFileInstance->MutexLogFileStreamOut);
			LogFileInstance->LogFileWriteQueue.push(_LogBuffer.TimeStr);
			LogFileInstance->LogFileWriteQueue.push(_LogBuffer.Text);
		}
		template<typename T = bool>
		bool CreateLogFileManageThread(const Tstr& _LogFilePath)
		{
			if (LogFileInstance == nullptr) {
				Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::CreateLogFileManageThread: LogFileInstance无效!\n"), Log_Warning), ANSIESC_RESET, LogErr);
				return false;
			}

			LogFileInstance->LogFileStreamOut = std::make_shared<Tofstream>(_LogFilePath, std::ios::out);

			if (!LogFileInstance->LogFileStreamOut->is_open()) {
				Log_Out(ANSIESC_RED, LogErrOstream, Printf(TEXT("%sLog::CreateLogFileManageThread: [%s]打开文件失败!\n"), Log_Error, _LogFilePath), ANSIESC_RESET, LogErr);
				return false;
			}
			Log_Out(ANSIESC_GREEN, LogErrOstream, Printf(TEXT("%sLog::CreateLogFileManageThread: [%s]打开文件成功!\n"), Log_Tips, _LogFilePath), ANSIESC_RESET, LogErr);

			//初始化: 日志WriteConfigFile线程
			std::thread LogWriteToFileThread([=]() {
				LogWriteToFile(_LogFilePath, LogFileInstance);
				});
			LogFileInstance->LogFileProcessing = std::move(LogWriteToFileThread);
			Log_Out(ANSIESC_GREEN, LogErrOstream, Printf(TEXT("%sLog::CreateLogFileManageThread: 日志写入线程 启动!\n"), Log_Tips), ANSIESC_RESET, LogErr);

			return true;
		}
				
		template<typename T = bool>
		void LogWriteToFile(const Tstr& _LogFilePath, std::shared_ptr<Typical_Tool::LogFile<>> _LogFileInstance)
		{
			try {
				// 不退出, 且队列不为空
				while (1) {
					bool bSleep = false;
					{
						if (_LogFileInstance == nullptr) {
							Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::LogWriteToFile: LogFileInstance无效!\n"), Log_Warning), ANSIESC_RESET, LogErr);
							return;
						}

						std::lock_guard<std::mutex> QueueGuard(_LogFileInstance->MutexLogFileStreamOut);

						//写入日志
						if (!_LogFileInstance->LogFileWriteQueue.empty()) {
							if (_LogFileInstance->LogFileStreamOut->good()) {
								*_LogFileInstance->LogFileStreamOut << _LogFileInstance->LogFileWriteQueue.front();
								_LogFileInstance->LogFileWriteQueue.pop();
							}
							else { //日志文件写入流出错
								_LogFileInstance->LogFileStreamOut->open(_LogFilePath, std::ios::app);
							}
						}
						else { //没有剩余日志写入
							if (_LogFileInstance->LogFileStreamOut->good()) {
								_LogFileInstance->LogFileStreamOut->close();
							}
							else { //文件流关闭后休眠, 减少cpu占用
								bSleep = true;
							}

							if (_LogFileInstance->bIsLogFileWriteThreadStop.load()) { //停止写入线程
								if (_LogFileInstance->LogFileStreamOut->good()) {
									_LogFileInstance->LogFileStreamOut->close();
								}
								break;
							}
						}
					}

					if (bSleep) {
						Log::Sleep(50, false);
						bSleep = false;
					}
				}
				Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::LogWriteToFile: 日志写入线程: 退出!\n"), Log_Warning), ANSIESC_RESET, LogErr);
			}
			catch (const std::exception& e) {
				Log_Out(ANSIESC_RED, LogErrOstream, Printf(TEXT("%sLog::LogWriteToFile: 日志写入线程: 异常[%s]!\n"), Log_Warning, u8tow(e.what())), ANSIESC_RESET, LogErr);
			}
			catch (...) {
				Log_Out(ANSIESC_RED, LogErrOstream, Printf(TEXT("%sLog::LogWriteToFile: 日志写入线程: 未知异常!\n"), Log_Warning), ANSIESC_RESET, LogErr);
			}
		}

	private:
		/*
		* 启用 ANSI转义符支持
		*/
		template<typename T = bool>
		void EnableAnsiEscape()
		{
#ifdef _WINDOWS
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			DWORD dwMode = 0;
			if (GetConsoleMode(hOut, &dwMode)) {
				dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				SetConsoleMode(hOut, dwMode);
			}
#endif
		}

		template<typename T = bool>
		void LogMessageBox(const Tstr& Text, const Tchar* _LogLevel, int32_t MessageBoxIcon = MB_OK)
		{
#ifdef _WINDOWS
			MessageBox(NULL, Text.c_str(), _LogLevel, MessageBoxIcon);
#endif
		}

	public:
		template<typename T = bool>
		static Tstr GetFormatTime(const Tstr& _TimeFormat = TEXT("%Y-%m-%d %H:%M:%S"),
			const Tstr& _TextLeftFormat = TEXT("["), const Tstr& _TextRigthFormat = TEXT("]"))
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();;
			// 获取当前时间点（自epoch以来的时间）
			// 将时间点转换为time_t（用于localtime函数）
			std::time_t ttm = std::chrono::system_clock::to_time_t(now);
			// 使用localtime函数将time_t转换为本地时间（std::tm结构）
			std::tm* now_tm = std::localtime(&ttm);
			// 使用 std::put_time 格式化时间
			Tostringstream oss;
			oss << std::put_time(now_tm, _TimeFormat.c_str()); // 自定义时间格式
			//不需要修饰字符时, 直接返回格式化后的时间文本
			if (_TextLeftFormat == TEXT("") && _TextRigthFormat == TEXT("")) {
				return oss.str();
			}
			else {
				return Printf(TEXT("%s%s%s"), _TextLeftFormat, oss.str(), _TextRigthFormat);
			}
		}

		template<typename T = bool>
		static void Sleep(int64_t _Number, bool _bShowLog)
		{
			if (_bShowLog) {
				Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::Sleep: [%s]ms\n"), Log_Warning, _Number).str(), ANSIESC_RESET, LogErr);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(_Number));
			if (_bShowLog) {
				Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::Sleep: 结束\n"), Log_Warning).str(), ANSIESC_RESET, LogErr);
			}
		}

		template<typename T = bool>
		static void Wait(int64_t _Number, bool _bShowLog)
		{
			if (_bShowLog) {
				Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::Wait: [%s]ms\n"), Log_Warning, _Number).str(), ANSIESC_RESET, LogErr);
			}
			std::chrono::milliseconds timeTarget = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()) +
				std::chrono::milliseconds(_Number);
			while (timeTarget > std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())) {}
			if (_bShowLog) {
				Log_Out(ANSIESC_YELLOW, LogErrOstream, Printf(TEXT("%sLog::Wait: 结束\n"), Log_Warning).str(), ANSIESC_RESET, LogErr);
			}
		}
	};


	/* 模式 mode : (_CONSOLE | _WINDOWS) && #ifdef _DEBUG
	* tip, war, LogErrOstream
	*/
	TYPICALTOOL_API extern std::shared_ptr<Log> lg;
	/* 模式 mode: (_CONSOLE) && #ifdef _DEBUG
	* tip, war, LogErrOstream
	*/
	TYPICALTOOL_API extern std::shared_ptr<Log> lgc;
	/* 模式 mode: (_CONSOLE | _WINDOWS) && #ifndef _DEBUG
	* tip, war, LogErrOstream
	*/
	TYPICALTOOL_API extern std::shared_ptr<Log> lgr;
	/* 模式 mode: (_CONSOLE) && #ifndef _DEBUG
	* tip, war, LogErrOstream
	*/
	TYPICALTOOL_API extern std::shared_ptr<Log> lgcr;

#define LogDebug (*lgc)
#define LogRelease (*lgcr)
}


#endif
