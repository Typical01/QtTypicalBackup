#ifndef _JSON_TYPICAL_H
#define _JSON_TYPICAL_H

#include <TypicalTool/Tool/pch.h>
#include <Json/json.h>
#include <TypicalTool/Tool/Log.h>

namespace Typical_Tool {

	class TYPICALTOOL_API JsonManage {
	private:
		Json::Value Value;
		Json::StreamWriterBuilder WriterBuilder;
		Json::CharReaderBuilder ReaderBuilder;
		std::string JsonFilePath;

	public:
		JsonManage()
		{}
		//Json写入流配置: utf-8非英文字符显示 | 缩进 | 保存文件路径 | 读取并解析Json文件(return bool)
		JsonManage(const std::string& _JsonFilePath, bool _IsReadJsonFile = false)
		{
			this->Init(_JsonFilePath, _IsReadJsonFile);
		}

		//Json写入流配置: utf-8非英文字符显示 | 缩进 | 保存文件路径 | 读取并解析Json文件(return bool)
		template<typename T = bool>
		bool Init(const std::string& _JsonFilePath, bool _IsReadJsonFile)
		{
			this->WriterBuilder["emitUTF8"] = true; //utf-8字符显示为非 /uxxx
			this->WriterBuilder["indentation"] = "    "; // 设置缩进
			this->JsonFilePath = _JsonFilePath; //保存 Json文件路径

			if (_IsReadJsonFile) {
				return ReadJsonFile(_JsonFilePath);
			}
			return true;
		}
		template<typename T = bool>
		bool Init(const std::string& _JsonFilePath, bool _IsReadJsonFile, int32_t& _ErrorTips)
		{
			this->WriterBuilder["emitUTF8"] = true; //utf-8字符显示为非 /uxxx
			this->WriterBuilder["indentation"] = "    "; // 设置缩进
			this->JsonFilePath = _JsonFilePath; //保存 Json文件路径

			if (_IsReadJsonFile) {
				return ReadJsonFile(_JsonFilePath, _ErrorTips);
			}
			return true;
		}

	private:
		template<typename T = bool>
		bool WriteStream(const std::string& _JsonFilePath, Json::Value& _Value, std::ios::ios_base::openmode _StreamOpenMode)
		{
			std::ofstream JsonFileOut(_JsonFilePath, _StreamOpenMode);
			if (!JsonFileOut.is_open()) {
				std::error_code ec(errno, std::generic_category());
				std::string errorMsg = ec.message();
				LogDebug(Printf(TEXT("JsonManage::ReadStream: 打开Json文件失败: [%s]"), stow(errorMsg)), LogErr);
				LogDebug(Printf(TEXT("JsonManage::WriteStream: Json文件路径: [%s]"), stow(_JsonFilePath)), LogErr);
				return false;
			}
			JsonFileOut << Json::writeString(this->WriterBuilder, _Value);
			return true;
		}

		template<typename T = bool>
		bool ReadStream(const std::string& _JsonFilePath, Json::Value& _Value)
		{
			std::ifstream JsonFile(_JsonFilePath, std::ifstream::binary);
			std::string ErrorCode;
			if (!JsonFile.is_open()) {
				std::error_code ec(errno, std::generic_category());
				std::string errorMsg = ec.message();
				LogDebug(Printf(TEXT("JsonManage::ReadStream: 打开Json文件失败: [%s]"), stow(errorMsg)), LogErr);
				LogDebug(Printf(TEXT("JsonManage::ReadStream: Json文件路径: [%s]"), stow(_JsonFilePath)), LogErr);
				return false;
			}

			if (!Json::parseFromStream(this->ReaderBuilder, JsonFile, &_Value, &ErrorCode)) {
				LogDebug(Printf(TEXT("JsonManage::ReadStream: 解析 Json失败: [%s]"), stow(ErrorCode)), LogErr);
				LogDebug(Printf(TEXT("JsonManage::ReadStream: Json文件路径: [%s]"), stow(_JsonFilePath)), LogErr);
				return false;
			}
			return true;
		}

