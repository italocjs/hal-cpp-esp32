#include <iostream>
#include <functional>
#include <queue>
#include <string>
#include <vector>
#include <thread>
#include <deque>

// Detect environment and include appropriate headers for sleep/delay
#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#elif defined(ESP32)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#elif defined(ARDUINO)
#include <Arduino.h>
#endif

// Italo_HAL class for sleep abstraction and task control
class Italo_HAL {
public:
  static std::vector<std::thread> tasks;
  static std::deque<bool> taskFlags;

  static void sleep(int milliseconds) {
    std::cout << "Sleeping for " << milliseconds << " milliseconds.\n";
#ifdef _WIN32
    Sleep(milliseconds);
#elif defined(__linux__)
    usleep(milliseconds * 1000);
#elif defined(ESP32)
    vTaskDelay(milliseconds / portTICK_PERIOD_MS);
#elif defined(ARDUINO)
    delay(milliseconds);
#endif
  }

  static void startTask(std::function<void(bool&)> func) {
    std::cout << "Starting task.\n";
    taskFlags.push_back(true);
    tasks.push_back(std::thread(func, std::ref(taskFlags.back())));
  }

  static void stopAllTasks() {
    std::cout << "Stopping all tasks.\n";
    for (auto& flag : taskFlags) {
      flag = false;
    }
    for (auto& t : tasks) {
      t.join();
    }
  }
};

std::vector<std::thread> Italo_HAL::tasks;
std::deque<bool> Italo_HAL::taskFlags;

// CommandProcessor class to process commands
class CommandProcessor {
public:
  std::queue<std::string> commandQueue;

  void processCommands() {
    std::cout << "Processing commands.\n";
    while (!commandQueue.empty()) {
      std::string command = commandQueue.front();
      commandQueue.pop();

      if (command == "ok" || command == "command") {
        std::cout << "Command found: " << command << '\n';
      } else {
        std::cout << "Command error\n";
      }
    }
  }
};

int main() {
  // Create tasks
  Italo_HAL::startTask([](bool& flag) {
    while (flag) {
      std::cout << "Task1 alive\n";
      Italo_HAL::sleep(1000);
    }
  });

  Italo_HAL::startTask([](bool& flag) {
    while (flag) {
      std::cout << "Task2 alive\n";
      Italo_HAL::sleep(2000);
    }
  });

  // Test sleep function
  Italo_HAL::sleep(5000);

  // Stop all tasks
  Italo_HAL::stopAllTasks();

  std::cout << "End of program.\n";

  return 0;
}