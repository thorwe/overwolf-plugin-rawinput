#ifndef NSPLUGININSTANCEKEYCAPTURE_H
#define NSPLUGININSTANCEKEYCAPTURE_H

#include <memory> // smart ptrs
#include <windows.h> // required for pluginbase.h
#include "plugin_common/pluginbase.h" // nsPluginInstanceBase

class nsPluginInstanceKeyCapture : public nsPluginInstanceBase {
public:
	nsPluginInstanceKeyCapture(NPP instance);
	virtual ~nsPluginInstanceKeyCapture();

public:
  NPBool init(NPWindow* window);
  void shut();
  NPBool isInitialized();

  NPError GetValue(NPPVariable variable, void* scriptable_object_);

  // NOTE: do not implement windows within an Overwolf plugin,
  // this will not work in future versions of Overwolf
  //NPError SetWindow(NPWindow* aWindow);

private:
  NPP instance_;
  NPBool initialized_;
  
  NPObject* scriptable_object_;

  static int ref_count_;
};

#endif // NSPLUGININSTANCEKEYCAPTURE_H