#pragma once

#include "esphome/components/audio/audio.h"
#include "esphome/components/media_source/media_source.h"
#include "esphome/components/microphone/microphone.h"
#include "esphome/core/component.h"

#include <deque>
#include <mutex>
#include <string>
#include <vector>

namespace esphome::line_in {

class LineInMediaSource : public Component, public media_source::MediaSource {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_microphone(microphone::Microphone *microphone) { this->microphone_ = microphone; }
  void set_sample_rate(uint32_t sample_rate) { this->sample_rate_ = sample_rate; }
  void set_channels(uint8_t channels) { this->channels_ = channels; }
  void set_buffer_size(size_t buffer_size) { this->buffer_size_ = buffer_size; }

  bool play_uri(const std::string &uri) override;
  void handle_command(media_source::MediaSourceCommand command) override;
  bool can_handle(const std::string &uri) const override;

 protected:
  void start_();
  void pause_();
  void stop_();
  void receive_audio_(const std::vector<uint8_t> &data);
  void clear_queue_();

  microphone::Microphone *microphone_{nullptr};
  uint32_t sample_rate_{48000};
  uint8_t channels_{2};
  size_t buffer_size_{32768};
  size_t queued_bytes_{0};
  bool active_{false};
  bool paused_{false};
  std::deque<std::vector<uint8_t>> queue_;
  std::mutex queue_mutex_;
};

}  // namespace esphome::line_in
