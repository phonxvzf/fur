#include "job_master.hpp"

void job_master::init(const bounds2i& bounds, const vector2i& tile_size) {
  for (int y = bounds.p_min.y; y < bounds.p_max.y; y += tile_size.y) {
    for (int x = bounds.p_min.x; x < bounds.p_max.x; x += tile_size.x) {
      job_queue.push(job(
            {
              { x, y },
              {
                std::min(x + tile_size.x, bounds.p_max.x),
                std::min(y + tile_size.y, bounds.p_max.y)
              }
            }
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
