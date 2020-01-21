/*************************************************************************/
/*  space_bullet.cpp                                                     */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "space_bullet.h"

#include "bullet_physics_server.h"
#include "bullet_types_converter.h"
#include "bullet_utilities.h"
#include "constraint_bullet.h"
#include "core/config/project_settings.h"
#include "core/string/ustring.h"
#include "godot_collision_configuration.h"
#include "godot_collision_dispatcher.h"
#include "rigid_body_bullet.h"
#include "servers/physics_server_3d.h"
#include "soft_body_bullet.h"

#include <BulletCollision/BroadphaseCollision/btBroadphaseProxy.h>
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.h>
#include <BulletCollision/NarrowPhaseCollision/btGjkPairDetector.h>
#include <BulletCollision/NarrowPhaseCollision/btPointCollector.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <btBulletDynamicsCommon.h>

#include <assert.h>

/**
	@author AndreaCatania
*/

BulletPhysicsDirectSpaceState::BulletPhysicsDirectSpaceState(SpaceBullet *p_space) :
		PhysicsDirectSpaceState3D(),
		space(p_space) {}

int BulletPhysicsDirectSpaceState::intersect_point(const Vector3 &p_point, ShapeResult *r_results, int p_result_max, const Set<RID> &p_exclude, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas) {
	if (p_result_max <= 0) {
		return 0;
	}

	btVector3 bt_point;
	G_TO_B(p_point, bt_point);

	btSphereShape sphere_point(0.001f);
	btCollisionObject collision_object_point;
	collision_object_point.setCollisionShape(&sphere_point);
	collision_object_point.setWorldTransform(btTransform(btQuaternion::getIdentity(), bt_point));

	// Setup query
	GodotAllContactResultCallback btResult(&collision_object_point, r_results, p_result_max, &p_exclude, p_collide_with_bodies, p_collide_with_areas);
	btResult.m_collisionFilterGroup = 0;
	btResult.m_collisionFilterMask = p_collision_mask;
	space->dynamicsWorld->contactTest(&collision_object_point, btResult);

	// The results is already populated by GodotAllConvexResultCallback
	return btResult.m_count;
}

bool BulletPhysicsDirectSpaceState::intersect_ray(const Vector3 &p_from, const Vector3 &p_to, RayResult &r_result, const Set<RID> &p_exclude, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, bool p_pick_ray) {
	btVector3 btVec_from;
	btVector3 btVec_to;

	G_TO_B(p_from, btVec_from);
	G_TO_B(p_to, btVec_to);

	// setup query
	GodotClosestRayResultCallback btResult(btVec_from, btVec_to, &p_exclude, p_collide_with_bodies, p_collide_with_areas);
	btResult.m_collisionFilterGroup = 0;
	btResult.m_collisionFilterMask = p_collision_mask;
	btResult.m_pickRay = p_pick_ray;

	space->dynamicsWorld->rayTest(btVec_from, btVec_to, btResult);
	if (btResult.hasHit()) {
		B_TO_G(btResult.m_hitPointWorld, r_result.position);
		B_TO_G(btResult.m_hitNormalWorld.normalize(), r_result.normal);
		CollisionObjectBullet *gObj = static_cast<CollisionObjectBullet *>(btResult.m_collisionObject->getUserPointer());
		if (gObj) {
			r_result.shape = btResult.m_shapeId;
			r_result.rid = gObj->get_self();
			r_result.collider_id = gObj->get_instance_id();
			r_result.collider = r_result.collider_id.is_null() ? nullptr : ObjectDB::get_instance(r_result.collider_id);
		} else {
			WARN_PRINT("The raycast performed has hit a collision object that is not part of Godot scene, please check it.");
		}
		return true;
	} else {
		return false;
	}
}

int BulletPhysicsDirectSpaceState::intersect_shape(const RID &p_shape, const Transform &p_xform, real_t p_margin, ShapeResult *r_results, int p_result_max, const Set<RID> &p_exclude, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas) {
	if (p_result_max <= 0) {
		return 0;
	}

	ShapeBullet *shape = space->get_physics_server()->get_shape_owner()->getornull(p_shape);
	ERR_FAIL_COND_V(!shape, 0);

	btCollisionShape *btShape = shape->create_bt_shape(p_xform.basis.get_scale_abs(), p_margin);
	if (!btShape->isConvex()) {
		bulletdelete(btShape);
		ERR_PRINT("The shape is not a convex shape, then is not supported: shape type: " + itos(shape->get_type()));
		return 0;
	}
	btConvexShape *btConvex = static_cast<btConvexShape *>(btShape);

	btTransform bt_xform;
	G_TO_B(p_xform, bt_xform);
	UNSCALE_BT_BASIS(bt_xform);

	btCollisionObject collision_object;
	collision_object.setCollisionShape(btConvex);
	collision_object.setWorldTransform(bt_xform);

	GodotAllContactResultCallback btQuery(&collision_object, r_results, p_result_max, &p_exclude, p_collide_with_bodies, p_collide_with_areas);
	btQuery.m_collisionFilterGroup = 0;
	btQuery.m_collisionFilterMask = p_collision_mask;
	btQuery.m_closestDistanceThreshold = 0;
	space->dynamicsWorld->contactTest(&collision_object, btQuery);

	bulletdelete(btConvex);

	return btQuery.m_count;
}

bool BulletPhysicsDirectSpaceState::cast_motion(const RID &p_shape, const Transform &p_xform, const Vector3 &p_motion, real_t p_margin, real_t &r_closest_safe, real_t &r_closest_unsafe, const Set<RID> &p_exclude, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas, ShapeRestInfo *r_info) {
	r_closest_safe = 0.0f;
	r_closest_unsafe = 0.0f;
	btVector3 bt_motion;
	G_TO_B(p_motion, bt_motion);

	ShapeBullet *shape = space->get_physics_server()->get_shape_owner()->getornull(p_shape);
	ERR_FAIL_COND_V(!shape, false);

	btCollisionShape *btShape = shape->create_bt_shape(p_xform.basis.get_scale(), p_margin);
	if (!btShape->isConvex()) {
		bulletdelete(btShape);
		ERR_PRINT("The shape is not a convex shape, then is not supported: shape type: " + itos(shape->get_type()));
		return false;
	}
	btConvexShape *bt_convex_shape = static_cast<btConvexShape *>(btShape);

	btTransform bt_xform_from;
	G_TO_B(p_xform, bt_xform_from);
	UNSCALE_BT_BASIS(bt_xform_from);

	btTransform bt_xform_to(bt_xform_from);
	bt_xform_to.getOrigin() += bt_motion;

	if ((bt_xform_to.getOrigin() - bt_xform_from.getOrigin()).fuzzyZero()) {
		r_closest_safe = 1.0f;
		r_closest_unsafe = 1.0f;
		bulletdelete(btShape);
		return true;
	}

	GodotClosestConvexResultCallback btResult(bt_xform_from.getOrigin(), bt_xform_to.getOrigin(), &p_exclude, p_collide_with_bodies, p_collide_with_areas);
	btResult.m_collisionFilterGroup = 0;
	btResult.m_collisionFilterMask = p_collision_mask;

	space->dynamicsWorld->convexSweepTest(bt_convex_shape, bt_xform_from, bt_xform_to, btResult, space->dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration);

	if (btResult.hasHit()) {
		const btScalar l = bt_motion.length();
		r_closest_unsafe = btResult.m_closestHitFraction;
		r_closest_safe = MAX(r_closest_unsafe - (1 - ((l - 0.01) / l)), 0);
		if (r_info) {
			if (btCollisionObject::CO_RIGID_BODY == btResult.m_hitCollisionObject->getInternalType()) {
				B_TO_G(static_cast<const btRigidBody *>(btResult.m_hitCollisionObject)->getVelocityInLocalPoint(btResult.m_hitPointWorld), r_info->linear_velocity);
			}
			CollisionObjectBullet *collision_object = static_cast<CollisionObjectBullet *>(btResult.m_hitCollisionObject->getUserPointer());
			B_TO_G(btResult.m_hitPointWorld, r_info->point);
			B_TO_G(btResult.m_hitNormalWorld, r_info->normal);
			r_info->rid = collision_object->get_self();
			r_info->collider_id = collision_object->get_instance_id();
			r_info->shape = btResult.m_shapeId;
		}
	} else {
		r_closest_safe = 1.0f;
		r_closest_unsafe = 1.0f;
	}

	bulletdelete(bt_convex_shape);
	return true; // Mean success
}

