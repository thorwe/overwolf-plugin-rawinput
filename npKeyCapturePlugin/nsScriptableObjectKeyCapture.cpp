/*
  Simple IO Plugin
  Copyright (c) 2014 Overwolf Ltd.
*/
#include "nsScriptableObjectKeyCapture.h"
#include "utils/Thread.h"

#include "plugin_method.h"
#include "plugin_method_key_capture.h"

#define REGISTER_METHOD(name, class) { \
  methods_[NPN_GetStringIdentifier(name)] = \
    new class(this, npp_); \
}

nsScriptableObjectKeyCapture::nsScriptableObjectKeyCapture(NPP npp) :
  nsScriptableObjectBase(npp),
  shutting_down_(false) {
}

nsScriptableObjectKeyCapture::~nsScriptableObjectKeyCapture(void) {
  shutting_down_ = true;
  
  if (thread_.get()) {
    thread_->Stop();
  }
}

bool nsScriptableObjectKeyCapture::Init() {
#pragma region public methods
  REGISTER_METHOD("startKeyCapture", PluginMethodKeyCapture);


#pragma endregion public methods

#pragma region read-only properties
#pragma endregion read-only properties

  thread_.reset(new utils::Thread());
  return thread_->Start();
}

bool nsScriptableObjectKeyCapture::HasMethod(NPIdentifier name) {
#ifdef _DEBUG
  NPUTF8* name_utf8 = NPN_UTF8FromIdentifier(name);
  NPN_MemFree((void*)name_utf8);
#endif

  // does the method exist?
  return (methods_.find(name) != methods_.end());
}

bool nsScriptableObjectKeyCapture::Invoke(
  NPIdentifier name, 
  const NPVariant *args, 
  uint32_t argCount, 
  NPVariant *result) {
#ifdef _DEBUG
      NPUTF8* szName = NPN_UTF8FromIdentifier(name);
      NPN_MemFree((void*)szName);
#endif

  // dispatch method to appropriate handler
  MethodsMap::iterator iter = methods_.find(name);
  
  if (iter == methods_.end()) {
    // should never reach here
    NPN_SetException(this, "bad function called??");
    return false;
  }

  PluginMethod* plugin_method = 
    iter->second->Clone(this, npp_, args, argCount, result);

  if (nullptr == plugin_method) {
    return false;
  }

  // post to separate thread so that we are responsive
  return thread_->PostTask(
    std::bind(
    &nsScriptableObjectKeyCapture::ExecuteMethod, 
    this,
    plugin_method));
}

/************************************************************************/
/* Public properties
/************************************************************************/
bool nsScriptableObjectKeyCapture::HasProperty(NPIdentifier name) {
#ifdef _DEBUG
  NPUTF8* name_utf8 = NPN_UTF8FromIdentifier(name);
  NPN_MemFree((void*)name_utf8);
#endif

  // does the property exist?
  return (properties_.find(name) != properties_.end());
}

bool nsScriptableObjectKeyCapture::GetProperty(
  NPIdentifier name, NPVariant *result) {

  PropertiesMap::iterator iter = properties_.find(name);
  if (iter == properties_.end()) {
    NPN_SetException(this, "unknown property!?");
    return true;
  }

  char *resultString = (char*)NPN_MemAlloc(iter->second.size());
  memcpy(
    resultString, 
    iter->second.c_str(), 
    iter->second.size());

  STRINGN_TO_NPVARIANT(resultString, iter->second.size(), *result);

  return true;
}

bool nsScriptableObjectKeyCapture::SetProperty(
  NPIdentifier name, const NPVariant *value) {
  NPN_SetException(this, "this property is read-only!");
  return true;
}


/************************************************************************/
/*
/************************************************************************/
void nsScriptableObjectKeyCapture::ExecuteMethod(PluginMethod* method) {
  if (shutting_down_) {
    return;
  }

  if (nullptr == method) {
    return;
  }

  while ((!shutting_down_) && nullptr != method) {
	  method->Execute();

	  if (!method->HasCallback()) {
		delete method;
		return;
	  }

	  NPN_PluginThreadAsyncCall(
		npp_, 
		nsScriptableObjectKeyCapture::ExecuteCallback, 
		method);
  }
  if (nullptr != method)
	  delete method;
}

//static
void nsScriptableObjectKeyCapture::ExecuteCallback(void* method) {
  if (nullptr == method) {
    return;
  }

  PluginMethod* plugin_method = reinterpret_cast<PluginMethod*>(method);
  plugin_method->TriggerCallback();

  // delete plugin_method;
}
