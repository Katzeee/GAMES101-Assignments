//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"

void Scene::buildBVH() {
  printf(" - Generating BVH...\n\n");
  this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const { return this->bvh->Intersect(ray); }

void Scene::sampleLight(Intersection &pos, float &pdf) const {
  float emit_area_sum = 0;
  for (uint32_t k = 0; k < objects.size(); ++k) {
    if (objects[k]->hasEmit()) {
      emit_area_sum += objects[k]->getArea();
    }
  }
  float p = get_random_float() * emit_area_sum;
  emit_area_sum = 0;
  for (uint32_t k = 0; k < objects.size(); ++k) {
    if (objects[k]->hasEmit()) {
      emit_area_sum += objects[k]->getArea();
      if (p <= emit_area_sum) {
        objects[k]->Sample(pos, pdf);
        break;
      }
    }
  }
}

bool Scene::trace(const Ray &ray, const std::vector<Object *> &objects, float &tNear, uint32_t &index,
                  Object **hitObject) {
  *hitObject = nullptr;
  for (uint32_t k = 0; k < objects.size(); ++k) {
    float tNearK = kInfinity;
    uint32_t indexK;
    Vector2f uvK;
    if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
      *hitObject = objects[k];
      tNear = tNearK;
      index = indexK;
    }
  }

  return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const {
  Vector3f color{0, 0, 0};
  if (depth > maxDepth) {
    return color;
  }
  // Find out the out light intersect with which object
  auto p_insect = intersect(ray);
  if (!p_insect.happened) {
    return color;
  }
  // Find out the direct light contribution
  Intersection light_sample_insect;
  float light_pdf;
  sampleLight(light_sample_insect, light_pdf);
  auto wi_dir = light_sample_insect.coords - p_insect.coords;
  auto N = p_insect.normal;
  Vector3f wi_orig = (dotProduct(wi_dir, N) < 0) ? p_insect.coords - N * EPSILON : p_insect.coords + N * EPSILON;
  Ray wi{wi_orig, wi_dir.normalized()};
  auto wi_insect = intersect(wi);
  if ((wi(wi_insect.distance) - light_sample_insect.coords).norm() < 0.1) {
    float cost = dotProduct(p_insect.normal, wi.direction);
    color += p_insect.m->eval(wi.direction, ray.direction, p_insect.normal) / light_pdf;
  }

  return {};
}