/// Returns the list of contacts pairs in this order: Local contact, other body contact
bool BulletPhysicsDirectSpaceState::collide_shape(RID p_shape, const Transform &p_shape_xform, real_t p_margin, Vector3 *r_results, int p_result_max, int &r_result_count, const Set<RID> &p_exclude, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas) {
	if (p_result_max <= 0) {
		return false;
	}

	ShapeBullet *shape = space->get_physics_server()->get_shape_owner()->getornull(p_shape);
	ERR_FAIL_COND_V(!shape, false);

	btCollisionShape *btShape = shape->create_bt_shape(p_shape_xform.basis.get_scale_abs(), p_margin);
	if (!btShape->isConvex()) {
		bulletdelete(btShape);
		ERR_PRINT("The shape is not a convex shape, then is not supported: shape type: " + itos(shape->get_type()));
		return false;
	}
	btConvexShape *btConvex = static_cast<btConvexShape *>(btShape);

	btTransform bt_xform;
	G_TO_B(p_shape_xform, bt_xform);
	UNSCALE_BT_BASIS(bt_xform);

	btCollisionObject collision_object;
	collision_object.setCollisionShape(btConvex);
	collision_object.setWorldTransform(bt_xform);

	GodotContactPairContactResultCallback btQuery(&collision_object, r_results, p_result_max, &p_exclude, p_collide_with_bodies, p_collide_with_areas);
	btQuery.m_collisionFilterGroup = 0;
	btQuery.m_collisionFilterMask = p_collision_mask;
	btQuery.m_closestDistanceThreshold = 0;
	space->dynamicsWorld->contactTest(&collision_object, btQuery);

	r_result_count = btQuery.m_count;
	bulletdelete(btConvex);

	return btQuery.m_count;
}

bool BulletPhysicsDirectSpaceState::rest_info(RID p_shape, const Transform &p_shape_xform, real_t p_margin, ShapeRestInfo *r_info, const Set<RID> &p_exclude, uint32_t p_collision_mask, bool p_collide_with_bodies, bool p_collide_with_areas) {
	ShapeBullet *shape = space->get_physics_server()->get_shape_owner()->getornull(p_shape);
	ERR_FAIL_COND_V(!shape, false);

	btCollisionShape *btShape = shape->create_bt_shape(p_shape_xform.basis.get_scale_abs(), p_margin);
	if (!btShape->isConvex()) {
		bulletdelete(btShape);
		ERR_PRINT("The shape is not a convex shape, then is not supported: shape type: " + itos(shape->get_type()));
		return false;
	}
	btConvexShape *btConvex = static_cast<btConvexShape *>(btShape);

	btTransform bt_xform;
	G_TO_B(p_shape_xform, bt_xform);
	UNSCALE_BT_BASIS(bt_xform);

	btCollisionObject collision_object;
	collision_object.setCollisionShape(btConvex);
	collision_object.setWorldTransform(bt_xform);

	GodotRestInfoContactResultCallback btQuery(&collision_object, r_info, &p_exclude, p_collide_with_bodies, p_collide_with_areas);
	btQuery.m_collisionFilterGroup = 0;
	btQuery.m_collisionFilterMask = p_collision_mask;
	btQuery.m_closestDistanceThreshold = 0;
	space->dynamicsWorld->contactTest(&collision_object, btQuery);

	bulletdelete(btConvex);

	if (btQuery.m_collided) {
		if (btCollisionObject::CO_RIGID_BODY == btQuery.m_rest_info_collision_object->getInternalType()) {
			B_TO_G(static_cast<const btRigidBody *>(btQuery.m_rest_info_collision_object)->getVelocityInLocalPoint(btQuery.m_rest_info_bt_point), r_info->linear_velocity);
		}
		B_TO_G(btQuery.m_rest_info_bt_point, r_info->point);
	}

	return btQuery.m_collided;
}

Vector3 BulletPhysicsDirectSpaceState::get_closest_point_to_object_volume(RID p_object, const Vector3 p_point) const {
	RigidCollisionObjectBullet *rigid_object = space->get_physics_server()->get_rigid_collision_object(p_object);
	ERR_FAIL_COND_V(!rigid_object, Vector3());

	btVector3 out_closest_point(0, 0, 0);
	btScalar out_distance = 1e20;

	btVector3 bt_point;
	G_TO_B(p_point, bt_point);

	btSphereShape point_shape(0.);

	btCollisionShape *shape;
	btConvexShape *convex_shape;
	btTransform child_transform;
	btTransform body_transform(rigid_object->get_bt_collision_object()->getWorldTransform());

	btGjkPairDetector::ClosestPointInput input;
	input.m_transformA.getBasis().setIdentity();
	input.m_transformA.setOrigin(bt_point);

	bool shapes_found = false;

	for (int i = rigid_object->get_shape_count() - 1; 0 <= i; --i) {
		shape = rigid_object->get_bt_shape(i);
		if (shape->isConvex()) {
			child_transform = rigid_object->get_bt_shape_transform(i);
			convex_shape = static_cast<btConvexShape *>(shape);

			input.m_transformB = body_transform * child_transform;

			btPointCollector result;
			btGjkPairDetector gjk_pair_detector(&point_shape, convex_shape, space->gjk_simplex_solver, space->gjk_epa_pen_solver);
			gjk_pair_detector.getClosestPoints(input, result, nullptr);

			if (out_distance > result.m_distance) {
				out_distance = result.m_distance;
				out_closest_point = result.m_pointInWorld;
			}
		}
		shapes_found = true;
	}

	if (shapes_found) {
		Vector3 out;
		B_TO_G(out_closest_point, out);
		return out;
	} else {
		// no shapes found, use distance to origin.
		return rigid_object->get_transform().get_origin();
	}
}

SpaceBullet::SpaceBullet() {
	create_empty_world(GLOBAL_DEF("physics/3d/active_soft_world", true));
	direct_access = memnew(BulletPhysicsDirectSpaceState(this));
}

SpaceBullet::~SpaceBullet() {
	memdelete(direct_access);
	destroy_world();
}

void SpaceBullet::flush_queries() {
	const btCollisionObjectArray &colObjArray = dynamicsWorld->getCollisionObjectArray();
	for (int i = colObjArray.size() - 1; 0 <= i; --i) {
		static_cast<CollisionObjectBullet *>(colObjArray[i]->getUserPointer())->dispatch_callbacks();
	}
}

void SpaceBullet::step(real_t p_delta_time) {
	delta_time = p_delta_time;
	dynamicsWorld->stepSimulation(p_delta_time, 0, 0);
}

void SpaceBullet::set_param(PhysicsServer3D::AreaParameter p_param, const Variant &p_value) {
	assert(dynamicsWorld);

	switch (p_param) {
		case PhysicsServer3D::AREA_PARAM_GRAVITY:
			gravityMagnitude = p_value;
			update_gravity();
			break;
		case PhysicsServer3D::AREA_PARAM_GRAVITY_VECTOR:
			gravityDirection = p_value;
			update_gravity();
			break;
		case PhysicsServer3D::AREA_PARAM_LINEAR_DAMP:
			linear_damp = p_value;
			break;
		case PhysicsServer3D::AREA_PARAM_ANGULAR_DAMP:
			angular_damp = p_value;
			break;
		case PhysicsServer3D::AREA_PARAM_PRIORITY:
			// Priority is always 0, the lower
			break;
		case PhysicsServer3D::AREA_PARAM_GRAVITY_IS_POINT:
		case PhysicsServer3D::AREA_PARAM_GRAVITY_DISTANCE_SCALE:
		case PhysicsServer3D::AREA_PARAM_GRAVITY_POINT_ATTENUATION:
			break;
		default:
			WARN_PRINT("This set parameter (" + itos(p_param) + ") is ignored, the SpaceBullet doesn't support it.");
			break;
	}
}

