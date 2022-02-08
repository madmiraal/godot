/*************************************************************************/
/*  transform_2d.cpp                                                     */
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

#include "transform_2d.h"

#include "core/string/ustring.h"

void Transform2D::invert() {
	// FIXME: this function assumes the basis is a rotation matrix, with no scaling.
	// Transform2D::affine_inverse can handle matrices with scaling, so GDScript should eventually use that.
	SWAP(elements[0][1], elements[1][0]);
	elements[2] = basis_xform(-elements[2]);
}

Transform2D Transform2D::inverse() const {
	Transform2D inv = *this;
	inv.invert();
	return inv;
}

void Transform2D::affine_invert() {
	real_t det = basis_determinant();
#ifdef MATH_CHECKS
	ERR_FAIL_COND(det == 0);
#endif
	real_t idet = 1.0 / det;

	SWAP(elements[0][0], elements[1][1]);
	elements[0] *= Vector2(idet, -idet);
	elements[1] *= Vector2(-idet, idet);

	elements[2] = basis_xform(-elements[2]);
}

Transform2D Transform2D::affine_inverse() const {
	Transform2D inv = *this;
	inv.affine_invert();
	return inv;
}

real_t Transform2D::basis_determinant() const {
	return elements[0].x * elements[1].y - elements[0].y * elements[1].x;
}

Size2 Transform2D::get_scale() const {
	real_t det_sign = SIGN(basis_determinant());
	return Size2(elements[0].length(), det_sign * elements[1].length());
}

void Transform2D::set_scale(const Size2 &p_scale) {
	elements[0].normalize();
	elements[1].normalize();
	elements[0] *= p_scale.x;
	elements[1] *= p_scale.y;
}

real_t Transform2D::get_rotation() const {
	return Math::atan2(elements[0].y, elements[0].x);
}

void Transform2D::set_rotation(const real_t p_rot) {
	Size2 scale = get_scale();
	real_t cr = Math::cos(p_rot);
	real_t sr = Math::sin(p_rot);
	elements[0][0] = cr;
	elements[0][1] = sr;
	elements[1][0] = -sr;
	elements[1][1] = cr;
	set_scale(scale);
}

real_t Transform2D::get_skew() const {
	real_t det = basis_determinant();
	return Math::acos(elements[0].normalized().dot(SIGN(det) * elements[1].normalized())) - Math_PI * 0.5;
}

void Transform2D::set_skew(const real_t p_angle) {
	real_t det = basis_determinant();
	elements[1] = SIGN(det) * elements[0].rotated((Math_PI * 0.5 + p_angle)).normalized() * elements[1].length();
}

void Transform2D::set_rotation_and_scale(const real_t p_rot, const Size2 &p_scale) {
	elements[0][0] = Math::cos(p_rot) * p_scale.x;
	elements[1][1] = Math::cos(p_rot) * p_scale.y;
	elements[1][0] = -Math::sin(p_rot) * p_scale.y;
	elements[0][1] = Math::sin(p_rot) * p_scale.x;
}

void Transform2D::set_rotation_scale_and_skew(const real_t p_rot, const Size2 &p_scale, const real_t p_skew) {
	elements[0][0] = Math::cos(p_rot) * p_scale.x;
	elements[1][1] = Math::cos(p_rot + p_skew) * p_scale.y;
	elements[1][0] = -Math::sin(p_rot + p_skew) * p_scale.y;
	elements[0][1] = Math::sin(p_rot) * p_scale.x;
}

void Transform2D::translate(const Vector2 &p_translation) {
	elements[2].x = tdotx(p_translation);
	elements[2].y = tdoty(p_translation);
}

void Transform2D::pre_translate(const Vector2 &p_translation) {
	elements[2] += p_translation;
}

Transform2D Transform2D::translated(const Vector2 &p_translation) const {
	Transform2D result = *this;
	result.translate(p_translation);
	return result;
}

Transform2D Transform2D::pre_translated(const Vector2 &p_translation) const {
	Transform2D result = *this;
	result.pre_translate(p_translation);
	return result;
}

void Transform2D::scale(const Size2 &p_scale) {
	elements[0] *= p_scale.x;
	elements[1] *= p_scale.y;
}

void Transform2D::pre_scale(const Size2 &p_scale) {
	elements[0] *= p_scale;
	elements[1] *= p_scale;
	elements[2] *= p_scale;
}

Transform2D Transform2D::scaled(const Size2 &p_scale) const {
	Transform2D result = *this;
	result.scale(p_scale);
	return result;
}

Transform2D Transform2D::pre_scaled(const Size2 &p_scale) const {
	Transform2D result = *this;
	result.pre_scale(p_scale);
	return result;
}

void Transform2D::rotate(const real_t p_radians) {
	*this *= Transform2D(p_radians, Vector2());
}

void Transform2D::pre_rotate(const real_t p_radians) {
	*this = Transform2D(p_radians, Vector2()) * (*this);
}

Transform2D Transform2D::rotated(const real_t p_radians) const {
	Transform2D result = *this;
	result.rotate(p_radians);
	return result;
}

