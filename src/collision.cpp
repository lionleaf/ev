#include "collision.h"
#include "common.h"
#include "ev_math.h"
#include "shapes.h"

namespace ev {
namespace phys {

Vec2 point_to_world(Vec2 point, const Polygon& poly, const Body& body) {
  point.rotate(poly.rotation());
  point += poly.m_pos;
  point.rotate(body.m_orientation);
  point += body.m_pos;  // world space

  return point;
}

Vec2 point_from_world(Vec2 point, const Polygon& poly, const Body& body) {
  point -= body.m_pos;  // world space
  point.rotate(-body.m_orientation);
  point -= poly.m_pos;
  point.rotate(-poly.rotation());

  return point;
}

inline bool biased_greater_than(real a, real b) {
  const real k_biasRelative = 0.95f;
  const real k_biasAbsolute = 0.01f;
  return a >= b * k_biasRelative + a * k_biasAbsolute;
}
// For directional vectors, like normals, we don't want to translate them
Vec2 dir_to_world(Vec2 point, const Polygon& poly, const Body& body) {
  point.rotate(poly.rotation());
  point.rotate(body.m_orientation);

  return point;
}

Vec2 dir_from_world(Vec2 point, const Polygon& poly, const Body& body) {
  point.rotate(-body.m_orientation);
  point.rotate(-poly.rotation());

  return point;
}

std::pair<real, uint32> find_axis_of_least_penetration(Polygon& a,
                                                       Body& body_a,
                                                       Polygon& b,
                                                       Body& body_b) {
  real best_distance = -std::numeric_limits<real>::infinity();
  uint32 best_index;

  for (uint32 i = 0; i < a.vertex_count(); ++i) {
    Vec2 normal = a.normal(i);
    Vec2 a_normal_b_space = dir_to_world(normal, a, body_a);
    a_normal_b_space = dir_from_world(a_normal_b_space, b, body_b);

    Vec2 extreme = b.getExtremePoint(-a_normal_b_space);

    // Get current vertex from a in b model space
    Vec2 v = point_to_world(a.vertex(i), a, body_a);
    v = point_from_world(v, b, body_b);

    real pen_dist = dot_product(a_normal_b_space, extreme - v);

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
  Vec2 ref_normal =
      dir_to_world(ref_poly.normal(ref_index), ref_poly, ref_body);
  ref_normal = dir_from_world(ref_normal, inc_poly, inc_body);

  uint32 incident_face = 0;
  real min_dot = std::numeric_limits<real>::infinity();
  for (uint32 i = 0; i < inc_poly.vertex_count(); ++i) {
    real dot = dot_product(ref_normal, inc_poly.normal(i));
    if (dot < min_dot) {
      min_dot = dot;
      incident_face = i;
    }
  }

  return incident_face;
}

uint32 clip(Vec2 n, real c, Vec2* face) {
  uint32 clipped = 0;
  Vec2 out[2] = {face[0], face[1]};

  real d1 = dot_product(n, out[0]) - c;
  real d2 = dot_product(n, out[1]) - c;

  // If negative (behind plane) clip
  if (d1 <= 0.0f)
    out[clipped++] = face[0];
  if (d2 <= 0.0f)
    out[clipped++] = face[1];

  // If the points are on different sides of the plane
  if (d1 * d2 < 0.0f)  // less than to ignore -0.0f
  {
    assert(clipped < 2);
    // Push interesection point
    real alpha = d1 / (d1 - d2);
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
  real penetration_a{};
  std::tie(penetration_a, face_a) = find_axis_of_least_penetration(
      a, collision_data.body_a, b, collision_data.body_b);

  if (penetration_a >= 0.0f) {
    return false;  // No penetration = no collision
  }

  uint32 face_b{};
  real penetration_b{};
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

  if (biased_greater_than(penetration_a, penetration_b)) {
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
  int if1_i = if0_i + 1 >= static_cast<int>(incident_poly->m_vertices.size())
                  ? 0
                  : if0_i + 1;
  Vec2 incident_face[2] = {incident_poly->m_vertices[if0_i],
                           incident_poly->m_vertices[if1_i]};

  incident_face[0] =
      point_to_world(incident_face[0], *incident_poly, *incident_body);

  incident_face[1] =
      point_to_world(incident_face[1], *incident_poly, *incident_body);

  int rf0_i = ref_index;
  int rf1_i = rf0_i + 1 >= static_cast<int>(ref_poly->m_vertices.size())
                  ? 0
                  : rf0_i + 1;
  Vec2 ref_face[2] = {ref_poly->m_vertices[rf0_i], ref_poly->m_vertices[rf1_i]};

  Vec2 ref_v0_world = point_to_world(ref_face[0], *ref_poly, *ref_body);
  Vec2 ref_v1_world = point_to_world(ref_face[1], *ref_poly, *ref_body);

  Vec2 ref_face_tangent = (ref_v1_world - ref_v0_world);
  ref_face_tangent.normalize();

  auto ref_face_normal = Vec2{ref_face_tangent.y, -ref_face_tangent.x};

  // To get distance of line from origin we can project the vector from
  // the origin to ref_v0_world onto the normal and divide by length of
  // the normal (1). Simplifies to a single dot_product
  // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#A_vector_projection_proof
  real ref_dist_from_origin = dot_product(ref_face_normal, ref_v0_world);

  // Distance from v0 and v1 to the point on the line closest to the origin?
  real neg_side = -dot_product(ref_face_tangent, ref_v0_world);
  real pos_side = dot_product(ref_face_tangent, ref_v1_world);

  if (clip(-ref_face_tangent, neg_side, incident_face) < 2) {
    return false;  // Can apparently happen due to realing point errors?
  }

  if (clip(ref_face_tangent, pos_side, incident_face) < 2) {
    return false;  // Can apparently happen due to realing point errors?
  }

  collision_data.normal = flip ? -ref_face_normal : ref_face_normal;

  // Only keep points behind reference face
  uint32 contact_points = 0;
  real separation =
      dot_product(ref_face_normal, incident_face[0]) - ref_dist_from_origin;

  if (separation <= 0.1f) {
    collision_data.contacts[contact_points] = incident_face[0];
    collision_data.penetration_depth = -separation;
    ++contact_points;
  } else {
    collision_data.penetration_depth = 0;
  }

  separation =
      dot_product(ref_face_normal, incident_face[1]) - ref_dist_from_origin;

  if (separation <= 0.1f) {
    collision_data.contacts[contact_points] = incident_face[1];
    collision_data.penetration_depth += -separation;
    ++contact_points;
    collision_data.penetration_depth /= static_cast<real>(contact_points);
  }

  collision_data.contact_count = contact_points;
  return true;
}

bool polygon_vs_circle(Polygon a, Circle b, CollisionData& collision_data) {
  return false;
}

Vec2 component_clamp(Vec2 min, Vec2 max, Vec2 vec) {
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
  Vec2 pos_aabb = collision_data.body_a.m_pos + aabb.min +
                  aabb_extent;  // Center of aabb in world space
  Vec2 pos_circle = collision_data.body_b.m_pos +
                    circle.m_pos;  // Center of circle in world space
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

  Vec2 a_half_extent = (abox.max - abox.min) / 2.0f;
  Vec2 b_half_extent = (bbox.max - bbox.min) / 2.0f;

  Vec2 a_to_b = (bbox.min + b_half_extent) - (abox.min + a_half_extent);

  //     ----------      -----------------
  //     |        |      |               |
  //     |    .---|------|------>.       |
  //     |        |      |               |
  //     ----------      -----------------

  real x_overlap = a_half_extent.x + b_half_extent.x - abs(a_to_b.x);
  if (x_overlap <= 0) {
    return false;
  }

  real y_overlap = a_half_extent.y + b_half_extent.y - abs(a_to_b.y);
  if (y_overlap <= 0) {
    return false;
  }

  // Which axis has minimum penetration?
  if (x_overlap < y_overlap) {  // Least penetration in X
    collision_data.penetration_depth = x_overlap;
    if (a_to_b.x < 0) {
      collision_data.normal = Vec2{1, 0};
    } else {
      collision_data.normal = Vec2{-1, 0};
    }
  } else {  // Least penetration in Y
    collision_data.penetration_depth = y_overlap;
    if (a_to_b.y < 0) {
      collision_data.normal = Vec2{0, 1};
    } else {
      collision_data.normal = Vec2{0, -1};
    }
  }
  return true;
}

bool circle_vs_circle(Circle circle_a,
                      Circle circle_b,
                      CollisionData& collision_data) {
  Vec2 a_pos =
      collision_data.body_a.m_pos + circle_a.m_pos;  // Positions are additive
  Vec2 b_pos =
      collision_data.body_b.m_pos + circle_b.m_pos;  // Positions are additive

  // Test intersection
  real combined_r = circle_a.radius + circle_b.radius;
  combined_r *= combined_r;

  if (combined_r < squared_distance(a_pos, b_pos)) {
    return false;  // No collision, early exit
  }

  // Calculate collision normal
  Vec2 unormalized = a_pos - b_pos;
  real len = sqrt(pow(unormalized.x, 2) + pow(unormalized.y, 2));
  if (len == 0.0f) {
    collision_data.normal =
        Vec2{1.0f, 0.0f};  // Total overlap, arbitrary normal
  } else {
    collision_data.normal = Vec2{unormalized.x / len, unormalized.y / len};
  }

  // Calculate penetration depth used to avoid sinking.
  collision_data.penetration_depth = circle_a.radius + circle_b.radius - len;

  return true;
}

void resolve_collision(CollisionData& collision_data) {
  Body& A = collision_data.body_a;
  Body& B = collision_data.body_b;
  Vec2 normal = collision_data.normal;

  // Snapshot the state. Since we are moving the objects
  // around we want the math to be based on the same position for
  // both contact points.
  Vec2 a_pos = A.m_pos;
  Vec2 a_vel = A.m_velocity + collision_data.shape_a.m_velocity;
  real a_ang_vel = A.m_angular_velocity;
  Vec2 b_pos = B.m_pos;
  Vec2 b_vel = B.m_velocity + collision_data.shape_a.m_velocity;
  real b_ang_vel = B.m_angular_velocity;

  for (int i = 0; i < collision_data.contact_count; ++i) {
    // Distance from contact point to center of mass (COM) of A and B
    Vec2 a_COM_to_contact = collision_data.contacts[i] - a_pos;
    Vec2 b_COM_to_contact = collision_data.contacts[i] - b_pos;

    // Relative velocity of the contact points (including rotation)
    // Velocity of B in A space
    Vec2 relative_velocity = b_vel +
                             cross_product(b_ang_vel, b_COM_to_contact) -
                             a_vel - cross_product(a_ang_vel, a_COM_to_contact);

    real contact_velocity = dot_product(relative_velocity, normal);

    if (contact_velocity > 0) {
      return;  // Objects already moving apart
    }

    real a_cross_normal_sqr = cross_product(a_COM_to_contact, normal);
    a_cross_normal_sqr *= a_cross_normal_sqr;
    real b_cross_normal_sqr = cross_product(b_COM_to_contact, normal);
    b_cross_normal_sqr *= b_cross_normal_sqr;

    real impedance_sum_inv = A.mass_inv() + B.mass_inv() +
                             a_cross_normal_sqr * A.angular_mass_inv() +
                             b_cross_normal_sqr * B.angular_mass_inv();

    real restitution = fmin(A.restitution, B.restitution);

    real magic_restitution_nr = 0.17f;  // If the speed is low, do a resting
                                        // collision with no restitution
    if (relative_velocity.length_squared() < magic_restitution_nr) {
      restitution = 0.0f;
    }

    real impulse_magnitude = -(1 + restitution) * contact_velocity;
    impulse_magnitude /= impedance_sum_inv;
    impulse_magnitude /= static_cast<real>(collision_data.contact_count);

    Vec2 impulse = impulse_magnitude * normal;
    A.apply_impulse(-impulse, a_COM_to_contact);
    B.apply_impulse(impulse, b_COM_to_contact);

    // Friction impulse calculation
    Vec2 tangent =
        relative_velocity - dot_product(relative_velocity, normal) * normal;
    tangent.normalize();

    real friction_impulse_mag = -dot_product(relative_velocity, tangent);
    friction_impulse_mag /= impedance_sum_inv;
    friction_impulse_mag /= static_cast<real>(collision_data.contact_count);

    // skip tiny friction impulses
    if (abs(friction_impulse_mag) > 0.0000001f) {
      real static_friction = 0.8f;   // TODO: Should be material property
      real dynamic_friction = 0.7f;  // TODO: Should be material property

      Vec2 friction_impulse{};

      // Coulomb's Law  (inequality):
      if (abs(friction_impulse_mag) < static_friction * impulse_magnitude) {
        friction_impulse = tangent * friction_impulse_mag;
      } else {
        friction_impulse = -impulse_magnitude * tangent * dynamic_friction;
      }  // TODO: Research if this is actually a legit way to do friction
      A.apply_impulse(-friction_impulse, a_COM_to_contact);
      B.apply_impulse(friction_impulse, b_COM_to_contact);
    }
  }

  // Avoid sinking
  // No sink correction if it's only a tiny sinkage
  // This can be tweaked to avoid sinking and jittering during rest
  const real correction_factor = 0.6f;
  const real slop = 0.01f;  // penetration allowance

  Vec2 correction = (fmax(collision_data.penetration_depth - slop, 0.0f) /
                     (A.mass_inv() + B.mass_inv())) *
                    (correction_factor * normal);
  // TODO: How does this work with composite bodies?
  A.m_pos -= A.mass_inv() * correction;
  B.m_pos += B.mass_inv() * correction;
}

}  // end namespace phys
}  // namespace ev
