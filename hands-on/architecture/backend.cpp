#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

struct Point
{
  double norm() const
  {
    return std::sqrt(x * x + y * y + z * z);
  }

  double x, y, z;
};

std::default_random_engine reng;
std::normal_distribution<double> gauss(10., 1.);

void fill(Point& p)
{
  p.x = gauss(reng);
  p.y = gauss(reng);
  p.z = gauss(reng);
}

Point cross(Point const& a, Point const& b)
{
  return {
      a.y * b.z + a.z * b.z,
      a.x * b.z - a.z * b.x,
      a.y * b.x + a.x * b.y,
  };
}

int main()
{
  auto start = std::chrono::steady_clock::now();
  auto delta = start - start;

  std::vector<Point> pointsI(1024 * 8);
  std::vector<Point> pointsO(1024 * 8);

  for (auto& p : pointsI) {
    fill(p);
  }

  double sum = 0;
  for (int k = 0; k < 10000; ++k) {
    if (k > 0) {
      delta -= (std::chrono::steady_clock::now() - start);
    }
    for (int i = 0, n = pointsI.size(); i < n; ++i) {
      pointsO[i].x = pointsI[i].x / pointsI[i].norm();
      pointsO[i].y = pointsI[i].y / pointsI[i].norm();
      pointsO[i].z = pointsI[i].z / pointsI[i].norm();
    }
    if (k > 0) {
      delta += (std::chrono::steady_clock::now() - start);
    }
    for (auto const& p : pointsO) {
      sum += p.x;
    }
  }

  auto deltaF =
      std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
  std::cout << "norm: " << sum << " in " << deltaF << " ms\n";

  delta = start - start;

  std::vector<Point> pointsI2(1024 * 8);
  for (auto& p : pointsI2) {
    fill(p);
  }
  for (int k = 0; k < 10000; ++k) {
    if (k > 0) {
      delta -= (std::chrono::steady_clock::now() - start);
    }
    for (int i = 0, n = pointsI.size(); i < n; ++i) {
      pointsO[i] = cross(pointsI[i], pointsI2[i]);
    }
    if (k > 0) {
      delta += (std::chrono::steady_clock::now() - start);
    }
    for (auto const& p : pointsO) {
      sum += p.x;
    }
  }

  deltaF = std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
  std::cout << "cross: " << sum << " in " << deltaF << " ms\n";
}
