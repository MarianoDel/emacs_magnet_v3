//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### TIMER.H ################################
//---------------------------------------------

#ifndef _TIMER_H_
#define _TIMER_H_

//-- Exported Hardware Macros ---------------------
#define EnableTimer1    (TIM1->CR1 |= TIM_CR1_CEN)
#define DisableTimer1    (TIM1->CR1 &= ~TIM_CR1_CEN)

#define EnableTimer3    (TIM3->CR1 |= TIM_CR1_CEN)
#define DisableTimer3    (TIM3->CR1 &= ~TIM_CR1_CEN)

#ifdef STM32F10X_HD
#define EnableTimer6 TIM6->CR1 |= TIM_CR1_CEN
#define DisableTimer6 TIM6->CR1 &= ~TIM_CR1_CEN

#define EnableTimer7 TIM7->CR1 |= TIM_CR1_CEN
#define DisableTimer7 TIM7->CR1 &= ~TIM_CR1_CEN
#endif

#define RCC_TIM1_CLK 		(RCC->APB2ENR & 0x00000800)
#define RCC_TIM1_CLKEN     	RCC->APB2ENR |= 0x00000800
#define RCC_TIM1_CLKDIS 	RCC->APB2ENR &= ~0x00000800

#define RCC_TIM2_CLK            (RCC->APB1ENR & 0x00000001)
#define RCC_TIM2_CLKEN          RCC->APB1ENR |= 0x00000001
#define RCC_TIM2_CLKDIS         RCC->APB1ENR &= ~0x00000001

#define RCC_TIM3_CLK 		(RCC->APB1ENR & 0x00000002)
#define RCC_TIM3_CLKEN     	RCC->APB1ENR |= 0x00000002
#define RCC_TIM3_CLKDIS    	RCC->APB1ENR &= ~0x00000002

#define RCC_TIM4_CLK            (RCC->APB1ENR & 0x00000004)
#define RCC_TIM4_CLKEN          RCC->APB1ENR |= 0x00000004
#define RCC_TIM4_CLKDIS         RCC->APB1ENR &= ~0x00000004

#ifdef STM32F10X_HD
#define RCC_TIM5_CLK            (RCC->APB1ENR & 0x00000008)
#define RCC_TIM5_CLKEN          RCC->APB1ENR |= 0x00000008
#define RCC_TIM5_CLKDIS         RCC->APB1ENR &= ~0x00000008

#define RCC_TIM6_CLK            (RCC->APB1ENR & 0x00000010)
#define RCC_TIM6_CLKEN          RCC->APB1ENR |= 0x00000010;
#define RCC_TIM6_CLKDIS         RCC->APB1ENR &= ~0x00000010;

#define RCC_TIM7_CLK            (RCC->APB1ENR & 0x00000020)
#define RCC_TIM7_CLKEN          RCC->APB1ENR |= 0x00000020;
#define RCC_TIM7_CLKDIS         RCC->APB1ENR &= ~0x00000020;
#endif

//--- Exported Functions ------------------------------
void TIM7_IRQHandler (void);
void TIM7_Init(void);
void TIM6_IRQHandler (void);
void TIM6_Init(void);
void TIM2_Init(void);
void Wait_ms (unsigned short);

#endif /* _TIMER_H_ */
