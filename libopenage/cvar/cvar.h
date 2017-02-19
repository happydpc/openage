// Copyright 2016-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <stdlib.h>
// pxd: from libcpp.string cimport string
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <mutex>

// pxd: from libopenage.pyinterface.functional cimport PyIfFunc0, PyIfFunc2, PyIfFunc3
#include "../pyinterface/functional.h"
// pxd: from libopenage.util.path cimport Path
#include "../util/path.h"


namespace qtsdl {
class GuiItemLink;
} // qtsdl

namespace openage {
namespace cvar {

class CVarChangedCallback;

/** function type used to get a configuration value */
using get_func = std::function<std::string()>;

/** function type to set the value of a config entry */
using set_func = std::function<void(std::string)>;


/**
 * Configuration manager.
 *
 * Stores key-value pairs of config data.
 * Actually it doesn't store data, instead functions
 * that perform/fetch the configuration at the appropriate place.
 *
 * pxd:
 *
 * cppclass CVarManager:
 *     string get(string name) except +
 *     void set(string name, string value) except +
 *     void load_config(string path) except +
 */
class CVarManager {

public:
	CVarManager(const util::Path &path);
	~CVarManager();

	/**
	 * Creates a configuration entry
	 * @returns if the entry name was successful.
	 *          It won't be if the name was used before.
	 */
	bool create(const std::string &name,
	            const std::pair<get_func, set_func> &accessors);

	/**
	 * Gets the value of a config entry.
	 * Internally calls the stored get function.
	 * MT safe.
	 */
	std::string get(const std::string &name) const;

	/**
	 * Gets all names of config entries.
	 * MT safe.
	 */
	std::vector<std::string> get_names() const;

	/**
	 * Sets the config entry value.
	 * MT safe.
	 */
	void set(const std::string &name, const std::string &value);

	/**
	 * Performs the loading of a configuration file
	 * via the Python implementation.
	 */
	void load_config(const util::Path &path);

	/**
	 * Perform the load of the default config files.
	 */
	void load_all();

private:
	friend class CVarChangedCallback;

	void add_callback(CVarChangedCallback &callback);
	void replace_callback(CVarChangedCallback &former, CVarChangedCallback &current);
	void remove_callback(CVarChangedCallback &callback);

	/**
	 * Store the key-value pair of config options.
	 * The clue is to store the "what does the config do",
	 * not the actual value.
	 * That way the system is universal.
	 */
	std::unordered_map<std::string, std::pair<get_func, set_func>> store;
	mutable std::mutex store_mutex;

	/**
	 * Options that are set but not yet created.
	 */
	std::vector<std::pair<std::string, std::string>> orphaned;

	/**
	 * Callbacks to call when any option is being set.
	 */
	std::vector<CVarChangedCallback*> callbacks;

	/**
	 * Avoid saving caused by loading.
	 */
	bool config_loading;

	/**
	 * Magic path that stores config files.
	 * Auto-redirects to the default and write paths in the home folder.
	 * It is set up in openage/cvar/location.py and openage/game/main.py
	 */
	util::Path path;

public:
	/**
	 * Link to the Qt GUI.
	 */
	qtsdl::GuiItemLink *gui_link;
};


/**
 * Callback installer.
 *
 * Installs a callback on CVarManager to track changes in options.
 */
class CVarChangedCallback {
public:
	using Handler = std::function<void(const std::string&)>;

	explicit CVarChangedCallback();

	/**
	 * @param cvar_manager
	 * @param handler
	 */
	explicit CVarChangedCallback(CVarManager &cvar_manager, Handler handler);
	~CVarChangedCallback();

	CVarChangedCallback(CVarChangedCallback &&o);
	CVarChangedCallback& operator=(CVarChangedCallback &&o);

	/**
	 * @return nullptr if not attached to a manager
	 */
	CVarManager* manager() const;

private:
	CVarChangedCallback(const CVarChangedCallback&) = delete;
	CVarChangedCallback& operator=(const CVarChangedCallback&) = delete;

	friend class CVarManager;

	void operator()(const std::string &name) const;

	CVarManager *cvar_manager;
	Handler handler;
};


/**
 * Python function to load a configuration file.
 * The config manager is passed into it.
 *
 * pxd:
 * ctypedef CVarManager * CVarManagerPtr
 * PyIfFunc2[void, CVarManagerPtr, const Path&] pyx_load_config_file
 */
extern pyinterface::PyIfFunc<void, CVarManager *, const util::Path &> pyx_load_config_file;


/**
 * Python function to set a value in a configuration file.
 * The file name is passed into it.
 *
 * pxd:
 * PyIfFunc3[void, const Path&, string, string] pyx_config_file_set_option
 */
extern pyinterface::PyIfFunc<void, const util::Path &, std::string, std::string> pyx_config_file_set_option;

}} // openage::cvar
