/*
  Simple IO Plugin
  Copyright (c) 2014 Overwolf Ltd.
*/
#include "nsPluginInstanceKeyCapture.h"
#include "nsScriptableObjectKeyCapture.h" // our specific API

// we use this to force our plugin container to shut down
// when no one is using it.  Browsers try to keep the plugin
// open for optimization reasons - we don't want it
int nsPluginInstanceKeyCapture::ref_count_ = 0;

////////////////////////////////////////
//
// nsPluginInstanceKeyCapture class implementation
//
nsPluginInstanceKeyCapture::nsPluginInstanceKeyCapture(NPP instance) :
  nsPluginInstanceBase(),
  instance_(instance),
  initialized_(FALSE),
  scriptable_object_(nullptr) {

  nsPluginInstanceKeyCapture::ref_count_++;
}

nsPluginInstanceKeyCapture::~nsPluginInstanceKeyCapture() {
  nsPluginInstanceKeyCapture::ref_count_--;

  if (0 == nsPluginInstanceKeyCapture::ref_count_) {
    PostQuitMessage(0);
  }
}

// NOTE:
// ------
// Overwolf plugins should not implement windows - NPAPI will
// probably be removed in the near feature and will be changed
// by a different method that will only support non-visual
// plugins
NPBool nsPluginInstanceKeyCapture::init(NPWindow* window) {
  // no GUI to init in windowless case
  initialized_ = TRUE;
  return TRUE;
}

void nsPluginInstanceKeyCapture::shut() {
  if (nullptr != scriptable_object_) {
    NPN_ReleaseObject(scriptable_object_);
  }

  initialized_ = FALSE;
}

NPBool nsPluginInstanceKeyCapture::isInitialized() {
  return initialized_;
}

// here we supply our scriptable object
NPError nsPluginInstanceKeyCapture::GetValue(
  NPPVariable variable, void* ret_value) {
  
  NPError rv = NPERR_INVALID_PARAM;

  switch (variable) {
    case NPPVpluginScriptableNPObject:
    {
      if (nullptr == scriptable_object_) {
        scriptable_object_ = 
          NPN_CreateObject(
            instance_, 
            GET_NPOBJECT_CLASS(nsScriptableObjectKeyCapture));

        NPN_RetainObject(scriptable_object_);

        ((nsScriptableObjectKeyCapture*)scriptable_object_)->Init();
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