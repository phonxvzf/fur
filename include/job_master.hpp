#ifndef JOB_MASTER_HPP
#define JOB_MASTER_HPP

#include <queue>
#include <mutex>

#include "tracer/bounds.hpp"
#include "math/random.hpp"

using namespace math;
using namespace tracer;

struct job {
  bounds2i bounds;
  random::rng rng;

  job() {}
  job(const bounds2i& bounds) : bounds(bounds) {}
  job& operator=(const job& j) { bounds = j.bounds; return *this; }
};

class job_master {
  private:
    std::mutex job_queue_mutex;
    std::queue<job> job_queue;

  public:
    job_master() {}

    void init(const bounds2i& bounds, const vector2i& tile_size);
    bool get_job(job* j);
};

#endif /* JOB_MASTER_HPP */
