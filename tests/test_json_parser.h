/*************************************************************************/
/*  test_json_parser.h                                                   */
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

#ifndef TEST_JSON_PARSER_H
#define TEST_JSON_PARSER_H

#include "core/io/json_parser.h"

#include "thirdparty/doctest/doctest.h"

namespace TestJSONParser {

// NOTE: The current JSONParser accepts many non-conformant strings such as
// single-quoted strings, duplicate commas and trailing commas.
// This is intentionally not tested as users shouldn't rely on this behavior.

TEST_CASE("[JSONParser] Parsing single data types") {
	// Parsing a single data type as JSON is valid per the JSON specification.

	JSONParser json_parser;

	json_parser.parse("null");
	CHECK_MESSAGE(
			json_parser.get_error_line() == 0,
			"Parsing `null` as JSON should parse successfully.");
	CHECK_MESSAGE(
			json_parser.get_data() == Variant(),
			"Parsing a double quoted string as JSON should return the expected value.");

	json_parser.parse("true");
	CHECK_MESSAGE(
			json_parser.get_error_line() == 0,
			"Parsing boolean `true` as JSON should parse successfully.");
	CHECK_MESSAGE(
			json_parser.get_data(),
			"Parsing boolean `true` as JSON should return the expected value.");

	json_parser.parse("false");
	CHECK_MESSAGE(
			json_parser.get_error_line() == 0,
			"Parsing boolean `false` as JSON should parse successfully.");
	CHECK_MESSAGE(
			!json_parser.get_data(),
			"Parsing boolean `false` as JSON should return the expected value.");

	// JSON only has a floating-point number type, no integer type.
	// This is why we use `is_equal_approx()` for the comparison.
	json_parser.parse("123456");
	CHECK_MESSAGE(
			json_parser.get_error_line() == 0,
			"Parsing an integer number as JSON should parse successfully.");
	CHECK_MESSAGE(
			Math::is_equal_approx(json_parser.get_data(), 123'456),
			"Parsing an integer number as JSON should return the expected value.");

	json_parser.parse("0.123456");
	CHECK_MESSAGE(
			json_parser.get_error_line() == 0,
			"Parsing a floating-point number as JSON should parse successfully.");
	CHECK_MESSAGE(
			Math::is_equal_approx(json_parser.get_data(), 0.123456),
			"Parsing a floating-point number as JSON should return the expected value.");

	json_parser.parse("\"hello\"");
	CHECK_MESSAGE(
			json_parser.get_error_line() == 0,
			"Parsing a double quoted string as JSON should parse successfully.");
	CHECK_MESSAGE(
			json_parser.get_data() == "hello",
			"Parsing a double quoted string as JSON should return the expected value.");
}

TEST_CASE("[JSONParser] Parsing arrays") {
	JSONParser json_parser;

	// JSON parsing fails if it's split over several lines (even if leading indentation is removed).
	json_parser.parse(R"(["Hello", "world.", "This is",["a","json","array.",[]], "Empty arrays ahoy:", [[["Gotcha!"]]]])");

	const Array array = json_parser.get_data();
	CHECK_MESSAGE(
			json_parser.get_error_line() == 0,
			"Parsing a JSON array should parse successfully.");
	CHECK_MESSAGE(
			array[0] == "Hello",
			"The parsed JSON should contain the expected values.");
	const Array sub_array = array[3];
	CHECK_MESSAGE(
			sub_array.size() == 4,
			"The parsed JSON should contain the expected values.");
	CHECK_MESSAGE(
			sub_array[1] == "json",
			"The parsed JSON should contain the expected values.");
	CHECK_MESSAGE(
			sub_array[3].hash() == Array().hash(),
			"The parsed JSON should contain the expected values.");
	const Array deep_array = Array(Array(array[5])[0])[0];
	CHECK_MESSAGE(
			deep_array[0] == "Gotcha!",
			"The parsed JSON should contain the expected values.");
}

TEST_CASE("[JSONParser] Parsing objects (dictionaries)") {
	JSONParser json_parser;

	json_parser.parse(R"({"name": "Godot Engine", "is_free": true, "bugs": null, "apples": {"red": 500, "green": 0, "blue": -20}, "empty_object": {}})");

	const Dictionary dictionary = json_parser.get_data();
	CHECK_MESSAGE(
			dictionary["name"] == "Godot Engine",
			"The parsed JSON should contain the expected values.");
	CHECK_MESSAGE(
			dictionary["is_free"],
			"The parsed JSON should contain the expected values.");
	CHECK_MESSAGE(
			dictionary["bugs"] == Variant(),
			"The parsed JSON should contain the expected values.");
	CHECK_MESSAGE(
			Math::is_equal_approx(Dictionary(dictionary["apples"])["blue"], -20),
			"The parsed JSON should contain the expected values.");
	CHECK_MESSAGE(
			dictionary["empty_object"].hash() == Dictionary().hash(),
			"The parsed JSON should contain the expected values.");
}
} // namespace TestJSONParser

#endif // TEST_JSON_PARSER_H
