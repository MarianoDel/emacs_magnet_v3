#ifndef PWM_H_
#define PWM_H_


//Funciones de PWM -- PWM_TIM1_CONFIG
//recibe prescaler, periodo, duty1, duty2, duty3, duty4, channel_en
#define PWM1_Init() PWM_TIM1_CONFIG (10, 1000, 0, 0, 0, 0, (PWM_CHANNEL_1 | PWM_CHANNEL_2))
#define PWM2_Init() PWM_TIM2_CONFIG (10, 1000, 0, 0, 0, 0, (PWM_CHANNEL_1 | PWM_CHANNEL_2))
#define PWM3_Init() PWM_TIM3_CONFIG (10, 1000, 0, 0, 0, 0, (PWM_CHANNEL_1 | PWM_CHANNEL_2 | PWM_CHANNEL_3 | PWM_CHANNEL_4))
#define PWM4_Init() PWM_TIM4_CONFIG (10, 1000, 0, 0, 0, 0, (PWM_CHANNEL_1 | PWM_CHANNEL_2 | PWM_CHANNEL_3 | PWM_CHANNEL_4))

#define PWM_CH1_TiempoSubida(d) 			PWM_TIM2_DUTY (d, PWM_CHANNEL_1)
#define PWM_CH1_TiempoMantenimiento(d) 		PWM_TIM2_DUTY (d, PWM_CHANNEL_2)
#define PWM_CH1_TiempoBajada(d) 			PWM_TIM1_DUTY (d, PWM_CHANNEL_1)

#define PWM_CH2_TiempoSubida(d) 			PWM_TIM1_DUTY (d, PWM_CHANNEL_2)
#define PWM_CH2_TiempoMantenimiento(d) 		PWM_TIM3_DUTY (d, PWM_CHANNEL_1)
#define PWM_CH2_TiempoBajada(d) 			PWM_TIM3_DUTY (d, PWM_CHANNEL_2)

#define PWM_CH3_TiempoSubida(d) 			PWM_TIM3_DUTY (d, PWM_CHANNEL_3)
#define PWM_CH3_TiempoMantenimiento(d) 		PWM_TIM3_DUTY (d, PWM_CHANNEL_4)
#define PWM_CH3_TiempoBajada(d) 			PWM_TIM4_DUTY (d, PWM_CHANNEL_1)

#define PWM_CH4_TiempoSubida(d) 			PWM_TIM4_DUTY (d, PWM_CHANNEL_2)
#define PWM_CH4_TiempoMantenimiento(d) 		PWM_TIM4_DUTY (d, PWM_CHANNEL_3)
#define PWM_CH4_TiempoBajada(d) 			PWM_TIM4_DUTY (d, PWM_CHANNEL_4)


enum channels_pwm
{
	PWM_CHANNEL_1 = 0x01,
	PWM_CHANNEL_2 = 0x02,
	PWM_CHANNEL_3 = 0x04,
	PWM_CHANNEL_4 = 0x08
};

void PWM_TIM1_CONFIG (unsigned short prescaler, unsigned short periodo, unsigned short duty1, unsigned short duty2, unsigned short duty3, unsigned short duty4, unsigned char channel_en);
void PWM_TIM1_DUTY (unsigned short duty, unsigned short channel);
void PWM_TIM2_CONFIG (unsigned short prescaler, unsigned short periodo, unsigned short duty1, unsigned short duty2, unsigned short duty3, unsigned short duty4, unsigned char channel_en);
void PWM_TIM2_DUTY (unsigned short duty, unsigned short channel);
void PWM_TIM3_CONFIG (unsigned short prescaler, unsigned short periodo, unsigned short duty1, unsigned short duty2, unsigned short duty3, unsigned short duty4, unsigned char channel_en);
void PWM_TIM3_DUTY (unsigned short duty, unsigned short channel);
void PWM_TIM4_CONFIG (unsigned short prescaler, unsigned short periodo, unsigned short duty1, unsigned short duty2, unsigned short duty3, unsigned short duty4, unsigned char channel_en);
void PWM_TIM4_DUTY (unsigned short duty, unsigned short channel);


#define HIGH_LEFT_PWM_CH1(X)    TIM8_Update_CH3(X)
#define LOW_RIGHT_PWM_CH1(X)    TIM8_Update_CH4(X)

#define HIGH_LEFT_PWM_CH2(X)    TIM8_Update_CH2(X)
#define LOW_RIGHT_PWM_CH2(X)    TIM8_Update_CH1(X)

#define HIGH_LEFT_PWM_CH3(X)    TIM4_Update_CH2(X)
#define LOW_RIGHT_PWM_CH3(X)    TIM4_Update_CH3(X)

#define HIGH_LEFT_PWM_CH4(X)    TIM5_Update_CH1(X)
#define LOW_RIGHT_PWM_CH4(X)    TIM5_Update_CH2(X)

#endif
