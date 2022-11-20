#ifndef NEWSBOAT_CONFIGCONTAINER_H_
#define NEWSBOAT_CONFIGCONTAINER_H_

#include <map>
#include <mutex>
#include <vector>

#include "configactionhandler.h"
#include "utf8string.h"

namespace newsboat {

class ConfigData;
class ConfigParser;

enum class FeedSortMethod {
	NONE,
	FIRST_TAG,
	TITLE,
	ARTICLE_COUNT,
	UNREAD_ARTICLE_COUNT,
	LAST_UPDATED
};

enum class ArtSortMethod { TITLE, FLAGS, AUTHOR, LINK, GUID, DATE, RANDOM };

enum class SortDirection { ASC, DESC };

struct FeedSortStrategy {
	FeedSortMethod sm = FeedSortMethod::NONE;
	SortDirection sd = SortDirection::DESC;

	bool operator==(const FeedSortStrategy& other) const
	{
		return sm == other.sm && sd == other.sd;
	}

	bool operator!=(const FeedSortStrategy& other) const
	{
		return !(*this == other);
	}
};

struct ArticleSortStrategy {
	ArtSortMethod sm = ArtSortMethod::DATE;
	SortDirection sd = SortDirection::ASC;

	bool operator==(const ArticleSortStrategy& other) const
	{
		return sm == other.sm && sd == other.sd;
	}

	bool operator!=(const ArticleSortStrategy& other) const
	{
		return !(*this == other);
	}
};

class ConfigContainer : public ConfigActionHandler {
public:
	ConfigContainer();
	~ConfigContainer() override;
	void register_commands(ConfigParser& cfgparser);
	void handle_action(const Utf8String& action,
		const std::vector<Utf8String>& params) override;
	void dump_config(std::vector<Utf8String>& config_output) const override;

	bool get_configvalue_as_bool(const Utf8String& key) const;
	int get_configvalue_as_int(const Utf8String& key) const;
	Utf8String get_configvalue(const Utf8String& key) const;
	void set_configvalue(const Utf8String& key, const Utf8String& value);
	void reset_to_default(const Utf8String& key);
	void toggle(const Utf8String& key);
	std::vector<Utf8String> get_suggestions(const Utf8String& fragment) const;
	FeedSortStrategy get_feed_sort_strategy() const;
	ArticleSortStrategy get_article_sort_strategy() const;

	static const Utf8String PARTIAL_FILE_SUFFIX;

private:
	std::map<Utf8String, ConfigData> config_data;
	mutable std::recursive_mutex config_data_mtx;
};

} // namespace newsboat

#endif /* NEWSBOAT_CONFIGCONTAINER_H_ */
