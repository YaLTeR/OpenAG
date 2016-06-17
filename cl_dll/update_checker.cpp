#ifdef UPDATE_CHECK
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <thread>

#define CURL_STATICLIB
#include "curl/curl.h"

#undef min

using namespace std;

namespace update_checker
{
	#define UPDATE_URL "http://play.sourceruns.org/yalter/AG-Compat/latest_version"

	// Replace this with the latest commit timestamp.
	// git show -s --format=%ct
	//#define CURRENT_VERSION 0ull
	#define CURRENT_VERSION UINT64_MAX

	namespace
	{
		atomic<bool> update_available{ false };

		struct user_data
		{
			uint64_t value;
			size_t bytes_received;

			user_data() : bytes_received(0)
			{
			}
		};

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
			CURL *curl = curl_easy_init();

			if (curl) {
				user_data data;

				curl_easy_setopt(curl, CURLOPT_URL, UPDATE_URL);
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

				curl_easy_perform(curl);

				update_available =
					(data.bytes_received == sizeof(data.value) && data.value > CURRENT_VERSION);

				curl_easy_cleanup(curl);
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