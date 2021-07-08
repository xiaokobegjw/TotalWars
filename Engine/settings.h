#pragma once

#include "utils/string_utils.h"
#include "math2d/math2d.h"

#include <string>
#include <list>
#include <set>
#include <mutex>

class Settings;

// Global objects
extern Settings *g_settings; // Same as Settings::getLayer(SL_GLOBAL);
extern std::string g_settings_path;

// Type for a settings changed callback function
typedef void (*SettingsChangedCallback)(const std::string &name, void *data);

typedef std::vector<
	std::pair<
		SettingsChangedCallback,
		void *
	>
> SettingsCallbackList;

typedef std::unordered_map<std::string, SettingsCallbackList> SettingsCallbackMap;

enum ValueType {
	VALUETYPE_STRING,
	VALUETYPE_FLAG // Doesn't take any arguments
};

enum SettingsParseEvent {
	SPE_NONE,
	SPE_INVALID,
	SPE_COMMENT,
	SPE_KVPAIR,
	SPE_END,
	SPE_GROUP,
	SPE_MULTILINE,
};

enum SettingsLayer {
	SL_DEFAULTS,
	SL_GAME,
	SL_GLOBAL,
	SL_MAP,
	SL_TOTAL_COUNT
};

struct ValueSpec {
	ValueSpec(ValueType a_type, const char *a_help=NULL)
	{
		type = a_type;
		help = a_help;
	}

	ValueType type;
	const char *help;
};

struct SettingsEntry {
	SettingsEntry() = default;

	SettingsEntry(const std::string &value_) :
		value(value_)
	{}

	SettingsEntry(Settings *group_) :
		group(group_),
		is_group(true)
	{}

	std::string value = "";
	Settings *group = nullptr;
	bool is_group = false;
};

typedef std::unordered_map<std::string, SettingsEntry> SettingEntries;

class Settings {
public:
	static Settings *createLayer(SettingsLayer sl, const std::string &end_tag = "");
	static Settings *getLayer(SettingsLayer sl);
	SettingsLayer getLayerType() const { return m_settingslayer; }

	Settings(const std::string &end_tag = "") :
		m_end_tag(end_tag)
	{}
	~Settings();

	Settings & operator += (const Settings &other);
	Settings & operator = (const Settings &other);

	/***********************
	 * Reading and writing *
	 ***********************/

	// Read configuration file.  Returns success.
	bool readConfigFile(const char *filename);
	//Updates configuration file.  Returns success.
	bool updateConfigFile(const char *filename);
	// NOTE: Types of allowed_options are ignored.  Returns success.
	bool parseCommandLine(int argc, char *argv[],
			std::map<std::string, ValueSpec> &allowed_options);
	bool parseConfigLines(std::istream &is);
	void writeLines(std::ostream &os, uint32_t tab_depth=0) const;

	/***********
	 * Getters *
	 ***********/

	Settings *getGroup(const std::string &name) const;
	const std::string &get(const std::string &name) const;
	bool getBool(const std::string &name) const;
	uint16_t getU16(const std::string &name) const;
	int16_t getS16(const std::string &name) const;
	uint32_t getU32(const std::string &name) const;
	int32_t getS32(const std::string &name) const;
	uint64_t getU64(const std::string &name) const;
	float getFloat(const std::string &name) const;
	std::tuple<float,float> getV2F(const std::string &name) const;
	std::tuple<float, float,float> getV3F(const std::string &name) const;
	uint32_t getFlagStr(const std::string &name, const FlagDesc *flagdesc,
			uint32_t *flagmask) const;

	// return all keys used
	std::vector<std::string> getNames() const;
	bool exists(const std::string &name) const;


	/***************************************
	 * Getters that don't throw exceptions *
	 ***************************************/

	bool getGroupNoEx(const std::string &name, Settings *&val) const;
	bool getNoEx(const std::string &name, std::string &val) const;
	bool getFlag(const std::string &name) const;
	bool getU16NoEx(const std::string &name, uint16_t &val) const;
	bool getS16NoEx(const std::string &name, int16_t &val) const;
	bool getS32NoEx(const std::string &name, int32_t &val) const;
	bool getU64NoEx(const std::string &name, uint64_t &val) const;
	bool getFloatNoEx(const std::string &name, float &val) const;
	bool getV2FNoEx(const std::string &name, std::tuple<float,float> &val) const;
	bool getV3FNoEx(const std::string &name, std::tuple<float, float, float> &val) const;

	// Like other getters, but handling each flag individualy:
	// 1) Read default flags (or 0)
	// 2) Override using user-defined flags
	bool getFlagStrNoEx(const std::string &name, uint32_t &val,
		const FlagDesc *flagdesc) const;


	/***********
	 * Setters *
	 ***********/

	// N.B. Groups not allocated with new must be set to NULL in the settings
	// tree before object destruction.
	bool setEntry(const std::string &name, const void *entry,
		bool set_group);
	bool set(const std::string &name, const std::string &value);
	bool setDefault(const std::string &name, const std::string &value);
	bool setGroup(const std::string &name, const Settings &group);
	bool setBool(const std::string &name, bool value);
	bool setS16(const std::string &name, int16_t value);
	bool setU16(const std::string &name, uint16_t value);
	bool setS32(const std::string &name, int32_t value);
	bool setU64(const std::string &name, uint64_t value);
	bool setFloat(const std::string &name, float value);
	bool setV2F(const std::string &name, std::tuple<float, float> value);
	bool setV3F(const std::string &name, std::tuple<float, float,float> value);
	bool setFlagStr(const std::string &name, uint32_t flags,
		const FlagDesc *flagdesc = nullptr, uint32_t flagmask = MAX_UINT32);

	// remove a setting
	bool remove(const std::string &name);

	/**************
	 * Miscellany *
	 **************/

	void setDefault(const std::string &name, const FlagDesc *flagdesc, uint32_t flags);
	const FlagDesc *getFlagDescFallback(const std::string &name) const;

	void registerChangedCallback(const std::string &name,
		SettingsChangedCallback cbf, void *userdata = NULL);
	void deregisterChangedCallback(const std::string &name,
		SettingsChangedCallback cbf, void *userdata = NULL);

	void removeSecureSettings();

private:
	/***********************
	 * Reading and writing *
	 ***********************/

	SettingsParseEvent parseConfigObject(const std::string &line,
		std::string &name, std::string &value);
	bool updateConfigObject(std::istream &is, std::ostream &os,
		uint32_t tab_depth=0);

	static bool checkNameValid(const std::string &name);
	static bool checkValueValid(const std::string &value);
	static std::string getMultiline(std::istream &is, size_t *num_lines=NULL);
	static void printEntry(std::ostream &os, const std::string &name,
		const SettingsEntry &entry, uint32_t tab_depth=0);

	/***********
	 * Getters *
	 ***********/
	Settings *getParent() const;

	const SettingsEntry &getEntry(const std::string &name) const;

	// Allow TestSettings to run sanity checks using private functions.
	friend class TestSettings;

	void updateNoLock(const Settings &other);
	void clearNoLock();
	void clearDefaultsNoLock();

	void doCallbacks(const std::string &name) const;

	SettingEntries m_settings;
	SettingsCallbackMap m_callbacks;
	std::string m_end_tag;

	mutable std::mutex m_callback_mutex;

	// All methods that access m_settings/m_defaults directly should lock this.
	mutable std::mutex m_mutex;

	static Settings *s_layers[SL_TOTAL_COUNT];
	SettingsLayer m_settingslayer = SL_TOTAL_COUNT;
	static std::unordered_map<std::string, const FlagDesc *> s_flags;
};
