/*
  Simple IO Plugin
  Copyright (c) 2014 Overwolf Ltd.
*/
#include "nsPluginInstanceRawInput.h"
#include "nsScriptableObjectRawInput.h" // our specific API

// we use this to force our plugin container to shut down
// when no one is using it.  Browsers try to keep the plugin
// open for optimization reasons - we don't want it
int nsPluginInstanceRawInput::ref_count_ = 0;

////////////////////////////////////////
//
// nsPluginInstance class implementation
//
nsPluginInstanceRawInput::nsPluginInstanceRawInput(NPP instance) :
  nsPluginInstanceBase(),
  instance_(instance),
  initialized_(FALSE),
  scriptable_object_(nullptr) {

  nsPluginInstanceRawInput::ref_count_++;
}

nsPluginInstanceRawInput::~nsPluginInstanceRawInput() {
  nsPluginInstanceRawInput::ref_count_--;

  if (0 == nsPluginInstanceRawInput::ref_count_) {
    PostQuitMessage(0);
  }
}

// NOTE:
// ------
// Overwolf plugins should not implement windows - NPAPI will
// probably be removed in the near feature and will be changed
// by a different method that will only support non-visual
// plugins
NPBool nsPluginInstanceRawInput::init(NPWindow* window) {
  // no GUI to init in windowless case
  initialized_ = TRUE;
  return TRUE;
}

void nsPluginInstanceRawInput::shut() {
  if (nullptr != scriptable_object_) {
    NPN_ReleaseObject(scriptable_object_);
  }

  initialized_ = FALSE;
}

NPBool nsPluginInstanceRawInput::isInitialized() {
  return initialized_;
}

// here we supply our scriptable object
NPError nsPluginInstanceRawInput::GetValue(
  NPPVariable variable, void* ret_value) {
  
  NPError rv = NPERR_INVALID_PARAM;

  switch (variable) {
    case NPPVpluginScriptableNPObject:
    {
      if (nullptr == scriptable_object_) {
        scriptable_object_ = 
          NPN_CreateObject(
            instance_, 
            GET_NPOBJECT_CLASS(nsScriptableObjectRawInput));

        NPN_RetainObject(scriptable_object_);

        ((nsScriptableObjectRawInput*)scriptable_object_)->Init();
        *(NPObject **)ret_value = scriptable_object_;
      }

      rv = NPERR_NO_ERROR;
      return rv;
    }
    default:
      break;
  }

  return rv;
}