/*************************************************************************/
/*  gjk_epa.h                                                            */
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

#ifndef GJK_EPA_H
#define GJK_EPA_H

#include "collision_solver_3d_sw.h"
#include "shape_3d_sw.h"

typedef struct {
	enum eStatus {
		Separated, // Shapes doesn't penetrate
		Penetrating, // Shapes are penetrating
		GJK_Failed, // GJK phase fail, no big issue, shapes are probably just 'touching'
		EPA_Failed // EPA phase fail, bigger problem, need to save parameters, and debug
	} status;
	Vector3 witnesses[2];
	Vector3 normal;
	real_t distance;
} GjkEpaResult;

bool gjk_epa_calculate_penetration(const Shape3DSW *p_shape_A, const Transform &p_transform_A, const Shape3DSW *p_shape_B, const Transform &p_transform_B, GjkEpaResult &r_result);
bool gjk_epa_calculate_distance(const Shape3DSW *p_shape_A, const Transform &p_transform_A, const Shape3DSW *p_shape_B, const Transform &p_transform_B, GjkEpaResult &r_result);

#endif
