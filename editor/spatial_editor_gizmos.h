/*************************************************************************/
/*  spatial_editor_gizmos.h                                              */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
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

#ifndef SPATIAL_EDITOR_GIZMOS_H
#define SPATIAL_EDITOR_GIZMOS_H

#include "editor/plugins/spatial_editor_plugin.h"
#include "scene/3d/camera.h"

class Camera;

class LightSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(LightSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;

	virtual String get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual Variant get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual void set_handle(EditorSpatialGizmo *p_gizmo, int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel = false) override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	LightSpatialGizmoPlugin();
};

class AudioStreamPlayer3DSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(AudioStreamPlayer3DSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;

	virtual String get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual Variant get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual void set_handle(EditorSpatialGizmo *p_gizmo, int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel = false) override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	AudioStreamPlayer3DSpatialGizmoPlugin();
};

class ListenerSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(ListenerSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;

	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	ListenerSpatialGizmoPlugin();
};

class CameraSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(CameraSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;

	virtual String get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual Variant get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual void set_handle(EditorSpatialGizmo *p_gizmo, int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel = false) override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	CameraSpatialGizmoPlugin();
};

class MeshInstanceSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(MeshInstanceSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual bool can_be_hidden() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	MeshInstanceSpatialGizmoPlugin();
};

class Sprite3DSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(Sprite3DSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual bool can_be_hidden() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	Sprite3DSpatialGizmoPlugin();
};

class Position3DSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(Position3DSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

	Ref<ArrayMesh> pos3d_mesh;
	Vector<Vector3> cursor_points;

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	Position3DSpatialGizmoPlugin();
};

class SkeletonSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(SkeletonSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	SkeletonSpatialGizmoPlugin();
};

class PhysicalBoneSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(PhysicalBoneSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	PhysicalBoneSpatialGizmoPlugin();
};

class RayCastSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(RayCastSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	RayCastSpatialGizmoPlugin();
};

class SpringArmSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(SpringArmSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	SpringArmSpatialGizmoPlugin();
};

class VehicleWheelSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(VehicleWheelSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	VehicleWheelSpatialGizmoPlugin();
};

class SoftBodySpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(SoftBodySpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual bool is_selectable_when_hidden() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	virtual String get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual Variant get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual void commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel) override;
	virtual bool is_handle_highlighted(const EditorSpatialGizmo *p_gizmo, int idx) const override;

	SoftBodySpatialGizmoPlugin();
};

class VisibilityNotifierGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(VisibilityNotifierGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	virtual String get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual Variant get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual void set_handle(EditorSpatialGizmo *p_gizmo, int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel = false) override;

	VisibilityNotifierGizmoPlugin();
};

class CPUParticlesGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(CPUParticlesGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual bool is_selectable_when_hidden() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;
	CPUParticlesGizmoPlugin();
};

class ParticlesGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(ParticlesGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual bool is_selectable_when_hidden() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	virtual String get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual Variant get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual void set_handle(EditorSpatialGizmo *p_gizmo, int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel = false) override;

	ParticlesGizmoPlugin();
};

class ReflectionProbeGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(ReflectionProbeGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	virtual String get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual Variant get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual void set_handle(EditorSpatialGizmo *p_gizmo, int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel = false) override;

	ReflectionProbeGizmoPlugin();
};

class GIProbeGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(GIProbeGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	virtual String get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual Variant get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual void set_handle(EditorSpatialGizmo *p_gizmo, int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel = false) override;

	GIProbeGizmoPlugin();
};

class BakedIndirectLightGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(BakedIndirectLightGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	virtual String get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual Variant get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual void set_handle(EditorSpatialGizmo *p_gizmo, int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel = false) override;

	BakedIndirectLightGizmoPlugin();
};

class CollisionObjectGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(CollisionObjectGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	CollisionObjectGizmoPlugin();
};

class CollisionShapeSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(CollisionShapeSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	virtual String get_handle_name(const EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual Variant get_handle_value(EditorSpatialGizmo *p_gizmo, int p_idx) const override;
	virtual void set_handle(EditorSpatialGizmo *p_gizmo, int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(EditorSpatialGizmo *p_gizmo, int p_idx, const Variant &p_restore, bool p_cancel = false) override;

	CollisionShapeSpatialGizmoPlugin();
};

class CollisionPolygonSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(CollisionPolygonSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;
	CollisionPolygonSpatialGizmoPlugin();
};

class NavigationMeshSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(NavigationMeshSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

	struct _EdgeKey {
		Vector3 from;
		Vector3 to;

		bool operator<(const _EdgeKey &p_with) const { return from == p_with.from ? to < p_with.to : from < p_with.from; }
	};

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	NavigationMeshSpatialGizmoPlugin();
};

class JointGizmosDrawer {
public:
	static Basis look_body(const Transform &p_joint_transform, const Transform &p_body_transform);
	static Basis look_body_toward(Vector3::Axis p_axis, const Transform &joint_transform, const Transform &body_transform);
	static Basis look_body_toward_x(const Transform &p_joint_transform, const Transform &p_body_transform);
	static Basis look_body_toward_y(const Transform &p_joint_transform, const Transform &p_body_transform);
	/// Special function just used for physics joints, it returns a basis constrained toward Joint Z axis
	/// with axis X and Y that are looking toward the body and oriented toward up
	static Basis look_body_toward_z(const Transform &p_joint_transform, const Transform &p_body_transform);

	// Draw circle around p_axis
	static void draw_circle(Vector3::Axis p_axis, real_t p_radius, const Transform &p_offset, const Basis &p_base, real_t p_limit_lower, real_t p_limit_upper, Vector<Vector3> &r_points, bool p_inverse = false);
	static void draw_cone(const Transform &p_offset, const Basis &p_base, real_t p_swing, real_t p_twist, Vector<Vector3> &r_points);
};

class JointSpatialGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(JointSpatialGizmoPlugin, EditorSpatialGizmoPlugin);

	Timer *update_timer;
	uint64_t update_idx = 0;

	void incremental_update_gizmos();

protected:
	static void _bind_methods();

public:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual void redraw(EditorSpatialGizmo *p_gizmo) override;

	static void CreatePinJointGizmo(const Transform &p_offset, Vector<Vector3> &r_cursor_points);
	static void CreateHingeJointGizmo(const Transform &p_offset, const Transform &p_trs_joint, const Transform &p_trs_body_a, const Transform &p_trs_body_b, real_t p_limit_lower, real_t p_limit_upper, bool p_use_limit, Vector<Vector3> &r_common_points, Vector<Vector3> *r_body_a_points, Vector<Vector3> *r_body_b_points);
	static void CreateSliderJointGizmo(const Transform &p_offset, const Transform &p_trs_joint, const Transform &p_trs_body_a, const Transform &p_trs_body_b, real_t p_angular_limit_lower, real_t p_angular_limit_upper, real_t p_linear_limit_lower, real_t p_linear_limit_upper, Vector<Vector3> &r_points, Vector<Vector3> *r_body_a_points, Vector<Vector3> *r_body_b_points);
	static void CreateConeTwistJointGizmo(const Transform &p_offset, const Transform &p_trs_joint, const Transform &p_trs_body_a, const Transform &p_trs_body_b, real_t p_swing, real_t p_twist, Vector<Vector3> *r_body_a_points, Vector<Vector3> *r_body_b_points);
	static void CreateGeneric6DOFJointGizmo(
			const Transform &p_offset,
			const Transform &p_trs_joint,
			const Transform &p_trs_body_a,
			const Transform &p_trs_body_b,
			real_t p_angular_limit_lower_x,
			real_t p_angular_limit_upper_x,
			real_t p_linear_limit_lower_x,
			real_t p_linear_limit_upper_x,
			bool p_enable_angular_limit_x,
			bool p_enable_linear_limit_x,
			real_t p_angular_limit_lower_y,
			real_t p_angular_limit_upper_y,
			real_t p_linear_limit_lower_y,
			real_t p_linear_limit_upper_y,
			bool p_enable_angular_limit_y,
			bool p_enable_linear_limit_y,
			real_t p_angular_limit_lower_z,
			real_t p_angular_limit_upper_z,
			real_t p_linear_limit_lower_z,
			real_t p_linear_limit_upper_z,
			bool p_enable_angular_limit_z,
			bool p_enable_linear_limit_z,
			Vector<Vector3> &r_points,
			Vector<Vector3> *r_body_a_points,
			Vector<Vector3> *r_body_b_points);

	JointSpatialGizmoPlugin();
};

class Room;

class RoomSpatialGizmo : public EditorSpatialGizmo {
	GDCLASS(RoomSpatialGizmo, EditorSpatialGizmo);

	Room *_room = nullptr;

public:
	virtual String get_handle_name(int p_idx) const override;
	virtual Variant get_handle_value(int p_idx) override;
	virtual void set_handle(int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(int p_idx, const Variant &p_restore, bool p_cancel = false) override;
	virtual void redraw() override;

	RoomSpatialGizmo(Room *p_room = nullptr);
};

class RoomGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(RoomGizmoPlugin, EditorSpatialGizmoPlugin);

protected:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual Ref<EditorSpatialGizmo> create_gizmo(Spatial *p_spatial) override;

public:
	RoomGizmoPlugin();
};

class Portal;

class PortalSpatialGizmo : public EditorSpatialGizmo {
	GDCLASS(PortalSpatialGizmo, EditorSpatialGizmo);

	Portal *_portal = nullptr;
	Color _color_portal_front;
	Color _color_portal_back;

public:
	virtual String get_handle_name(int p_idx) const override;
	virtual Variant get_handle_value(int p_idx) override;
	virtual void set_handle(int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(int p_idx, const Variant &p_restore, bool p_cancel = false) override;
	virtual void redraw() override;

	PortalSpatialGizmo(Portal *p_portal = nullptr);
};

class PortalGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(PortalGizmoPlugin, EditorSpatialGizmoPlugin);

protected:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual Ref<EditorSpatialGizmo> create_gizmo(Spatial *p_spatial) override;

public:
	PortalGizmoPlugin();
};

class Occluder;
class OccluderShapeSphere;

class OccluderSpatialGizmo : public EditorSpatialGizmo {
	GDCLASS(OccluderSpatialGizmo, EditorSpatialGizmo);

	Occluder *_occluder = nullptr;

	OccluderShapeSphere *get_occluder_shape_sphere();
	const OccluderShapeSphere *get_occluder_shape_sphere() const;

public:
	virtual String get_handle_name(int p_idx) const override;
	virtual Variant get_handle_value(int p_idx) override;
	virtual void set_handle(int p_idx, Camera *p_camera, const Point2 &p_point) override;
	virtual void commit_handle(int p_idx, const Variant &p_restore, bool p_cancel = false) override;
	virtual void redraw() override;

	OccluderSpatialGizmo(Occluder *p_occluder = nullptr);
};

class OccluderGizmoPlugin : public EditorSpatialGizmoPlugin {
	GDCLASS(OccluderGizmoPlugin, EditorSpatialGizmoPlugin);

protected:
	virtual bool has_gizmo(Spatial *p_spatial) override;
	virtual String get_name() const override;
	virtual int get_priority() const override;
	virtual Ref<EditorSpatialGizmo> create_gizmo(Spatial *p_spatial) override;

public:
	OccluderGizmoPlugin();
};

#endif // SPATIAL_EDITOR_GIZMOS_H
