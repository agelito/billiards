// linux_mouse_raw.c

#define MOUSE_XI2_MAX_AXIS 16

typedef struct
{
    int major_opcode;
    int selected_device;

    int axis_count;
    double axis_raw_data[MOUSE_XI2_MAX_AXIS];
} xinput2;

typedef struct
{
    double x;
    double y;
    double wheel;
} xinput2_mouse;

xinput2
xinput2_init(Display* display)
{
    xinput2 input = (xinput2){0};
    
    int major_opcode, first_event, first_error;
    if(!XQueryExtension(display, "XInputExtension", &major_opcode, &first_event, &first_error))
    {
	fprintf(stderr, "XInputExtension is not available\n");
	return input;
    }

    int major_version = 2;
    int minor_version = 1;
    int version_status = XIQueryVersion(display, &major_version, &minor_version);

    if(version_status == Success)
    {
	fprintf(stdout, "XInput %d.%d supported\n", major_version, minor_version);
    }
    else if(version_status == BadRequest)
    {
	fprintf(stderr, "XInput 2.1 not supported (%d.%d supported)\n", major_version, minor_version);
    }
    else
    {
	fprintf(stderr, "XIQueryVersion unknown error\n");
    }

    input.major_opcode = major_opcode;

    int selected_device = 0;
    int selected_axis_count = 0;

    int device_count;
    XIDeviceInfo* devices = XIQueryDevice(display, XIAllDevices, &device_count);
    while(device_count--)
    {
	XIDeviceInfo* device = (devices + device_count);
	if(!device->enabled) continue;

	XIValuatorClassInfo* class_info;

	int valuator_axis_count = 0;

	int class_index;
	for(class_index = 0; class_index < device->num_classes; class_index++)
	{
	    class_info = (XIValuatorClassInfo*)device->classes[class_index];
	    if(class_info->type == XIValuatorClass) valuator_axis_count++;
	}

	if(valuator_axis_count >= 2)
	{
	    selected_device = device->deviceid;
	    selected_axis_count = valuator_axis_count;
	    fprintf(stdout, "%s (%d)\n", device->name, device->deviceid); 
	    break;
	}
    }
    XIFreeDeviceInfo(devices);
    
    input.selected_device = selected_device;
    input.axis_count = selected_axis_count;

    Window root_window = DefaultRootWindow(display);

    int mask = XI_RawMotionMask;
    
    XIEventMask event_mask = (XIEventMask){0};
    event_mask.deviceid = selected_device;
    event_mask.mask_len = sizeof(int);
    event_mask.mask = (unsigned char*)&mask;

    int select_events_status = XISelectEvents(display, root_window, &event_mask, 1);
    if(select_events_status != Success)
    {
	fprintf(stderr, "couldn't select event mask %d for device %d\n", mask, selected_device);
    }

    return input;
}

int
xinput2_process_event(Display* display, xinput2* input, XEvent* event)
{
    XGenericEventCookie* cookie = &event->xcookie;
    if(cookie->type == GenericEvent &&
       cookie->extension == input->major_opcode)
    {
	if(XGetEventData(display, cookie))
	{
	    int is_xinput = (cookie->type == GenericEvent && cookie->extension == input->major_opcode);
	    if(!is_xinput)
	    {
		XFreeEventData(display, cookie);
		return 0;
	    }

	    XIRawEvent* raw_event = cookie->data;
	    if(raw_event->evtype == XI_RawMotion)
	    {
		double* values = raw_event->raw_values;
		unsigned char* mask = raw_event->valuators.mask;
		int bit_count = raw_event->valuators.mask_len * 8;
		
		int i;
		for(i = 0; i < bit_count; i++)
		{
		    if(i < input->axis_count && XIMaskIsSet(mask, i))
		    {
			double value = *(values++);
			*(input->axis_raw_data + i) = value;
		    }
		}
	    }

	    XFreeEventData(display, cookie);
	    return 1;
	}
    }

    return 0;
}

void
xinput2_reset_axis_data(xinput2* input)
{
    int i;
    for(i = 0; i < input->axis_count; i++)
    {
	*(input->axis_raw_data + i) = 0.0;
    }
}

xinput2_mouse
xinput2_get_default_axis(xinput2* input)
{
    xinput2_mouse mouse_axis = (xinput2_mouse){0};
    if(input->axis_count >= 2)
    {
	mouse_axis.x = *(input->axis_raw_data + 0);
	mouse_axis.y = *(input->axis_raw_data + 1);
    }
    if(input->axis_count >= 3)
    {
	mouse_axis.wheel = *(input->axis_raw_data + 2);
    }

    return mouse_axis;
}
