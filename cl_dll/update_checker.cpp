#ifdef UPDATE_CHECK
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <thread>

#ifndef _WIN32
#include <dlfcn.h>
#endif

#define CURL_STATICLIB
#include "curl/curl.h"

#undef min

using namespace std;

namespace update_checker
{
	#define UPDATE_URL "http://play.sourceruns.org/yalter/OpenAG/latest_version"

	// Replace this with the latest commit timestamp.
	// git show -s --format=%ct
	#include "current_version.h"

	namespace
	{
		atomic<bool> update_available{ false };

		#ifdef _WIN32
		constexpr auto my_curl_easy_init = curl_easy_init;
		constexpr auto my_curl_easy_setopt = curl_easy_setopt;
		constexpr auto my_curl_easy_perform = curl_easy_perform;
		constexpr auto my_curl_easy_cleanup = curl_easy_cleanup;
		#else
		// Due to problems with CURL_OPENSSL_{3,4}, load libcurl statically on Linux.
		std::once_flag flag_get_curl_pointers;

		decltype(curl_easy_init)* my_curl_easy_init = nullptr;
		decltype(curl_easy_setopt)* my_curl_easy_setopt = nullptr;
		decltype(curl_easy_perform)* my_curl_easy_perform = nullptr;
		decltype(curl_easy_cleanup)* my_curl_easy_cleanup = nullptr;

		void get_curl_pointers()
		{
			#ifdef __APPLE__
			constexpr auto libcurl_name = "libcurl.4.dylib";
			#else
			constexpr auto libcurl_name = "libcurl.so.4";
			#endif

			auto so = dlopen(libcurl_name, RTLD_LAZY);
			if (so) {
				#define GET(ptr, name) \
					ptr = reinterpret_cast<decltype(ptr)>(dlsym(so, name))

				GET(my_curl_easy_init, "curl_easy_init");
				GET(my_curl_easy_setopt, "curl_easy_setopt");
				GET(my_curl_easy_perform, "curl_easy_perform");
				GET(my_curl_easy_cleanup, "curl_easy_cleanup");

				#undef GET
			}
		}
		#endif

		struct user_data
		{
			uint64_t value;
			size_t bytes_received;

			user_data() : bytes_received(0)
			{
			}
		};

		bool is_curl_available()
		{
			return my_curl_easy_init
				&& my_curl_easy_setopt
				&& my_curl_easy_cleanup
				&& my_curl_easy_perform;
		}

		size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
		{
			auto total_size = size * nmemb;
			if (total_size == 0)
				return 0;

			auto target = reinterpret_cast<user_data*>(userdata);

			auto size_to_read = std::min(total_size, sizeof(target->value) - target->bytes_received);
			std::memcpy(&target->value + target->bytes_received, ptr, size_to_read);
			target->bytes_received += size_to_read;

			return size_to_read;
		}

		void actually_check_for_updates()
		{
			#ifndef _WIN32
			std::call_once(flag_get_curl_pointers, get_curl_pointers);
			#endif

			if (!is_curl_available())
				return;

			CURL *curl = my_curl_easy_init();

			if (curl) {
				user_data data;

				my_curl_easy_setopt(curl, CURLOPT_URL, UPDATE_URL);
				my_curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

				my_curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
				my_curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

				my_curl_easy_perform(curl);

				update_available =
					(data.bytes_received == sizeof(data.value) && data.value > CURRENT_VERSION);

				my_curl_easy_cleanup(curl);
			}
		}
	}

	void check_for_updates()
	{
		std::thread(actually_check_for_updates).detach();
	}

	bool is_update_available()
	{
		return update_available;
	}
}
#else
namespace update_checker {
	void check_for_updates()
	{
	}

	bool is_update_available()
	{
		return false;
	}
}
#endif
