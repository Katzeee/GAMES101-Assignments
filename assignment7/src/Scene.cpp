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
  auto p_insect = intersect(ray);
  // Find out the out light intersect with which object
  if (!p_insect.happened) {
    return color;
  }
  // Find out emission
  if (p_insect.obj->hasEmit()) {
    color += p_insect.m->getEmission();
  }
  // Find out the direct light contribution
  Intersection light_sample_insect;
  float light_pdf;
  sampleLight(light_sample_insect, light_pdf);
  auto wi_dir = light_sample_insect.coords - p_insect.coords;
  auto N = p_insect.normal;
  Vector3f wi_orig = (dotProduct(wi_dir, N) < 0) ? p_insect.coords - N * EPSILON : p_insect.coords + N * EPSILON;
  Ray wi_light{wi_orig, wi_dir.normalized()};
  auto wi_light_insect = intersect(wi_light);
  if (wi_light_insect.happened && (wi_light(wi_light_insect.distance) - light_sample_insect.coords).norm() < 0.01) {
    float costi = dotProduct(p_insect.normal, wi_light.direction);              // cos(theta_i)
    float costl = dotProduct(light_sample_insect.normal, -wi_light.direction);  // cos(theta_light)
    auto brdf = p_insect.m->eval(ray.direction, wi_light.direction, p_insect.normal);
    color += light_sample_insect.emit * brdf * costi * costl / (wi_dir.norm() * wi_dir.norm() * light_pdf);
    // L_dir = lightInter.emit * f_r * dotProduct(lightDir, N) * dotProduct(-lightDir, NN) / lightDistance /
  }

  // Find out the light from other position
  if (get_random_float() < RussianRoulette) {
    auto wi_other_dir = p_insect.m->sample(ray.direction, p_insect.normal);
    Ray wi_other{p_insect.coords, wi_other_dir.normalized()};
    auto other_insect = intersect(wi_other);
    if (!other_insect.happened || other_insect.m->hasEmission()) {  // doesn't happen or sample to light
      return color;
    }
    auto other_pdf = p_insect.m->pdf(ray.direction, wi_other.direction, p_insect.normal);
    float costi = dotProduct(p_insect.normal, wi_other.direction);  // cos(theta_i)
    auto brdf = p_insect.m->eval(ray.direction, wi_other.direction, p_insect.normal);
    color += castRay(wi_other, depth + 1) * brdf * costi / other_pdf / RussianRoulette;
  }

  return color;
}

// Vector3f Scene::castRay(const Ray &ray, int depth) const {
//   Intersection inter = intersect(ray);

//   if (inter.happened) {
//     if (inter.m->hasEmission()) {
//       if (depth == 0) {
//         return inter.m->getEmission();
//       } else
//         return Vector3f(0, 0, 0);
//     }

//     Vector3f L_dir(0, 0, 0);
//     Vector3f L_indir(0, 0, 0);

//     Intersection lightInter;
//     float pdf_light = 0.0f;
//     sampleLight(lightInter, pdf_light);

//     auto &N = inter.normal;
//     auto &NN = lightInter.normal;

//     auto &objPos = inter.coords;
//     auto &lightPos = lightInter.coords;

//     auto diff = lightPos - objPos;
//     auto lightDir = diff.normalized();
//     float lightDistance = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

//     Ray light(objPos, lightDir);
//     Intersection light2obj = intersect(light);

//     if (light2obj.happened && (light2obj.coords - lightPos).norm() < 1e-2) {
//       Vector3f f_r = inter.m->eval(ray.direction, lightDir, N);
//       L_dir = lightInter.emit * f_r * dotProduct(lightDir, N) * dotProduct(-lightDir, NN) / lightDistance /
//       pdf_light;
//     }

//     if (get_random_float() < RussianRoulette) {
//       Vector3f nextDir = inter.m->sample(ray.direction, N).normalized();

//       Ray nextRay(objPos, nextDir);
//       Intersection nextInter = intersect(nextRay);
//       if (nextInter.happened && !nextInter.m->hasEmission()) {
//         float pdf = inter.m->pdf(ray.direction, nextDir, N);
//         Vector3f f_r = inter.m->eval(ray.direction, nextDir, N);
//         L_indir = castRay(nextRay, depth + 1) * f_r * dotProduct(nextDir, N) / pdf / RussianRoulette;
//       }
//     }

//     return L_dir + L_indir;
//   }

//   return Vector3f(0, 0, 0);
// }
