#include "threadbase.hpp"
#include "freertos/idf_additions.h"
#include <cassert>

namespace sw {

ThreadBase::ThreadBase(const Config config) : config_{config} {}

ThreadBase::~ThreadBase() { delete_; }

common::Error ThreadBase::start() {
  if (handle_) {
    resume_();
    return common::Error::OK;
  }

  BaseType_t ec = xTaskCreatePinnedToCore(
      taskFunction_, config_.name.data(), config_.stackDepth, this,
      static_cast<UBaseType_t>(config_.priority),
      reinterpret_cast<TaskHandle_t*>(&handle_),
      static_cast<UBaseType_t>(config_.coreId));

  return ec == pdPASS ? common::Error::OK : common::Error::FAIL;
}

common::Error ThreadBase::stop() {
  suspend_();
  return common::Error::OK;
}

void ThreadBase::cleanup_() { delete_; }

void ThreadBase::suspend_() {
  vTaskSuspend(static_cast<TaskHandle_t>(handle_));
}

void ThreadBase::resume_() { vTaskResume(static_cast<TaskHandle_t>(handle_)); }

void ThreadBase::resumeFromISR_() {
  xTaskResumeFromISR(static_cast<TaskHandle_t>(handle_));
}

void ThreadBase::delete_() {
  if (not handle_) {
    return;
  }

  vTaskDelete(static_cast<TaskHandle_t>(handle_));
  handle_ = nullptr;
}

void ThreadBase::taskFunction_(void* arg) {
  assert(arg);
  ThreadBase* thread = static_cast<ThreadBase*>(arg);
  thread->run_();
  thread->cleanup_();
}

} // namespace sw
