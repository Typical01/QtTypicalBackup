#pragma once

//常用工具
#ifndef _COMMONTOOLS_H
#define _COMMONTOOLS_H

#include <TypicalTool/Tool/pch.h>
#include <TypicalTool/Tool/Log.h>
#include <TypicalTool/Tool/Time_Typical.h>



namespace Typical_Tool {

	enum FileSystemType {
		File = 0,
		Directory,
		Other
	};

	template<class T = bool>
	Tstr FileTypeToStr(const FileSystemType& _FileType) {
		switch (_FileType) {
		case FileSystemType::File:
		{
			return TEXT("文件");
		}
		case FileSystemType::Directory:
		{
			return TEXT("目录");
		}
		case FileSystemType::Other:
		{
			return TEXT("其他");
		}
		}
		return TEXT("");
	}

	class TYPICALTOOL_API FileSystem {
	private:
		std::filesystem::path Path;
		Tstr ErrorMessage;

	public:
		FileSystem()
		{

		}
		FileSystem(const std::filesystem::path& _Path)
			: Path(_Path)
		{
		}

		template<typename T = bool>
		FileSystem& operator/=(const Tstr& _PathName)
		{
			this->Path /= _PathName;
			return *this;
		}

	public:
		/*
		* _TipsChar: 提示文本
		* _Path: 检查的路径
		* _bShowRelease: 输出到 Release
		* _bFileTrue: 文件存在时为真, 否则取反
		*/
		template<typename T = bool>
		bool Exists(const Tstr& _TipsChar, const std::filesystem::path& _Path, bool _bShowRelease, FileSystemType& PathOfFileType, bool _bFileTrue = true)
		{
			//FileSystemType PathOfFileType;
			std::error_code ErrorCode;
			if (std::filesystem::exists(_Path, ErrorCode)) {
				if (std::filesystem::is_regular_file(_Path)) { // 检查路径是否是普通文件
					PathOfFileType = FileSystemType::File;
				}
				else if (std::filesystem::is_directory(_Path)) { // 检查路径是否是目录
					PathOfFileType = FileSystemType::Directory;
				}
				else {
					PathOfFileType = FileSystemType::Other;
				}

				if (_bShowRelease) {
					if (_bFileTrue) {
						LogRelease(LogErr, Printf(TEXT("%s: 路径[%s][%s] 存在!"), _TipsChar, _Path.PathToStr(), FileTypeToStr(PathOfFileType)));
					}
					else {
						LogRelease(LogTip, Printf(TEXT("%s: 路径[%s][%s] 存在!"), _TipsChar, _Path.PathToStr(), FileTypeToStr(PathOfFileType)));
					}
					return true;
				}
				else {
					if (_bFileTrue) {
						LogDebug(LogErr, Printf(TEXT("%s: 路径[%s][%s] 存在!"), _TipsChar, _Path.PathToStr(), FileTypeToStr(PathOfFileType)));
					}
					else {
						LogDebug(LogTip, Printf(TEXT("%s: 路径[%s][%s] 存在!"), _TipsChar, _Path.PathToStr(), FileTypeToStr(PathOfFileType)));
					}
					return true;
				}
			}
			else {
				if (!ErrorCode.message().empty()) {
					LogRelease(LogErr, Printf(TEXT("%s: 路径[%s]\n 异常[%s]!"), _TipsChar, _Path.PathToStr(), stow(ErrorCode.message().c_str())));
				}

				if (_bShowRelease) {
					if (_bFileTrue) {
						LogRelease(LogErr, Printf(TEXT("%s: 路径[%s][%s] 不存在!"), _TipsChar, _Path.PathToStr(), FileTypeToStr(PathOfFileType)));
					}
					else {
						LogRelease(LogTip, Printf(TEXT("%s: 路径[%s][%s] 不存在!"), _TipsChar, _Path.PathToStr(), FileTypeToStr(PathOfFileType)));
					}
					return false;
				}
				else {
					if (_bFileTrue) {
						LogDebug(LogErr, Printf(TEXT("%s: 路径[%s][%s] 不存在!"), _TipsChar, _Path.PathToStr(), FileTypeToStr(PathOfFileType)));
					}
					else {
						LogDebug(LogTip, Printf(TEXT("%s: 路径[%s][%s] 不存在!"), _TipsChar, _Path.PathToStr(), FileTypeToStr(PathOfFileType)));
					}
					return false;
				}
			}
		}
		template<typename T = bool>
		bool Exists(const Tstr& _TipsChar, const std::filesystem::path& _Path, bool _bShowRelease = true, bool _bFileTrue = true)
		{
			//FileSystemType PathOfFileType;
			std::error_code ErrorCode;
			if (std::filesystem::exists(_Path, ErrorCode)) {
				if (_bShowRelease) {
					if (_bFileTrue) {
						LogRelease(LogErr, Printf(TEXT("%s: 路径[%s] 存在!"), _TipsChar, _Path.PathToStr()));
					}
					else {
						LogRelease(LogTip, Printf(TEXT("%s: 路径[%s] 存在!"), _TipsChar, _Path.PathToStr()));
					}
					return true;
				}
				else {
					if (_bFileTrue) {
						LogDebug(LogErr, Printf(TEXT("%s: 路径[%s] 存在!"), _TipsChar, _Path.PathToStr()));
					}
					else {
						LogDebug(LogTip, Printf(TEXT("%s: 路径[%s] 存在!"), _TipsChar, _Path.PathToStr()));
					}
					return true;
				}
			}
			else {
				if (!ErrorCode.message().empty()) {
					LogRelease(LogErr, Printf(TEXT("%s: 路径[%s]\n 异常[%s]!"), _TipsChar, _Path.PathToStr(), stow(ErrorCode.message().c_str())));
				}

				if (_bShowRelease) {
					if (_bFileTrue) {
						LogRelease(LogErr, Printf(TEXT("%s: 路径[%s] 不存在!"), _TipsChar, _Path.PathToStr()));
					}
					else {
						LogRelease(LogTip, Printf(TEXT("%s: 路径[%s] 不存在!"), _TipsChar, _Path.PathToStr()));
					}
					return false;
				}
				else {
					if (_bFileTrue) {
						LogDebug(LogErr, Printf(TEXT("%s: 路径[%s] 不存在!"), _TipsChar, _Path.PathToStr()));
					}
					else {
						LogDebug(LogTip, Printf(TEXT("%s: 路径[%s] 不存在!"), _TipsChar, _Path.PathToStr()));
					}
					return false;
				}
			}
		}

