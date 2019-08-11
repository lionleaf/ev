#include "collision.h"
#include "common.h"
#include "ev_math.h"
#include "shapes.h"

namespace ev {
namespace phys {

Vec2f point_to_world(Vec2f point, const Polygon& poly, const Body& body) {
  point.rotate(poly.rotation());
  point += poly.m_pos;
  point.rotate(body.m_orientation);
  point += body.m_pos;  // world space

  return point;
}

Vec2f point_from_world(Vec2f point, const Polygon& poly, const Body& body) {
  point -= body.m_pos;  // world space
  point.rotate(-body.m_orientation);
  point -= poly.m_pos;
  point.rotate(-poly.rotation());

  return point;
}

// For directional vectors, like normals, we don't want to translate them
Vec2f dir_to_world(Vec2f point, const Polygon& poly, const Body& body) {
  point.rotate(poly.rotation());
  point.rotate(body.m_orientation);

  return point;
}

Vec2f dir_from_world(Vec2f point, const Polygon& poly, const Body& body) {
  point.rotate(-body.m_orientation);
  point.rotate(-poly.rotation());

  return point;
}

std::pair<float, uint32> find_axis_of_least_penetration(Polygon& a,
                                                        Body& body_a,
                                                        Polygon& b,
                                                        Body& body_b) {
  float best_distance = -std::numeric_limits<float>::infinity();
  uint32 best_index;

  for (uint32 i = 0; i < a.vertex_count(); ++i) {
    Vec2f normal = a.normal(i);
    Vec2f a_normal_b_space = dir_to_world(normal, a, body_a);
    a_normal_b_space = dir_from_world(a_normal_b_space, b, body_b);

    Vec2f extreme = b.getExtremePoint(-a_normal_b_space);

    // Get current vertex from a in b model space
    Vec2f v = point_to_world(a.vertex(i), a, body_a);
    v = point_from_world(v, b, body_b);

    float pen_dist = dot_product(a_normal_b_space, extreme - v);

    if (pen_dist > best_distance) {
      best_distance = pen_dist;
      best_index = i;
    }
  }

  return std::make_pair(best_distance, best_index);
}

uint32 find_incident_face(Polygon& ref_poly,
                          Body& ref_body,
                          Polygon& inc_poly,
                          Body& inc_body,
                          uint32 ref_index) {
  Vec2f ref_normal =
      dir_to_world(ref_poly.normal(ref_index), ref_poly, ref_body);
  ref_normal = dir_from_world(ref_normal, inc_poly, inc_body);

  uint32 incident_face = 0;
  float min_dot = std::numeric_limits<float>::infinity();
  for (uint32 i = 0; i < inc_poly.vertex_count(); ++i) {
    float dot = dot_product(ref_normal, inc_poly.normal(i));
    if (dot < min_dot) {
      min_dot = dot;
      incident_face = i;
    }
  }

  return incident_face;
}

uint32 clip(Vec2f n, float c, Vec2f* face) {
  uint32 clipped = 0;
  Vec2f out[2] = {face[0], face[1]};

  float d1 = dot_product(n, out[0]) - c;
  float d2 = dot_product(n, out[1]) - c;

  // If negative (behind plane) clip
  if (d1 <= 0.0f)
    out[clipped++] = face[0];
  if (d2 <= 0.0f)
    out[clipped++] = face[1];

  // If the points are on different sides of the plane
  if (d1 * d2 < 0.0f)  // less than to ignore -0.0f
  {
    // Push interesection point
    float alpha = d1 / (d1 - d2);
    out[clipped] = face[0] + alpha * (face[1] - face[0]);
    ++clipped;
  }

  face[0] = out[0];
  face[1] = out[1];

  assert(clipped != 3);

  return clipped;
}

bool polygon_vs_polygon(Polygon a, Polygon b, CollisionData& collision_data) {
  // Based on the theorem of axis of separation
  uint32 face_a{};
  float penetration_a{};
  std::tie(penetration_a, face_a) = find_axis_of_least_penetration(
      a, collision_data.body_a, b, collision_data.body_b);

  if (penetration_a >= 0.0f) {
    return false;  // No penetration = no collision
  }

  uint32 face_b{};
  float penetration_b{};
  std::tie(penetration_b, face_b) = find_axis_of_least_penetration(
      b, collision_data.body_b, a, collision_data.body_a);

  if (penetration_b >= 0.0f) {
    return false;  // No penetration = no collision
  }

  uint32 ref_index{};
  bool flip{};

  Polygon* ref_poly;
  Body* ref_body;
  Polygon* incident_poly;
  Body* incident_body;

  if (penetration_a >= penetration_b) {
    flip = false;
    ref_poly = &a;
    ref_body = &collision_data.body_a;
    incident_poly = &b;
    incident_body = &collision_data.body_b;
    ref_index = face_a;
  } else {
    flip = true;
    ref_poly = &b;
    ref_body = &collision_data.body_b;
    incident_poly = &a;
    incident_body = &collision_data.body_a;
    ref_index = face_b;
  }

  uint32 incident_face_index = find_incident_face(
      *ref_poly, *ref_body, *incident_poly, *incident_body, ref_index);

  int if0_i = incident_face_index;
  int if1_i = if0_i + 1 >= incident_poly->m_vertices.size() ? 0 : if0_i + 1;
  Vec2f incident_face[2] = {incident_poly->m_vertices[if0_i],
                            incident_poly->m_vertices[if1_i]};

  incident_face[0] =
      point_to_world(incident_face[0], *incident_poly, *incident_body);

  incident_face[1] =
      point_to_world(incident_face[1], *incident_poly, *incident_body);

  int rf0_i = ref_index;
  int rf1_i = rf0_i + 1 >= ref_poly->m_vertices.size() ? 0 : rf0_i + 1;
  Vec2f ref_face[2] = {ref_poly->m_vertices[rf0_i],
                       ref_poly->m_vertices[rf1_i]};

  Vec2f ref_v0_world = point_to_world(ref_face[0], *ref_poly, *ref_body);
  Vec2f ref_v1_world = point_to_world(ref_face[1], *ref_poly, *ref_body);

  Vec2f ref_face_tangent = (ref_v1_world - ref_v0_world);
  ref_face_tangent.normalize();

  auto ref_face_normal = Vec2f{ref_face_tangent.y, -ref_face_tangent.x};

  // To get distance of line from origin we can project the vector from
  // the origin to ref_v0_world onto the normal and divide by length of
  // the normal (1). Simplifies to a single dot_product
  // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#A_vector_projection_proof
  float ref_dist_from_origin = dot_product(ref_face_normal, ref_v0_world);

  // Distance from v0 and v1 to the point on the line closest to the origin?
  float neg_side = -dot_product(ref_face_tangent, ref_v0_world);
  float pos_side = dot_product(ref_face_tangent, ref_v1_world);

  if (clip(-ref_face_tangent, neg_side, incident_face) < 2) {
    return false;  // Can apparently happen due to floating point errors?
  }

  if (clip(ref_face_tangent, pos_side, incident_face) < 2) {
    return false;  // Can apparently happen due to floating point errors?
  }

  collision_data.normal = !flip ? -ref_face_normal : ref_face_normal;

  // Only keep points behind reference face
  uint32 contact_points = 0;
  float separation =
      dot_product(ref_face_normal, incident_face[0]) - ref_dist_from_origin;

  if (separation <= 0.0f) {
    // collision_data.contacts[contact_points] = incident_face[0]
    collision_data.penetration_depth = -separation;
    ++contact_points;
  } else {
    collision_data.penetration_depth = 0;
  }

  separation =
      dot_product(ref_face_normal, incident_face[1]) - ref_dist_from_origin;

  if (separation <= 0.0f) {
    // collision_data.contacts[contact_points] = incident_face[1]
    collision_data.penetration_depth += -separation;
    ++contact_points;
    collision_data.penetration_depth /= static_cast<float>(contact_points);
  }
  // collision_data.contact_count = contact_points
  return true;
}

bool polygon_vs_circle(Polygon a, Circle b, CollisionData& collision_data) {
  return false;
}

Vec2f component_clamp(Vec2f min, Vec2f max, Vec2f vec) {
  // Clamps each component of vec between min and max
  // Since vec is passed by value we can just modify and return the local copy
  vec.x = vec.x < min.x ? min.x : vec.x;
  vec.x = vec.x > max.x ? max.x : vec.x;

  vec.y = vec.y < min.y ? min.y : vec.y;
  vec.y = vec.y > max.y ? max.y : vec.y;
  return vec;
}

bool AABB_vs_circle(AABB aabb, Circle circle, CollisionData& collision_data) {
  auto aabb_extent = (aabb.max - aabb.min) / 2.0f;
  Vec2f pos_aabb = collision_data.body_a.m_pos + aabb.min +
                   aabb_extent;  // Center of aabb in world space
  Vec2f pos_circle = collision_data.body_b.m_pos +
                     circle.pos;  // Center of circle in world space
  auto aabb_to_circle = pos_circle - pos_aabb;

  // Compute closest point on the AABB to the circle
  // By clamping the position of the circle center to the AABB, we get the
  // closest point on (or inside) the AABB to the center
  auto closest_to_circle = component_clamp(pos_aabb - aabb_extent,
                                           pos_aabb + aabb_extent, pos_circle);

  // If the point on the AABB closest to the circle is hitting the center of
  // the circle, we know it's inside the AABB and need a special case (flip
  // normal)
  auto inside = false;
  if (aabb_to_circle == closest_to_circle) {
    inside = true;

    if (abs(aabb_to_circle.x) > abs(aabb_to_circle.y)) {
      if (closest_to_circle.x > 0) {
        closest_to_circle.x = aabb_extent.x;
      } else {
        closest_to_circle.x = -1 * aabb_extent.x;
      }
    } else {
      if (closest_to_circle.y > 0) {
        closest_to_circle.y = aabb_extent.y;
      } else {
        closest_to_circle.y = -1 * aabb_extent.y;
      }
    }
  }

  auto normal = pos_circle - closest_to_circle;
  auto dis = squared_length(normal);

  if (!inside && dis > circle.radius * circle.radius) {
    return false;
  }

  dis = sqrt(dis);  // Delay this until after early exit

  if (inside) {
    collision_data.normal = normal;
    collision_data.normal.normalize();
    collision_data.penetration_depth = circle.radius - dis;
  } else {
    collision_data.normal = -1 * normal;
    collision_data.normal.normalize();
    collision_data.penetration_depth = circle.radius - dis;
  }

  return true;
}

bool AABB_vs_AABB(AABB relative_a,
                  AABB relative_b,
                  CollisionData& collision_data) {
  // Create new AABB in world space
  AABB abox = relative_a;
  abox.min += collision_data.body_a.m_pos;
  abox.max += collision_data.body_a.m_pos;
  AABB bbox = relative_b;
  bbox.min += collision_data.body_b.m_pos;
  bbox.max += collision_data.body_b.m_pos;

  Vec2f a_half_extent = (abox.max - abox.min) / 2.0f;
  Vec2f b_half_extent = (bbox.max - bbox.min) / 2.0f;

  Vec2f a_to_b = (bbox.min + b_half_extent) - (abox.min + a_half_extent);

  //     ----------      -----------------
  //     |        |      |               |
  //     |    .---|------|------>.       |
  //     |        |      |               |
  //     ----------      -----------------

  float x_overlap = a_half_extent.x + b_half_extent.x - abs(a_to_b.x);
  if (x_overlap <= 0) {
    return false;
  }

  float y_overlap = a_half_extent.y + b_half_extent.y - abs(a_to_b.y);
  if (y_overlap <= 0) {
    return false;
  }

  // Which axis has minimum penetration?
  if (x_overlap < y_overlap) {  // Least penetration in X
    collision_data.penetration_depth = x_overlap;
    if (a_to_b.x < 0) {
      collision_data.normal = Vec2f{1, 0};
    } else {
      collision_data.normal = Vec2f{-1, 0};
    }
  } else {  // Least penetration in Y
    collision_data.penetration_depth = y_overlap;
    if (a_to_b.y < 0) {
      collision_data.normal = Vec2f{0, 1};
    } else {
      collision_data.normal = Vec2f{0, -1};
    }
  }
  return true;
}

bool circle_vs_circle(Circle circle_a,
                      Circle circle_b,
                      CollisionData& collision_data) {
  Vec2f a_pos =
      collision_data.body_a.m_pos + circle_a.pos;  // Positions are additive
  Vec2f b_pos =
      collision_data.body_b.m_pos + circle_b.pos;  // Positions are additive

  // Test intersection
  float combined_r = circle_a.radius + circle_b.radius;
  combined_r *= combined_r;

  if (combined_r < squared_distance(a_pos, b_pos)) {
    return false;  // No collision, early exit
  }

  // Calculate collision normal
  Vec2f unormalized = a_pos - b_pos;
  float len = sqrt(pow(unormalized.x, 2) + pow(unormalized.y, 2));
  if (len == 0.0f) {
    collision_data.normal =
        Vec2f{1.0f, 0.0f};  // Total overlap, arbitrary normal
  } else {
    collision_data.normal = Vec2f{unormalized.x / len, unormalized.y / len};
  }

  // Calculate penetration depth used to avoid sinking.
  collision_data.penetration_depth = circle_a.radius + circle_b.radius - len;

  return true;
}

void resolve_collision(CollisionData& collision_data) {
  Body& A = collision_data.body_a;
  Body& B = collision_data.body_b;
  Vec2f normal = collision_data.normal;

  Vec2f relative_velocity = A.m_velocity - B.m_velocity;

  float vel_along_normal = dot_product(relative_velocity, normal);

  if (vel_along_normal > 0) {
    return;  // Objects already moving apart
  }

  float restitution = fmin(A.restitution, B.restitution);

  float a_inv_mass = A.mass == 0.0f ? 0.0f : 1.0f / A.mass;  // TODO: Precalc
  float b_inv_mass = B.mass == 0.0f ? 0.0f : 1.0f / B.mass;

  float impulse_magnitude = -(1 + restitution) * vel_along_normal;
  impulse_magnitude /= (a_inv_mass + b_inv_mass);

  Vec2f impulse = impulse_magnitude * normal;

  A.m_velocity += a_inv_mass * impulse;
  B.m_velocity -= b_inv_mass * impulse;

  // Avoid sinking

  // No sink correction if it's only a tiny sinkage
  const float epsilon = 0.01f;
  // This can be tweaked to avoid sinking and jittering during rest
  const float correction_factor = 0.2f;

  Vec2f correction = (fmax(collision_data.penetration_depth - epsilon, 0.0f) /
                      (a_inv_mass + b_inv_mass)) *
                     (correction_factor * normal);
  // TODO: How does this work with composite bodies?
  A.m_pos += a_inv_mass * correction;
  B.m_pos -= b_inv_mass * correction;

  // Apply friction
  relative_velocity = A.m_velocity - B.m_velocity;  // This has changed now
  Vec2f tangent =
      relative_velocity - dot_product(relative_velocity, normal) * normal;
  tangent.normalize();

  float friction_impulse_magnitude =
      dot_product(relative_velocity, tangent) * -1;
  friction_impulse_magnitude /= (a_inv_mass + b_inv_mass);

  float static_friction = 0.5f;   // TODO: Should be material property
  float dynamic_friction = 0.3f;  // TODO: Should be material property

  Vec2f friction_impulse{};

  // Coulomb's Law  (inequality):
  if (abs(friction_impulse_magnitude) < static_friction * impulse_magnitude) {
    friction_impulse = tangent * friction_impulse_magnitude;
  } else {
    friction_impulse = -1 * impulse_magnitude * tangent * dynamic_friction;
  }  // TODO: Research if this is actually a legit way to do friction

  A.m_velocity += a_inv_mass * friction_impulse;
  B.m_velocity -= b_inv_mass * friction_impulse;
}

}  // end namespace phys
}  // namespace ev
