#ifndef PLUGIN_METHODS_PLUGIN_METHOD_KEY_CAPTURE_H_
#define PLUGIN_METHODS_PLUGIN_METHOD_KEY_CAPTURE_H_

#include "plugin_method.h"
#include <string>
#include <map>
//#include <mutex>
#include "utils\CriticalSectionLock.h"

class PluginMethodKeyCapture : public PluginMethod {
public:
	PluginMethodKeyCapture(NPObject* object, NPP npp);
	~PluginMethodKeyCapture();

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

	virtual std::string GetName() { return "RawInputMonitor"; };
	virtual int32_t CallbacksCount();

protected:
	//NPObject* callback_;
	std::map<int32_t, NPObject*> callbacks_;
	
	utils::CriticalSection lockerCbks_;	////std::mutex mutex_callbacks_; msvc 2010 not available; CriticalSection is a better choice anyways (faster)

	// callback
	std::string output_;

	//int32_t id_;

private:
	SHORT last_keyCode;
	bool done_;

	static PluginMethodKeyCapture* instance_;
	utils::CriticalSection locker_instance_; //std::mutex mutex_instance_;
};


#endif // PLUGIN_METHODS_PLUGIN_METHOD_KEY_CAPTURE_H_