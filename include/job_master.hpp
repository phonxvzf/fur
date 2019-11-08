#ifndef JOB_MASTER_HPP
#define JOB_MASTER_HPP

#include <queue>
#include <mutex>

#include "math/vector.hpp"

struct job {
  math::vector2i start;
  math::vector2i end;

  job() {}
  job(const math::vector2i& start, const math::vector2i& end) : start(start), end(end) {}
  job& operator=(const job& j) { start = j.start; end = j.end; return *this; }
};

class job_master {
  private:
    std::mutex job_queue_mutex;
    std::queue<job> job_queue;

  public:
    job_master() {}

    void init(const math::vector2i& img_res, const math::vector2i& tile_size);
    bool get_job(job* j);
};

#endif /* JOB_MASTER_HPP */
