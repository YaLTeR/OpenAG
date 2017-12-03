#pragma once
#include <cstddef>
#include <functional>

namespace color_tags {
	/**
	 * Copies at most count characters (including the terminating null character)
	 * from src to dest, omitting the color tags. The resulting array is always
	 * null-terminated.
	 */
	void strip_color_tags(char* dest, const char* src, size_t count);

	/**
	 * Strips the color tags into an internal buffer and returns a pointer to that buffer.
	 */
	char* strip_color_tags_thread_unsafe(const char* string);

	/**
	 * Returns true if the given string contains any color tags.
	 * A color tag is something that's stripped by strip_color_tags.
	 */
	bool contains_color_tags(const char* string);

	/**
	 * Applies the given function to each individually colored substring of the given string.
	 * The arguments are substring, whether there's a color set, and the color in R, G, B.
	 * If the color is unset, values of R, G, B are undefined.
	 */
	void for_each_colored_substr(char* string,
	                             std::function<void(const char* string,
	                                                bool custom_color,
	                                                int r,
	                                                int g,
	                                                int b)> function);
}