Variant SpaceBullet::get_param(PhysicsServer3D::AreaParameter p_param) {
	switch (p_param) {
		case PhysicsServer3D::AREA_PARAM_GRAVITY:
			return gravityMagnitude;
		case PhysicsServer3D::AREA_PARAM_GRAVITY_VECTOR:
			return gravityDirection;
		case PhysicsServer3D::AREA_PARAM_LINEAR_DAMP:
			return linear_damp;
		case PhysicsServer3D::AREA_PARAM_ANGULAR_DAMP:
			return angular_damp;
		case PhysicsServer3D::AREA_PARAM_PRIORITY:
			return 0; // Priority is always 0, the lower
		case PhysicsServer3D::AREA_PARAM_GRAVITY_IS_POINT:
			return false;
		case PhysicsServer3D::AREA_PARAM_GRAVITY_DISTANCE_SCALE:
			return 0;
		case PhysicsServer3D::AREA_PARAM_GRAVITY_POINT_ATTENUATION:
			return 0;
		default:
			WARN_PRINT("This get parameter (" + itos(p_param) + ") is ignored, the SpaceBullet doesn't support it.");
			return Variant();
	}
}

void SpaceBullet::set_param(PhysicsServer3D::SpaceParameter p_param, real_t p_value) {
	switch (p_param) {
		case PhysicsServer3D::SPACE_PARAM_CONTACT_RECYCLE_RADIUS:
		case PhysicsServer3D::SPACE_PARAM_CONTACT_MAX_SEPARATION:
		case PhysicsServer3D::SPACE_PARAM_BODY_MAX_ALLOWED_PENETRATION:
		case PhysicsServer3D::SPACE_PARAM_BODY_LINEAR_VELOCITY_SLEEP_THRESHOLD:
		case PhysicsServer3D::SPACE_PARAM_BODY_ANGULAR_VELOCITY_SLEEP_THRESHOLD:
		case PhysicsServer3D::SPACE_PARAM_BODY_TIME_TO_SLEEP:
		case PhysicsServer3D::SPACE_PARAM_BODY_ANGULAR_VELOCITY_DAMP_RATIO:
		case PhysicsServer3D::SPACE_PARAM_CONSTRAINT_DEFAULT_BIAS:
		default:
			WARN_PRINT("This set parameter (" + itos(p_param) + ") is ignored, the SpaceBullet doesn't support it.");
			break;
	}
}

real_t SpaceBullet::get_param(PhysicsServer3D::SpaceParameter p_param) {
	switch (p_param) {
		case PhysicsServer3D::SPACE_PARAM_CONTACT_RECYCLE_RADIUS:
		case PhysicsServer3D::SPACE_PARAM_CONTACT_MAX_SEPARATION:
		case PhysicsServer3D::SPACE_PARAM_BODY_MAX_ALLOWED_PENETRATION:
		case PhysicsServer3D::SPACE_PARAM_BODY_LINEAR_VELOCITY_SLEEP_THRESHOLD:
		case PhysicsServer3D::SPACE_PARAM_BODY_ANGULAR_VELOCITY_SLEEP_THRESHOLD:
		case PhysicsServer3D::SPACE_PARAM_BODY_TIME_TO_SLEEP:
		case PhysicsServer3D::SPACE_PARAM_BODY_ANGULAR_VELOCITY_DAMP_RATIO:
		case PhysicsServer3D::SPACE_PARAM_CONSTRAINT_DEFAULT_BIAS:
		default:
			WARN_PRINT("The SpaceBullet  doesn't support this get parameter (" + itos(p_param) + "), 0 is returned.");
			return 0.f;
	}
}

void SpaceBullet::add_area(AreaBullet *p_area) {
	areas.push_back(p_area);
	dynamicsWorld->addCollisionObject(p_area->get_bt_ghost(), p_area->get_collision_layer(), p_area->get_collision_mask());
}

void SpaceBullet::remove_area(AreaBullet *p_area) {
	areas.erase(p_area);
	dynamicsWorld->removeCollisionObject(p_area->get_bt_ghost());
}

void SpaceBullet::reload_collision_filters(AreaBullet *p_area) {
	btGhostObject *ghost_object = p_area->get_bt_ghost();

	btBroadphaseProxy *ghost_proxy = ghost_object->getBroadphaseHandle();
	ghost_proxy->m_collisionFilterGroup = p_area->get_collision_layer();
	ghost_proxy->m_collisionFilterMask = p_area->get_collision_mask();

	dynamicsWorld->refreshBroadphaseProxy(ghost_object);
}

void SpaceBullet::add_rigid_body(RigidBodyBullet *p_body) {
	if (p_body->is_static()) {
		dynamicsWorld->addCollisionObject(p_body->get_bt_rigid_body(), p_body->get_collision_layer(), p_body->get_collision_mask());
	} else {
		dynamicsWorld->addRigidBody(p_body->get_bt_rigid_body(), p_body->get_collision_layer(), p_body->get_collision_mask());
		p_body->scratch_space_override_modificator();
	}
}

void SpaceBullet::remove_rigid_body_constraints(RigidBodyBullet *p_body) {
	btRigidBody *btBody = p_body->get_bt_rigid_body();

	int constraints = btBody->getNumConstraintRefs();
	if (constraints > 0) {
		ERR_PRINT("A body connected to joints was removed.");
		for (int i = 0; i < constraints; i++) {
			dynamicsWorld->removeConstraint(btBody->getConstraintRef(i));
		}
	}
}

void SpaceBullet::remove_rigid_body(RigidBodyBullet *p_body) {
	btRigidBody *btBody = p_body->get_bt_rigid_body();

	if (p_body->is_static()) {
		dynamicsWorld->removeCollisionObject(btBody);
	} else {
		dynamicsWorld->removeRigidBody(btBody);
	}
}

void SpaceBullet::reload_collision_filters(RigidBodyBullet *p_body) {
	btRigidBody *rigid_body = p_body->get_bt_rigid_body();

	btBroadphaseProxy *body_proxy = rigid_body->getBroadphaseProxy();
	body_proxy->m_collisionFilterGroup = p_body->get_collision_layer();
	body_proxy->m_collisionFilterMask = p_body->get_collision_mask();

	dynamicsWorld->refreshBroadphaseProxy(rigid_body);
}

void SpaceBullet::add_soft_body(SoftBodyBullet *p_body) {
	if (is_using_soft_world()) {
		if (p_body->get_bt_soft_body()) {
			p_body->get_bt_soft_body()->m_worldInfo = get_soft_body_world_info();
			static_cast<btSoftRigidDynamicsWorld *>(dynamicsWorld)->addSoftBody(p_body->get_bt_soft_body(), p_body->get_collision_layer(), p_body->get_collision_mask());
		}
	} else {
		ERR_PRINT("This soft body can't be added to non soft world");
	}
}

void SpaceBullet::remove_soft_body(SoftBodyBullet *p_body) {
	if (is_using_soft_world()) {
		if (p_body->get_bt_soft_body()) {
			static_cast<btSoftRigidDynamicsWorld *>(dynamicsWorld)->removeSoftBody(p_body->get_bt_soft_body());
			p_body->get_bt_soft_body()->m_worldInfo = nullptr;
		}
	}
}

void SpaceBullet::reload_collision_filters(SoftBodyBullet *p_body) {
	// This is necessary to change collision filter
	remove_soft_body(p_body);
	add_soft_body(p_body);
}

void SpaceBullet::add_constraint(ConstraintBullet *p_constraint, bool disableCollisionsBetweenLinkedBodies) {
	p_constraint->set_space(this);
	dynamicsWorld->addConstraint(p_constraint->get_bt_constraint(), disableCollisionsBetweenLinkedBodies);
}

void SpaceBullet::remove_constraint(ConstraintBullet *p_constraint) {
	dynamicsWorld->removeConstraint(p_constraint->get_bt_constraint());
}

int SpaceBullet::get_num_collision_objects() const {
	return dynamicsWorld->getNumCollisionObjects();
}

