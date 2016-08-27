#ifndef EFFECTS_H
#define EFFECTS_H


void fire_effect();
void camera_flash();

// struct render_args {
// 	struct effect*      effect;
// 	int                 shift_quotient;
// 	uint8_t             shift_remainder;
// 	int                 framenum;
// 	char*               framebuf;
// 	size_t              framelen;
// };

// struct effect {
// 	const char*         name;
// 	long                argument;
// 	long                arg_default;
// 	const char*         arg_desc;
// 	union color_arg     color_arg;
// 	long                screen_saver;
// 	long                sensor_driven;

// 	void (*render)(struct render_args* args);
// };

struct effect {
	uint32_t frame_delay;
	void (*render)();
};


#endif /* EFFECTS_H */