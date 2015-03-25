#include "plugin_method_xinput_capture.h"

#include "utils/VersionHelpers.h"

#include "utils\Encoders.h"

#include "utils/easyx/easyx.hpp"

int32_t xinput_id = 0;	// there is quite likely a better way to do this...
std::map<int32_t, PluginMethodXInputCapture*> instanceMap;

// fileExists( filename, callback(status) )
PluginMethodXInputCapture::PluginMethodXInputCapture(NPObject* object, NPP npp) :
PluginMethod(object, npp) {
}

bool PluginMethodXInputCapture::DeleteInstance(int32_t id_in)	// static
{
	if (instanceMap.find(id_in) == instanceMap.end())
		return false;

	PluginMethodXInputCapture* instance = instanceMap[id_in];
	instance->callback_ = nullptr;

	// delete instance;
	instanceMap.erase(id_in);
	return true;
}

//virtual 
PluginMethod* PluginMethodXInputCapture::Clone(
	NPObject* object,
	NPP npp,
	const NPVariant *args,
	uint32_t argCount,
	NPVariant *result) {

	PluginMethodXInputCapture* clone =
		new PluginMethodXInputCapture(object, npp);

	try {
		if (argCount != 2 ||
			!NPVARIANT_IS_OBJECT(args[0]) ||
			!NPVARIANT_IS_OBJECT(args[1])) {
			NPN_SetException(
				__super::object_,
				"invalid params passed to function");
			delete clone;
			return nullptr;
		}
		if (!IsWindowsVistaOrGreater()) {
			NPN_SetException(
				__super::object_,
				"Windows Vista or higher required currently.");
			delete clone;
			return nullptr;
		}

		clone->last_ltrigger_type_ = "RELEASE";
		clone->last_rtrigger_type_ = "RELEASE";

		clone->callback_ = NPVARIANT_TO_OBJECT(args[1]);


		// add ref count to callback object so it won't delete
		NPN_RetainObject(clone->callback_);


		// id + callback
		clone->id_ = xinput_id;
		++xinput_id;

		instanceMap.insert(std::make_pair(clone->id_, clone));
		NPVariant arg;
		NPVariant ret_val;
		NPObject* id_callback = NPVARIANT_TO_OBJECT(args[0]);

		INT32_TO_NPVARIANT(
			clone->id_,
			arg
			);

		// fire callback
		NPN_InvokeDefault(
			__super::npp_,
			id_callback,
			&arg,
			1,
			&ret_val);

		NPN_ReleaseVariantValue(&ret_val);
		// / id + callback

		return clone;
	}
	catch (...) {

	}

	delete clone;
	return nullptr;
}

// virtual
bool PluginMethodXInputCapture::HasCallback() {
	return (nullptr != callback_);
}

// virtual
void PluginMethodXInputCapture::Execute() {

	easyx::Event event;

	done_ = false;
	while (!done_) {
		easyx::update();
		while (easyx::poll(&event)) {

			int eventType = event.type;

			output_gamepad_ = event.gamepad;
			switch (eventType) {
			case EASYX_EVENT_CONNECT:
				output_type_ = "CONNECT";
				output_which_ = "";
				done_ = true;
				break;
			case EASYX_EVENT_DISCONNECT:
				output_type_ = "DISCONNECT";
				output_which_ = "";
				done_ = true;
				break;
			case EASYX_EVENT_PRESS:
				output_type_ = "PRESS";
				output_which_ = easyx::name(event.which);
				done_ = true;
				break;
			case EASYX_EVENT_RELEASE:
				output_type_ = "RELEASE";
				output_which_ = easyx::name(event.which);
				if (output_which_.length() == 0) {
					if (event.which == EASYX_LTRIGGER) {
						output_which_ = "LTRIGGER";
						last_ltrigger_type_ = output_type_;
					}
					else if (event.which == EASYX_RTRIGGER) {
						output_which_ = "RTRIGGER";
						last_rtrigger_type_ = output_type_;
					}
					else
						output_which_ = std::to_string(event.which);
				}
				done_ = true;
				break;
			case EASYX_EVENT_DOWN:
				switch (event.which) {
				case EASYX_LTRIGGER:
					output_type_ = "PRESS";
					output_which_ = "LTRIGGER";
					if (last_ltrigger_type_ == "RELEASE") {
						last_ltrigger_type_ = "PRESS";
						done_ = true;
					}
					break;
				case EASYX_RTRIGGER:
					output_type_ = "PRESS";
					output_which_ = "RTRIGGER";
					if (last_rtrigger_type_ == "RELEASE") {
						last_rtrigger_type_ = "PRESS";
						done_ = true;
					}
					break;
				}
				if (done_)
					break;
				/*output_which_ = easyx::name(event.which);
				if (output_which_.length() == 0) {
					if (event.which == EASYX_LTRIGGER)
						output_which_ = "LTRIGGER";
					else if (event.which == EASYX_RTRIGGER)
						output_which_ = "RTRIGGER";
					else
						output_which_ = std::to_string(event.which);
				}
				if (event.which == EASYX_LTRIGGER) {
					if (last_ltrigger_type_ == "RELEASE") {
						last_ltrigger_type_ = output_type_;
						done_ = true;
					}

				}
				else if (event.which == EASYX_RTRIGGER) {
					if (last_rtrigger_type_ == "RELEASE") {
						last_rtrigger_type_ = output_type_;
						done_ = true;
					}
				}
				else
					done_ = true;*/

				break;
			default:	// should not happen at all actually
				output_type_ = "";
				output_which_ = "";
			}
		}
	}
}

// virtual
void PluginMethodXInputCapture::TriggerCallback() {
	if (!HasCallback())
		return;

	NPVariant args[3];
	NPVariant ret_val;

	STRINGN_TO_NPVARIANT(
		output_type_.c_str(),
		output_type_.size(),
		args[0]);

	STRINGN_TO_NPVARIANT(
		output_which_.c_str(),
		output_which_.size(),
		args[1]);

	INT32_TO_NPVARIANT(
		output_gamepad_,
		args[2]);

	// fire callback
	NPN_InvokeDefault(
		__super::npp_,
		callback_,
		args,
		3,
		&ret_val);

	NPN_ReleaseVariantValue(&ret_val);
}