		template<typename T = bool>
		bool ReadStream(const std::string& _JsonFilePath, Json::Value& _Value, Tstr& _ErrorTips)
		{
			std::ifstream JsonFile(_JsonFilePath, std::ifstream::binary);

			std::string ErrorCode;
			if (!JsonFile.is_open()) {
				std::error_code ec(errno, std::generic_category());
				std::string errorMsg = ec.message();
				LogDebug(Printf(TEXT("JsonManage::ReadStream: 打开Json文件失败: [%s]"), stow(errorMsg)), LogErr);
				LogDebug(Printf(TEXT("JsonManage::ReadStream: Json文件路径: [%s]"), stow(_JsonFilePath)), LogErr);
				_ErrorTips = stow(errorMsg);

				return false;
			}

			if (!Json::parseFromStream(this->ReaderBuilder, JsonFile, &_Value, &ErrorCode)) {
				LogDebug(Printf(TEXT("JsonManage::ReadStream: 解析 Json失败: [%s]"), stow(ErrorCode)), LogErr);
				LogDebug(Printf(TEXT("JsonManage::ReadStream: Json文件路径: [%s]"), stow(_JsonFilePath)), LogErr);
				_ErrorTips = stow(ErrorCode);

				return false;
			}
			_ErrorTips = TEXT("Not Error");
			return true;
		}

	public:
		//读取 Json文件到 _Value
		template<typename T = bool>
		bool ReadJsonFile()
		{
			return ReadStream(this->JsonFilePath, this->Value);
		}
		//读取 Json文件到 _Value
		template<typename T = bool>
		bool ReadJsonFile(const std::string& _JsonFilePath)
		{
			return ReadStream(_JsonFilePath, this->Value);
		}
		template<typename T = bool>
		bool ReadJsonFile(const std::string& _JsonFilePath, int32_t& _ErrorTips)
		{
			return ReadStream(_JsonFilePath, this->Value, _ErrorTips);
		}
		//读取 Json文件到 _Value
		template<typename T = bool>
		bool ReadJsonFile(const std::string& _JsonFilePath, Json::Value& _JsonValue)
		{
			return ReadStream(_JsonFilePath, _JsonValue);
		}
		//写入 Value到 Json文件
		template<typename T = bool>
		bool WriteJsonFile(std::ios::ios_base::openmode _StreamOpenMode = std::ios::trunc)
		{
			return WriteStream(this->JsonFilePath, this->Value, _StreamOpenMode);
		}
		//写入 Value到 Json文件
		template<typename T = bool>
		bool WriteJsonFile(const std::string& _JsonFilePath, std::ios::ios_base::openmode _StreamOpenMode = std::ios::trunc)
		{
			return WriteStream(_JsonFilePath, this->Value, _StreamOpenMode);
		}
		//写入 _Value到 Json文件
		template<typename T = bool>
		bool WriteJsonFile(const std::string& _JsonFilePath, Json::Value& _Value, std::ios::ios_base::openmode _StreamOpenMode = std::ios::trunc)
		{
			return WriteStream(_JsonFilePath, _Value, _StreamOpenMode);
		}

	public:
		template<typename T = bool>
		Json::Value GetJsonValue() const
		{
			return this->Value;
		}
		template<typename T = bool>
		void SetJsonValue(Json::Value _Value)
		{
			this->Value = _Value;
		}
		template<typename T = bool>
		Json::StreamWriterBuilder GetWriterBuilder() const
		{
			return this->WriterBuilder;
		}
		template<typename T = bool>
		void SetWriterBuilder(Json::StreamWriterBuilder _WriterBuilder)
		{
			this->WriterBuilder = _WriterBuilder;
		}
		template<typename T = bool>
		Json::CharReaderBuilder GetReaderBuilder() const
		{
			return this->ReaderBuilder;
		}
		template<typename T = bool>
		void SetReaderBuilder(Json::CharReaderBuilder _ReaderBuilder)
		{
			this->ReaderBuilder = _ReaderBuilder;
		}
		template<typename T = bool>
		std::string GetJsonFilePath() const
		{
			return this->JsonFilePath;
		}
		template<typename T = bool>
		void SetJsonFilePath(std::string _JsonFilePath)
		{
			this->JsonFilePath = _JsonFilePath;
		}

	public:

		//输出 writeString到 Terr
		template<typename T = bool>
		void ToStreamString(int32_t _lm = LogLnf)
		{
			LogDebug(LogWar, Printf(TEXT("JsonManage::ToStreamString: JsonData to string: ")));
			LogDebug(stow(Json::writeString(this->WriterBuilder, this->Value)), _lm);
		}
	};
}






#endif