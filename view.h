extern cvar_t v_gamma;

extern uint8_t gammatable[256]; // palette is sent through this
extern uint8_t ramps[3][256];
extern float v_blend[4];

extern cvar_t lcd_x;

void V_Init (void);
void V_RenderView (void);
float V_CalcRoll (vec3_t angles, vec3_t velocity);
void V_UpdatePalette (void);

