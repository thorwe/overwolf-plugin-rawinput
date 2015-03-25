#ifndef PLUGIN_METHODS_PLUGIN_METHOD_XINPUT_CAPTURE_H_
#define PLUGIN_METHODS_PLUGIN_METHOD_XINPUT_CAPTURE_H_

#include "plugin_method.h"
#include <string>
#include <map>

class PluginMethodXInputCapture : public PluginMethod {
public:
	PluginMethodXInputCapture(NPObject* object, NPP npp);

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

	// callback
	std::string output_type_;
	std::string output_which_;
	int output_gamepad_;
	
	int32_t id_;

private:
	std::string last_ltrigger_type_;
	std::string last_rtrigger_type_;
	bool done_;
};


#endif // PLUGIN_METHODS_PLUGIN_METHOD_XINPUT_CAPTURE_H_