Transform2D Transform2D::pre_rotated(const real_t p_radians) const {
	Transform2D result = *this;
	result.pre_rotate(p_radians);
	return result;
}

Transform2D Transform2D::untranslated() const {
	Transform2D result = *this;
	result.elements[2] = Vector2();
	return result;
}

Transform2D::Transform2D(const real_t p_rot, const Vector2 &p_pos) {
	real_t cr = Math::cos(p_rot);
	real_t sr = Math::sin(p_rot);
	elements[0][0] = cr;
	elements[0][1] = sr;
	elements[1][0] = -sr;
	elements[1][1] = cr;
	elements[2] = p_pos;
}

Transform2D::Transform2D(const real_t p_rot, const Size2 &p_scale, const real_t p_skew, const Vector2 &p_pos) {
	elements[0][0] = Math::cos(p_rot) * p_scale.x;
	elements[1][1] = Math::cos(p_rot + p_skew) * p_scale.y;
	elements[1][0] = -Math::sin(p_rot + p_skew) * p_scale.y;
	elements[0][1] = Math::sin(p_rot) * p_scale.x;
	elements[2] = p_pos;
}

void Transform2D::orthonormalize() {
	// Gram-Schmidt Process

	Vector2 x = elements[0];
	Vector2 y = elements[1];

	x.normalize();
	y = (y - x * (x.dot(y)));
	y.normalize();

	elements[0] = x;
	elements[1] = y;
}

Transform2D Transform2D::orthonormalized() const {
	Transform2D on = *this;
	on.orthonormalize();
	return on;
}

bool Transform2D::is_equal_approx(const Transform2D &p_transform) const {
	return elements[0].is_equal_approx(p_transform.elements[0]) && elements[1].is_equal_approx(p_transform.elements[1]) && elements[2].is_equal_approx(p_transform.elements[2]);
}

Transform2D Transform2D::looking_at(const Vector2 &p_target) const {
	Transform2D return_trans = Transform2D(get_rotation(), get_origin());
	Vector2 target_position = affine_inverse().xform(p_target);
	return_trans.set_rotation(return_trans.get_rotation() + (target_position * get_scale()).angle());
	return return_trans;
}

bool Transform2D::operator==(const Transform2D &p_transform) const {
	for (int i = 0; i < 3; i++) {
		if (elements[i] != p_transform.elements[i]) {
			return false;
		}
	}

	return true;
}

bool Transform2D::operator!=(const Transform2D &p_transform) const {
	for (int i = 0; i < 3; i++) {
		if (elements[i] != p_transform.elements[i]) {
			return true;
		}
	}

	return false;
}

void Transform2D::operator*=(const Transform2D &p_transform) {
	elements[2] = xform(p_transform.elements[2]);

	real_t x0, x1, y0, y1;

	x0 = tdotx(p_transform.elements[0]);
	x1 = tdoty(p_transform.elements[0]);
	y0 = tdotx(p_transform.elements[1]);
	y1 = tdoty(p_transform.elements[1]);

	elements[0][0] = x0;
	elements[0][1] = x1;
	elements[1][0] = y0;
	elements[1][1] = y1;
}

Transform2D Transform2D::operator*(const Transform2D &p_transform) const {
	Transform2D t = *this;
	t *= p_transform;
	return t;
}

Transform2D Transform2D::interpolate_with(const Transform2D &p_transform, const real_t p_c) const {
	//extract parameters
	Vector2 p1 = get_origin();
	Vector2 p2 = p_transform.get_origin();

	real_t r1 = get_rotation();
	real_t r2 = p_transform.get_rotation();

	Size2 s1 = get_scale();
	Size2 s2 = p_transform.get_scale();

	//slerp rotation
	Vector2 v1(Math::cos(r1), Math::sin(r1));
	Vector2 v2(Math::cos(r2), Math::sin(r2));

	real_t dot = v1.dot(v2);

	dot = CLAMP(dot, -1.0, 1.0);

	Vector2 v;

	if (dot > 0.9995) {
		v = v1.lerp(v2, p_c).normalized(); //linearly interpolate to avoid numerical precision issues
	} else {
		real_t angle = p_c * Math::acos(dot);
		Vector2 v3 = (v2 - v1 * dot).normalized();
		v = v1 * Math::cos(angle) + v3 * Math::sin(angle);
	}

	//construct matrix
	Transform2D res(v.angle(), p1.lerp(p2, p_c));
	res.pre_scale(s1.lerp(s2, p_c));
	return res;
}

void Transform2D::operator*=(const real_t p_val) {
	elements[0] *= p_val;
	elements[1] *= p_val;
	elements[2] *= p_val;
}

Transform2D Transform2D::operator*(const real_t p_val) const {
	Transform2D ret(*this);
	ret *= p_val;
	return ret;
}

Transform2D::operator String() const {
	return "[X: " + elements[0].operator String() +
			", Y: " + elements[1].operator String() +
			", O: " + elements[2].operator String() + "]";
}
