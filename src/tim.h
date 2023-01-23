//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### TIM.H #################################
//---------------------------------------------

#ifndef _TIMER_H_
#define _TIMER_H_

// Module Exported Types Constants and Macros ----------------------------------
#define DUTY_10_PERCENT		100
#define DUTY_50_PERCENT		500
#define DUTY_100_PERCENT        1000
#define DUTY_FOR_DMAX           450

#define DUTY_ALWAYS    (DUTY_100_PERCENT + 1)
#define DUTY_NONE    0

#define EnableTimer1    (TIM1->CR1 |= TIM_CR1_CEN)
#define DisableTimer1    (TIM1->CR1 &= ~TIM_CR1_CEN)

#define EnableTimer3 (TIM3->CR1 |= TIM_CR1_CEN)
#define DisableTimer3    (TIM3->CR1 &= ~TIM_CR1_CEN)

#ifdef STM32F10X_HD
#define EnableTimer6    (TIM6->CR1 |= TIM_CR1_CEN)
#define DisableTimer6    (TIM6->CR1 &= ~TIM_CR1_CEN)

#define EnableTimer7    (TIM7->CR1 |= TIM_CR1_CEN)
#define DisableTimer7    (TIM7->CR1 &= ~TIM_CR1_CEN)
#endif


#define PIN_LEFT_ON    Update_TIM1_CH1(DUTY_ALWAYS)
#define PIN_LEFT_OFF    Update_TIM1_CH1(DUTY_NONE)
#define PIN_LEFT_50    Update_TIM1_CH1(DUTY_50_PERCENT)    
#define PIN_RIGHT_ON    Update_TIM1_CH2(DUTY_ALWAYS)
#define PIN_RIGHT_50    Update_TIM1_CH2(DUTY_50_PERCENT)
#define PIN_RIGHT_OFF    Update_TIM1_CH2(DUTY_NONE)

#define PIN_LEFT_PWM(X)    Update_TIM1_CH1(X)
#define PIN_RIGHT_PWM(X)    Update_TIM1_CH2(X)

//--- Exported Functions ------------------------------
void Wait_ms (unsigned short);

// void TIM2_Init(void);
void TIM_1_Init (void);
void TIM_3_Init (void);

void UpdateTIMSync (unsigned short);
void UpdateTIM_MosfetA (unsigned short);
void UpdateTIM_MosfetB (unsigned short);
void EnablePreload_MosfetA (void);
void DisablePreload_MosfetA (void);
void EnablePreload_MosfetB (void);
void DisablePreload_MosfetB (void);
void TIM_1_OPM_us (unsigned short);

void Update_TIM1_CH1 (unsigned short);
void Update_TIM1_CH2 (unsigned short);

void Update_TIM3_CH2 (unsigned short);
void Update_TIM3_CH3 (unsigned short);
void Update_TIM3_CH4 (unsigned short);

void TIM_4_Init (void);
void TIM_4_Delay_us (unsigned short);
void Update_TIM4_CH1 (unsigned short a);

#ifdef STM32F10X_HD
// Timer 5 Functions
void TIM5_Init (void);
void TIM5_Update_CH1 (unsigned short a);
void TIM5_Update_CH2 (unsigned short a);

void TIM6_Init(void);
void TIM6_IRQHandler (void);
void TIM7_Init(void);
void TIM7_IRQHandler (void);

// Timer 8 Functions
void TIM8_Init (void);
void TIM8_Update_CH1 (unsigned short a);
void TIM8_Update_CH2 (unsigned short a);
void TIM8_Update_CH3 (unsigned short a);
void TIM8_Update_CH4 (unsigned short a);
#endif

#endif /* _TIMER_H_ */
