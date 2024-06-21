#pragma once

#include <spdlog/details/null_mutex.h>
#include <spdlog/details/synchronous_factory.h>
#include <spdlog/sinks/base_sink.h>
#include "spdlog/details/log_msg.h"

#include <sys/slog2.h>
#include <array>

namespace spdlog {
namespace sinks {

/**
 * Sink that creates a qnx slogger buffer and writes to it.
 */
template <typename Mutex>
class slog_sink : public base_sink<Mutex> {
public:
    explicit slog_sink(std::string name, std::string buffer_name, int num_pages)
        : buffer_config({}) {
        buffer_config.buffer_set_name = name.c_str();
        buffer_config.num_buffers = buffer_handle.size();
        buffer_config.verbosity_level = SLOG2_DEBUG2;
        buffer_config.buffer_config[0].buffer_name = buffer_name.c_str();
        buffer_config.buffer_config[0].num_pages = num_pages;

        slog2_register(&buffer_config, buffer_handle.data(), 0);
    }
    slog_sink(const slog_sink &) = delete;
    slog_sink &operator=(const slog_sink &) = delete;
    ~slog_sink() override = default;

protected:
    void sink_it(const details::log_msg &msg) override {}

    void flush_() override {}
    bool enable_formatting = false;

private:
    slog2_buffer_set_config_t buffer_config;

    std::array<slog2_buffer_t, 1> buffer_handle{};
};

}  // namespace sinks

}  // namespace spdlog
