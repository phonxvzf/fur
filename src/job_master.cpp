#include "job_master.hpp"

void job_master::init(const math::vector2i& img_res, const math::vector2i& tile_size) {
  for (int y = 0; y < img_res.y; y += tile_size.y) {
    for (int x = 0; x < img_res.x; x += tile_size.x) {
      job_queue.push(job(
            { x, y },
            { std::min(x + tile_size.x, img_res.x), std::min(y + tile_size.y, img_res.y) }
            ));
    }
  }
}

bool job_master::get_job(job* j) {
  std::lock_guard<std::mutex> lock(job_queue_mutex);
  if (job_queue.empty()) return false;
  *j = job_queue.front();
  job_queue.pop();
  return true;
}