void SpaceBullet::remove_all_collision_objects() {
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; 0 <= i; --i) {
		btCollisionObject *btObj = dynamicsWorld->getCollisionObjectArray()[i];
		CollisionObjectBullet *colObj = static_cast<CollisionObjectBullet *>(btObj->getUserPointer());
		colObj->set_space(nullptr);
	}
}

void onBulletTickCallback(btDynamicsWorld *p_dynamicsWorld, btScalar timeStep) {
	const btCollisionObjectArray &colObjArray = p_dynamicsWorld->getCollisionObjectArray();

	// Notify all Collision objects the collision checker is started
	for (int i = colObjArray.size() - 1; 0 <= i; --i) {
		static_cast<CollisionObjectBullet *>(colObjArray[i]->getUserPointer())->on_collision_checker_start();
	}

	SpaceBullet *sb = static_cast<SpaceBullet *>(p_dynamicsWorld->getWorldUserInfo());
	sb->check_ghost_overlaps();
	sb->check_body_collision();

	for (int i = colObjArray.size() - 1; 0 <= i; --i) {
		static_cast<CollisionObjectBullet *>(colObjArray[i]->getUserPointer())->on_collision_checker_end();
	}
}

BulletPhysicsDirectSpaceState *SpaceBullet::get_direct_state() {
	return direct_access;
}

btScalar calculateGodotCombinedRestitution(const btCollisionObject *body0, const btCollisionObject *body1) {
	return CLAMP(body0->getRestitution() + body1->getRestitution(), 0, 1);
}

btScalar calculateGodotCombinedFriction(const btCollisionObject *body0, const btCollisionObject *body1) {
	return ABS(MIN(body0->getFriction(), body1->getFriction()));
}

void SpaceBullet::create_empty_world(bool p_create_soft_world) {
	gjk_epa_pen_solver = bulletnew(btGjkEpaPenetrationDepthSolver);
	gjk_simplex_solver = bulletnew(btVoronoiSimplexSolver);

	void *world_mem;
	if (p_create_soft_world) {
		world_mem = malloc(sizeof(btSoftRigidDynamicsWorld));
	} else {
		world_mem = malloc(sizeof(btDiscreteDynamicsWorld));
	}

	ERR_FAIL_COND_MSG(!world_mem, "Out of memory.");

	if (p_create_soft_world) {
		collisionConfiguration = bulletnew(GodotSoftCollisionConfiguration(static_cast<btDiscreteDynamicsWorld *>(world_mem)));
	} else {
		collisionConfiguration = bulletnew(GodotCollisionConfiguration(static_cast<btDiscreteDynamicsWorld *>(world_mem)));
	}

	dispatcher = bulletnew(GodotCollisionDispatcher(collisionConfiguration));
	broadphase = bulletnew(btDbvtBroadphase);
	solver = bulletnew(btSequentialImpulseConstraintSolver);

	if (p_create_soft_world) {
		dynamicsWorld = new (world_mem) btSoftRigidDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
		soft_body_world_info = bulletnew(btSoftBodyWorldInfo);
	} else {
		dynamicsWorld = new (world_mem) btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	}

	ghostPairCallback = bulletnew(btGhostPairCallback);
	godotFilterCallback = bulletnew(GodotFilterCallback);
	gCalculateCombinedRestitutionCallback = &calculateGodotCombinedRestitution;
	gCalculateCombinedFrictionCallback = &calculateGodotCombinedFriction;
	gContactAddedCallback = &godotContactAddedCallback;

	dynamicsWorld->setWorldUserInfo(this);

	dynamicsWorld->setInternalTickCallback(onBulletTickCallback, this, false);
	dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(ghostPairCallback); // Setup ghost check
	dynamicsWorld->getPairCache()->setOverlapFilterCallback(godotFilterCallback);

	if (soft_body_world_info) {
		soft_body_world_info->m_broadphase = broadphase;
		soft_body_world_info->m_dispatcher = dispatcher;
		soft_body_world_info->m_sparsesdf.Initialize();
	}

	update_gravity();
}

void SpaceBullet::destroy_world() {
	/// The world elements (like: Collision Objects, Constraints, Shapes) are managed by godot

	dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(nullptr);
	dynamicsWorld->getPairCache()->setOverlapFilterCallback(nullptr);

	bulletdelete(ghostPairCallback);
	bulletdelete(godotFilterCallback);

	// Deallocate world
	dynamicsWorld->~btDiscreteDynamicsWorld();
	free(dynamicsWorld);
	dynamicsWorld = nullptr;

	bulletdelete(solver);
	bulletdelete(broadphase);
	bulletdelete(dispatcher);
	bulletdelete(collisionConfiguration);
	bulletdelete(soft_body_world_info);
	bulletdelete(gjk_simplex_solver);
	bulletdelete(gjk_epa_pen_solver);
}

void SpaceBullet::check_ghost_overlaps() {
	/// Algorithm support variables
	btCollisionShape *other_body_shape;
	btConvexShape *area_shape;
	btGjkPairDetector::ClosestPointInput gjk_input;
	AreaBullet *area;
	int x(-1), i(-1), y(-1), z(-1), indexOverlap(-1);

	/// For each areas
	for (x = areas.size() - 1; 0 <= x; --x) {
		area = areas[x];

		btVector3 area_scale(area->get_bt_body_scale());

		if (!area->is_monitoring()) {
			continue;
		}

		/// 1. Reset all states
		for (i = area->overlappingObjects.size() - 1; 0 <= i; --i) {
			AreaBullet::OverlappingObjectData &otherObj = area->overlappingObjects.write[i];
			// This check prevent the overwrite of ENTER state
			// if this function is called more times before dispatchCallbacks
			if (otherObj.state != AreaBullet::OVERLAP_STATE_ENTER) {
				otherObj.state = AreaBullet::OVERLAP_STATE_DIRTY;
			}
		}

		/// 2. Check all overlapping objects using GJK

		const btAlignedObjectArray<btCollisionObject *> ghostOverlaps = area->get_bt_ghost()->getOverlappingPairs();

		// For each overlapping
		for (i = ghostOverlaps.size() - 1; 0 <= i; --i) {
			bool hasOverlap = false;
			btCollisionObject *overlapped_bt_co = ghostOverlaps[i];
			RigidCollisionObjectBullet *otherObject = static_cast<RigidCollisionObjectBullet *>(overlapped_bt_co->getUserPointer());
			btVector3 other_body_scale(otherObject->get_bt_body_scale());

			if (!area->is_transform_changed() && !otherObject->is_transform_changed()) {
				hasOverlap = -1 != area->find_overlapping_object(otherObject);
				goto collision_found;
			}

			if (overlapped_bt_co->getUserIndex() == CollisionObjectBullet::TYPE_AREA) {
				if (!static_cast<AreaBullet *>(overlapped_bt_co->getUserPointer())->is_monitorable()) {
					continue;
				}
			} else if (overlapped_bt_co->getUserIndex() != CollisionObjectBullet::TYPE_RIGID_BODY) {
				continue;
			}

			// For each area shape
			for (y = area->get_shape_count() - 1; 0 <= y; --y) {
				if (!area->get_bt_shape(y)->isConvex()) {
					continue;
				}

				btTransform area_shape_treansform(area->get_bt_shape_transform(y));
				area_shape_treansform.getOrigin() *= area_scale;

				gjk_input.m_transformA =
						area->get_transform__bullet() *
						area_shape_treansform;

				area_shape = static_cast<btConvexShape *>(area->get_bt_shape(y));

				// For each other object shape
				for (z = otherObject->get_shape_count() - 1; 0 <= z; --z) {
					other_body_shape = static_cast<btCollisionShape *>(otherObject->get_bt_shape(z));

					btTransform other_shape_transform(otherObject->get_bt_shape_transform(z));
					other_shape_transform.getOrigin() *= other_body_scale;

					gjk_input.m_transformB =
							otherObject->get_transform__bullet() *
							other_shape_transform;

					if (other_body_shape->isConvex()) {
						btPointCollector result;
						btGjkPairDetector gjk_pair_detector(
								area_shape,
								static_cast<btConvexShape *>(other_body_shape),
								gjk_simplex_solver,
								gjk_epa_pen_solver);
						gjk_pair_detector.getClosestPoints(gjk_input, result, nullptr);

						if (0 >= result.m_distance) {
							hasOverlap = true;
							goto collision_found;
						}

					} else {
						btCollisionObjectWrapper obA(nullptr, area_shape, area->get_bt_ghost(), gjk_input.m_transformA, -1, y);
						btCollisionObjectWrapper obB(nullptr, other_body_shape, otherObject->get_bt_collision_object(), gjk_input.m_transformB, -1, z);

						btCollisionAlgorithm *algorithm = dispatcher->findAlgorithm(&obA, &obB, nullptr, BT_CONTACT_POINT_ALGORITHMS);

						if (!algorithm) {
							continue;
						}

						GodotDeepPenetrationContactResultCallback contactPointResult(&obA, &obB);
						algorithm->processCollision(&obA, &obB, dynamicsWorld->getDispatchInfo(), &contactPointResult);

						algorithm->~btCollisionAlgorithm();
						dispatcher->freeCollisionAlgorithm(algorithm);

						if (contactPointResult.hasHit()) {
							hasOverlap = true;
							goto collision_found;
						}
					}

				} // ~For each other object shape
			} // ~For each area shape

		collision_found:
			if (!hasOverlap) {
				continue;
			}

			indexOverlap = area->find_overlapping_object(otherObject);
			if (-1 == indexOverlap) {
				// Not found
				area->add_overlap(otherObject);
			} else {
				// Found
				area->put_overlap_as_inside(indexOverlap);
			}
		}

		/// 3. Remove not overlapping
		for (i = area->overlappingObjects.size() - 1; 0 <= i; --i) {
			// If the overlap has DIRTY state it means that it's no more overlapping
			if (area->overlappingObjects[i].state == AreaBullet::OVERLAP_STATE_DIRTY) {
				area->put_overlap_as_exit(i);
			}
		}
	}
}

