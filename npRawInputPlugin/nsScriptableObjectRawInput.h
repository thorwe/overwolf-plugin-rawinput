/*
  RawInput Plugin
  Copyright (c) 2015 Thorsten Weinz
*/
#ifndef NNSSCRIPTABLEOBJECTRAWINPUT_H_
#define NNSSCRIPTABLEOBJECTRAWINPUT_H_

#include "nsScriptableObjectBase.h"
#include <map>

namespace utils {
class Thread; // forward declaration
}

class PluginMethod;

class nsScriptableObjectRawInput : public nsScriptableObjectBase {
public:
	nsScriptableObjectRawInput(NPP npp);
	virtual ~nsScriptableObjectRawInput(void);

public:
  bool Init();

// nsScriptableObjectBase overrides
public:
  virtual bool HasMethod(NPIdentifier name);
  virtual bool Invoke(
    NPIdentifier name, 
    const NPVariant *args, 
    uint32_t argCount, 
    NPVariant *result);
  virtual bool HasProperty(NPIdentifier name);
  virtual bool GetProperty(NPIdentifier name, NPVariant *result);
  virtual bool SetProperty(NPIdentifier name, const NPVariant *value);

private:
  void ExecuteMethod(PluginMethod* method);
  static void ExecuteCallback(void* method);

  bool DeleteInstance(
	  NPIdentifier name,
	  const NPVariant *args,
	  uint32_t argCount,
	  NPVariant *result);


// member variables
private:
  // holds the public methods
  typedef std::map<NPIdentifier, PluginMethod*> MethodsMap;
  MethodsMap methods_;

  // holds the public methods
  typedef std::map<NPIdentifier, std::string> PropertiesMap;
  PropertiesMap properties_;

  // generic method style
  // defines a generic method
  typedef bool (nsScriptableObjectRawInput::*GenericMethodHandle)(
	  NPIdentifier,
	  const NPVariant*,
	  uint32_t,
	  NPVariant*);

  // holds the public methods
  typedef std::map<NPIdentifier, GenericMethodHandle> GenericMethodsMap;
  GenericMethodsMap generic_methods_;

  // good idea for when having an autonomous thread sending callbacks
  bool shutting_down_;

  // this allows us to run our code on a separate thread than the 
  // main browser thread - to be more responsive
  std::auto_ptr<utils::Thread> thread_key;
  std::auto_ptr<utils::Thread> thread_xinput;
};

// declare our NPObject-derived scriptable object class
DECLARE_NPOBJECT_CLASS_WITH_BASE(
  nsScriptableObjectRawInput, 
  AllocateNpObject<nsScriptableObjectRawInput>);


#endif // NNSSCRIPTABLEOBJECTRAWINPUT_H_