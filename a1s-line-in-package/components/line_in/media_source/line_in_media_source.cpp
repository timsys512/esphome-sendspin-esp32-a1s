#include "line_in_media_source.h"
#include "esphome/core/log.h"

namespace esphome::line_in {

static const char *const TAG = "line_in.media_source";
static constexpr char URI_PREFIX[] = "line-in://";

void LineInMediaSource::setup() {
  if (this->microphone_ == nullptr) {
    ESP_LOGE(TAG, "No microphone configured");
    this->mark_failed();
    return;
  }

  this->microphone_->add_data_callback(
      [this](const std::vector<uint8_t> &data) { this->receive_audio_(data); });
}

void LineInMediaSource::dump_config() {
  ESP_LOGCONFIG(TAG, "Line-In Media Source:");
  ESP_LOGCONFIG(TAG, "  URI: line-in://line1");
  ESP_LOGCONFIG(TAG, "  Sample rate: %u Hz", this->sample_rate_);
  ESP_LOGCONFIG(TAG, "  Channels: %u", this->channels_);
  ESP_LOGCONFIG(TAG, "  Queue limit: %u bytes", static_cast<unsigned>(this->buffer_size_));
}

bool LineInMediaSource::can_handle(const std::string &uri) const {
  return uri.rfind(URI_PREFIX, 0) == 0;
}

bool LineInMediaSource::play_uri(const std::string &uri) {
  if (!this->can_handle(uri) || this->is_failed())
    return false;
  this->start_();
  return true;
}

void LineInMediaSource::handle_command(media_source::MediaSourceCommand command) {
  switch (command) {
    case media_source::MediaSourceCommand::PLAY:
      this->start_();
      break;
    case media_source::MediaSourceCommand::PAUSE:
      this->pause_();
      break;
    case media_source::MediaSourceCommand::STOP:
      this->stop_();
      break;
    default:
      break;
  }
}

void LineInMediaSource::start_() {
  if (this->active_ && !this->paused_)
    return;
  this->active_ = true;
  this->paused_ = false;
  this->clear_queue_();
  this->microphone_->start();
  this->set_state_(media_source::MediaSourceState::PLAYING);
  ESP_LOGI(TAG, "Line 1 capture started");
}

void LineInMediaSource::pause_() {
  if (!this->active_)
    return;
  this->paused_ = true;
  this->microphone_->stop();
  this->clear_queue_();
  this->set_state_(media_source::MediaSourceState::PAUSED);
}

void LineInMediaSource::stop_() {
  this->active_ = false;
  this->paused_ = false;
  this->microphone_->stop();
  this->clear_queue_();
  this->set_state_(media_source::MediaSourceState::IDLE);
  ESP_LOGI(TAG, "Line 1 capture stopped");
}

void LineInMediaSource::receive_audio_(const std::vector<uint8_t> &data) {
  if (!this->active_ || this->paused_ || data.empty())
    return;

  std::lock_guard<std::mutex> lock(this->queue_mutex_);
  while (!this->queue_.empty() && this->queued_bytes_ + data.size() > this->buffer_size_) {
    this->queued_bytes_ -= this->queue_.front().size();
    this->queue_.pop_front();
  }
  if (data.size() <= this->buffer_size_) {
    this->queue_.push_back(data);
    this->queued_bytes_ += data.size();
  }
}

void LineInMediaSource::loop() {
  if (!this->active_ || this->paused_ || !this->has_listener())
    return;

  std::vector<uint8_t> block;
  {
    std::lock_guard<std::mutex> lock(this->queue_mutex_);
    if (this->queue_.empty())
      return;
    block = std::move(this->queue_.front());
    this->queue_.pop_front();
    this->queued_bytes_ -= block.size();
  }

  const audio::AudioStreamInfo stream_info(16, this->channels_, this->sample_rate_);
  const size_t written = this->write_output(block.data(), block.size(), 20, stream_info);
  if (written < block.size()) {
    ESP_LOGV(TAG, "Dropped %u Line-In bytes", static_cast<unsigned>(block.size() - written));
  }
}

void LineInMediaSource::clear_queue_() {
  std::lock_guard<std::mutex> lock(this->queue_mutex_);
  this->queue_.clear();
  this->queued_bytes_ = 0;
}

}  // namespace esphome::line_in
