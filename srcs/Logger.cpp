#include "Logger.hh"

/**
 * Logger destructor - close opened file.
 */
Logger::~Logger() {

  for (auto &logFile : _logFiles)
    if (logFile.second.file.is_open()) {
      write(logFile.second);
      logFile.second.file.close();
    }
}

/**
 * push a vector of uint8_t to the text
 *
 * @param data: vector to hapen to the text
 * @param separator: character to write after the text
 */
void	Logger::push(const std::vector<uint8_t> &data, const std::string &file, char separator) {
  for (uint d : data)
    push<uint>(d, file, '\0');
  push<char>(separator, file);
}

void	Logger::write(LogFile &logFile) {
  logFile.file << logFile.text.str();
  logFile.file.close();
  logFile.text.str("");
  logFile.text.clear();
}

/**
 * Save text to file
 *
 * @param file: file in which the text has to be saved
 * @return: whether the operation succeed
 */
bool Logger::log(const std::string &filePath) {
  if (_logFiles.find(filePath) == _logFiles.end())
    return false;
  LogFile &logFile = _logFiles[filePath];
  logFile.file.open(filePath, std::ofstream::out | std::ofstream::trunc);
  if (logFile.file.is_open() == false)
    return false;
  write(logFile);
  return true;
}