	public:
		//创建目录: 自动递归创建下级目录
		template<typename T = bool>
		bool CreateDirectorys()
		{
			if (Exists(TEXT("创建文件夹"), this->Path, true)) {
				return false;
			}
			auto PathSlash = this->Path.PathToStr().find_last_of(TEXT("\\/"));

			std::error_code ErrorCode;
			try {
				if (PathSlash == Tstr::npos) {
					//单级目录
					std::filesystem::create_directory(this->Path);
				}
				else {
					//多级目录
					std::filesystem::create_directories(this->Path, ErrorCode);
					if (ErrorCode) {
						ErrorMessage += tytl::Printf(TEXT("{ 创建文件夹 }: %s"), ErrorCode.message());
						LogRelease(LogErr, Printf(TEXT("FileManage::Copy: 路径[%s]\n 异常[%s]!"), Path.PathToStr(), stow(ErrorCode.message().c_str())));
					}
				}
			}
			catch (const std::filesystem::filesystem_error& Error) {
				ErrorMessage = stow(Error.what());
				LogRelease(Printf(TEXT("FileSystem::CreateDirectory: 失败原因: { %s }"), ErrorMessage), LogErr);
				LogRelease(Printf(TEXT("FileSystem::CreateDirectory: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"), 
					Error.path1().PathToStr(), Error.path2().PathToStr()), LogWar);
				Exists(TEXT("创建文件夹(Path1)"), Error.path1(), true);
				Exists(TEXT("创建文件夹(Path2)"), Error.path2(), true);
				
				return false;
			}
			catch (...) {
				ErrorMessage = TEXT("FileSystem::CreateDirectory: 其他错误!");
				LogRelease(TEXT("FileSystem::CreateDirectory: 其他错误!"), LogErr);
				return false;
			}

			return !ErrorCode;
		}
		//删除文件/目录
		template<typename T = bool>
		bool Delete(bool _IsRecursive = false)
		{
			if (!Exists(TEXT("删除文件/目录"), this->Path, true)) {
				return false;
			}

			std::error_code ErrorCode;
			try {
				if (_IsRecursive) {
					std::filesystem::remove_all(this->Path, ErrorCode);
				}
				else {
					std::filesystem::remove(this->Path, ErrorCode);
				}
				if (ErrorCode) {
					ErrorMessage += tytl::Printf(TEXT("{ 删除文件/目录 }: %s"), ErrorCode.message());
					LogRelease(LogErr, Printf(TEXT("FileManage::Copy: 路径[%s]\n 异常[%s]!"), Path.PathToStr(), stow(ErrorCode.message().c_str())));
				}
			}
			catch (const std::filesystem::filesystem_error& Error) {
				ErrorMessage = stow(Error.what());
				LogRelease(Printf(TEXT("FileSystem::Delete: 失败原因: { %s }"), ErrorMessage), LogErr);
				LogRelease(Printf(TEXT("FileSystem::Delete: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
					Error.path1().PathToStr(), Error.path2().PathToStr()), LogWar);
				Exists(TEXT("删除文件/目录(Path1)"), Error.path1(), true);
				Exists(TEXT("删除文件/目录(Path2)"), Error.path2(), true);

				return false;
			}
			catch (...) {
				ErrorMessage = TEXT("FileSystem::Delete: 其他错误!");
				LogRelease(TEXT("FileSystem::Delete: 其他错误!"), LogErr);
				return false;
			}

			return !ErrorCode;
		}
		//复制文件/目录: _bOverWrite 覆盖原文件/目录
		template<typename T = bool>
		bool Copy(const Tstr& _TargetPath, std::filesystem::copy_options _copy_options = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive)
		{
			if (!Exists(TEXT("复制文件/目录"), this->Path, true) && !Exists(TEXT("复制文件/目录"), _TargetPath, true)) {
				return false;
			}
			std::error_code ErrorCode;

			try {
				std::filesystem::copy(this->Path, _TargetPath, _copy_options, ErrorCode);
				if (ErrorCode) {
					ErrorMessage += tytl::Printf(TEXT("{ 复制文件/目录 }: %s"), ErrorCode.message());
					LogRelease(LogErr, Printf(TEXT("FileManage::Copy: 路径[%s]->[%s]\n 异常[%s]!"), Path.PathToStr(), _TargetPath, stow(ErrorCode.message().c_str())));
				}
			}
			catch (const std::filesystem::filesystem_error& Error) {
				ErrorMessage = stow(Error.what());
				LogRelease(Printf(TEXT("FileSystem::Copy: 失败原因: { %s }"), ErrorMessage), LogErr);
				LogRelease(Printf(TEXT("FileSystem::Copy: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
					Error.path1().PathToStr(), Error.path2().PathToStr()), LogWar);
				Exists(TEXT("复制文件/目录(Path1)"), Error.path1(), true);
				Exists(TEXT("复制文件/目录(Path2)"), Error.path2(), true);

				return false;
			}
			catch (...) {
				ErrorMessage = TEXT("FileSystem::Copy: 其他错误!");
				LogRelease(TEXT("FileSystem::Copy: 其他错误!"), LogErr);
				return false;
			}

			return !ErrorCode;
		}
		//重命名/移动 文件/目录
		template<typename T = bool>
		bool ReName(const Tstr& _NewPathName)
		{
			std::error_code ErrorCode;
			if (this->Path.parent_path() == _NewPathName.parent_path()) { //同路径
				if (!Exists(TEXT("重命名文件/目录"), this->Path, true)) {
					return false;
				}

				try {
					std::filesystem::rename(this->Path, _NewPathName, ErrorCode);
					if (ErrorCode) {
						ErrorMessage += tytl::Printf(TEXT("{ 重命名文件/目录 }: %s"), ErrorCode.message());
						LogRelease(LogErr, Printf(TEXT("FileManage::ReName: 路径[%s]->[%s]\n 异常[%s]!"), Path.PathToStr(), _NewPathName, stow(ErrorCode.message().c_str())));
					}
				}
				catch (const std::filesystem::filesystem_error& Error) {
					ErrorMessage = stow(Error.what());
					LogRelease(Printf(TEXT("FileSystem::ReName: 失败原因: { %s }"), ErrorMessage), LogErr);
					LogRelease(Printf(TEXT("FileSystem::ReName: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
						Error.path1().PathToStr(), Error.path2().PathToStr()), LogWar);
					Exists(TEXT("重命名文件/目录(Path1)"), Error.path1(), true);
					Exists(TEXT("重命名文件/目录(Path2)"), Error.path2(), true);

					return false;
				}
				catch (...) {
					ErrorMessage = TEXT("FileSystem::ReName: 其他错误!");
					LogRelease(TEXT("FileSystem::ReName: 其他错误!"), LogErr);
					return false;
				}
			}
			else {
				if (!Exists(TEXT("移动文件/目录"), this->Path)) {
					return false;
				}

				try {
					std::filesystem::rename(this->Path, _NewPathName);
				}
				catch (const std::filesystem::filesystem_error& Error) {
					ErrorMessage = stow(Error.what());
					LogRelease(Printf(TEXT("FileSystem::ReName: 失败原因: { %s }"), ErrorMessage), LogErr);
					LogRelease(Printf(TEXT("FileSystem::ReName: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
						Error.path1().PathToStr(), Error.path2().PathToStr()), LogWar);
					Exists(TEXT("移动文件/目录(Path1)"), Error.path1(), true);
					Exists(TEXT("移动文件/目录(Path2)"), Error.path2(), true);

					return false;
				}
				catch (...) {
					ErrorMessage = TEXT("FileSystem::ReName: 其他错误!");
					LogRelease(TEXT("FileSystem::ReName: 其他错误!"), LogErr);
					return false;
				}
			}

			return !ErrorCode;
		}
		//修改文件/目录权限
		template<typename T = bool>
		bool SetPermissions(const std::filesystem::perms& _perms, std::filesystem::perm_options _perm_options = std::filesystem::perm_options::add)
		{
			if (!Exists(TEXT("修改文件/目录权限"), this->Path, true)) {
				return false;
			}

			std::error_code ErrorCode;
			try {
				std::filesystem::permissions(this->Path, _perms, _perm_options, ErrorCode);
				if (ErrorCode) {
					ErrorMessage += tytl::Printf(TEXT("{ 修改文件/目录权限 }: %s"), ErrorCode.message());
					LogRelease(LogErr, Printf(TEXT("FileManage::Copy: 路径[%s]\n 异常[%s]!"), Path.PathToStr(), stow(ErrorCode.message().c_str())));
				}
			}
			catch (const std::filesystem::filesystem_error& Error) {
				ErrorMessage = stow(Error.what());
				LogRelease(Printf(TEXT("FileSystem::SetPermissions: 失败原因: { %s }"), ErrorMessage), LogErr);
				LogRelease(Printf(TEXT("FileSystem::SetPermissions: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
					Error.path1().PathToStr(), Error.path2().PathToStr()), LogWar);
				Exists(TEXT("修改文件/目录权限(Path1)"), Error.path1(), true);
				Exists(TEXT("修改文件/目录权限(Path2)"), Error.path2(), true);

				return false;
			}
			catch (...) {
				ErrorMessage = TEXT("FileSystem::SetPermissions: 其他错误!");
				LogRelease(TEXT("FileSystem::SetPermissions: 其他错误!"), LogErr);
				return false;
			}

			return !ErrorCode;
		}

		//遍历目录: return 目录列表
		/* 
		* _IsRecursive: 是否递归
		* DirectoryList: 默认为空(不获取文件列表的字符串) | 非空(清楚内容后, 写入文件列表的字符串)
		*/
		template<typename T = bool>
		std::vector<std::filesystem::directory_entry> DirectoryIterator(bool _IsRecursive = false)
		{
			std::vector<std::filesystem::directory_entry> List; //目录列表
			std::error_code ErrorCode;

			try {
				if (_IsRecursive) { // 递归遍历
					for (const auto& entry : std::filesystem::recursive_directory_iterator(this->Path)) {
						List.push_back(entry);
					}
					if (ErrorCode) {
						ErrorMessage += tytl::Printf(TEXT("{ 遍历目录 }: %s"), ErrorCode.message());
						LogRelease(LogErr, Printf(TEXT("FileManage::DirectoryIterator: 路径[%s] 异常[%s]!"), Path.PathToStr(), stow(ErrorCode.message().c_str())));
					}
					return List;
				}
				else { // 非递归遍历
					for (const auto& entry : std::filesystem::directory_iterator(this->Path)) {
						List.push_back(entry);
					}
					if (ErrorCode) {
						ErrorMessage += tytl::Printf(TEXT("{ 遍历目录 }: %s"), ErrorCode.message());
						LogRelease(LogErr, Printf(TEXT("FileManage::DirectoryIterator: 路径[%s] 异常[%s]!"), Path.PathToStr(), stow(ErrorCode.message().c_str())));
					}
					return List;
				}
			}
			catch (const std::filesystem::filesystem_error& Error) {
				ErrorMessage = stow(Error.what());
				LogRelease(Printf(TEXT("FileSystem::DirectoryIterator: 失败原因: { %s }"), ErrorMessage), LogErr);
				LogRelease(Printf(TEXT("FileSystem::DirectoryIterator: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
					Error.path1().PathToStr(), Error.path2().PathToStr()), LogWar);
				Exists(TEXT("遍历目录(Path1)"), Error.path1(), true);
				Exists(TEXT("遍历目录(Path2)"), Error.path2(), true);

				return std::vector<std::filesystem::directory_entry>();
			}
			catch (...) {
				ErrorMessage = TEXT("FileSystem::DirectoryIterator: 其他错误!");
				LogRelease(TEXT("FileSystem::DirectoryIterator: 其他错误!"), LogErr);
				return std::vector<std::filesystem::directory_entry>();
			}
		}
		
		/// <summary>
		/// 遍历目录: return 目录列表
		/// </summary>
		/// <param name="_IsRecursive">: 是否递归</param>
		/// <param name="_FilePathSizeSum">: 文件路径总大小</param>
		/// <param name="DirectoryListStr">: 默认为空(不获取文件列表的字符串) | 非空(清楚内容后, 写入文件列表的字符串)</param>
		/// <returns></returns>
		template<typename T = bool>
		std::vector<std::filesystem::directory_entry> DirectoryIterator(bool _IsRecursive, unsigned __int64& _FilePathSizeSum)
		{
			std::vector<std::filesystem::directory_entry> List; //目录列表
			std::error_code ErrorCode;

			try {
				FileSystemType PathOfFileType;
				if (_IsRecursive) { // 递归遍历
					for (const auto& entry : std::filesystem::recursive_directory_iterator(this->Path, ErrorCode)) {
						if (!Exists(TEXT("遍历目录(递归)"), entry.path(), true, PathOfFileType)) {
							continue;
						}
						if (PathOfFileType == FileSystemType::File) {
							_FilePathSizeSum += std::filesystem::file_size(entry.path(), ErrorCode);
							if (ErrorCode) {
								ErrorMessage += tytl::Printf(TEXT("{ 遍历目录[子目录] }: %s"), ErrorCode.message());
								LogRelease(LogErr, Printf(TEXT("FileManage::DirectoryIterator: 路径[%s] 异常[%s]!"), Path.PathToStr(), stow(ErrorCode.message().c_str())));
							}
						}

						List.push_back(entry);
					}
					if (ErrorCode) {
						ErrorMessage += tytl::Printf(TEXT("{ 遍历目录 }: %s"), ErrorCode.message());
						LogRelease(LogErr, Printf(TEXT("FileManage::DirectoryIterator: 路径[%s] 异常[%s]!"), Path.PathToStr(), stow(ErrorCode.message().c_str())));
					}
					return List;
				}
				else { // 非递归遍历
					for (const auto& entry : std::filesystem::directory_iterator(this->Path, ErrorCode)) {
						if (!Exists(TEXT("遍历目录(不递归)"), entry.path(), true, PathOfFileType)) {
							continue;
						}
						if (PathOfFileType == FileSystemType::File) {
							_FilePathSizeSum += std::filesystem::file_size(entry.path(), ErrorCode);
							if (ErrorCode) {
								ErrorMessage += tytl::Printf(TEXT("{ 遍历目录[子目录] }: %s"), ErrorCode.message());
								LogRelease(LogErr, Printf(TEXT("FileManage::DirectoryIterator: 路径[%s] 异常[%s]!"), Path.PathToStr(), stow(ErrorCode.message().c_str())));
							}
						}

						List.push_back(entry);
					}
					if (ErrorCode) {
						ErrorMessage += tytl::Printf(TEXT("{ 遍历目录 }: %s"), ErrorCode.message());
						LogRelease(LogErr, Printf(TEXT("FileManage::DirectoryIterator: 路径[%s] 异常[%s]!"), Path.PathToStr(), stow(ErrorCode.message().c_str())));
					}
					return List;
				}
			}
			catch (const std::filesystem::filesystem_error& Error) {
				ErrorMessage = stow(Error.what());
				LogRelease(Printf(TEXT("FileSystem::DirectoryIterator: 失败原因: { %s }"), ErrorMessage), LogErr);
				LogRelease(Printf(TEXT("FileSystem::DirectoryIterator: 路径: { \n\tPath1: [%s]\n\tPath2: [%s]\n}"),
					Error.path1().PathToStr(), Error.path2().PathToStr()), LogWar);
				Exists(TEXT("遍历目录(Path1)"), Error.path1(), true);
				Exists(TEXT("遍历目录(Path2)"), Error.path2(), true);

				return std::vector< std::filesystem::directory_entry>();
			}
			catch (...) {
				ErrorMessage = TEXT("FileSystem::DirectoryIterator: 其他错误!");
				LogRelease(TEXT("FileSystem::DirectoryIterator: 其他错误!"), LogErr);
				return std::vector< std::filesystem::directory_entry>();
			}
		}

	public:
		//设置 std::filesystem::path
		template<typename T = bool>
		void SetPath(const std::filesystem::path& _Path) { this->Path = _Path; }

	public:
		//获取 当前工作路径: 可能变化
		template<typename T = bool>
	    static Tstr GetCurrentPath() {  return std::filesystem::current_path().PathToStr(); }
		//获取 std::filesystem::path
		template<typename T = bool>
		std::filesystem::path GetPath() { return this->Path; }
		//获取 错误信息
		template<typename T = bool>
		Tstr GetErrorMessage() { return this->ErrorMessage; }
		//获取 文件名
		template<typename T = bool>
		Tstr GetFileName() const { return this->Path.filename().PathToStr(); }
		//获取 文件扩展名
		template<typename T = bool>
		Tstr GetExtensionName() const { return this->Path.extension().PathToStr(); }
		//获取 父级目录名
		template<typename T = bool>
		Tstr GetParentName() const { return this->Path.parent_path().PathToStr(); }
	};



	class TYPICALTOOL_API ThreadPool {
	private:
		std::vector<std::thread> Workers;			// 工作线程
		std::queue<std::function<void()>> Tasks;	// 任务队列
		std::mutex QueueMutex;						// 保护任务队列的互斥锁
		std::condition_variable Condition;			// 条件变量，用于线程同步
		bool StopThread;							// 停止标志

	public:
		/// <summary>
		/// 构造函数，创建指定数量的工作线程
		/// </summary>
		/// <param name="_NumThreads">: 指定数量的工作线程</param>
		ThreadPool(size_t _NumThreads) : StopThread(false) 
		{
			for (size_t i = 0; i < _NumThreads; ++i) {
				Workers.emplace_back([this] {
					while (true) {
						std::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(QueueMutex);
							Condition.wait(lock, [this] { return StopThread || !Tasks.empty(); });
							if (StopThread && Tasks.empty()) return;
							task = std::move(Tasks.front());
							Tasks.pop();
						}
						task();
					}
					});
			}
		}

		/// <summary>
		/// 析构函数，停止线程池
		/// </summary>
		~ThreadPool() 
		{
			JoinAll();
		}

	public:
		// 提交任务到线程池
		template <class F, class... Args>
		auto Enqueue(F&& _Func, Args&&... _Args) -> std::future<typename std::invoke_result<F(Args...)>::type> 
		{
			using return_type = typename std::invoke_result<F(Args...)>::type;

			auto task = std::make_shared<std::packaged_task<return_type()>>(
				std::bind(std::forward<F>(_Func), std::forward<Args>(_Args)...)
			);

			std::future<return_type> res = task->get_future();
			{
				std::unique_lock<std::mutex> lock(QueueMutex);
				if (StopThread) {
					throw Truntime_error(_LOGERRORINFO(TEXT("tytl::ThreadPool: enqueue on stopped ThreadPool!")));
				}
				Tasks.emplace([task]() { (*task)(); });
			}
			Condition.notify_one();
			return res;
		}

		/*
		* 当前任务数量
		*/
		template<typename T = bool>
		int32_t WorkNumber() 
		{
			{
				std::unique_lock<std::mutex> lock(QueueMutex);
				return static_cast<int32_t>(Tasks.size());
			}
		}

		/*
		* 主动连接所有线程
		*/
		template<typename T = bool>
		void JoinAll() 
		{
			{
				std::unique_lock<std::mutex> lock(QueueMutex);
				StopThread = true;
			}
			Condition.notify_all();
			for (auto& tempThread : Workers) {
				if (tempThread.joinable()) {
					tempThread.join();
				}
			}
		}
	};
}

#endif