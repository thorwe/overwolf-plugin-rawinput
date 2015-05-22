/*
  Simple IO Plugin
  Copyright (c) 2014 Overwolf Ltd.
*/
#include "nsScriptableObjectRawInput.h"
#include "utils/Thread.h"

#include "plugin_method.h"
#include "plugin_method_key_capture.h"
//#include "plugin_method_xinput_capture.h"

#define REGISTER_METHOD(name, class) { \
	NPIdentifier ident = NPN_GetStringIdentifier(name); \
	methods_[ident] = new class(this, npp_); \
}

#define REGISTER_GENERIC_METHOD(name, method) { \
	generic_methods_[NPN_GetStringIdentifier(name)] = &method; \
}

nsScriptableObjectRawInput::nsScriptableObjectRawInput(NPP npp) :
  nsScriptableObjectBase(npp),
  shutting_down_(false) {
}

nsScriptableObjectRawInput::~nsScriptableObjectRawInput(void) {
  shutting_down_ = true;
  
  if (thread_key.get()) {
    thread_key->Stop();
  }
  /*if (thread_xinput.get()) {
	  thread_xinput->Stop();
  }*/
}

bool nsScriptableObjectRawInput::Init() {
#pragma region public methods
  REGISTER_METHOD("startKeyCapture", PluginMethodKeyCapture);
  REGISTER_GENERIC_METHOD("deleteKeyCaptureInstance", nsScriptableObjectRawInput::DeleteInstance);

  //REGISTER_METHOD("startXInputMonitor", PluginMethodXInputCapture);

#pragma endregion public methods

#pragma region read-only properties
#pragma endregion read-only properties

  thread_key.reset(new utils::Thread());
  //thread_xinput.reset(new utils::Thread());
  bool start_key = thread_key->Start();
  //bool start_xinput = thread_xinput->Start();
  return (start_key);// && start_xinput);
}

bool nsScriptableObjectRawInput::HasMethod(NPIdentifier name) {
#ifdef _DEBUG
  NPUTF8* name_utf8 = NPN_UTF8FromIdentifier(name);
  NPN_MemFree((void*)name_utf8);
#endif

  // does the method exist?
  return ((generic_methods_.find(name) != generic_methods_.end()) || (methods_.find(name) != methods_.end()));
}

bool nsScriptableObjectRawInput::Invoke(
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
	  GenericMethodsMap::iterator generic_iter = generic_methods_.find(name);
	  if (generic_iter == generic_methods_.end()) {
		  // should never reach here
		  NPN_SetException(this, "bad function called??");
		  return false;
	  }
	  return (this->*generic_iter->second)(name, args, argCount, result);
  }
  
  PluginMethod* plugin_method = iter->second->Clone(this, npp_, args, argCount, result);

  if (nullptr == plugin_method) {
    return false;
  }
  
  utils::Thread* thread = 0;
  if (plugin_method->GetName().compare("RawInputMonitor") == 0) {
	  
	  thread = thread_key.get();
	  if (thread->isQueueEmpty())	// this thread only has one task
		  return thread->PostTask(std::bind(&nsScriptableObjectRawInput::ExecuteMethod, this, plugin_method));
	  else
		  return true;
  }
  else
  {
	  NPN_SetException(this, "could not find thread??");
	  return false;
  }
  
}

/************************************************************************/
/* Public properties
/************************************************************************/
bool nsScriptableObjectRawInput::HasProperty(NPIdentifier name) {
#ifdef _DEBUG
  NPUTF8* name_utf8 = NPN_UTF8FromIdentifier(name);
  NPN_MemFree((void*)name_utf8);
#endif

  // does the property exist?
  return (properties_.find(name) != properties_.end());
}

bool nsScriptableObjectRawInput::GetProperty(
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

bool nsScriptableObjectRawInput::SetProperty(
  NPIdentifier name, const NPVariant *value) {
  NPN_SetException(this, "this property is read-only!");
  return true;
}


/************************************************************************/
/*
/************************************************************************/
void nsScriptableObjectRawInput::ExecuteMethod(PluginMethod* method) {

  while ((!shutting_down_) && nullptr != method) {
	  method->Execute();

	  if (!method->HasCallback()) {
		delete method;
		return;
	  }

	  NPN_PluginThreadAsyncCall(
		npp_, 
		nsScriptableObjectRawInput::ExecuteCallback, 
		method);
  }
  if (nullptr != method)
	  delete method;
}

//static
void nsScriptableObjectRawInput::ExecuteCallback(void* method) {
  if (nullptr == method) {
    return;
  }

  PluginMethod* plugin_method = reinterpret_cast<PluginMethod*>(method);
  plugin_method->TriggerCallback();

  // delete plugin_method;
}

bool nsScriptableObjectRawInput::DeleteInstance(
	NPIdentifier name,
	const NPVariant *args,
	uint32_t argCount,
	NPVariant *result) {
	if (argCount != 2 ||
		!NPVARIANT_IS_DOUBLE(args[0]) ||
		!NPVARIANT_IS_OBJECT(args[1])) {
		NPN_SetException(this, "(DeleteInstance) invalid params passed to function");
		return true;
	}
	int32_t id = (int32_t)(int)floor(NPVARIANT_TO_DOUBLE(args[0]) + 0.5);	// all numbers come in as double in chrome...

	bool foundAndDeleted = PluginMethodKeyCapture::DeleteInstance(id);

	NPVariant out_args[2];
	NPVariant ret_val;

	BOOLEAN_TO_NPVARIANT(foundAndDeleted, out_args[0]);
	INT32_TO_NPVARIANT(id, out_args[1]);

	// fire callback
	NPN_InvokeDefault(
		__super::npp_,
		NPVARIANT_TO_OBJECT(args[1]),
		out_args,
		2,
		&ret_val);

	NPN_ReleaseVariantValue(&ret_val);
	return true;
}