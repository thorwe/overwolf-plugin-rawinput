#ifndef PLUGIN_METHODS_PLUGIN_METHOD_KEY_CAPTURE_H_
#define PLUGIN_METHODS_PLUGIN_METHOD_KEY_CAPTURE_H_

#include "plugin_method.h"
#include <string>
#include <map>

class PluginMethodKeyCapture : public PluginMethod {
public:
	PluginMethodKeyCapture(NPObject* object, NPP npp);

public:
	virtual PluginMethod* Clone(
		NPObject* object,
		NPP npp,
		const NPVariant *args,
		uint32_t argCount,
		NPVariant *result);
	virtual bool HasCallback();
	virtual void Execute();
	virtual void TriggerCallback();
	static bool DeleteInstance(int32_t id);

protected:
	NPObject* callback_;
	std::string input_;

	// callback
	std::string output_;

	int32_t id_;

private:
	SHORT last_keyCode;
	bool done_;
};


#endif // PLUGIN_METHODS_PLUGIN_METHOD_KEY_CAPTURE_H_