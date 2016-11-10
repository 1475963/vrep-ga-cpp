#include "Logger.hh"

/*
** Logger destructor - close opened file.
*/
Logger::~Logger() {
  if (_logFile.is_open())
    _logFile.close();
}

/*
** push a vector of uint8_t to the text
**
** @param data: vector to hapen to the text
** @param separator: character to write after the text
*/
void	Logger::push(const std::vector<uint8_t> &data, char separator) {
  for (uint d : data)
    push<uint>(d);
  if (separator != '\0')
    push<char>(separator);
}

/*
** Add one newline to saved text
*/
void	Logger::newLine() {
  _text << '\n';
}

/*
** Save texxt to file
**
** @param file: file in which the text has to be saved
** @return: whether the operation succeed
*/
bool Logger::log(const std::string &file) {
  if (_currentFile != file) {
    if (_logFile.is_open())
      _logFile.close();
    _currentFile = file;
    _logFile.open(_currentFile, std::ofstream::out | std::ofstream::trunc);
    if (!_logFile.is_open())
      return false;
  }
  _logFile << _text.str();
  _text.str("");
  _text.clear();
  return true;
}
