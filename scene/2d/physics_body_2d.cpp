/*************************************************************************/
/*  physics_body_2d.cpp                                                  */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
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

#include "physics_body_2d.h"

#include "core/config/engine.h"
#include "core/core_string_names.h"
#include "core/math/math_funcs.h"
#include "core/object/class_db.h"
#include "core/templates/list.h"
#include "core/templates/rid.h"
#include "scene/scene_string_names.h"

void PhysicsBody2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_mode", "mode"), &PhysicsBody2D::set_mode);
	ClassDB::bind_method(D_METHOD("get_mode"), &PhysicsBody2D::get_mode);
	ClassDB::bind_method(D_METHOD("set_physics_material_override", "physics_material_override"), &PhysicsBody2D::set_physics_material_override);
	ClassDB::bind_method(D_METHOD("get_physics_material_override"), &PhysicsBody2D::get_physics_material_override);
	ClassDB::bind_method(D_METHOD("set_mass", "mass"), &PhysicsBody2D::set_mass);
	ClassDB::bind_method(D_METHOD("get_mass"), &PhysicsBody2D::get_mass);
	ClassDB::bind_method(D_METHOD("set_inertia", "inertia"), &PhysicsBody2D::set_inertia);
	ClassDB::bind_method(D_METHOD("get_inertia"), &PhysicsBody2D::get_inertia);
	ClassDB::bind_method(D_METHOD("set_gravity_scale", "gravity_scale"), &PhysicsBody2D::set_gravity_scale);
	ClassDB::bind_method(D_METHOD("get_gravity_scale"), &PhysicsBody2D::get_gravity_scale);
	ClassDB::bind_method(D_METHOD("set_linear_velocity", "linear_velocity"), &PhysicsBody2D::set_linear_velocity);
	ClassDB::bind_method(D_METHOD("set_axis_velocity", "axis_velocity"), &PhysicsBody2D::set_axis_velocity);
	ClassDB::bind_method(D_METHOD("get_linear_velocity"), &PhysicsBody2D::get_linear_velocity);
	ClassDB::bind_method(D_METHOD("set_linear_damp", "linear_damp"), &PhysicsBody2D::set_linear_damp);
	ClassDB::bind_method(D_METHOD("get_linear_damp"), &PhysicsBody2D::get_linear_damp);
	ClassDB::bind_method(D_METHOD("set_angular_velocity", "angular_velocity"), &PhysicsBody2D::set_angular_velocity);
	ClassDB::bind_method(D_METHOD("get_angular_velocity"), &PhysicsBody2D::get_angular_velocity);
	ClassDB::bind_method(D_METHOD("set_angular_damp", "angular_damp"), &PhysicsBody2D::set_angular_damp);
	ClassDB::bind_method(D_METHOD("get_angular_damp"), &PhysicsBody2D::get_angular_damp);
	ClassDB::bind_method(D_METHOD("set_continuous_collision_detection_mode", "mode"), &PhysicsBody2D::set_continuous_collision_detection_mode);
	ClassDB::bind_method(D_METHOD("get_continuous_collision_detection_mode"), &PhysicsBody2D::get_continuous_collision_detection_mode);
	ClassDB::bind_method(D_METHOD("set_can_sleep", "able_to_sleep"), &PhysicsBody2D::set_can_sleep);
	ClassDB::bind_method(D_METHOD("is_able_to_sleep"), &PhysicsBody2D::is_able_to_sleep);
	ClassDB::bind_method(D_METHOD("set_sleeping", "sleeping"), &PhysicsBody2D::set_sleeping);
	ClassDB::bind_method(D_METHOD("is_sleeping"), &PhysicsBody2D::is_sleeping);
	ClassDB::bind_method(D_METHOD("set_use_custom_integrator", "enable"), &PhysicsBody2D::set_use_custom_integrator);
	ClassDB::bind_method(D_METHOD("is_using_custom_integrator"), &PhysicsBody2D::is_using_custom_integrator);
	ClassDB::bind_method(D_METHOD("set_contact_monitor", "enabled"), &PhysicsBody2D::set_contact_monitor);
	ClassDB::bind_method(D_METHOD("is_contact_monitor_enabled"), &PhysicsBody2D::is_contact_monitor_enabled);
	ClassDB::bind_method(D_METHOD("set_max_contacts_reported", "amount"), &PhysicsBody2D::set_max_contacts_reported);
	ClassDB::bind_method(D_METHOD("get_max_contacts_reported"), &PhysicsBody2D::get_max_contacts_reported);
	ClassDB::bind_method(D_METHOD("set_applied_force", "force"), &PhysicsBody2D::set_applied_force);
	ClassDB::bind_method(D_METHOD("get_applied_force"), &PhysicsBody2D::get_applied_force);
	ClassDB::bind_method(D_METHOD("set_applied_torque", "torque"), &PhysicsBody2D::set_applied_torque);
	ClassDB::bind_method(D_METHOD("get_applied_torque"), &PhysicsBody2D::get_applied_torque);
	ClassDB::bind_method(D_METHOD("add_force", "force", "position"), &PhysicsBody2D::add_force, Vector2());
	ClassDB::bind_method(D_METHOD("add_central_force", "force"), &PhysicsBody2D::add_central_force);
	ClassDB::bind_method(D_METHOD("add_torque", "torque"), &PhysicsBody2D::add_torque);
	ClassDB::bind_method(D_METHOD("apply_impulse", "impulse", "position"), &PhysicsBody2D::apply_impulse, Vector2());
	ClassDB::bind_method(D_METHOD("apply_central_impulse", "impulse"), &PhysicsBody2D::apply_central_impulse, Vector2());
	ClassDB::bind_method(D_METHOD("apply_torque_impulse", "torque"), &PhysicsBody2D::apply_torque_impulse);
	ClassDB::bind_method(D_METHOD("set_safe_margin", "pixels"), &PhysicsBody2D::set_safe_margin);
	ClassDB::bind_method(D_METHOD("get_safe_margin"), &PhysicsBody2D::get_safe_margin);
	ClassDB::bind_method(D_METHOD("set_sync_to_physics", "enable"), &PhysicsBody2D::set_sync_to_physics);
	ClassDB::bind_method(D_METHOD("is_sync_to_physics_enabled"), &PhysicsBody2D::is_sync_to_physics_enabled);
	ClassDB::bind_method(D_METHOD("set_collision_layer", "layer"), &PhysicsBody2D::set_collision_layer);
	ClassDB::bind_method(D_METHOD("get_collision_layer"), &PhysicsBody2D::get_collision_layer);
	ClassDB::bind_method(D_METHOD("set_collision_mask", "mask"), &PhysicsBody2D::set_collision_mask);
	ClassDB::bind_method(D_METHOD("get_collision_mask"), &PhysicsBody2D::get_collision_mask);
	ClassDB::bind_method(D_METHOD("set_collision_layer_bit", "bit", "value"), &PhysicsBody2D::set_collision_layer_bit);
	ClassDB::bind_method(D_METHOD("get_collision_layer_bit", "bit"), &PhysicsBody2D::get_collision_layer_bit);
	ClassDB::bind_method(D_METHOD("set_collision_mask_bit", "bit", "value"), &PhysicsBody2D::set_collision_mask_bit);
	ClassDB::bind_method(D_METHOD("get_collision_mask_bit", "bit"), &PhysicsBody2D::get_collision_mask_bit);
	ClassDB::bind_method(D_METHOD("get_collision_exceptions"), &PhysicsBody2D::get_collision_exceptions);
	ClassDB::bind_method(D_METHOD("add_collision_exception_with", "body"), &PhysicsBody2D::add_collision_exception_with);
	ClassDB::bind_method(D_METHOD("remove_collision_exception_with", "body"), &PhysicsBody2D::remove_collision_exception_with);
	ClassDB::bind_method(D_METHOD("get_colliding_bodies"), &PhysicsBody2D::get_colliding_bodies);
	ClassDB::bind_method(D_METHOD("is_on_floor"), &PhysicsBody2D::is_on_floor);
	ClassDB::bind_method(D_METHOD("is_on_wall"), &PhysicsBody2D::is_on_wall);
	ClassDB::bind_method(D_METHOD("is_on_ceiling"), &PhysicsBody2D::is_on_ceiling);
	ClassDB::bind_method(D_METHOD("get_floor_normal"), &PhysicsBody2D::get_floor_normal);
	ClassDB::bind_method(D_METHOD("get_floor_velocity"), &PhysicsBody2D::get_floor_velocity);
	ClassDB::bind_method(D_METHOD("test_move", "from", "rel_vec", "infinite_inertia"), &PhysicsBody2D::test_move, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("move_and_collide", "rel_vec", "infinite_inertia", "exclude_raycast_shapes", "test_only"), &PhysicsBody2D::_move, DEFVAL(true), DEFVAL(true), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("move_and_slide", "linear_velocity", "up_direction", "stop_on_slope", "max_slides", "floor_max_angle", "infinite_inertia"), &PhysicsBody2D::move_and_slide, DEFVAL(Vector2(0, 0)), DEFVAL(false), DEFVAL(4), DEFVAL(Math::deg2rad((float)45)), DEFVAL(true));
	ClassDB::bind_method(D_METHOD("move_and_slide_with_snap", "linear_velocity", "snap", "up_direction", "stop_on_slope", "max_slides", "floor_max_angle", "infinite_inertia"), &PhysicsBody2D::move_and_slide_with_snap, DEFVAL(Vector2(0, 0)), DEFVAL(false), DEFVAL(4), DEFVAL(Math::deg2rad((float)45)), DEFVAL(true));
	ClassDB::bind_method(D_METHOD("get_slide_count"), &PhysicsBody2D::get_slide_count);
	ClassDB::bind_method(D_METHOD("get_slide_collision", "slide_idx"), &PhysicsBody2D::_get_slide_collision);
	ClassDB::bind_method(D_METHOD("_test_motion", "motion", "infinite_inertia", "margin", "result"), &PhysicsBody2D::_test_motion, DEFVAL(true), DEFVAL(0.08), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("_direct_state_changed"), &PhysicsBody2D::_direct_state_changed);

	BIND_VMETHOD(MethodInfo("_integrate_forces", PropertyInfo(Variant::OBJECT, "state", PROPERTY_HINT_RESOURCE_TYPE, "PhysicsDirectBodyState2D")));

	ADD_PROPERTY(PropertyInfo(Variant::INT, "mode", PROPERTY_HINT_ENUM, "Rigid,Static,Character,Kinematic"), "set_mode", "get_mode");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "physics_material_override", PROPERTY_HINT_RESOURCE_TYPE, "PhysicsMaterial"), "set_physics_material_override", "get_physics_material_override");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mass", PROPERTY_HINT_EXP_RANGE, "0.01,65535,0.01"), "set_mass", "get_mass");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity_scale", PROPERTY_HINT_RANGE, "-128,128,0.01"), "set_gravity_scale", "get_gravity_scale");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "inertia", PROPERTY_HINT_EXP_RANGE, "0.01,65535,0.01", 0), "set_inertia", "get_inertia");
	ADD_GROUP("Linear", "linear_");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "linear_velocity"), "set_linear_velocity", "get_linear_velocity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "linear_damp", PROPERTY_HINT_RANGE, "-1,100,0.001,or_greater"), "set_linear_damp", "get_linear_damp");
	ADD_GROUP("Angular", "angular_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "angular_velocity"), "set_angular_velocity", "get_angular_velocity");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "angular_damp", PROPERTY_HINT_RANGE, "-1,100,0.001,or_greater"), "set_angular_damp", "get_angular_damp");
	ADD_GROUP("Rigid Mode", "rigid_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rigid_continuous_cd", PROPERTY_HINT_ENUM, "Disabled,Cast Ray,Cast Shape"), "set_continuous_collision_detection_mode", "get_continuous_collision_detection_mode");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "rigid_can_sleep"), "set_can_sleep", "is_able_to_sleep");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "rigid_sleeping"), "set_sleeping", "is_sleeping");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "rigid_custom_integrator"), "set_use_custom_integrator", "is_using_custom_integrator");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "rigid_contact_monitor"), "set_contact_monitor", "is_contact_monitor_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rigid_contacts_reported", PROPERTY_HINT_RANGE, "0,64,1,or_greater"), "set_max_contacts_reported", "get_max_contacts_reported");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "rigid_applied_force"), "set_applied_force", "get_applied_force");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rigid_applied_torque"), "set_applied_torque", "get_applied_torque");
	ADD_GROUP("Kinematic Mode", "kinematic_");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "kinematic_safe_margin", PROPERTY_HINT_RANGE, "0.001,256,0.001"), "set_safe_margin", "get_safe_margin");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "kinematic_sync_to_physics"), "set_sync_to_physics", "is_sync_to_physics_enabled");
	ADD_GROUP("Collision", "collision_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_layer", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_layer", "get_collision_layer");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_mask", "get_collision_mask");

	ADD_SIGNAL(MethodInfo("body_shape_entered", PropertyInfo(Variant::INT, "body_id"), PropertyInfo(Variant::OBJECT, "body", PROPERTY_HINT_RESOURCE_TYPE, "Node"), PropertyInfo(Variant::INT, "body_shape"), PropertyInfo(Variant::INT, "local_shape")));
	ADD_SIGNAL(MethodInfo("body_shape_exited", PropertyInfo(Variant::INT, "body_id"), PropertyInfo(Variant::OBJECT, "body", PROPERTY_HINT_RESOURCE_TYPE, "Node"), PropertyInfo(Variant::INT, "body_shape"), PropertyInfo(Variant::INT, "local_shape")));
	ADD_SIGNAL(MethodInfo("body_entered", PropertyInfo(Variant::OBJECT, "body", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
	ADD_SIGNAL(MethodInfo("body_exited", PropertyInfo(Variant::OBJECT, "body", PROPERTY_HINT_RESOURCE_TYPE, "Node")));
	ADD_SIGNAL(MethodInfo("sleeping_state_changed"));

	BIND_ENUM_CONSTANT(MODE_RIGID);
	BIND_ENUM_CONSTANT(MODE_STATIC);
	BIND_ENUM_CONSTANT(MODE_CHARACTER);
	BIND_ENUM_CONSTANT(MODE_KINEMATIC);

	BIND_ENUM_CONSTANT(CCD_MODE_DISABLED);
	BIND_ENUM_CONSTANT(CCD_MODE_CAST_RAY);
	BIND_ENUM_CONSTANT(CCD_MODE_CAST_SHAPE);
}

void PhysicsBody2D::_notification(int p_what) {
	if (p_what == NOTIFICATION_ENTER_TREE) {
		last_valid_transform = get_global_transform();
		// Reset move_and_slide() data.
		on_floor = false;
		on_floor_body = RID();
		on_ceiling = false;
		on_wall = false;
		colliders.clear();
		floor_velocity = Vector2();
#ifdef TOOLS_ENABLED
		if (Engine::get_singleton()->is_editor_hint()) {
			set_notify_local_transform(true); //used for warnings and only in editor
		}
#endif // TOOLS_ENABLED
	}

	if (p_what == NOTIFICATION_LOCAL_TRANSFORM_CHANGED) {
		//used by sync to physics, send the new transform to the physics
		Transform2D new_transform = get_global_transform();
		PhysicsServer2D::get_singleton()->body_set_state(get_rid(), PhysicsServer2D::BODY_STATE_TRANSFORM, new_transform);
		//but then revert changes
		set_notify_local_transform(false);
		set_global_transform(last_valid_transform);
		set_notify_local_transform(true);
#ifdef TOOLS_ENABLED
		if (Engine::get_singleton()->is_editor_hint()) {
			update_configuration_warning();
		}
#endif // TOOLS_ENABLED
	}
}

String PhysicsBody2D::get_configuration_warning() const {
	Transform2D t = get_transform();
	String warning = CollisionObject2D::get_configuration_warning();

	if ((get_mode() == MODE_RIGID || get_mode() == MODE_CHARACTER) && (ABS(t.elements[0].length() - 1.0) > 0.05 || ABS(t.elements[1].length() - 1.0) > 0.05)) {
		if (!warning.empty()) {
			warning += "\n\n";
		}
		warning += TTR("Size changes to PhysicsBody2D (in character or rigid modes) will be overridden by the physics engine when running.\nChange the size in children collision shapes instead.");
	}

	return warning;
}

bool PhysicsBody2D::_test_motion(const Vector2 &p_motion, bool p_infinite_inertia, float p_margin, const Ref<PhysicsTestMotionResult2D> &p_result) {
	PhysicsServer2D::MotionResult *r = nullptr;
	if (p_result.is_valid()) {
		r = p_result->get_result_ptr();
	}
	return PhysicsServer2D::get_singleton()->body_test_motion(get_rid(), get_global_transform(), p_motion, p_infinite_inertia, p_margin, r);
}

void PhysicsBody2D::_direct_state_changed(Object *p_state) {
#ifdef DEBUG_ENABLED
	state = Object::cast_to<PhysicsDirectBodyState2D>(p_state);
#else
	state = (PhysicsDirectBodyState2D *)p_state; // Trust it
#endif
	set_block_transform_notify(true); // Don't notify (would create feedback loop)

	if (mode == MODE_KINEMATIC) {
		if (!sync_to_physics) {
			return;
		}
		last_valid_transform = state->get_transform();
		set_notify_local_transform(false);
		set_global_transform(last_valid_transform);
		set_notify_local_transform(true);
		return;
	}

	set_global_transform(state->get_transform());
	linear_velocity = state->get_linear_velocity();
	angular_velocity = state->get_angular_velocity();
	if (sleeping != state->is_sleeping()) {
		sleeping = state->is_sleeping();
		emit_signal(SceneStringNames::get_singleton()->sleeping_state_changed);
	}

	if (get_script_instance()) {
		get_script_instance()->call("_integrate_forces", state);
	}

	set_block_transform_notify(false); // Restart notifications

	if (contact_monitor) {
		contact_monitor->locked = true;

		// Untag all
		int rc = 0;
		for (Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.front(); E; E = E->next()) {
			for (int i = 0; i < E->get().shapes.size(); i++) {
				E->get().shapes[i].tagged = false;
				rc++;
			}
		}

		PhysicsBody2DInOut *toadd = (PhysicsBody2DInOut *)alloca(state->get_contact_count() * sizeof(PhysicsBody2DInOut));
		int toadd_count = 0;
		PhysicsBody2D_RemoveAction *toremove = (PhysicsBody2D_RemoveAction *)alloca(rc * sizeof(PhysicsBody2D_RemoveAction));
		int toremove_count = 0;

		// Collect ones to add
		for (int i = 0; i < state->get_contact_count(); i++) {
			ObjectID obj = state->get_contact_collider_id(i);
			int local_shape = state->get_contact_local_shape(i);
			int shape = state->get_contact_collider_shape(i);

			Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.find(obj);
			if (!E) {
				toadd[toadd_count].id = obj;
				toadd[toadd_count].shape = shape;
				toadd[toadd_count].local_shape = local_shape;
				toadd_count++;
				continue;
			}

			ShapePair sp(shape, local_shape);
			int idx = E->get().shapes.find(sp);
			if (idx == -1) {
				toadd[toadd_count].id = obj;
				toadd[toadd_count].shape = shape;
				toadd[toadd_count].local_shape = local_shape;
				toadd_count++;
				continue;
			}

			E->get().shapes[idx].tagged = true;
		}

		// Collect ones to remove
		for (Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.front(); E; E = E->next()) {
			for (int i = 0; i < E->get().shapes.size(); i++) {
				if (!E->get().shapes[i].tagged) {
					toremove[toremove_count].body_id = E->key();
					toremove[toremove_count].pair = E->get().shapes[i];
					toremove_count++;
				}
			}
		}

		// Process removals
		for (int i = 0; i < toremove_count; i++) {
			_body_inout(0, toremove[i].body_id, toremove[i].pair.body_shape, toremove[i].pair.local_shape);
		}
		// Process additions
		for (int i = 0; i < toadd_count; i++) {
			_body_inout(1, toadd[i].id, toadd[i].shape, toadd[i].local_shape);
		}

		contact_monitor->locked = false;
	}
	state = nullptr;
}

void PhysicsBody2D::_body_enter_tree(ObjectID p_id) {
	Object *obj = ObjectDB::get_instance(p_id);
	Node *node = Object::cast_to<Node>(obj);
	ERR_FAIL_COND(!node);

	ERR_FAIL_COND(!contact_monitor);
	Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.find(p_id);
	ERR_FAIL_COND(!E);
	ERR_FAIL_COND(E->get().in_scene);
	E->get().in_scene = true;

	contact_monitor->locked = true;
	emit_signal(SceneStringNames::get_singleton()->body_entered, node);
	for (int i = 0; i < E->get().shapes.size(); i++) {
		emit_signal(SceneStringNames::get_singleton()->body_shape_entered, p_id, node, E->get().shapes[i].body_shape, E->get().shapes[i].local_shape);
	}
	contact_monitor->locked = false;
}

void PhysicsBody2D::_body_exit_tree(ObjectID p_id) {
	Object *obj = ObjectDB::get_instance(p_id);
	Node *node = Object::cast_to<Node>(obj);
	ERR_FAIL_COND(!node);

	ERR_FAIL_COND(!contact_monitor);
	Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.find(p_id);
	ERR_FAIL_COND(!E);
	ERR_FAIL_COND(!E->get().in_scene);
	E->get().in_scene = false;

	contact_monitor->locked = true;
	emit_signal(SceneStringNames::get_singleton()->body_exited, node);
	for (int i = 0; i < E->get().shapes.size(); i++) {
		emit_signal(SceneStringNames::get_singleton()->body_shape_exited, p_id, node, E->get().shapes[i].body_shape, E->get().shapes[i].local_shape);
	}
	contact_monitor->locked = false;
}

void PhysicsBody2D::_body_inout(int p_status, ObjectID p_instance, int p_body_shape, int p_local_shape) {
	bool body_in = p_status == 1;
	ObjectID objid = p_instance;
	Object *obj = ObjectDB::get_instance(objid);
	Node *node = Object::cast_to<Node>(obj);
	ERR_FAIL_COND(!contact_monitor);
	Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.find(objid);
	ERR_FAIL_COND(!body_in && !E);

	if (body_in) {
		if (!E) {
			E = contact_monitor->body_map.insert(objid, BodyState());
			E->get().in_scene = node && node->is_inside_tree();
			if (node) {
				node->connect(SceneStringNames::get_singleton()->tree_entered, callable_mp(this, &PhysicsBody2D::_body_enter_tree), make_binds(objid));
				node->connect(SceneStringNames::get_singleton()->tree_exiting, callable_mp(this, &PhysicsBody2D::_body_exit_tree), make_binds(objid));
				if (E->get().in_scene) {
					emit_signal(SceneStringNames::get_singleton()->body_entered, node);
				}
			}
		}
		if (node) {
			E->get().shapes.insert(ShapePair(p_body_shape, p_local_shape));
		}
		if (E->get().in_scene) {
			emit_signal(SceneStringNames::get_singleton()->body_shape_entered, objid, node, p_body_shape, p_local_shape);
		}
	} else { // body_out
		if (node) {
			E->get().shapes.erase(ShapePair(p_body_shape, p_local_shape));
		}
		bool in_scene = E->get().in_scene;
		if (E->get().shapes.empty()) {
			if (node) {
				node->disconnect(SceneStringNames::get_singleton()->tree_entered, callable_mp(this, &PhysicsBody2D::_body_enter_tree));
				node->disconnect(SceneStringNames::get_singleton()->tree_exiting, callable_mp(this, &PhysicsBody2D::_body_exit_tree));
				if (in_scene) {
					emit_signal(SceneStringNames::get_singleton()->body_exited, node);
				}
			}
			contact_monitor->body_map.erase(E);
		}
		if (node && in_scene) {
			emit_signal(SceneStringNames::get_singleton()->body_shape_exited, objid, node, p_body_shape, p_local_shape);
		}
	}
}

void PhysicsBody2D::_reload_physics_characteristics() {
	if (physics_material_override.is_null()) {
		PhysicsServer2D::get_singleton()->body_set_param(get_rid(), PhysicsServer2D::BODY_PARAM_BOUNCE, 0);
		PhysicsServer2D::get_singleton()->body_set_param(get_rid(), PhysicsServer2D::BODY_PARAM_FRICTION, 1);
	} else {
		PhysicsServer2D::get_singleton()->body_set_param(get_rid(), PhysicsServer2D::BODY_PARAM_BOUNCE, physics_material_override->computed_bounce());
		PhysicsServer2D::get_singleton()->body_set_param(get_rid(), PhysicsServer2D::BODY_PARAM_FRICTION, physics_material_override->computed_friction());
	}
}

bool PhysicsBody2D::_separate_raycast_shapes(bool p_infinite_inertia, Collision &r_collision) {
	PhysicsServer2D::SeparationResult sep_res[8]; // Max 8 rays
	Transform2D gt = get_global_transform();
	Vector2 recover;
	int hits = PhysicsServer2D::get_singleton()->body_test_ray_separation(get_rid(), gt, p_infinite_inertia, recover, sep_res, 8, margin);
	int deepest = -1;
	float deepest_depth;

	for (int i = 0; i < hits; i++) {
		if (deepest == -1 || sep_res[i].collision_depth > deepest_depth) {
			deepest = i;
			deepest_depth = sep_res[i].collision_depth;
		}
	}

	gt.elements[2] += recover;
	set_global_transform(gt);

	if (deepest != -1) {
		r_collision.collider = sep_res[deepest].collider_id;
		r_collision.collider_shape = sep_res[deepest].collider_shape;
		r_collision.collider_velocity = sep_res[deepest].collider_velocity;
		r_collision.collider_metadata = sep_res[deepest].collider_metadata;
		r_collision.point = sep_res[deepest].collision_point;
		r_collision.normal = sep_res[deepest].collision_normal;
		r_collision.travel = recover;
		r_collision.remainder = Vector2();
		r_collision.local_shape = sep_res[deepest].collision_local_shape;
		return true;
	} else {
		return false;
	}
}

Ref<KinematicCollision2D> PhysicsBody2D::_move(const Vector2 &p_motion, bool p_infinite_inertia, bool p_exclude_raycast_shapes, bool p_test_only) {
	Collision col;

	if (move_and_collide(p_motion, p_infinite_inertia, col, p_exclude_raycast_shapes, p_test_only)) {
		if (motion_cache.is_null()) {
			motion_cache.instance();
			motion_cache->owner = this;
		}
		motion_cache->collision = col;
		return motion_cache;
	}

	return Ref<KinematicCollision2D>();
}

Ref<KinematicCollision2D> PhysicsBody2D::_get_slide_collision(int p_bounce) {
	ERR_FAIL_INDEX_V(p_bounce, colliders.size(), Ref<KinematicCollision2D>());

	if (p_bounce >= slide_colliders.size()) {
		slide_colliders.resize(p_bounce + 1);
	}

	if (slide_colliders[p_bounce].is_null()) {
		slide_colliders.write[p_bounce].instance();
		slide_colliders.write[p_bounce]->owner = this;
	}

	slide_colliders.write[p_bounce]->collision = colliders[p_bounce];
	return slide_colliders[p_bounce];
}

void PhysicsBody2D::set_mode(Mode p_mode) {
	mode = p_mode;
	switch (p_mode) {
		case MODE_RIGID: {
			PhysicsServer2D::get_singleton()->body_set_mode(get_rid(), PhysicsServer2D::BODY_MODE_RIGID);
		} break;
		case MODE_STATIC: {
			PhysicsServer2D::get_singleton()->body_set_mode(get_rid(), PhysicsServer2D::BODY_MODE_STATIC);

		} break;
		case MODE_KINEMATIC: {
			PhysicsServer2D::get_singleton()->body_set_mode(get_rid(), PhysicsServer2D::BODY_MODE_KINEMATIC);

		} break;
		case MODE_CHARACTER: {
			PhysicsServer2D::get_singleton()->body_set_mode(get_rid(), PhysicsServer2D::BODY_MODE_CHARACTER);

		} break;
	}
}

PhysicsBody2D::Mode PhysicsBody2D::get_mode() const {
	return mode;
}

void PhysicsBody2D::set_physics_material_override(const Ref<PhysicsMaterial> &p_physics_material_override) {
	if (physics_material_override.is_valid()) {
		if (physics_material_override->is_connected(CoreStringNames::get_singleton()->changed, callable_mp(this, &PhysicsBody2D::_reload_physics_characteristics))) {
			physics_material_override->disconnect(CoreStringNames::get_singleton()->changed, callable_mp(this, &PhysicsBody2D::_reload_physics_characteristics));
		}
	}

	physics_material_override = p_physics_material_override;
	if (physics_material_override.is_valid()) {
		physics_material_override->connect(CoreStringNames::get_singleton()->changed, callable_mp(this, &PhysicsBody2D::_reload_physics_characteristics));
	}

	_reload_physics_characteristics();
}

Ref<PhysicsMaterial> PhysicsBody2D::get_physics_material_override() const {
	return physics_material_override;
}

void PhysicsBody2D::set_mass(real_t p_mass) {
	ERR_FAIL_COND(p_mass <= 0);
	mass = p_mass;
	PhysicsServer2D::get_singleton()->body_set_param(get_rid(), PhysicsServer2D::BODY_PARAM_MASS, mass);
}

real_t PhysicsBody2D::get_mass() const {
	return mass;
}

void PhysicsBody2D::set_inertia(real_t p_inertia) {
	ERR_FAIL_COND(p_inertia < 0);
	PhysicsServer2D::get_singleton()->body_set_param(get_rid(), PhysicsServer2D::BODY_PARAM_INERTIA, p_inertia);
}

real_t PhysicsBody2D::get_inertia() const {
	return PhysicsServer2D::get_singleton()->body_get_param(get_rid(), PhysicsServer2D::BODY_PARAM_INERTIA);
}

void PhysicsBody2D::set_gravity_scale(real_t p_gravity_scale) {
	gravity_scale = p_gravity_scale;
	PhysicsServer2D::get_singleton()->body_set_param(get_rid(), PhysicsServer2D::BODY_PARAM_GRAVITY_SCALE, gravity_scale);
}

real_t PhysicsBody2D::get_gravity_scale() const {
	return gravity_scale;
}

void PhysicsBody2D::set_linear_velocity(const Vector2 &p_velocity) {
	linear_velocity = p_velocity;
	if (state) {
		state->set_linear_velocity(linear_velocity);
	} else {
		PhysicsServer2D::get_singleton()->body_set_state(get_rid(), PhysicsServer2D::BODY_STATE_LINEAR_VELOCITY, linear_velocity);
	}
}

void PhysicsBody2D::set_axis_velocity(const Vector2 &p_axis) {
	Vector2 v = state ? state->get_linear_velocity() : linear_velocity;
	Vector2 axis = p_axis.normalized();
	v -= axis * axis.dot(v);
	v += p_axis;
	if (state) {
		set_linear_velocity(v);
	} else {
		PhysicsServer2D::get_singleton()->body_set_axis_velocity(get_rid(), p_axis);
		linear_velocity = v;
	}
}

Vector2 PhysicsBody2D::get_linear_velocity() const {
	return linear_velocity;
}

void PhysicsBody2D::set_linear_damp(real_t p_linear_damp) {
	ERR_FAIL_COND(p_linear_damp < -1);
	linear_damp = p_linear_damp;
	PhysicsServer2D::get_singleton()->body_set_param(get_rid(), PhysicsServer2D::BODY_PARAM_LINEAR_DAMP, linear_damp);
}

real_t PhysicsBody2D::get_linear_damp() const {
	return linear_damp;
}

void PhysicsBody2D::set_angular_velocity(real_t p_velocity) {
	angular_velocity = p_velocity;
	if (state) {
		state->set_angular_velocity(angular_velocity);
	} else {
		PhysicsServer2D::get_singleton()->body_set_state(get_rid(), PhysicsServer2D::BODY_STATE_ANGULAR_VELOCITY, angular_velocity);
	}
}

real_t PhysicsBody2D::get_angular_velocity() const {
	return angular_velocity;
}

void PhysicsBody2D::set_angular_damp(real_t p_angular_damp) {
	ERR_FAIL_COND(p_angular_damp < -1);
	angular_damp = p_angular_damp;
	PhysicsServer2D::get_singleton()->body_set_param(get_rid(), PhysicsServer2D::BODY_PARAM_ANGULAR_DAMP, angular_damp);
}

real_t PhysicsBody2D::get_angular_damp() const {
	return angular_damp;
}

void PhysicsBody2D::set_continuous_collision_detection_mode(CCDMode p_mode) {
	ccd_mode = p_mode;
	PhysicsServer2D::get_singleton()->body_set_continuous_collision_detection_mode(get_rid(), PhysicsServer2D::CCDMode(p_mode));
}

PhysicsBody2D::CCDMode PhysicsBody2D::get_continuous_collision_detection_mode() const {
	return ccd_mode;
}

void PhysicsBody2D::set_can_sleep(bool p_active) {
	can_sleep = p_active;
	PhysicsServer2D::get_singleton()->body_set_state(get_rid(), PhysicsServer2D::BODY_STATE_CAN_SLEEP, p_active);
}

bool PhysicsBody2D::is_able_to_sleep() const {
	return can_sleep;
}

void PhysicsBody2D::set_sleeping(bool p_sleeping) {
	sleeping = p_sleeping;
	PhysicsServer2D::get_singleton()->body_set_state(get_rid(), PhysicsServer2D::BODY_STATE_SLEEPING, sleeping);
}

bool PhysicsBody2D::is_sleeping() const {
	return sleeping;
}

void PhysicsBody2D::set_use_custom_integrator(bool p_enable) {
	if (custom_integrator == p_enable) {
		return;
	}
	custom_integrator = p_enable;
	PhysicsServer2D::get_singleton()->body_set_omit_force_integration(get_rid(), p_enable);
}

bool PhysicsBody2D::is_using_custom_integrator() {
	return custom_integrator;
}

void PhysicsBody2D::set_contact_monitor(bool p_enabled) {
	if (p_enabled == is_contact_monitor_enabled()) {
		return;
	}

	if (!p_enabled) {
		ERR_FAIL_COND_MSG(contact_monitor->locked, "Can't disable contact monitoring during in/out callback. Use call_deferred(\"set_contact_monitor\", false) instead.");

		for (Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.front(); E; E = E->next()) {
			// Clean up
			Object *obj = ObjectDB::get_instance(E->key());
			Node *node = Object::cast_to<Node>(obj);
			if (node) {
				node->disconnect(SceneStringNames::get_singleton()->tree_entered, callable_mp(this, &PhysicsBody2D::_body_enter_tree));
				node->disconnect(SceneStringNames::get_singleton()->tree_exiting, callable_mp(this, &PhysicsBody2D::_body_exit_tree));
			}
		}

		memdelete(contact_monitor);
		contact_monitor = nullptr;
	} else {
		contact_monitor = memnew(ContactMonitor);
		contact_monitor->locked = false;
	}
}

bool PhysicsBody2D::is_contact_monitor_enabled() const {
	return contact_monitor != nullptr;
}

void PhysicsBody2D::set_max_contacts_reported(int p_amount) {
	max_contacts_reported = p_amount;
	PhysicsServer2D::get_singleton()->body_set_max_contacts_reported(get_rid(), p_amount);
}

int PhysicsBody2D::get_max_contacts_reported() const {
	return max_contacts_reported;
}

void PhysicsBody2D::set_applied_force(const Vector2 &p_force) {
	PhysicsServer2D::get_singleton()->body_set_applied_force(get_rid(), p_force);
};

Vector2 PhysicsBody2D::get_applied_force() const {
	return PhysicsServer2D::get_singleton()->body_get_applied_force(get_rid());
};

void PhysicsBody2D::set_applied_torque(const float p_torque) {
	PhysicsServer2D::get_singleton()->body_set_applied_torque(get_rid(), p_torque);
};

float PhysicsBody2D::get_applied_torque() const {
	return PhysicsServer2D::get_singleton()->body_get_applied_torque(get_rid());
};

void PhysicsBody2D::add_force(const Vector2 &p_force, const Vector2 &p_position) {
	PhysicsServer2D::get_singleton()->body_add_force(get_rid(), p_force, p_position);
}

void PhysicsBody2D::add_central_force(const Vector2 &p_force) {
	PhysicsServer2D::get_singleton()->body_add_central_force(get_rid(), p_force);
}

void PhysicsBody2D::add_torque(const float p_torque) {
	PhysicsServer2D::get_singleton()->body_add_torque(get_rid(), p_torque);
}

void PhysicsBody2D::apply_impulse(const Vector2 &p_impulse, const Vector2 &p_position) {
	PhysicsServer2D::get_singleton()->body_apply_impulse(get_rid(), p_impulse, p_position);
}

void PhysicsBody2D::apply_central_impulse(const Vector2 &p_impulse) {
	PhysicsServer2D::get_singleton()->body_apply_central_impulse(get_rid(), p_impulse);
}

void PhysicsBody2D::apply_torque_impulse(float p_torque) {
	PhysicsServer2D::get_singleton()->body_apply_torque_impulse(get_rid(), p_torque);
}

void PhysicsBody2D::set_safe_margin(float p_margin) {
	margin = p_margin;
}

float PhysicsBody2D::get_safe_margin() const {
	return margin;
}

void PhysicsBody2D::set_sync_to_physics(bool p_enable) {
	if (sync_to_physics == p_enable) {
		return;
	}
	sync_to_physics = p_enable;

	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	if (p_enable) {
		PhysicsServer2D::get_singleton()->body_set_force_integration_callback(get_rid(), this, "_direct_state_changed");
		set_only_update_transform_changes(true);
		set_notify_local_transform(true);
	} else {
		PhysicsServer2D::get_singleton()->body_set_force_integration_callback(get_rid(), nullptr, "");
		set_only_update_transform_changes(false);
		set_notify_local_transform(false);
	}
}

bool PhysicsBody2D::is_sync_to_physics_enabled() const {
	return sync_to_physics;
}

void PhysicsBody2D::set_collision_layer(uint32_t p_layer) {
	collision_layer = p_layer;
	PhysicsServer2D::get_singleton()->body_set_collision_layer(get_rid(), p_layer);
}

uint32_t PhysicsBody2D::get_collision_layer() const {
	return collision_layer;
}

void PhysicsBody2D::set_collision_mask(uint32_t p_mask) {
	collision_mask = p_mask;
	PhysicsServer2D::get_singleton()->body_set_collision_mask(get_rid(), p_mask);
}

uint32_t PhysicsBody2D::get_collision_mask() const {
	return collision_mask;
}

void PhysicsBody2D::set_collision_layer_bit(int p_bit, bool p_value) {
	uint32_t collision_layer = get_collision_layer();
	if (p_value) {
		collision_layer |= 1 << p_bit;
	} else {
		collision_layer &= ~(1 << p_bit);
	}
	set_collision_layer(collision_layer);
}

bool PhysicsBody2D::get_collision_layer_bit(int p_bit) const {
	return get_collision_layer() & (1 << p_bit);
}

void PhysicsBody2D::set_collision_mask_bit(int p_bit, bool p_value) {
	uint32_t mask = get_collision_mask();
	if (p_value) {
		mask |= 1 << p_bit;
	} else {
		mask &= ~(1 << p_bit);
	}
	set_collision_mask(mask);
}

bool PhysicsBody2D::get_collision_mask_bit(int p_bit) const {
	return get_collision_mask() & (1 << p_bit);
}

TypedArray<PhysicsBody2D> PhysicsBody2D::get_collision_exceptions() {
	List<RID> exceptions;
	PhysicsServer2D::get_singleton()->body_get_collision_exceptions(get_rid(), &exceptions);
	Array ret;
	for (List<RID>::Element *E = exceptions.front(); E; E = E->next()) {
		RID body = E->get();
		ObjectID instance_id = PhysicsServer2D::get_singleton()->body_get_object_instance_id(body);
		Object *obj = ObjectDB::get_instance(instance_id);
		PhysicsBody2D *physics_body = Object::cast_to<PhysicsBody2D>(obj);
		ret.append(physics_body);
	}
	return ret;
}

void PhysicsBody2D::add_collision_exception_with(Node *p_node) {
	ERR_FAIL_NULL(p_node);
	PhysicsBody2D *physics_body = Object::cast_to<PhysicsBody2D>(p_node);
	ERR_FAIL_COND_MSG(!physics_body, "Collision exception only works between two objects of PhysicsBody2D type.");
	PhysicsServer2D::get_singleton()->body_add_collision_exception(get_rid(), physics_body->get_rid());
}

void PhysicsBody2D::remove_collision_exception_with(Node *p_node) {
	ERR_FAIL_NULL(p_node);
	PhysicsBody2D *physics_body = Object::cast_to<PhysicsBody2D>(p_node);
	ERR_FAIL_COND_MSG(!physics_body, "Collision exception only works between two objects of PhysicsBody2D type.");
	PhysicsServer2D::get_singleton()->body_remove_collision_exception(get_rid(), physics_body->get_rid());
}

TypedArray<Node2D> PhysicsBody2D::get_colliding_bodies() const {
	ERR_FAIL_COND_V(!contact_monitor, Array());
	TypedArray<Node2D> ret;
	ret.resize(contact_monitor->body_map.size());
	int idx = 0;
	for (const Map<ObjectID, BodyState>::Element *E = contact_monitor->body_map.front(); E; E = E->next()) {
		Object *obj = ObjectDB::get_instance(E->key());
		if (!obj) {
			ret.resize(ret.size() - 1); //ops
		} else {
			ret[idx++] = obj;
		}
	}
	return ret;
}

bool PhysicsBody2D::is_on_floor() const {
	return on_floor;
}

bool PhysicsBody2D::is_on_wall() const {
	return on_wall;
}

bool PhysicsBody2D::is_on_ceiling() const {
	return on_ceiling;
}

Vector2 PhysicsBody2D::get_floor_normal() const {
	return floor_normal;
}

Vector2 PhysicsBody2D::get_floor_velocity() const {
	return floor_velocity;
}

bool PhysicsBody2D::test_move(const Transform2D &p_from, const Vector2 &p_motion, bool p_infinite_inertia) {
	ERR_FAIL_COND_V(!is_inside_tree(), false);
	return PhysicsServer2D::get_singleton()->body_test_motion(get_rid(), p_from, p_motion, p_infinite_inertia, margin);
}

bool PhysicsBody2D::move_and_collide(const Vector2 &p_motion, bool p_infinite_inertia, Collision &r_collision, bool p_exclude_raycast_shapes, bool p_test_only) {
	if (sync_to_physics) {
		ERR_PRINT("Functions move_and_slide and move_and_collide do not work together with 'sync to physics' option. Please read the documentation.");
	}
	Transform2D gt = get_global_transform();
	PhysicsServer2D::MotionResult result;
	bool colliding = PhysicsServer2D::get_singleton()->body_test_motion(get_rid(), gt, p_motion, p_infinite_inertia, margin, &result, p_exclude_raycast_shapes);

	if (colliding) {
		r_collision.collider = result.collider_id;
		r_collision.collider_rid = result.collider;
		r_collision.collider_shape = result.collider_shape;
		r_collision.collider_velocity = result.collider_velocity;
		r_collision.collider_metadata = result.collider_metadata;
		r_collision.point = result.collision_point;
		r_collision.normal = result.collision_normal;
		r_collision.travel = result.motion;
		r_collision.remainder = result.remainder;
		r_collision.local_shape = result.collision_local_shape;
	}

	if (!p_test_only) {
		gt.elements[2] += result.motion;
		set_global_transform(gt);
	}

	return colliding;
}

// Avoid numerical precision errors when angle is 45 degrees.
#define FLOOR_ANGLE_THRESHOLD 0.01

Vector2 PhysicsBody2D::move_and_slide(const Vector2 &p_linear_velocity, const Vector2 &p_up_direction, bool p_stop_on_slope, int p_max_slides, float p_floor_max_angle, bool p_infinite_inertia) {
	Vector2 body_velocity = p_linear_velocity;
	Vector2 body_velocity_normal = body_velocity.normalized();
	Vector2 up_direction = p_up_direction.normalized();

	Vector2 current_floor_velocity = floor_velocity;
	if (on_floor && on_floor_body.is_valid()) {
		// This approach makes sure there is less delay between the actual body velocity and the one we saved
		PhysicsDirectBodyState2D *bs = PhysicsServer2D::get_singleton()->body_get_direct_state(on_floor_body);
		if (bs) {
			current_floor_velocity = bs->get_linear_velocity();
		}
	}

	// Hack in order to work with calling from _process as well as from _physics_process; calling from thread is risky
	Vector2 motion = (current_floor_velocity + body_velocity) * (Engine::get_singleton()->is_in_physics_frame() ? get_physics_process_delta_time() : get_process_delta_time());
	on_floor = false;
	on_floor_body = RID();
	on_ceiling = false;
	on_wall = false;
	colliders.clear();
	floor_normal = Vector2();
	floor_velocity = Vector2();

	while (p_max_slides) {
		Collision collision;
		bool found_collision = false;

		for (int i = 0; i < 2; ++i) {
			bool collided;
			if (i == 0) { // Check for collision
				collided = move_and_collide(motion, p_infinite_inertia, collision);
				if (!collided) {
					motion = Vector2(); // Clear because no collision happened and motion completed
				}
			} else { // Separate raycasts (if any)
				collided = _separate_raycast_shapes(p_infinite_inertia, collision);
				if (collided) {
					collision.remainder = motion; // Keep
					collision.travel = Vector2();
				}
			}

			if (collided) {
				found_collision = true;
				colliders.push_back(collision);
				motion = collision.remainder;

				if (up_direction == Vector2()) {
					// All is a wall
					on_wall = true;
				} else {
					if (Math::acos(collision.normal.dot(up_direction)) <= p_floor_max_angle + FLOOR_ANGLE_THRESHOLD) { // Floor
						on_floor = true;
						floor_normal = collision.normal;
						on_floor_body = collision.collider_rid;
						floor_velocity = collision.collider_velocity;

						if (p_stop_on_slope) {
							if ((body_velocity_normal + up_direction).length() < 0.01 && collision.travel.length() < 1) {
								Transform2D gt = get_global_transform();
								gt.elements[2] -= collision.travel.slide(up_direction);
								set_global_transform(gt);
								return Vector2();
							}
						}
					} else if (Math::acos(collision.normal.dot(-up_direction)) <= p_floor_max_angle + FLOOR_ANGLE_THRESHOLD) { //ceiling
						on_ceiling = true;
					} else {
						on_wall = true;
					}
				}

				motion = motion.slide(collision.normal);
				body_velocity = body_velocity.slide(collision.normal);
			}
		}
		if (!found_collision || motion == Vector2()) {
			break;
		}
		--p_max_slides;
	}
	return body_velocity;
}

Vector2 PhysicsBody2D::move_and_slide_with_snap(const Vector2 &p_linear_velocity, const Vector2 &p_snap, const Vector2 &p_up_direction, bool p_stop_on_slope, int p_max_slides, float p_floor_max_angle, bool p_infinite_inertia) {
	Vector2 up_direction = p_up_direction.normalized();
	bool was_on_floor = on_floor;

	Vector2 ret = move_and_slide(p_linear_velocity, up_direction, p_stop_on_slope, p_max_slides, p_floor_max_angle, p_infinite_inertia);
	if (!was_on_floor || p_snap == Vector2()) {
		return ret;
	}

	Collision col;
	Transform2D gt = get_global_transform();
	if (move_and_collide(p_snap, p_infinite_inertia, col, false, true)) {
		bool apply = true;
		if (up_direction != Vector2()) {
			if (Math::acos(col.normal.dot(up_direction)) <= p_floor_max_angle + FLOOR_ANGLE_THRESHOLD) {
				on_floor = true;
				floor_normal = col.normal;
				on_floor_body = col.collider_rid;
				floor_velocity = col.collider_velocity;
				if (p_stop_on_slope) {
					// Move and collide may stray the object a bit because of pre un-stucking,
					// so only ensure that motion happens on floor direction in this case.
					col.travel = up_direction * up_direction.dot(col.travel);
				}
			} else {
				apply = false; // Snapped with floor direction, but did not snap to a floor, do not snap.
			}
		}
		if (apply) {
			gt.elements[2] += col.travel;
			set_global_transform(gt);
		}
	}

	return ret;
}

int PhysicsBody2D::get_slide_count() const {
	return colliders.size();
}

PhysicsBody2D::Collision PhysicsBody2D::get_slide_collision(int p_bounce) const {
	ERR_FAIL_INDEX_V(p_bounce, colliders.size(), Collision());
	return colliders[p_bounce];
}

PhysicsBody2D::PhysicsBody2D() :
		CollisionObject2D(PhysicsServer2D::get_singleton()->body_create(), false) {
	PhysicsServer2D::get_singleton()->body_set_mode(get_rid(), PhysicsServer2D::BodyMode::BODY_MODE_RIGID);
	PhysicsServer2D::get_singleton()->body_set_force_integration_callback(get_rid(), this, "_direct_state_changed");
	set_pickable(false);
}

PhysicsBody2D::~PhysicsBody2D() {
	if (contact_monitor) {
		memdelete(contact_monitor);
	}
	if (motion_cache.is_valid()) {
		motion_cache->owner = nullptr;
	}
	for (int i = 0; i < slide_colliders.size(); i++) {
		if (slide_colliders[i].is_valid()) {
			slide_colliders.write[i]->owner = nullptr;
		}
	}
}

// KinematicCollision2D

void KinematicCollision2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_collider"), &KinematicCollision2D::get_collider);
	ClassDB::bind_method(D_METHOD("get_collider_id"), &KinematicCollision2D::get_collider_id);
	ClassDB::bind_method(D_METHOD("get_collider_shape"), &KinematicCollision2D::get_collider_shape);
	ClassDB::bind_method(D_METHOD("get_collider_shape_index"), &KinematicCollision2D::get_collider_shape_index);
	ClassDB::bind_method(D_METHOD("get_collider_velocity"), &KinematicCollision2D::get_collider_velocity);
	ClassDB::bind_method(D_METHOD("get_collider_metadata"), &KinematicCollision2D::get_collider_metadata);
	ClassDB::bind_method(D_METHOD("get_position"), &KinematicCollision2D::get_position);
	ClassDB::bind_method(D_METHOD("get_normal"), &KinematicCollision2D::get_normal);
	ClassDB::bind_method(D_METHOD("get_travel"), &KinematicCollision2D::get_travel);
	ClassDB::bind_method(D_METHOD("get_remainder"), &KinematicCollision2D::get_remainder);
	ClassDB::bind_method(D_METHOD("get_local_shape"), &KinematicCollision2D::get_local_shape);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collider"), "", "get_collider");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collider_id"), "", "get_collider_id");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collider_shape"), "", "get_collider_shape");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collider_shape_index"), "", "get_collider_shape_index");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "collider_velocity"), "", "get_collider_velocity");
	ADD_PROPERTY(PropertyInfo(Variant::NIL, "collider_metadata", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NIL_IS_VARIANT), "", "get_collider_metadata");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "position"), "", "get_position");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "normal"), "", "get_normal");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "travel"), "", "get_travel");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "remainder"), "", "get_remainder");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "local_shape"), "", "get_local_shape");
}

