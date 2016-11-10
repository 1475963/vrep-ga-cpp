# pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <iostream>

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
  void	push(const T &data, char separator = '\0') {
    _text << data;
    if (separator != '\0')
      _text << separator;
    std::cout << _text.str() << std::endl;
  }

  void push(const std::vector<uint8_t> &data, char separator = '\0');
  void newLine();
  bool log(const std::string &file);

private:
  std::ostringstream _text;
  std::string _currentFile;
  std::ofstream _logFile;
};
