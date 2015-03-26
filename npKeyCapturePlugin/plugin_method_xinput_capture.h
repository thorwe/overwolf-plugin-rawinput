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

	virtual std::string GetName() { return "XInputMonitor"; };
	
protected:
	NPObject* callback_;

	// callback
	std::string output_type_;
	std::string output_which_;
	int output_gamepad_;
	int16_t output_angle_;
	
	int32_t id_;

private:
	std::string last_ltrigger_type_;
	std::string last_rtrigger_type_;
	int16_t last_thumb_l_x;
	int16_t last_thumb_l_y;
	int16_t last_thumb_r_x;
	int16_t last_thumb_r_y;
	bool done_;
};


#endif // PLUGIN_METHODS_PLUGIN_METHOD_XINPUT_CAPTURE_H_