void SpaceBullet::check_body_collision() {
#ifdef DEBUG_ENABLED
	reset_debug_contact_count();
#endif

	const int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; ++i) {
		btPersistentManifold *contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

		// I know this static cast is a bit risky. But I'm checking its type just after it.
		// This allow me to avoid a lot of other cast and checks
		RigidBodyBullet *bodyA = static_cast<RigidBodyBullet *>(contactManifold->getBody0()->getUserPointer());
		RigidBodyBullet *bodyB = static_cast<RigidBodyBullet *>(contactManifold->getBody1()->getUserPointer());

		if (CollisionObjectBullet::TYPE_RIGID_BODY == bodyA->getType() && CollisionObjectBullet::TYPE_RIGID_BODY == bodyB->getType()) {
			if (!bodyA->can_add_collision() && !bodyB->can_add_collision()) {
				continue;
			}

			const int numContacts = contactManifold->getNumContacts();

			/// Since I don't need report all contacts for these objects,
			/// So report only the first
#define REPORT_ALL_CONTACTS 0
#if REPORT_ALL_CONTACTS
			for (int j = 0; j < numContacts; j++) {
				btManifoldPoint &pt = contactManifold->getContactPoint(j);
#else
			if (numContacts) {
				btManifoldPoint &pt = contactManifold->getContactPoint(0);
#endif
				if (
						pt.getDistance() <= 0.0 ||
						bodyA->was_colliding(bodyB) ||
						bodyB->was_colliding(bodyA)) {
					Vector3 collisionWorldPosition;
					Vector3 collisionLocalPosition;
					Vector3 normalOnB;
					real_t appliedImpulse = pt.m_appliedImpulse;
					B_TO_G(pt.m_normalWorldOnB, normalOnB);

					// The pt.m_index only contains the shape index when more than one collision shape is used
					// and only if the collision shape is not a concave collision shape.
					// A value of -1 in pt.m_partId indicates the pt.m_index is a shape index.
					int shape_index_a = 0;
					if (bodyA->get_shape_count() > 1 && pt.m_partId0 == -1) {
						shape_index_a = pt.m_index0;
					}
					int shape_index_b = 0;
					if (bodyB->get_shape_count() > 1 && pt.m_partId1 == -1) {
						shape_index_b = pt.m_index1;
					}

					if (bodyA->can_add_collision()) {
						B_TO_G(pt.getPositionWorldOnB(), collisionWorldPosition);
						/// pt.m_localPointB Doesn't report the exact point in local space
						B_TO_G(pt.getPositionWorldOnB() - contactManifold->getBody1()->getWorldTransform().getOrigin(), collisionLocalPosition);
						bodyA->add_collision_object(bodyB, collisionWorldPosition, collisionLocalPosition, normalOnB, appliedImpulse, shape_index_b, shape_index_a);
					}
					if (bodyB->can_add_collision()) {
						B_TO_G(pt.getPositionWorldOnA(), collisionWorldPosition);
						/// pt.m_localPointA Doesn't report the exact point in local space
						B_TO_G(pt.getPositionWorldOnA() - contactManifold->getBody0()->getWorldTransform().getOrigin(), collisionLocalPosition);
						bodyB->add_collision_object(bodyA, collisionWorldPosition, collisionLocalPosition, normalOnB * -1, appliedImpulse * -1, shape_index_a, shape_index_b);
					}

#ifdef DEBUG_ENABLED
					if (is_debugging_contacts()) {
						add_debug_contact(collisionWorldPosition);
					}
#endif
				}
			}
		}
	}
}

void SpaceBullet::update_gravity() {
	btVector3 btGravity;
	G_TO_B(gravityDirection * gravityMagnitude, btGravity);
	//dynamicsWorld->setGravity(btGravity);
	dynamicsWorld->setGravity(btVector3(0, 0, 0));
	if (soft_body_world_info) {
		soft_body_world_info->m_gravity = btGravity;
	}
}

