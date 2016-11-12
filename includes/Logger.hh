# pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <iostream>
#include <map>

class Logger {
public:
  Logger() = default;
  ~Logger();

/*
 * push any data type to the text
 *
 * @param data: data to hapen to the text
 * @param separator: character to write after the text
 */
  template <class T>
  void	push(const T &data, const std::string &file, char separator = '\n') {
    _logFiles[file].text << data;
    _logFiles[file].text << separator;
  }

  void	push(const std::vector<uint8_t> &data, const std::string &file, char separator = '\n');
  bool	log(const std::string &file);

private:

  struct LogFile {
    std::ostringstream	text;
    std::ofstream	file;
  };

  std::map<std::string, LogFile>	_logFiles;

  void	write(LogFile &logFile);
};