Object *KinematicCollision2D::get_collider() const {
	if (collision.collider.is_valid()) {
		return ObjectDB::get_instance(collision.collider);
	}
	return nullptr;
}

ObjectID KinematicCollision2D::get_collider_id() const {
	return collision.collider;
}

Object *KinematicCollision2D::get_collider_shape() const {
	Object *collider = get_collider();
	if (collider) {
		CollisionObject2D *obj2d = Object::cast_to<CollisionObject2D>(collider);
		if (obj2d) {
			uint32_t ownerid = obj2d->shape_find_owner(collision.collider_shape);
			return obj2d->shape_owner_get_owner(ownerid);
		}
	}
	return nullptr;
}

int KinematicCollision2D::get_collider_shape_index() const {
	return collision.collider_shape;
}

Vector2 KinematicCollision2D::get_collider_velocity() const {
	return collision.collider_velocity;
}

Variant KinematicCollision2D::get_collider_metadata() const {
	return Variant();
}

Vector2 KinematicCollision2D::get_position() const {
	return collision.point;
}

Vector2 KinematicCollision2D::get_normal() const {
	return collision.normal;
}

Vector2 KinematicCollision2D::get_travel() const {
	return collision.travel;
}

Vector2 KinematicCollision2D::get_remainder() const {
	return collision.remainder;
}

Object *KinematicCollision2D::get_local_shape() const {
	if (!owner) {
		return nullptr;
	}
	uint32_t ownerid = owner->shape_find_owner(collision.local_shape);
	return owner->shape_owner_get_owner(ownerid);
}