bool SpaceBullet::test_body_motion(RigidBodyBullet *p_body, const Transform &p_from, const Vector3 &p_motion, bool p_infinite_inertia, PhysicsServer3D::MotionResult *r_motion_result, bool p_exclude_raycast_shapes) {
	bool is_colliding = false;
	btVector3 motion;
	G_TO_B(p_motion, motion);
	btTransform body_transform;
	G_TO_B(p_from, body_transform);
	UNSCALE_BT_BASIS(body_transform);

	// Step 1: Free body if stuck
	btVector3 recover_motion(0, 0, 0);
	{
		KinematicCollisionResult collision_result;
		kinematic_collision_test(p_body, body_transform, p_infinite_inertia, recover_motion, &collision_result);
		// Move body to a safe place.
		body_transform.getOrigin() += recover_motion;
	}

	// Step 2: Test motion.
	{
		const int shape_count(p_body->get_shape_count());

		for (int shape_idx = 0; shape_idx < shape_count; ++shape_idx) {
			if (p_body->is_shape_disabled(shape_idx)) {
				continue;
			}

			// Must be a convex shape.
			if (!p_body->get_bt_shape(shape_idx)->isConvex()) {
				continue;
			}

			// Skip ray shapes if specified.
			if (p_exclude_raycast_shapes && p_body->get_bt_shape(shape_idx)->getShapeType() == CUSTOM_CONVEX_SHAPE_TYPE) {
				continue;
			}

			btConvexShape *convex_shape(static_cast<btConvexShape *>(p_body->get_bt_shape(shape_idx)));
			btTransform shape_world_from = body_transform * p_body->get_kinematic_utilities()->shapes[shape_idx].transform;
			btTransform shape_world_to(shape_world_from);
			shape_world_to.getOrigin() += motion;

			if ((shape_world_to.getOrigin() - shape_world_from.getOrigin()).fuzzyZero()) {
				motion = btVector3(0, 0, 0);
				break;
			}

			GodotKinClosestConvexResultCallback btResult(shape_world_from.getOrigin(), shape_world_to.getOrigin(), p_body, p_infinite_inertia);
			btResult.m_collisionFilterGroup = p_body->get_collision_layer();
			btResult.m_collisionFilterMask = p_body->get_collision_mask();

			dynamicsWorld->convexSweepTest(convex_shape, shape_world_from, shape_world_to, btResult);

			if (btResult.hasHit()) {
				is_colliding = true;

				// Extract available collision information.
				// This is required, for shapes that aren't detected in the kinematic collision test.
				if (r_motion_result) {
					const btRigidBody *btRigid = static_cast<const btRigidBody *>(btResult.m_hitCollisionObject);
					CollisionObjectBullet *collisionObject = static_cast<CollisionObjectBullet *>(btRigid->getUserPointer());

					B_TO_G(btResult.m_hitPointWorld, r_motion_result->collision_point);
					B_TO_G(btResult.m_hitNormalWorld, r_motion_result->collision_normal);
					B_TO_G(btRigid->getVelocityInLocalPoint(btResult.m_hitPointWorld - btRigid->getWorldTransform().getOrigin()), r_motion_result->collider_velocity);
					r_motion_result->collider = collisionObject->get_self();
					r_motion_result->collider_id = collisionObject->get_instance_id();
					r_motion_result->collision_local_shape = shape_idx;
				}

				// Adjust the motion so only closer shapes will also hit.
				motion *= btResult.m_closestHitFraction;
			}
		}
	}

	if (r_motion_result) {
		if (is_colliding) {
			btVector3 discard_motion(0, 0, 0);
			btTransform collision_transform = body_transform;
			if (motion.fuzzyZero()) {
				btVector3 original_motion;
				G_TO_B(p_motion, original_motion);
				collision_transform.getOrigin() += original_motion * 0.1f;
			} else {
				collision_transform.getOrigin() += motion;
			}

			KinematicCollisionResult collision_result;
			if (kinematic_collision_test(p_body, collision_transform, p_infinite_inertia, discard_motion, &collision_result)) {
				// Extract collision information.
				const btRigidBody *btRigid = static_cast<const btRigidBody *>(collision_result.other_collision_object);
				CollisionObjectBullet *collisionObject = static_cast<CollisionObjectBullet *>(btRigid->getUserPointer());

				B_TO_G(collision_result.pointWorld, r_motion_result->collision_point);
				B_TO_G(collision_result.normal, r_motion_result->collision_normal);
				B_TO_G(btRigid->getVelocityInLocalPoint(collision_result.pointWorld - btRigid->getWorldTransform().getOrigin()), r_motion_result->collider_velocity);
				r_motion_result->collider = collisionObject->get_self();
				r_motion_result->collider_id = collisionObject->get_instance_id();
				r_motion_result->collider_shape = collision_result.other_compound_shape_index;
				r_motion_result->collision_local_shape = collision_result.local_shape_most_recovered;

				// Use binary search to find the safe motion.
				btScalar safe = 0.f;
				btScalar unsafe = 1.f;
				for (int step = 0; step < 8; step++) {
					btScalar mid_point = (safe + unsafe) * 0.5f;
					btTransform test_transform = body_transform;
					test_transform.getOrigin() += motion * mid_point;
					if (kinematic_collision_test(p_body, test_transform, p_infinite_inertia, discard_motion)) {
						unsafe = mid_point;
					} else {
						safe = mid_point;
					}
				}
				motion *= safe;
			}
		}

		B_TO_G(motion + recover_motion, r_motion_result->motion);
		r_motion_result->remainder = p_motion - r_motion_result->motion;
	}

	return is_colliding;
}

int SpaceBullet::test_ray_separation(RigidBodyBullet *p_body, const Transform &p_transform, bool p_infinite_inertia, Vector3 &r_recover_motion, PhysicsServer3D::SeparationResult *r_separation_results, int p_result_max, real_t p_margin) {
	btTransform body_transform;
	G_TO_B(p_transform, body_transform);
	UNSCALE_BT_BASIS(body_transform);
	btVector3 recover_motion(0, 0, 0);

	int rays_found = kinematic_ray_separation(p_body, body_transform, p_infinite_inertia, p_result_max, recover_motion, r_separation_results);

	B_TO_G(recover_motion, r_recover_motion);

	return rays_found;
}

struct KinematicBroadPhaseCallback : public btBroadphaseAabbCallback {
private:
	btDbvtVolume bounds;

	const btCollisionObject *self_collision_object;
	uint32_t collision_layer = 0;
	uint32_t collision_mask = 0;

	struct CompoundLeafCallback : btDbvt::ICollide {
	private:
		KinematicBroadPhaseCallback *parent_callback = nullptr;
		btCollisionObject *collision_object = nullptr;

	public:
		CompoundLeafCallback(KinematicBroadPhaseCallback *p_parent_callback, btCollisionObject *p_collision_object) :
				parent_callback(p_parent_callback),
				collision_object(p_collision_object) {
		}

		void Process(const btDbvtNode *leaf) {
			BroadphaseResult result;
			result.collision_object = collision_object;
			result.compound_child_index = leaf->dataAsInt;
			parent_callback->results.push_back(result);
		}
	};

public:
	struct BroadphaseResult {
		btCollisionObject *collision_object = nullptr;
		int compound_child_index = 0;
	};

	Vector<BroadphaseResult> results;

public:
	KinematicBroadPhaseCallback(const btCollisionObject *p_self_collision_object, uint32_t p_collision_layer, uint32_t p_collision_mask, btVector3 p_aabb_min, btVector3 p_aabb_max) :
			self_collision_object(p_self_collision_object),
			collision_layer(p_collision_layer),
			collision_mask(p_collision_mask) {
		bounds = btDbvtVolume::FromMM(p_aabb_min, p_aabb_max);
	}

	virtual ~KinematicBroadPhaseCallback() {}

	virtual bool process(const btBroadphaseProxy *proxy) {
		btCollisionObject *co = static_cast<btCollisionObject *>(proxy->m_clientObject);
		if (co->getInternalType() <= btCollisionObject::CO_RIGID_BODY) {
			if (self_collision_object != proxy->m_clientObject && GodotFilterCallback::test_collision_filters(collision_layer, collision_mask, proxy->m_collisionFilterGroup, proxy->m_collisionFilterMask)) {
				if (co->getCollisionShape()->isCompound()) {
					const btCompoundShape *cs = static_cast<btCompoundShape *>(co->getCollisionShape());

					if (cs->getNumChildShapes() > 1) {
						const btDbvt *tree = cs->getDynamicAabbTree();
						ERR_FAIL_COND_V(tree == nullptr, true);

						// Transform bounds into compound shape local space
						const btTransform other_in_compound_space = co->getWorldTransform().inverse();
						const btMatrix3x3 abs_b = other_in_compound_space.getBasis().absolute();
						const btVector3 local_center = other_in_compound_space(bounds.Center());
						const btVector3 local_extent = bounds.Extents().dot3(abs_b[0], abs_b[1], abs_b[2]);
						const btVector3 local_aabb_min = local_center - local_extent;
						const btVector3 local_aabb_max = local_center + local_extent;
						const btDbvtVolume local_bounds = btDbvtVolume::FromMM(local_aabb_min, local_aabb_max);

						// Test collision against compound child shapes using its AABB tree
						CompoundLeafCallback compound_leaf_callback(this, co);
						tree->collideTV(tree->m_root, local_bounds, compound_leaf_callback);
					} else {
						// If there's only a single child shape then there's no need to search any more, we know which child overlaps
						BroadphaseResult result;
						result.collision_object = co;
						result.compound_child_index = 0;
						results.push_back(result);
					}
				} else {
					BroadphaseResult result;
					result.collision_object = co;
					result.compound_child_index = -1;
					results.push_back(result);
				}
				return true;
			}
		}
		return false;
	}
};

