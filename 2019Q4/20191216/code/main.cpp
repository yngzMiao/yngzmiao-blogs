# include <iostream>
# include <chrono>
# include <random>
#include <arm_neon.h>

int main(int argc, char const *argv[])
{
  float *data_tmp = new float[1080 * 720 * 3];
  std::default_random_engine e;
  std::uniform_real_distribution<float> u(0, 255);
  for(int i = 0; i < 1080 * 720 * 3; ++i) {
    *(data_tmp + i) = u(e);
  }

  float *data = data_tmp;
  float *data_res1 = new float[1080 * 720 * 3];

  std::chrono::microseconds start_time = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  );

  for(int i = 0; i < 1080 * 720 * 3; ++i) {
    *data_res1 = ((*data) + 3.4 ) / 3.1;
    ++data_res1;
    ++data;
  }

  std::chrono::microseconds end_time = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  );

  std::cout << "cost total time : " << (end_time - start_time).count() << " microseconds  -- common method" << std::endl;

  data = data_tmp;
  float *data_res2 = new float[1080 * 720 * 3];

  start_time = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  );

  float32x4_t A = vdupq_n_f32(3.4);
  float32x4_t B = vdupq_n_f32(3.1);
  for(int i = 0; i < 1080 * 720 * 3 / 4; ++i) {
    float32x4_t C = (float32x4_t){*data, *(data + 1), *(data + 2), *(data + 3)};
    float32x4_t D = vmulq_f32(vaddq_f32(C, A), B);
    vst1q_f32(data_res2, D);
    data = data + 4;
    data_res2 = data_res2 + 4;
  }

  end_time = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  );

  std::cout << "cost total time : " << (end_time - start_time).count() << " microseconds  -- neon method" << std::endl;

  return 0;
}
