#pragma once

#include "spdlog/sinks/base_sink.h"
#include "spdlog/common.h"
#include <cstdint>
#include <mutex>

#include "sys/slog2.h"

class slogger_sink : public spdlog::sinks::base_sink<std::mutex> {
public:
    slogger_sink(const std::string& buffer_set_name,
                 const std::string& buffer_name,
                 int num_pages = 1,
                 spdlog::level::level_enum verbosity = spdlog::level::trace) {
        config.buffer_set_name = buffer_set_name.c_str();
        config.verbosity_level = map_level(verbosity);
        config.num_buffers = 1;
        config.buffer_config[0].buffer_name = buffer_name.c_str();
        config.buffer_config[0].num_pages = num_pages;

        if (-1 == slog2_register(&config, &buffer, 0)) {
            buffer = nullptr;
        }
    }

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        if (buffer == nullptr) return;

        auto mapped_level = map_level(msg.level);
        slog2c(buffer, 0, mapped_level, msg.payload.data());
    }
    void flush_() override {
        // flush is not used
    }

private:
    static std::uint8_t map_level(spdlog::level::level_enum level) {
        switch (level) {
            case spdlog::level::trace:
                return SLOG2_DEBUG2;
            case spdlog::level::debug:
                return SLOG2_DEBUG1;
            case spdlog::level::info:
                return SLOG2_INFO;
            case spdlog::level::warn:
                return SLOG2_WARNING;
            case spdlog::level::err:
                return SLOG2_ERROR;
            case spdlog::level::critical:
                return SLOG2_CRITICAL;
            case spdlog::level::off:
                return SLOG2_SHUTDOWN;
            case spdlog::level::n_levels:
                return 0;
        }
    }

    slog2_buffer_set_config_t config{};
    slog2_buffer_t buffer{};
};