bool SpaceBullet::kinematic_collision_test(RigidBodyBullet *p_body, const btTransform &p_body_position, bool p_infinite_inertia, btVector3 &r_recover_motion, KinematicCollisionResult *r_collision_result) {
	// Calculate the cumulative AABB of all shapes of the kinematic body
	btVector3 aabb_min, aabb_max;
	bool shapes_found = false;

	for (int kinIndex = p_body->get_kinematic_utilities()->shapes.size() - 1; 0 <= kinIndex; --kinIndex) {
		const RigidBodyBullet::KinematicShape &kin_shape(p_body->get_kinematic_utilities()->shapes[kinIndex]);
		if (!kin_shape.is_active()) {
			continue;
		}

		if (kin_shape.shape->getShapeType() == CUSTOM_CONVEX_SHAPE_TYPE) {
			// Skip rayshape in order to implement custom separation process
			continue;
		}

		btTransform shape_transform = p_body_position * kin_shape.transform;
		btVector3 shape_aabb_min, shape_aabb_max;
		kin_shape.shape->getAabb(shape_transform, shape_aabb_min, shape_aabb_max);

		if (!shapes_found) {
			aabb_min = shape_aabb_min;
			aabb_max = shape_aabb_max;
			shapes_found = true;
		} else {
			aabb_min.setX((aabb_min.x() < shape_aabb_min.x()) ? aabb_min.x() : shape_aabb_min.x());
			aabb_min.setY((aabb_min.y() < shape_aabb_min.y()) ? aabb_min.y() : shape_aabb_min.y());
			aabb_min.setZ((aabb_min.z() < shape_aabb_min.z()) ? aabb_min.z() : shape_aabb_min.z());

			aabb_max.setX((aabb_max.x() > shape_aabb_max.x()) ? aabb_max.x() : shape_aabb_max.x());
			aabb_max.setY((aabb_max.y() > shape_aabb_max.y()) ? aabb_max.y() : shape_aabb_max.y());
			aabb_max.setZ((aabb_max.z() > shape_aabb_max.z()) ? aabb_max.z() : shape_aabb_max.z());
		}
	}

	// If there are no shapes then there are no collisions either.
	if (!shapes_found) {
		return false;
	}

	// Perform broadphase test
	KinematicBroadPhaseCallback kinematic_broadphase_callback(p_body->get_bt_collision_object(), p_body->get_collision_layer(), p_body->get_collision_mask(), aabb_min, aabb_max);
	dynamicsWorld->getBroadphase()->aabbTest(aabb_min, aabb_max, kinematic_broadphase_callback);

	bool penetration = false;
	btScalar max_x = 0.f, max_y = 0.f, max_z = 0.f;
	btScalar min_x = 0.f, min_y = 0.f, min_z = 0.f;

	// Perform narrowphase per shape
	for (int kinIndex = p_body->get_kinematic_utilities()->shapes.size() - 1; 0 <= kinIndex; --kinIndex) {
		const RigidBodyBullet::KinematicShape &kin_shape(p_body->get_kinematic_utilities()->shapes[kinIndex]);
		if (!kin_shape.is_active()) {
			continue;
		}

		if (kin_shape.shape->getShapeType() == CUSTOM_CONVEX_SHAPE_TYPE) {
			// Skip rayshape in order to implement custom separation process
			continue;
		}

		btTransform shape_transform = p_body_position * kin_shape.transform;

		for (int i = kinematic_broadphase_callback.results.size() - 1; 0 <= i; --i) {
			btCollisionObject *otherObject = kinematic_broadphase_callback.results[i].collision_object;
			if (p_infinite_inertia && !otherObject->isStaticOrKinematicObject()) {
				// Wake up rigid or soft body.
				otherObject->activate();
				continue;
			} else if (!p_body->get_bt_collision_object()->checkCollideWith(otherObject) || !otherObject->checkCollideWith(p_body->get_bt_collision_object())) {
				continue;
			}

			btVector3 recover_motion(0, 0, 0);
			if (otherObject->getCollisionShape()->isCompound()) {
				const btCompoundShape *cs = static_cast<const btCompoundShape *>(otherObject->getCollisionShape());
				int child_shape_idx = kinematic_broadphase_callback.results[i].compound_child_index;
				ERR_FAIL_COND_V(child_shape_idx < 0 || child_shape_idx >= cs->getNumChildShapes(), false);

				if (cs->getChildShape(child_shape_idx)->isConvex()) {
					if (convex_convex_test(kin_shape.shape, static_cast<const btConvexShape *>(cs->getChildShape(child_shape_idx)), otherObject, kinIndex, child_shape_idx, shape_transform, otherObject->getWorldTransform() * cs->getChildTransform(child_shape_idx), recover_motion, r_collision_result)) {
						penetration = true;
					}
				} else {
					if (convex_world_test(kin_shape.shape, cs->getChildShape(child_shape_idx), p_body->get_bt_collision_object(), otherObject, kinIndex, child_shape_idx, shape_transform, otherObject->getWorldTransform() * cs->getChildTransform(child_shape_idx), recover_motion, r_collision_result)) {
						penetration = true;
					}
				}
			} else if (otherObject->getCollisionShape()->isConvex()) { /// Execute GJK test against object shape
				if (convex_convex_test(kin_shape.shape, static_cast<const btConvexShape *>(otherObject->getCollisionShape()), otherObject, kinIndex, 0, shape_transform, otherObject->getWorldTransform(), recover_motion, r_collision_result)) {
					penetration = true;
				}
			} else {
				if (convex_world_test(kin_shape.shape, otherObject->getCollisionShape(), p_body->get_bt_collision_object(), otherObject, kinIndex, 0, shape_transform, otherObject->getWorldTransform(), recover_motion, r_collision_result)) {
					penetration = true;
				}
			}

			if (recover_motion.getX() > max_x)
				max_x = recover_motion.getX();
			if (recover_motion.getY() > max_y)
				max_y = recover_motion.getY();
			if (recover_motion.getZ() > max_z)
				max_z = recover_motion.getZ();
			if (recover_motion.getX() < min_x)
				min_x = recover_motion.getX();
			if (recover_motion.getY() < min_y)
				min_y = recover_motion.getY();
			if (recover_motion.getZ() < min_z)
				min_z = recover_motion.getZ();
		}
	}

	r_recover_motion.setX(max_x + min_x);
	r_recover_motion.setY(max_y + min_y);
	r_recover_motion.setZ(max_z + min_z);

	return penetration;
}

bool SpaceBullet::convex_convex_test(const btConvexShape *p_shapeA, const btConvexShape *p_shapeB, btCollisionObject *p_objectB, int p_shapeId_A, int p_shapeId_B, const btTransform &p_transformA, const btTransform &p_transformB, btVector3 &r_recover_motion, KinematicCollisionResult *r_collision_result) {
	// Initialize GJK input
	btGjkPairDetector::ClosestPointInput gjk_input;
	gjk_input.m_transformA = p_transformA;
	gjk_input.m_transformB = p_transformB;

	// Perform GJK test
	btPointCollector point_collector;
	btGjkPairDetector gjk_pair_detector(p_shapeA, p_shapeB, gjk_simplex_solver, gjk_epa_pen_solver);
	gjk_pair_detector.getClosestPoints(gjk_input, point_collector, nullptr);

	// If distance between shapes is less than 0, the shapes are colliding.
	if (point_collector.m_distance < 0) {
		r_recover_motion = point_collector.m_normalOnBInWorld * -point_collector.m_distance;
		if (r_collision_result) {
			if (point_collector.m_distance < r_collision_result->penetration_distance) {
				r_collision_result->isColliding = true;
				r_collision_result->local_shape_most_recovered = p_shapeId_A;
				r_collision_result->other_collision_object = p_objectB;
				r_collision_result->other_compound_shape_index = p_shapeId_B;
				r_collision_result->penetration_distance = point_collector.m_distance;
				r_collision_result->pointWorld = point_collector.m_pointInWorld;
				r_collision_result->normal = point_collector.m_normalOnBInWorld;
			}
		}
		return true;
	}
	return false;
}

