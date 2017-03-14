#include <fcntl.h>
#include <snsr.h>
#include <sys/stat.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <string>
#include <thread>

#include "alexa_wakeword.h"

const std::string ALEXA_TASK_VERSION = "~0.7.0";
const std::string MODEL_FILE = "/usr/local/resources/spot-alexa-rpi.snsr";

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

SnsrRC samplesReadySensoryCallback(SnsrSession s, const char* key,
                                   void* userData) {
  return SNSR_RC_OK;
}

SnsrRC wakeWordDetectedSensoryCallback(SnsrSession s, const char* key,
                                       void* userData) {
  std::cout << " *** Wakeword Detected ***" << std::endl;
  std::cout.flush();

  std::string alexa = "alexa";

  int named_pipe_handle = open("/tmp/wakeword_pipe", O_WRONLY | O_NONBLOCK);
  write(named_pipe_handle, alexa.c_str(), alexa.length());

  write(named_pipe_handle, "\n", 1);
  close(named_pipe_handle);

  return SNSR_RC_OK;
}

AlexaWakeWord::AlexaWakeWord() : session_{nullptr} {
  Setup();
  Start();
}

bool AlexaWakeWord::Setup() {
  if (session_) return false;

  SnsrRC result;

  result = snsrNew(&session_);

  if (result != SNSR_RC_OK) {
    std::cerr << "Could not allocation a new Sensory session: " +
                     getSensoryDetails(session_, result)
              << std::endl;
    return false;
  }

  const char* info = nullptr;
  result = snsrGetString(session_, SNSR_LICENSE_EXPIRES, &info);
  if (result == SNSR_RC_OK && info) {
    std::cerr << "Library expires on: " + std::string(info) << std::endl;
  } else {
    std::cerr << "Library does not expire" << std::endl;
  }

  if (snsrLoad(session_, snsrStreamFromFileName(MODEL_FILE.c_str(), "r")) !=
          SNSR_RC_OK ||
      snsrRequire(session_, SNSR_TASK_TYPE, SNSR_PHRASESPOT) != SNSR_RC_OK ||
      snsrRequire(session_, SNSR_TASK_VERSION, ALEXA_TASK_VERSION.c_str()) !=
          SNSR_RC_OK) {
    std::cerr << "Could not load and configure Sensory model: " +
                     getSensoryDetails(session_, result)
              << std::endl;
    return false;
  }

  result = snsrSetHandler(session_, SNSR_SAMPLES_EVENT,
                          snsrCallback(samplesReadySensoryCallback, nullptr,
                                       reinterpret_cast<void*>(this)));
  if (result != SNSR_RC_OK) {
    std::cerr << "Could not set audio samples callback: " +
                     getSensoryDetails(session_, result)
              << std::endl;
    return false;
  }

  result = snsrSetHandler(session_, SNSR_RESULT_EVENT,
                          snsrCallback(wakeWordDetectedSensoryCallback, nullptr,
                                       reinterpret_cast<void*>(this)));
  if (result != SNSR_RC_OK) {
    std::cerr << "Could not set wake word detected callback: " +
                     getSensoryDetails(session_, result)
              << std::endl;
    return false;
  }

  if (mkfifo("/tmp/wakeword_pipe", 0666) != 0) {
    if (errno == EEXIST) {
      // everytihng is OK
    } else {
      std::cerr << "Unable to create wake word fifo" << std::endl;
      return false;
    }
  }

  return true;
}

bool AlexaWakeWord::Start() {
  std::cout << "SensoryWakeWord: starting" << std::endl;

  if (snsrSetStream(session_, SNSR_SOURCE_AUDIO_PCM,
                    snsrStreamFromAudioDevice(SNSR_ST_AF_DEFAULT)) !=
      SNSR_RC_OK) {
    SnsrRC result;

    std::cerr << "Could not set audio stream from default mic: " +
                     getSensoryDetails(session_, result)
              << std::endl;
    return false;
  }
  thread_ = make_unique<std::thread>(&AlexaWakeWord::Loop, this);
  return true;
}

void AlexaWakeWord::Join() { thread_->join(); }

void AlexaWakeWord::Stop() {
  std::cout << " *** THREAD JOINING: Sensory ***" << std::endl;

  thread_->join();
  snsrClearRC(session_);
  snsrSetStream(session_, SNSR_SOURCE_AUDIO_PCM, nullptr);
}

void AlexaWakeWord::Loop() {
  std::cout << "SensoryWakeWord: mainLoop thread started" << std::endl;

  SnsrRC result = snsrRun(session_);
  if (result != SNSR_RC_OK && result != SNSR_RC_INTERRUPTED) {
    std::cerr << "An error happened in the mainLoop of SensoryWakeWord " +
                     getSensoryDetails(session_, result)
              << std::endl;
  }

  std::cout << "SensoryWakeWord: mainLoop thread ended" << std::endl;
}

std::string AlexaWakeWord::getSensoryDetails(SnsrSession session,
                                             SnsrRC result) {
  std::string message;

  if (session) {
    message = snsrErrorDetail(session);
  } else {
    message = snsrRCMessage(result);
  }
  if ("" == message) {
    message = "Unrecognized error";
  }
  return message;
}
