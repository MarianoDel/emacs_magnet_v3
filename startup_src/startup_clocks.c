//---------------------------------------------------------
// #### Part of Startup Files for STM32F103RC
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// * .    - SystemInit(): Setups the system clock (System clock source, PLL Multiplier
// *                      factors, AHB/APBx prescalers and Flash settings). 
// *                      This function is called at startup just after reset and 
// *                      before branch to main program. This call is made inside
// *                      the "startup_stm32f10x_xx.s" file.
// *
// * .  After each device reset the HSI (8 MHz) is used as system clock source.
// *    Then SystemInit() function is called, in "startup_stm32f10x_xx.s" file, to
// *    configure the system clock before to branch to main program.
// *
// #### STARTUP_CLOCKS.C ############################################
//---------------------------------------------------------

#include "stm32f10x.h"
#include "startup_clocks.h"


#if (!defined HSI_INTERNAL_RC) && (!defined HSE_CRYSTAL_OSC)
#error	"No Oscillator Source Defined on startup_clock.h"
#endif

// check sysclk selection
#if (!defined SYSCLK_FREQ_72MHz) && \
    (!defined SYSCLK_FREQ_64MHz) && \
    (!defined SYSCLK_FREQ_52MHz) && \
    (!defined SYSCLK_FREQ_48MHz) && \
    (!defined SYSCLK_FREQ_8MHz)
#error	"No Sysclk Frequency Selected (check startup_clocks.h)"
#endif

// check xtal selection
#ifdef HSE_CRYSTAL_OSC
#if (!defined CRYSTAL_8MHZ) && (!defined CRYSTAL_12MHZ)
#error  "HSE selected but not Crystal frequency defined"
#endif
#endif


void SystemInit (void)
{
    // clock settings, pll and flash latency
#ifdef HSE_CRYSTAL_OSC
    // SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---
    // Enable HSE 
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
    // Wait till HSE is ready
    while (!(RCC->CR & RCC_CR_HSERDY));

    // Flash Config
    // Enable Prefetch Buffer 
    FLASH->ACR |= FLASH_ACR_PRFTBE;

#ifdef SYSCLK_FREQ_72MHz
    // Flash
    // 0 wait state if 0 < SYSCLK <= 24MHz
    // 1 wait state if 24MHz < SYSCLK <= 48MHz
    // 2 wait state if 48MHz < SYSCLK <= 72MHz
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);    
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

    // Sysclk config
    // AHB prescaler HCLK = SYSCLK
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    // APB2 prescaler PCLK2 = HCLK
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    // APB1 prescaler PCLK1 = HCLK/2
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

    // PLL Config
    // PLL configuration reset
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));

#ifdef CRYSTAL_8MHZ
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);
#endif
#ifdef CRYSTAL_12MHZ
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL6);
#endif
#endif    // SYSCLK_FREQ_72MHz


#ifdef SYSCLK_FREQ_48MHz
    // Flash
    // 0 wait state if 0 < SYSCLK <= 24MHz
    // 1 wait state if 24MHz < SYSCLK <= 48MHz
    // 2 wait state if 48MHz < SYSCLK <= 72MHz
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);    
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

    // Sysclk config
    // AHB prescaler HCLK = SYSCLK
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    // APB2 prescaler PCLK2 = HCLK
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    // APB1 prescaler PCLK1 = HCLK/2
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

    // PLL Config
    // PLL configuration reset
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));

#ifdef CRYSTAL_8MHZ
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL6);
#endif
#ifdef CRYSTAL_12MHZ
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL4);
#endif
#endif    // SYSCLK_FREQ_48MHz


#ifdef SYSCLK_FREQ_8MHz
    // Flash
    // 0 wait state if 0 < SYSCLK <= 24MHz
    // 1 wait state if 24MHz < SYSCLK <= 48MHz
    // 2 wait state if 48MHz < SYSCLK <= 72MHz
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);    
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;

    // Sysclk config
    // AHB prescaler HCLK = SYSCLK
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    // APB2 prescaler PCLK2 = HCLK
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    // APB1 prescaler PCLK1 = HCLK
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

    // PLL Config
    // PLL configuration reset
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));

#ifdef CRYSTAL_8MHZ    
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE |
                            RCC_CFGR_PLLMULL2 |
                            RCC_CFGR_PLLXTPRE);
#endif
#ifdef CRYSTAL_12MHZ
#error "don't be so stupid!"
#endif
#endif    // SYSCLK_FREQ_8MHz

    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;

    // Wait till PLL is ready
    while((RCC->CR & RCC_CR_PLLRDY) == 0);
    
    // Select PLL as system clock source
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    // Wait till PLL is used as system clock source
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08);
            
#endif    //HSE_CRYSTAL_OSC

#ifdef HSI_INTERNAL_RC

#ifdef SYSCLK_FREQ_72MHz
#error "Max sysclk freq. with HSI is 64MHz"
#endif    //SYSCLK_FREQ_72MHz

#ifdef SYSCLK_FREQ_64MHz
    //me aseguro que el PLL este deshabilitado
    RCC->CR &= ~RCC_CR_PLLON;

    //ajuste fino del clk HSI (internal RC)
    //en RCC->CR; HSITRIM[7:3] user trim value, HSICAL[15:8] es factoy default (read only)
    //todo a 0 baja freq (mas chico menos freq)
    // RCC->CR &= ~RCC_CR_HSITRIM;
    // RCC->CR |= 0x000000F8;    //max freq
    // RCC->CR |= 0x00000000;    //min freq
    
    
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    // Flash
    // 0 wait state if 0 < SYSCLK <= 24MHz
    // 1 wait state if 24MHz < SYSCLK <= 48MHz
    // 2 wait state if 48MHz < SYSCLK <= 72MHz
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    // RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV4;    
    
    /* PCLK1 = HCLK/2 */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
    
    //configuro el PLL
    /*  PLL configuration: PLLCLK = HSI/2 * 16 = 64 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL16);
    
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
    
    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
    
#endif    //SYSCLK_FREQ_64MHz

#ifdef SYSCLK_FREQ_52MHz
    //me aseguro que el PLL este deshabilitado
    RCC->CR &= ~RCC_CR_PLLON;

    //ajuste fino del clk HSI (internal RC)
    //en RCC->CR; HSITRIM[7:3] user trim value, HSICAL[15:8] es factoy default (read only)
    //todo a 0 baja freq (mas chico menos freq)
    // RCC->CR &= ~RCC_CR_HSITRIM;
    // RCC->CR |= 0x000000F8;    //max freq
    // RCC->CR |= 0x00000000;    //min freq
    
    
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 2 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
    
    //configuro el PLL
    /*  PLL configuration: PLLCLK = HSI/2 * 13 = 52 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL13);
    
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
    
    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
    

#endif    //SYSCLK_FREQ_52MHz
    
#ifdef SYSCLK_FREQ_8MHz
    //uso todo por defualt despues del reset
    //me aseguro que el PLL este deshabilitado
    RCC->CR &= ~RCC_CR_PLLON;

    // /* Enable Prefetch Buffer */
    // FLASH->ACR |= FLASH_ACR_PRFTBE;

    // /* Flash 2 wait state */
    // FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    // FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

    // /* HCLK = SYSCLK */
    // RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    // /* PCLK2 = HCLK */
    // RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    // /* PCLK1 = HCLK */
    // RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
    
#endif    //SYSCLK_FREQ_8MHz
    
#endif    //HSI_INTERNAL_RC
    
}

