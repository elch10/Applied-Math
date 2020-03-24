#include "ILogger.h"
#include <set>
#include <memory>
#include <fstream>

namespace 
{
  class Logger: public ILogger
  {
  public:
    static Logger *createLogger(void* pClient);

    void destroyLogger(void* pClient) override;
    void log(char const* pMsg, enum RESULT_CODE err) override;
    RESULT_CODE setLogFile(char const* pLogFile) override;

    ~Logger() = default;
    static std::unique_ptr<Logger> singletonInstance_;
  private:
    std::ofstream file_;
    std::set<void*> clients_;
  };
}

ILogger::~ILogger() {}

ILogger* ILogger::createLogger(void* pClient)
{
  return Logger::createLogger(pClient);
}

void ILogger::destroyLogger(void* pClient)
{
  Logger::singletonInstance_->destroyLogger(pClient);
}

Logger *Logger::createLogger(void *pClient)
{
  if (!singletonInstance_) {
    singletonInstance_ = std::make_unique<Logger>();
  }
  if (!singletonInstance_->clients_.count(pClient)) {
    singletonInstance_->clients_.insert(pClient);
  }
  return singletonInstance_.get();
}

void Logger::destroyLogger(void* pClient)
{
  singletonInstance_->clients_.erase(pClient);
}

void Logger::log(char const* pMsg, enum RESULT_CODE err)
{
  file_ << pMsg << "\tError code: " << int(err) << std::endl;
}

RESULT_CODE Logger::setLogFile(char const* pLogFile)
{
  file_.open(pLogFile);
  if (!file_) {
    return RESULT_CODE::FILE_ERROR;
  }
  return RESULT_CODE::SUCCESS;
}

std::unique_ptr<Logger> Logger::singletonInstance_ = std::unique_ptr<Logger>(nullptr);
