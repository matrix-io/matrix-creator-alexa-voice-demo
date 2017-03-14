#include <snsr.h>

#include <string>
#include <thread>

class AlexaWakeWord {
 public:
  AlexaWakeWord();

  void Join();

 private:
  bool Setup();
  void Stop();
  bool Start();
  void Loop();

  std::string getSensoryDetails(SnsrSession session, SnsrRC result);
  std::unique_ptr<std::thread> thread_;
  SnsrSession session_;
};