bool SpaceBullet::convex_world_test(const btConvexShape *p_shapeA, const btCollisionShape *p_shapeB, btCollisionObject *p_objectA, btCollisionObject *p_objectB, int p_shapeId_A, int p_shapeId_B, const btTransform &p_transformA, const btTransform &p_transformB, btVector3 &r_recover_motion, KinematicCollisionResult *r_collision_result) {
	btCollisionObjectWrapper obA(nullptr, p_shapeA, p_objectA, p_transformA, -1, p_shapeId_A);
	btCollisionObjectWrapper obB(nullptr, p_shapeB, p_objectB, p_transformB, -1, p_shapeId_B);
	btCollisionAlgorithm *algorithm = dispatcher->findAlgorithm(&obA, &obB, nullptr, BT_CONTACT_POINT_ALGORITHMS);

	if (algorithm) {
		// Discrete collision detection query
		GodotDeepPenetrationContactResultCallback contactPointResult(&obA, &obB);
		algorithm->processCollision(&obA, &obB, dynamicsWorld->getDispatchInfo(), &contactPointResult);

		algorithm->~btCollisionAlgorithm();
		dispatcher->freeCollisionAlgorithm(algorithm);

		if (contactPointResult.hasHit()) {
			r_recover_motion = contactPointResult.m_pointNormalWorld * -contactPointResult.m_penetration_distance;
			if (r_collision_result) {
				if (contactPointResult.m_penetration_distance < r_collision_result->penetration_distance) {
					r_collision_result->isColliding = true;
					r_collision_result->local_shape_most_recovered = p_shapeId_A;
					r_collision_result->other_collision_object = p_objectB;
					r_collision_result->other_compound_shape_index = p_shapeId_B;
					r_collision_result->penetration_distance = contactPointResult.m_penetration_distance;
					r_collision_result->pointWorld = contactPointResult.m_pointWorld;
					r_collision_result->normal = contactPointResult.m_pointNormalWorld;
				}
			}
			return true;
		}
	}
	return false;
}

int SpaceBullet::kinematic_ray_separation(RigidBodyBullet *p_body, const btTransform &p_body_position, bool p_infinite_inertia, int p_result_max, btVector3 &r_recover_motion, PhysicsServer3D::SeparationResult *r_separation_results) {
	// Calculate the cumulative AABB of all shapes of the kinematic body
	btVector3 aabb_min, aabb_max;
	bool shapes_found = false;

	for (int kinIndex = p_body->get_kinematic_utilities()->shapes.size() - 1; 0 <= kinIndex; --kinIndex) {
		const RigidBodyBullet::KinematicShape &kin_shape(p_body->get_kinematic_utilities()->shapes[kinIndex]);
		if (!kin_shape.is_active()) {
			continue;
		}

		if (kin_shape.shape->getShapeType() != CUSTOM_CONVEX_SHAPE_TYPE) {
			continue;
		}

		btTransform shape_transform = p_body_position * kin_shape.transform;

		btVector3 shape_aabb_min, shape_aabb_max;
		kin_shape.shape->getAabb(shape_transform, shape_aabb_min, shape_aabb_max);

		if (!shapes_found) {
			aabb_min = shape_aabb_min;
			aabb_max = shape_aabb_max;
			shapes_found = true;
		} else {
			aabb_min.setX((aabb_min.x() < shape_aabb_min.x()) ? aabb_min.x() : shape_aabb_min.x());
			aabb_min.setY((aabb_min.y() < shape_aabb_min.y()) ? aabb_min.y() : shape_aabb_min.y());
			aabb_min.setZ((aabb_min.z() < shape_aabb_min.z()) ? aabb_min.z() : shape_aabb_min.z());

			aabb_max.setX((aabb_max.x() > shape_aabb_max.x()) ? aabb_max.x() : shape_aabb_max.x());
			aabb_max.setY((aabb_max.y() > shape_aabb_max.y()) ? aabb_max.y() : shape_aabb_max.y());
			aabb_max.setZ((aabb_max.z() > shape_aabb_max.z()) ? aabb_max.z() : shape_aabb_max.z());
		}
	}

	// If there are no shapes then there are no collisions either.
	if (!shapes_found) {
		return 0;
	}

	// Perform broadphase test
	KinematicBroadPhaseCallback kinematic_broadphase_callback(p_body->get_bt_collision_object(), p_body->get_collision_layer(), p_body->get_collision_mask(), aabb_min, aabb_max);
	dynamicsWorld->getBroadphase()->aabbTest(aabb_min, aabb_max, kinematic_broadphase_callback);

	int ray_count = 0;
	btScalar max_x = 0.f, max_y = 0.f, max_z = 0.f;
	btScalar min_x = 0.f, min_y = 0.f, min_z = 0.f;

	// Perform narrowphase per shape
	for (int kinIndex = p_body->get_kinematic_utilities()->shapes.size() - 1; 0 <= kinIndex; --kinIndex) {
		if (ray_count >= p_result_max) {
			break;
		}

		const RigidBodyBullet::KinematicShape &kin_shape(p_body->get_kinematic_utilities()->shapes[kinIndex]);
		if (!kin_shape.is_active()) {
			continue;
		}

		if (kin_shape.shape->getShapeType() != CUSTOM_CONVEX_SHAPE_TYPE) {
			continue;
		}

		btTransform shape_transform = p_body_position * kin_shape.transform;

		for (int i = kinematic_broadphase_callback.results.size() - 1; 0 <= i; --i) {
			btCollisionObject *other_object = kinematic_broadphase_callback.results[i].collision_object;
			if (p_infinite_inertia && !other_object->isStaticOrKinematicObject()) {
				// Wake up rigid or soft boddy.
				other_object->activate();
				continue;
			} else if (!p_body->get_bt_collision_object()->checkCollideWith(other_object) || !other_object->checkCollideWith(p_body->get_bt_collision_object())) {
				continue;
			}

			btVector3 recover_motion(0, 0, 0);
			KinematicCollisionResult collision_result;
			if (other_object->getCollisionShape()->isCompound()) {
				const btCompoundShape *cs = static_cast<const btCompoundShape *>(other_object->getCollisionShape());
				int child_shape_idx = kinematic_broadphase_callback.results[i].compound_child_index;
				ERR_FAIL_COND_V(child_shape_idx < 0 || child_shape_idx >= cs->getNumChildShapes(), false);

				if (convex_world_test(kin_shape.shape, cs->getChildShape(child_shape_idx), p_body->get_bt_collision_object(), other_object, kinIndex, child_shape_idx, shape_transform, other_object->getWorldTransform() * cs->getChildTransform(child_shape_idx), recover_motion, &collision_result)) {
					ray_count += add_separation_result(collision_result, kinIndex, other_object, &r_separation_results[ray_count]);
				}
			} else {
				if (convex_world_test(kin_shape.shape, other_object->getCollisionShape(), p_body->get_bt_collision_object(), other_object, kinIndex, 0, shape_transform, other_object->getWorldTransform(), recover_motion, &collision_result)) {
					ray_count += add_separation_result(collision_result, kinIndex, other_object, &r_separation_results[ray_count]);
				}
			}

			if (recover_motion.getX() > max_x)
				max_x = recover_motion.getX();
			if (recover_motion.getY() > max_y)
				max_y = recover_motion.getY();
			if (recover_motion.getZ() > max_z)
				max_z = recover_motion.getZ();
			if (recover_motion.getX() < min_x)
				min_x = recover_motion.getX();
			if (recover_motion.getY() < min_y)
				min_y = recover_motion.getY();
			if (recover_motion.getZ() < min_z)
				min_z = recover_motion.getZ();
		}
	}

	r_recover_motion = btVector3(max_x + min_x, max_y + min_y, max_z + min_z);

	return ray_count;
}

int SpaceBullet::add_separation_result(const SpaceBullet::KinematicCollisionResult &p_collision_result, int p_shape_id, const btCollisionObject *p_other_object, PhysicsServer3D::SeparationResult *r_separation_results) const {
	// Optimize results by ignoring non-colliding shapes.
	if (p_collision_result.penetration_distance < 0.f) {
		const btRigidBody *btRigid = static_cast<const btRigidBody *>(p_other_object);
		CollisionObjectBullet *collisionObject = static_cast<CollisionObjectBullet *>(p_other_object->getUserPointer());

		r_separation_results->collision_depth = p_collision_result.penetration_distance;
		B_TO_G(p_collision_result.pointWorld, r_separation_results->collision_point);
		B_TO_G(p_collision_result.normal, r_separation_results->collision_normal);
		B_TO_G(btRigid->getVelocityInLocalPoint(p_collision_result.pointWorld - btRigid->getWorldTransform().getOrigin()), r_separation_results->collider_velocity);
		r_separation_results->collision_local_shape = p_shape_id;
		r_separation_results->collider_id = collisionObject->get_instance_id();
		r_separation_results->collider = collisionObject->get_self();
		r_separation_results->collider_shape = p_collision_result.other_compound_shape_index;

		return 1;
	}

	return 0;
}
