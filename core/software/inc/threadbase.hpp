/**
 * Copyright 2025
 *
 * This class was created based on the FreeRTOS Add-ons project:
 * https://github.com/michaelbecker/freertos-addons
 *
 * Adjustments and modifications were made to fit the specific needs
 * of this project.
 */

/****************************************************************************
 *
 *  Copyright (c) 2023, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS Add-ons project.
 *
 *  Source Code:
 *  https://github.com/michaelbecker/freertos-addons
 *
 *  Project Page:
 *  http://michaelbecker.github.io/freertos-addons/
 *
 *  On-line Documentation:
 *  http://michaelbecker.github.io/freertos-addons/docs/html/index.html
 *
 *  MIT License
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files
 *  (the "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so,subject to the
 *  following conditions:
 *
 *  + The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *  + Credit is appreciated, but not required, if you find this project
 *    useful enough to include in your application, product, device, etc.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************/

#pragma once

#include <cstdint>
#include <string_view>

#include "types.hpp"

namespace sw {
namespace thread {

/**
 * @brief Core identifier
 */
enum class CoreId : uint8_t { _0, _1 };
}; // namespace thread

/**
 * @brief Base class for a thread
 */
class ThreadBase {
  public:
    /**
     * @brief Thread configuration
     */
    struct Config {
        std::string_view name;
        uint32_t stackDepth;
        unsigned int priority;
        thread::CoreId coreId;
    };

    /**
     * @brief ThreadBase constructor
     *
     * @param config Thread configuration
     */
    explicit ThreadBase(const Config config);

    /**
     * @brief ThreadBase destructor
     */
    virtual ~ThreadBase();

    /**
     * @brief Starts the thread
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error start();

    /**
     * @brief Stops the thread
     *
     * @return
     *   - common::Error::OK: Success.
     */
    common::Error stop();

  protected:
    using ThreadHandle = void*;

    /**
     * @brief Main function of the thread, must be implemented in the derived
     * class
     */
    virtual void run_() = 0;

    /**
     * @brief Main function of the thread, must be implemented in the derived
     * class
     *
     * This function must be an infinite loop to ensure continuous execution.
     * Example implementation:
     *
     * @code
     * void run_() override {
     *     while (1) {
     *         // Thread execution logic
     *     }
     * }
     * @endcode
     */
    virtual void cleanup_();

    /**
     * @brief Suspends the thread execution
     */
    void suspend_();

    /**
     * @brief Resumes the thread execution
     */
    void resume_();

    /**
     * @brief Resumes the thread execution from an ISR
     */
    void resumeFromISR_();

    /**
     * @brief Deletes the thread
     */
    void delete_();

  private:
    static void taskFunction_(void* arg);

    Config config_;
    ThreadHandle handle_{nullptr};
};
} // namespace sw
